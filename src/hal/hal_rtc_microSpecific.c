#include <stdbool.h>
#include <string.h>

#include "stm32l031xx.h"
#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "hal_rtc_microSpecific.h"
#include "hal.h"
#include "hal_rtc.h"

#include "shared_defs.h"

static void hal_rtc_microSpecific_setInitMode(bool set);
static void hal_rtc_microSpecific_private_setWriteProtection(bool set);

static hal_rtc_config_S hal_rtc_config = {
    .initRealTimeClock = NULL,
};

hal_error_E hal_rtc_microSpecific_init() {
    // Power interface clock enable
    // TODO: See if we also need to enable this in sleep
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    // Write backup protection disable. Allows us to edit clock registers
    // normally write protected after reset
    PWR->CR |= PWR_CR_DBP;

    // Enable RTC External Low Speed Oscillator
    RCC->CSR |= RCC_CSR_LSEON;
    // set RTCSEL to 01 for LSE clock input to the RTC
    RCC->CSR |= RCC_CSR_RTCSEL_LSE;
    RCC->CSR |= RCC_CSR_RTCEN;

    // Taken from calendar initialization and configuration on reference manual
    // pg. 550
    hal_rtc_microSpecific_private_setWriteProtection(false);
    RTC->ISR |= RTC_ISR_INIT;
    // poll the INITF bit until it is set to confirm the RTC is in
    // initialization mode
    while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF)
        ;

    // Set the prescaler to generate a 1Hz clock
    // Use default values since we have 32.768kHz external oscillator
    // async prescaler is 128 so we get 32768/128 = 256 Hz
    // sync prescaler is 256 so we get 256/256 = 1Hz RTC heartbeat

    // See reference manual page 569
    RTC->PRER |= (0x7F << 16);
    RTC->PRER |= 0xFF;

    // Load initial date and time values in the RTC_TR and RTC_DR registsrs.
    // Also configure the format in the RTC_CR register

    // TODO: breaking this out into a separate function to set the time
    // TODO: add handling for BYPSHAD bit to determine when time is set / reset

    uint32_t timeStamp = 0U;
    uint32_t dateStamp = 0U;

    // Select the default time: September 14, 2000 1:41PM (13:41) (Made this up
    // until we have an interface for getting the correct time)

    // set the hours 10s digit BCD
    timeStamp |= 1U << RTC_TR_HT_Pos; // set to the 1 in 13:41

    // set the hours units digit BCD
    timeStamp |= 3U << RTC_TR_HU_Pos; // set to 3 in the 13:41

    // set the minute tens digit BCD
    timeStamp |= 4U << RTC_TR_MNT_Pos;

    // set the minute units digit BCD
    timeStamp |= 1U << RTC_TR_MNU_Pos;

    // Arbitrarily set 55s for the seconds registers
    timeStamp |= 5U << RTC_TR_ST_Pos;
    timeStamp |= 5U << RTC_TR_SU_Pos;

    RTC->TR = timeStamp;

    // Now set the date register

    // Year 10s and units already 0 (year 2000, no need to set)

    // 4 == Thursday, set the weekday register
    dateStamp |= 4U << RTC_DR_WDU_Pos;

    // Month tens is 0 (month 09 == September)

    // Set the months unit digit (9 == September)
    dateStamp |= 9U << RTC_DR_MU_Pos;

    // Set the 10s and units digit of the day (14th)
    dateStamp |= 1U << RTC_DR_DT_Pos;
    dateStamp |= 4U << RTC_DR_DU_Pos;

    RTC->DR = dateStamp;

    // Exit initialization mode
    // TODO: make a function to enter and exit initialization mode
    RTC->ISR = CLEAR_BIT_U32(RTC->ISR, RTC_ISR_INIT_Pos);
    hal_rtc_microSpecific_private_setWriteProtection(true);
    // disable backup write protection
    PWR->CR &= ~PWR_CR_DBP;

    return hal_rtc_init(&hal_rtc_config);
}

hal_error_E hal_rtc_microSpecific_getTime(hal_rtc_time_S *time) {
  hal_error_E ret = HAL_ERROR_OK;

  uint32_t rawTimeRegister;
  uint32_t rawDateRegister;
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
  // TODO
}

static void hal_rtc_microSpecific_private_setWriteProtection(bool set) {
  if (!set) {
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
  } else {
    // Write incorrect keys to re-enable protection
    RTC->WPR = 0xFF;
  }
}
