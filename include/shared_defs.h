#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

#define SET_BIT_U32(value, bit_idx)                                            \
    ((uint32_t)value | (uint32_t)(1 << (bit_idx)))
#define CLEAR_BIT_U32(value, bit_idx)                                          \
    ((uint32_t)value & (uint32_t)(~(1 << (bit_idx))))

#define TENS_DIGIT(value)                                                      \
    (value / 10)
#define UNITS_DIGIT(value)                                                      \
    (value % 10)
#endif /* SHARED_DEFS_H */
