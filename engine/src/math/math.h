#pragma once

#include "base/base.h"
#include "math/math_types.h"

// TODO: add random, time, date
#include <math.h>  // for trig and sqrt

#define TOLERANCE_F32           1.0e-6f
#define TOLERANCE_F64           1.0e-14f
#define EPSILON_F32             1.192092896e-07f
#define EPSILON_F64             2.220446049250313e-16
#define PI_F32                  3.14159265358979323846f
#define PI_F64                  3.14159265358979323846
#define TAU_F32                 6.28318530717958647692f
#define TAU_F64                 6.28318530717958647692
#define DEG_TO_RAD_F32          0.017453292519943295f
#define DEG_TO_RAD_F64          0.017453292519943295
#define RAD_TO_DEG_F32          57.29577951308232f
#define RAD_TO_DEG_F64          57.29577951308232
#define SQRT_TWO_F32            1.41421356237309504880f
#define SQRT_TWO_F64            1.41421356237309504880
#define SQRT_THREE_F32          1.73205080756887729352f
#define SQRT_THREE_F64          1.73205080756887729352
#define SQRT_ONE_OVER_TWO_F32   0.70710678118654752440f
#define SQRT_ONE_OVER_TWO_F64   0.70710678118654752440
#define SQRT_ONE_OVER_THREE_F32 0.57735026918962576450f
#define SQRT_ONE_OVER_THREE_F64 0.57735026918962576450

API inl F32 f32Inf(void);
API inl F64 f64Inf(void);
API inl F32 f32NegInf(void);
API inl F64 f64NegInf(void);
API inl B32 f32IsNan(F32 x);
API inl B32 f64IsNan(F64 x);
API inl F32 f32Abs(F32 x);
API inl F64 f64Abs(F64 x);
API inl F32 f32Sign(F32 x);
API inl F64 f64Sign(F64 x);
API inl F32 f32Trunc(F32 x);
API inl F64 f64Trunc(F64 x);
API inl F32 f32Floor(F32 x);
API inl F64 f64Floor(F64 x);
API inl F32 f32Round(F32 x);
API inl F64 f64Round(F64 x);
API inl F32 f32Ceil(F32 x);
API inl F64 f64Ceil(F64 x);

API inl F32 f32Sin(F32 x);
API inl F64 f64Sin(F64 x);
API inl F32 f32Cos(F32 x);
API inl F64 f64Cos(F64 x);
API inl F32 f32Tan(F32 x);
API inl F64 f64Tan(F64 x);
API inl F32 f32Atan(F32 x);
API inl F64 f64Atan(F64 x);
API inl F32 f32Atan2(F32 y, F32 x);
API inl F64 f64Atan2(F64 y, F64 x);
API inl F32 f32Asin(F32 x);
API inl F64 f64Asin(F64 x);
API inl F32 f32Acos(F32 x);
API inl F64 f64Acos(F64 x);
API inl F32 f32Sqrt(F32 x);
API inl F64 f64Sqrt(F64 x);
API inl F32 f32Ln(F32 x);
API inl F64 f64Ln(F64 x);
API inl F32 f32Log2(F32 x);
API inl F64 f64Log2(F64 x);
API inl F32 f32Pow(F32 x, F32 y);
API inl F64 f64Pow(F64 x, F64 y);

API inl B8 f32Equals(F32 a, F32 b);
API inl B8 f64Equals(F64 a, F64 b);
API inl F32 f32Lerp(F32 a, F32 b, F32 t);
API inl F64 f64Lerp(F64 a, F64 b, F64 t);
API inl F32 f32Remap01(F32 value, F32 minValue, F32 maxValue);
API inl F64 f64Remap01(F64 value, F64 minValue, F64 maxValue);
API inl F32 f32Remap(F32 value, F32 minValue, F32 maxValue, F32 newMinValue, F32 newMaxValue);
API inl F64 f64Remap(F64 value, F64 minValue, F64 maxValue, F64 newMinValue, F64 newMaxValue);

// BRIEF: Vector 2

