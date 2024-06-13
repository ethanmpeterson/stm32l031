#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

#define SET_BIT_U32(value, bit_idx)                                            \
  ((uint32_t)value | (uint32_t)(1 << (bit_idx)))
#define CLEAR_BIT_U32(value, bit_idx)                                          \
  ((uint32_t)value & (uint32_t)(~(1 << (bit_idx))))

#endif /* SHARED_DEFS_H */
