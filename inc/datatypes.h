#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdint.h>

#define PI32 3.14159265359f
#define PI64 3.141592653589793

#define global   static
#define local    static
#define internal static

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t  uint8;
typedef uint8_t  uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint8_t  u8;
typedef uint8_t  u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    float32;
typedef float    f32;
typedef float    real32;
typedef float    r32;

typedef double   float64;
typedef double   f64;
typedef double   real64;
typedef double   r64;

typedef int32_t  bool32;

#define true  1
#define false 0
#define TRUE  1
#define FALSE 0

#endif