API inl Vec2F32 vec2F32Zero(void);
API inl Vec2F32 vec2F32One(void);
API inl Vec2F32 vec2F32Up(void);
API inl Vec2F32 vec2F32Down(void);
API inl Vec2F32 vec2F32Left(void);
API inl Vec2F32 vec2F32Right(void);
API inl Vec2F32 vec2F32Add(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32AddScalar(Vec2F32 a, F32 scalar);
API inl Vec2F32 vec2F32Sub(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32SubScalar(Vec2F32 a, F32 scalar);
API inl Vec2F32 vec2F32Mul(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32MulScalar(Vec2F32 v, F32 scalar);
API inl Vec2F32 vec2F32Div(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32DivScalar(Vec2F32 v, F32 scalar);
API inl Vec2F32 vec2F32Negate(Vec2F32 v);
API inl Vec2F32 vec2F32Inverse(Vec2F32 v);
API inl F32 vec2F32LengthSquared(Vec2F32 v);
API inl F32 vec2F32Length(Vec2F32 v);
API inl Vec2F32 vec2F32Normalize(Vec2F32 v);
API inl F32 vec2F32Dot(Vec2F32 a, Vec2F32 b);
API inl F32 vec2F32Determinant(Vec2F32 a, Vec2F32 b);
API inl B8 vec2F32EqualsApprox(Vec2F32 a, Vec2F32 b, F32 tolerance);
API inl B8 vec2F32Equals(Vec2F32 a, Vec2F32 b);
API inl F32 vec2F32Distance(Vec2F32 a, Vec2F32 b);
API inl F32 vec2F32DistanceSquared(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32Transform(Vec2F32 v, Mat4F32 m);
API inl F32 vec2F32Angle(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32Clamp(Vec2F32 v, Vec2F32 minValue, Vec2F32 maxValue);
API inl Vec2F32 vec2F32ClampScalar(Vec2F32 v, F32 minValue, F32 maxValue);
API inl Vec2F32 vec2F32ClampLength(Vec2F32 v, F32 minValue, F32 maxValue);
API inl Vec2F32 vec2F32Lerp(Vec2F32 a, Vec2F32 b, F32 t);
API inl Vec2F32 vec2F32Project(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32Reject(Vec2F32 a, Vec2F32 b);
API inl Vec2F32 vec2F32Reflect(Vec2F32 v, Vec2F32 normal);
API inl Vec2F32 vec2F32Refract(Vec2F32 dirNormalized, Vec2F32 normal, F32 ratio);
API inl Vec2F32 vec2F32Rotate(Vec2F32 v, F32 angleRad);

// BRIEF: Vector 3

API inl Vec3F32 vec3F32Zero(void);
API inl Vec3F32 vec3F32One(void);
API inl Vec3F32 vec3F32Up(void);
API inl Vec3F32 vec3F32Down(void);
API inl Vec3F32 vec3F32Left(void);
API inl Vec3F32 vec3F32Right(void);
API inl Vec3F32 vec3F32Forward(void);
API inl Vec3F32 vec3F32Backward(void);
API inl Vec3F32 vec3F32Add(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32AddScalar(Vec3F32 a, F32 scalar);
API inl Vec3F32 vec3F32Sub(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32SubScalar(Vec3F32 a, F32 scalar);
API inl Vec3F32 vec3F32Mul(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32MulScalar(Vec3F32 v, F32 scalar);
API inl Vec3F32 vec3F32Div(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32DivScalar(Vec3F32 v, F32 scalar);
API inl Vec3F32 vec3F32Negate(Vec3F32 v);
API inl Vec3F32 vec3F32Inverse(Vec3F32 v);
API inl F32 vec3F32LengthSquared(Vec3F32 v);
API inl F32 vec3F32Length(Vec3F32 v);
API inl Vec3F32 Vec3Normalize(Vec3F32 v);
API inl F32 vec3F32Dot(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32Cross(Vec3F32 a, Vec3F32 b);
API inl B8 vec3F32EqualsApprox(Vec3F32 a, Vec3F32 b, F32 tolerance);
API inl B8 vec3F32Equals(Vec3F32 a, Vec3F32 b);
API inl F32 vec3F32Distance(Vec3F32 a, Vec3F32 b);
API inl F32 vec3F32DistanceSquared(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32Transform(Vec3F32 v, Mat4F32 m);
API inl F32 vec3F32Angle(Vec3F32 a, Vec3F32 b);
API inl QuatF32 vec3F32Quat(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32Clamp(Vec3F32 v, Vec3F32 minValue, Vec3F32 maxValue);
API inl Vec3F32 vec3F32ClampScalar(Vec3F32 v, F32 minValue, F32 maxValue);
API inl Vec3F32 vec3F32ClampLength(Vec3F32 v, F32 minValue, F32 maxValue);
API inl Vec3F32 vec3F32Lerp(Vec3F32 a, Vec3F32 b, F32 t);
API inl Vec3F32 vec3F32Project(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32Reject(Vec3F32 a, Vec3F32 b);
API inl Vec3F32 vec3F32Reflect(Vec3F32 v, Vec3F32 normal);
API inl Vec3F32 vec3F32Refract(Vec3F32 dirNormalized, Vec3F32 normal, F32 ratio);
API inl Vec3F32 vec3F32Unproject(Vec3F32 source, Mat4F32 projection, Mat4F32 view);
API inl Vec3F32 vec3F32RotateByQuat(Vec3F32 v, QuatF32 q);
API inl Vec3F32 vec3F32RotateByAxisAngle(Vec3F32 v, Vec3F32 axis, F32 angle);
API inl void vec3F32OrthoNormalize(Vec3F32* in_out_pLhs, Vec3F32* in_out_pRhs);

// BRIEF: Vector 4

API inl Vec4F32 vec4F32Zero(void);
API inl Vec4F32 vec4F32One(void);
API inl Vec4F32 vec4F32Add(Vec4F32 a, Vec4F32 b);
API inl Vec4F32 vec4F32AddScalar(Vec4F32 v, F32 scalar);
API inl Vec4F32 vec4F32Sub(Vec4F32 a, Vec4F32 b);
API inl Vec4F32 vec4F32SubScalar(Vec4F32 v, F32 scalar);
API inl Vec4F32 vec4F32Mul(Vec4F32 a, Vec4F32 b);
API inl Vec4F32 vec4F32MulScalar(Vec4F32 v, F32 scalar);
API inl Vec4F32 vec4F32Div(Vec4F32 a, Vec4F32 b);
API inl Vec4F32 vec4F32DivScalar(Vec4F32 v, F32 scalar);
API inl Vec4F32 vec4F32Negate(Vec4F32 a);
API inl Vec4F32 vec4F32Inverse(Vec4F32 a);
API inl F32 vec4F32LengthSquared(Vec4F32 v);
API inl F32 vec4F32Length(Vec4F32 v);
API inl Vec4F32 vec4F32Normalize(Vec4F32 v);
API inl F32 vec4F32Dot(Vec4F32 a, Vec4F32 b);
API inl B8 vec4F32EqualsApprox(Vec4F32 a, Vec4F32 b, F32 tolerance);
API inl B8 vec4F32Equals(Vec4F32 a, Vec4F32 b);
API inl Vec4F32 vec4F32Clamp(Vec4F32 v, Vec4F32 minValue, Vec4F32 maxValue);
API inl Vec4F32 vec4F32ClampScalar(Vec4F32 v, F32 minValue, F32 maxValue);
API inl Vec4F32 vec4F32Lerp(Vec4F32 a, Vec4F32 b, F32 t);

// BRIEF: Matrix 4

API inl Mat4F32 mat4F32Zero(void);
API inl Mat4F32 mat4F32Identity(void);
API inl Mat4F32 mat4F32Add(Mat4F32 a, Mat4F32 b);
API inl Mat4F32 mat4F32Sub(Mat4F32 a, Mat4F32 b);
API inl Mat4F32 mat4F32Mul(Mat4F32 a, Mat4F32 b);
API inl F32 mat4F32Trace(Mat4F32 m);
API inl F32 mat4F32Det(Mat4F32 m);
API inl Mat4F32 mat4F32Transpose(Mat4F32 m);
API inl Mat4F32 mat4F32Inverse(Mat4F32 m);
API inl Mat4F32 mat4F32Translate(Vec3F32 position);
API inl Mat4F32 mat4F32Scale(Vec3F32 scale);
API inl Mat4F32 mat4F32UniformScale(F32 scale);
API inl Mat4F32 mat4F32Rotate(Vec3F32 axis, F32 angleRad);
API inl Mat4F32 mat4F32RotateX(F32 angleRad);
API inl Mat4F32 mat4F32RotateY(F32 angleRad);
API inl Mat4F32 mat4F32RotateZ(F32 angleRad);
API inl Mat4F32 mat4F32RotateXyz(Vec3F32 angleRad);
API inl Mat4F32 mat4F32RotateZyx(Vec3F32 angleRad);
API inl Vec3F32 mat4F32Left(Mat4F32 m);
API inl Vec3F32 mat4F32Right(Mat4F32 m);
API inl Vec3F32 mat4F32Down(Mat4F32 m);
API inl Vec3F32 mat4F32Up(Mat4F32 m);
API inl Vec3F32 mat4F32Forward(Mat4F32 m);
API inl Vec3F32 mat4F32Backward(Mat4F32 m);
API inl Mat4F32 mat4F32Frustum(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
API inl Mat4F32 mat4F32Perspective(F32 fovRad, F32 aspect, F32 near, F32 far);
API inl Mat4F32 mat4F32Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
API inl Mat4F32 mat4F32LookAt(Vec3F32 position, Vec3F32 target, Vec3F32 up);

// BRIEF: Quaternion

API inl QuatF32 quatF32Identity(void);
API inl QuatF32 quatF32Add(QuatF32 a, QuatF32 b);
API inl QuatF32 quatF32AddScalar(QuatF32 a, F32 scalar);
API inl QuatF32 quatF32Sub(QuatF32 a, QuatF32 b);
API inl QuatF32 quatF32SubScalar(QuatF32 q, F32 scalar);
API inl QuatF32 quatF32Mul(QuatF32 a, QuatF32 b);
API inl QuatF32 quatF32MulScalar(QuatF32 q, F32 scalar);
API inl QuatF32 quatF32Div(QuatF32 a, QuatF32 b);
API inl QuatF32 quatF32DivScalar(QuatF32 a, F32 scalar);
API inl F32 quatF32LengthSquared(QuatF32 q);
API inl F32 quatF32Length(QuatF32 q);
API inl QuatF32 quatF32Normalize(QuatF32 q);
API inl QuatF32 quatF32Negate(QuatF32 q);
API inl QuatF32 quatF32Conjugate(QuatF32 q);
API inl QuatF32 quatF32Inverse(QuatF32 q);
API inl QuatF32 quatF32Transform(QuatF32 q, Mat4F32 m);
API inl B8 quatF32Equals(QuatF32 a, QuatF32 b);
API inl B8 quatF32Equivalent(QuatF32 a, QuatF32 b);
API inl F32 quatF32Dot(QuatF32 a, QuatF32 b);
API inl QuatF32 quatF32Lerp(QuatF32 a, QuatF32 b, F32 t);
API inl QuatF32 quatF32Nlerp(QuatF32 a, QuatF32 b, F32 t);
API inl QuatF32 quatF32Slerp(QuatF32 a, QuatF32 b, F32 t);
API inl QuatF32 quatF32FromMat4(Mat4F32 m);
API inl Mat4F32 quatF32ToMat4(QuatF32 q);
API inl QuatF32 quatF32FromAxisAngle(Vec3F32 axis, F32 angleRad);
API inl void quatF32ToAxisAngle(QuatF32 q, Vec3F32* out_pAxis, F32* out_pAngle);
API inl QuatF32 quatF32FromEuler(F32 pitch, F32 yaw, F32 roll);
API inl Vec3F32 quatF32ToEuler(QuatF32 q);
// BRIEF: Colors

API inl U32 u32FromColor3U32(Color3U32 c);
API inl Color3U32 color3U32FromU32(U32 u);
API inl Color3F32 color3F32FromColor3U32(Color3U32 c);
API inl Color3U32 color3U32FromColor3F32(Color3F32 c);
API inl ColorF32 colorF32FromColorU32(ColorU32 c);
API inl ColorU32 colorU32FromColorF32(ColorF32 c);
