#ifndef _MESSAGE_H
#define _MESSAGE_H


#define M_SOURCE(m) (*(int16_t*)(m))
#define M_TYPE(m) (*(int16_t*)(m + 2))

#define M1_I1(m) (*(int16_t*)(m + 4))
#define M1_I2(m) (*(int16_t*)(m + 6))
#define M1_I3(m) (*(int16_t*)(m + 8))
#define M1_P1(m) (*(uint16_t*)(m + 10))
#define M1_P2(m) (*(uint16_t*)(m + 12))
#define M1_P3(m) (*(uint16_t*)(m + 14))

#define M2_I1(m) (*(int16_t*)(m + 4))
#define M2_I2(m) (*(int16_t*)(m + 6))
#define M2_I3(m) (*(int16_t*)(m + 8))
#define M2_L1(m) (*(int32_t*)(m + 10))
#define M2_L2(m) (*(int32_t*)(m + 14))
#define M2_P1(m) (*(uint16_t*)(m + 18))

#define M3_I1(m) (*(int16_t*)(m + 4))
#define M3_I2(m) (*(int16_t*)(m + 6))
#define M3_P1(m) (*(uint16_t*)(m + 8))
#define M3_CAL(m) (*(uint8_t*)(m + 10))

#define M4_L1(m) (*(int32_t*)(m + 4))
#define M4_L2(m) (*(int32_t*)(m + 8))
#define M4_L3(m) (*(int32_t*)(m + 12))
#define M4_L4(m) (*(int32_t*)(m + 16))

#define M5_C1 (*(int8_t*)(m + 4))
#define M5_C2 (*(int8_t*)(m + 5))
#define M5_I1 (*(int16_t*)(m + 6))
#define M5_I2 (*(int16_t*)(m + 8))
#define M5_L1 (*(int32_t*)(m + 10))
#define M5_L2 (*(int32_t*)(m + 14))
#define M5_P1 (*(uint16_t*)(m + 18))

#define M6_I1 (*(int16_t*)(m + 4))
#define M6_I2 (*(int16_t*)(m + 6))
#define M6_I3 (*(int16_t*)(m + 8))
#define M6_L1 (*(int32_t*)(m + 10))
#define M6_F1 (*(uint16_t*)(m + 14))

#endif