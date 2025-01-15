#include <stdbool.h>
#include <string.h>

#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "hal_rtc_microSpecific.h"
#include "hal.h"
#include "hal_rtc.h"

#include "shared_defs.h"

static hal_error_E hal_rtc_microSpecific_private_initRTC(void);
static hal_error_E hal_rtc_microSpecific_private_getTimestamp(hal_rtc_time_S *time);
static hal_error_E hal_rtc_microSpecific_private_setTimestamp(const hal_rtc_time_S *time);
static void hal_rtc_microSpecific_private_setInitMode(bool set);
static void hal_rtc_microSpecific_private_setWriteProtection(bool set);

// Convert the time structure into a TR register value we can write directly into the RTC.
static uint32_t hal_rtc_microSpecific_private_timeToRegister(const hal_rtc_time_S *time);

// Convert the time structure into a DR register value we can write directly into the RTC.
static uint32_t hal_rtc_microSpecific_private_dateToRegister(const hal_rtc_time_S *time);

static hal_rtc_config_S hal_rtc_config = {
  .initRealTimeClock = hal_rtc_microSpecific_private_initRTC,
  .setTimestamp = hal_rtc_microSpecific_private_setTimestamp,
  .getTimestamp = hal_rtc_microSpecific_private_getTimestamp,
};

// Public function to be called in main()
hal_error_E hal_rtc_microSpecific_init() {
  return hal_rtc_init(&hal_rtc_config);
}

static hal_error_E hal_rtc_microSpecific_private_initRTC(void) {
  // Power interface clock enable
  // TODO: See if we also need to enable this in sleep
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  // Write backup protection disable. Allows us to edit clock registers
  // normally write protected after reset
  PWR->CR |= PWR_CR_DBP;

  // Reset the RTC domain (NOTE: This may need to be reconsidered after integrating stop mode)
  // Set and clear the reset bit
  RCC->CSR |= RCC_CSR_RTCRST;
  RCC->CSR = CLEAR_BIT_U32(RCC->CSR, RCC_CSR_RTCRST_Pos);

  // Enable RTC External Low Speed Oscillator
  RCC->CSR |= RCC_CSR_LSEON;
  // set RTCSEL to 01 for LSE clock input to the RTC
  RCC->CSR |= RCC_CSR_RTCSEL_LSE;
  RCC->CSR |= RCC_CSR_RTCEN;

  // Taken from calendar initialization and configuration on reference manual
  // pg. 550
  hal_rtc_microSpecific_private_setWriteProtection(false);
  hal_rtc_microSpecific_private_setInitMode(true);

  // Set the prescaler to generate a 1Hz clock
  // Use default values since we have 32.768kHz external oscillator
  // async prescaler is 128 so we get 32768/128 = 256 Hz
  // sync prescaler is 256 so we get 256/256 = 1Hz RTC heartbeat

  // See reference manual page 569
  RTC->PRER |= (0x7F << 16);
  RTC->PRER |= 0xFF;

  hal_rtc_microSpecific_private_setInitMode(false);
  hal_rtc_microSpecific_private_setWriteProtection(true);

  // disable backup write protection
  PWR->CR &= ~PWR_CR_DBP;

  return HAL_ERROR_OK;
}

static hal_error_E hal_rtc_microSpecific_private_setTimestamp(const hal_rtc_time_S *time) {
  hal_error_E ret = HAL_ERROR_OK;

  if (time != NULL) {
    // Write backup protection disable. Allows us to edit clock registers
    // normally write protected after reset
    PWR->CR |= PWR_CR_DBP;

    // Taken from calendar initialization and configuration on reference manual
    // pg. 550
    hal_rtc_microSpecific_private_setWriteProtection(false);
    hal_rtc_microSpecific_private_setInitMode(true);

    // TODO: add handling for BYPSHAD bit to determine when time is set / reset
    RTC->TR = hal_rtc_microSpecific_private_timeToRegister(time);
    RTC->DR = hal_rtc_microSpecific_private_dateToRegister(time);

    hal_rtc_microSpecific_private_setInitMode(false);
    hal_rtc_microSpecific_private_setWriteProtection(true);

    // enable backup write protection
    PWR->CR &= ~PWR_CR_DBP;
  } else {
    ret = HAL_ERROR_ERR;
  }

  return ret;
}

