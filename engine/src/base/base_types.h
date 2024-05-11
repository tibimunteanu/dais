#pragma once

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;

typedef signed char I8;
typedef short I16;
typedef int I32;
typedef long long I64;

typedef float F32;
typedef double F64;

typedef int B32;
typedef signed char B8;

#define true  1
#define false 0

#define MIN_I8  (I8)0x80
#define MIN_I16 (I16)0x8000
#define MIN_I32 (I32)0x80000000
#define MIN_I64 (I64)0x8000000000000000llu

#define MAX_I8  (I8)0x7f
#define MAX_I16 (I16)0x7fff
#define MAX_I32 (I32)0x7fffffff
#define MAX_I64 (I64)0x7fffffffffffffffllu

#define MIN_U8  (U8)0
#define MIN_U16 (U16)0
#define MIN_U32 (U32)0
#define MIN_U64 (U64)0llu

#define MAX_U8  (U8)0xff
#define MAX_U16 (U16)0xffff
#define MAX_U32 (U32)0xffffffff
#define MAX_U64 (U64)0xffffffffffffffffllu

#define SIGN_MASK_F32 (U32)0x80000000
#define SIGN_MASK_F64 (U64)0x8000000000000000ull

#define EXPONENT_MASK_F32 (U32)0x7F800000
#define EXPONENT_MASK_F64 (U64)0x7FF0000000000000ull

#define MANTISSA_MASK_F32 (U32)0x7FFFFF
#define MANTISSA_MASK_F64 (U64)0xFFFFFFFFFFFFFull

typedef char* CString;