static hal_error_E hal_rtc_microSpecific_private_getTimestamp(hal_rtc_time_S *time) {
  hal_error_E ret = HAL_ERROR_OK;

  uint32_t rawTimeRegister;
  uint32_t rawDateRegister;

  // NOTE: this RSF bit is handled by hardware and may not always be set if the
  // system is midway through a shift / copy operation to the RTC shadow
  // registers. When using this in other modules, the time should only be
  // updated if the error message is OK.

  // TODO: may want to update this so that we busy wait on some timeout instead for the RSF bit
  if ((RTC->ISR & RTC_ISR_RSF) == RTC_ISR_RSF && time != NULL) {
    rawTimeRegister = RTC->TR;
    rawDateRegister = RTC->DR;

    // Proceed with decode here
    uint8_t secondsTens = (uint8_t)((rawTimeRegister & RTC_TR_ST_Msk) >> RTC_TR_ST_Pos) * 10;
    uint8_t secondsUnits = (uint8_t)(rawTimeRegister & RTC_TR_SU_Msk);
    time->seconds = secondsTens + secondsUnits;

    uint8_t minutesTens = (uint8_t)((rawTimeRegister & RTC_TR_MNT_Msk) >> RTC_TR_MNT_Pos) * 10;
    uint8_t minutesUnits = (uint8_t)((rawTimeRegister & RTC_TR_MNU_Msk) >> RTC_TR_MNU_Pos);
    time->minute = minutesTens + minutesUnits;

    uint8_t hoursTens = (uint8_t)((rawTimeRegister & RTC_TR_HT_Msk) >> RTC_TR_HT_Pos) * 10;
    uint8_t hoursUnits = (uint8_t)((rawTimeRegister & RTC_TR_HU_Msk) >> RTC_TR_HU_Pos);
    time->hour = hoursTens + hoursUnits;

    uint8_t dayTens = (uint8_t)((rawDateRegister & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos) * 10;
    uint8_t dayUnits = (uint8_t)((rawDateRegister & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos);
    time->day = dayTens + dayUnits;

    uint8_t monthTens = (uint8_t)((rawDateRegister & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos) * 10;
    uint8_t monthUnits = (uint8_t)((rawDateRegister & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos);
    time->month = monthTens + monthUnits;

    uint8_t yearTens = (uint8_t)((rawDateRegister & RTC_DR_YT_Msk) >> RTC_DR_YT_Pos) * 10;
    uint8_t yearUnits = (uint8_t)((rawDateRegister & RTC_DR_YU_Msk) >> RTC_DR_YU_Pos);
    time->year = yearTens + yearUnits;
  } else {
    ret = HAL_ERROR_ERR;
  }

  return ret;
}

static void hal_rtc_microSpecific_private_setInitMode(bool set) {
  if (set) {
    RTC->ISR |= RTC_ISR_INIT;
    // poll the INITF bit until it is set to confirm the RTC is in
    // initialization mode
    while (!(RTC->ISR & RTC_ISR_INITF));
  } else {
    // Exit initialization mode
    RTC->ISR = CLEAR_BIT_U32(RTC->ISR, RTC_ISR_INIT_Pos);
  }
}

static void hal_rtc_microSpecific_private_setWriteProtection(bool set) {
  if (!set) {
    // These keys are from section 22.4.7 of the programmer's manual
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
  } else {
    // Write incorrect keys to re-enable protection
    RTC->WPR = 0xFF;
  }
}

static uint32_t hal_rtc_microSpecific_private_timeToRegister(const hal_rtc_time_S *time) {
  // Load date and time values in the RTC_TR and RTC_DR registsrs.
  uint32_t timeStamp = 0U;

  // set the hours 10s digit BCD
  timeStamp |= TENS_DIGIT(time->hour) << RTC_TR_HT_Pos;

  // set the hours units digit BCD
  timeStamp |= UNITS_DIGIT(time->hour) << RTC_TR_HU_Pos;

  // set the minute tens digit BCD
  timeStamp |= TENS_DIGIT(time->minute) << RTC_TR_MNT_Pos;

  // set the minute units digit BCD
  timeStamp |= UNITS_DIGIT(time->minute) << RTC_TR_MNU_Pos;

  // Set 10s and unit digits for the seconds registers
  timeStamp |= TENS_DIGIT(time->seconds) << RTC_TR_ST_Pos;
  timeStamp |= UNITS_DIGIT(time->seconds) << RTC_TR_SU_Pos;

  return timeStamp;
}


static uint32_t hal_rtc_microSpecific_private_dateToRegister(const hal_rtc_time_S *time) {
  uint32_t dateStamp = 0U;

  // Year 10s and units already 0 (year 2000, no need to set)

  // Totally arbitrary for now
  // 4 == Thursday, set the weekday register
  dateStamp |= time->weekday << RTC_DR_WDU_Pos;

  // Month tens is 0
  dateStamp |= TENS_DIGIT(time->month) << RTC_DR_MT_Pos;

  // Set the months unit digit
  dateStamp |= UNITS_DIGIT(time->month) << RTC_DR_MU_Pos;

  // Set the 10s and units digit of the day
  dateStamp |= TENS_DIGIT(time->day) << RTC_DR_DT_Pos;
  dateStamp |= UNITS_DIGIT(time->day) << RTC_DR_DU_Pos;

  return dateStamp;
}
