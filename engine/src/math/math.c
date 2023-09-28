#include "math.h"

F32 f32Inf(void) {
    union {
        F32 f;
        U32 u;
    } result;
    result.u = 0x7f800000;
    return result.f;
}

F32 f32NegInf(void) {
    union {
        F32 f;
        U32 u;
    } result;
    result.u = 0xff800000;
    return result.f;
}

F64 f64Inf(void) {
    union {
        F64 f;
        U64 u;
    } result;
    result.u = 0x7ff0000000000000;
    return result.f;
}

F64 f64NegInf(void) {
    union {
        F64 f;
        U64 u;
    } result;
    result.u = 0xfff0000000000000;
    return result.f;
}

B32 f32IsNan(F32 x) {
    union {
        F32 f;
        U32 u;
    } result;

    result.f = x;
    return ((result.u & EXPONENT_MASK_F32) == EXPONENT_MASK_F32) && ((result.u & MANTISSA_MASK_F32) != 0);
}

B32 f64IsNan(F64 x) {
    union {
        F64 f;
        U64 u;
    } result;

    result.f = x;
    return ((result.u & EXPONENT_MASK_F64) == EXPONENT_MASK_F64) && ((result.u & MANTISSA_MASK_F64) != 0);
}

F32 f32Abs(F32 x) {
    union {
        F32 f;
        U32 u;
    } result;
    result.f = x;
    result.u &= 0x7fffffff;
    return result.f;
}

F64 f64Abs(F64 x) {
    union {
        F64 f;
        U64 u;
    } result;
    result.f = x;
    result.u &= 0x7fffffffffffffff;
    return result.f;
}

F32 f32Sign(F32 x) {
    union {
        F32 f;
        U32 u;
    } result;
    result.f = x;
    return (result.u & 0x80000000) ? -1.0f : 1.0f;
}

F64 f64Sign(F64 x) {
    union {
        F64 f;
        U64 u;
    } result;
    result.f = x;
    return (result.u & 0x8000000000000000) ? -1.0 : 1.0;
}

F32 f32Trunc(F32 x) {
    return (F32)(I32)x;
}

F64 f64Trunc(F64 x) {
    return (F64)(I64)x;
}

F32 f32Floor(F32 x) {
    F32 result;
    if (x >= 0.f) {
        result = (F32)(I32)x;
    } else {
        result = (F32)(((I32)x) - 1);
    }
    return result;
}

F64 f64Floor(F64 x) {
    F64 result;
    if (x >= 0.f) {
        result = (F64)(I64)x;
    } else {
        result = (F64)(((I64)x) - 1);
    }
    return result;
}

F32 f32Round(F32 x) {
    F32 result;
    if (x >= 0.f) {
        result = (F32)(((I32)(x + 0.5f)) + 1);
    } else {
        result = (F32)(((I32)(x - 0.5f)) - 1);
    }
    return result;
}

F64 f64Round(F64 x) {
    F64 result;
    if (x >= 0.f) {
        result = (F64)(((I64)(x + 0.5)) + 1);
    } else {
        result = (F64)(((I64)(x - 0.5)) - 1);
    }
    return result;
}

F32 f32Ceil(F32 x) {
    F32 result;
    if (x >= 0.f) {
        result = (F32)(((I32)x) + 1);
    } else {
        result = (F32)(I32)x;
    }
    return result;
}

F64 f64Ceil(F64 x) {
    F64 result;
    if (x >= 0.f) {
        result = (F64)(((I64)x) + 1);
    } else {
        result = (F64)(I64)x;
    }
    return result;
}

F32 f32Sin(F32 x) {
    return sinf(x);
}

F64 f64Sin(F64 x) {
    return f32Sin(x);
}

F32 f32Cos(F32 x) {
    return cosf(x);
}

F64 f64Cos(F64 x) {
    return f32Cos(x);
}

F32 f32Tan(F32 x) {
    return tanf(x);
}

F64 f64Tan(F64 x) {
    return f32Tan(x);
}

F32 f32Atan(F32 x) {
    return atanf(x);
}

F64 f64Atan(F64 x) {
    return f32Atan(x);
}

F32 f32Atan2(F32 y, F32 x) {
    return atan2f(y, x);
}

F64 f64Atan2(F64 y, F64 x) {
    return f32Atan2(y, x);
}

F32 f32Asin(F32 x) {
    return asinf(x);
}

F64 f64Asin(F64 x) {
    return f32Asin(x);
}

F32 f32Acos(F32 x) {
    return acosf(x);
}

F64 f64Acos(F64 x) {
    return f32Acos(x);
}

F32 f32Sqrt(F32 x) {
    return sqrtf(x);
}

F64 f64Sqrt(F64 x) {
    return f32Sqrt(x);
}

F32 f32Ln(F32 x) {
    return logf(x);
}

F64 f64Ln(F64 x) {
    return log(x);
}

F32 f32Log2(F32 x) {
    return log2f(x);
}

F64 f64Log2(F64 x) {
    return f32Log2(x);
}

F32 f32Pow(F32 x, F32 y) {
    return powf(x, y);
}

F64 f64Pow(F64 x, F64 y) {
    return f32Pow(x, y);
}

B8 f32Equals(F32 a, F32 b) {
    return f32Abs(a - b) <= TOLERANCE_F32* max(max(f32Abs(a), f32Abs(b)), 1.0f);
}

B8 f64Equals(F64 a, F64 b) {
    return f64Abs(a - b) <= TOLERANCE_F64* max(max(f64Abs(a), f64Abs(b)), 1.0);
}

F32 f32Lerp(F32 a, F32 b, F32 t) {
    return a + (b - a) * t;
}

F64 f64Lerp(F64 a, F64 b, F64 t) {
    return a + (b - a) * t;
}

F32 f32Remap01(F32 value, F32 minValue, F32 maxValue) {
    return minValue != maxValue ? (value - minValue) / (maxValue - minValue) : 0.0f;
}

F64 f64Remap01(F64 value, F64 minValue, F64 maxValue) {
    return minValue != maxValue ? (value - minValue) / (maxValue - minValue) : 0.0;
}

F32 f32Remap(F32 value, F32 minValue, F32 maxValue, F32 newMinValue, F32 newMaxValue) {
    return (((value - minValue) * (newMaxValue - newMinValue)) / (maxValue - minValue)) + newMinValue;
}

F64 f64Remap(F64 value, F64 minValue, F64 maxValue, F64 newMinValue, F64 newMaxValue) {
    return (((value - minValue) * (newMaxValue - newMinValue)) / (maxValue - minValue)) + newMinValue;
}

// BRIEF: Vector 2
Vec2F32 vec2F32Zero(void) {
    return (Vec2F32) { 0.0f, 0.0f };
}

Vec2F32 vec2F32One(void) {
    return (Vec2F32) { 1.0f, 1.0f };
}

Vec2F32 vec2F32Up(void) {
    return (Vec2F32) { 0.0f, 1.0f };
}

Vec2F32 vec2F32Down(void) {
    return (Vec2F32) { 0.0f, -1.0f };
}

Vec2F32 vec2F32Left(void) {
    return (Vec2F32) { -1.0f, 0.0f };
}

Vec2F32 vec2F32Right(void) {
    return (Vec2F32) { 1.0f, 0.0f };
}

Vec2F32 vec2F32Add(Vec2F32 a, Vec2F32 b) {
    return (Vec2F32) { a.x + b.x, a.y + b.y };
}

Vec2F32 vec2F32AddScalar(Vec2F32 a, F32 scalar) {
    return (Vec2F32) { a.x + scalar, a.y + scalar };
}

Vec2F32 vec2F32Sub(Vec2F32 a, Vec2F32 b) {
    return (Vec2F32) { a.x - b.x, a.y - b.y };
}

Vec2F32 vec2F32SubScalar(Vec2F32 a, F32 scalar) {
    return (Vec2F32) { a.x - scalar, a.y - scalar };
}

Vec2F32 vec2F32Mul(Vec2F32 a, Vec2F32 b) {
    return (Vec2F32) { a.x* b.x, a.y* b.y };
}

Vec2F32 vec2F32MulScalar(Vec2F32 v, F32 scalar) {
    return (Vec2F32) { v.x* scalar, v.y* scalar };
}

Vec2F32 vec2F32Div(Vec2F32 a, Vec2F32 b) {
    return (Vec2F32) { a.x / b.x, a.y / b.y };
}

Vec2F32 vec2F32DivScalar(Vec2F32 v, F32 scalar) {
    return (Vec2F32) { v.x / scalar, v.y / scalar };
}

Vec2F32 vec2F32Negate(Vec2F32 v) {
    return (Vec2F32) { -v.x, -v.y };
}

Vec2F32 vec2F32Inverse(Vec2F32 v) {
    return (Vec2F32) { 1.0f / v.x, 1.0f / v.y };
}

F32 vec2F32LengthSquared(Vec2F32 v) {
    return squared(v.x) + squared(v.y);
}

F32 vec2F32Length(Vec2F32 v) {
    return f32Sqrt(vec2F32LengthSquared(v));
}

Vec2F32 vec2F32Normalize(Vec2F32 v) {
    Vec2F32 result;

    F32 lengthSquared = vec2F32LengthSquared(v);

    if (lengthSquared != 1.0f && lengthSquared != 0.0f) {
        F32 lengthInv = 1.0f / f32Sqrt(lengthSquared);

        result = vec2F32MulScalar(v, lengthInv);
    } else {
        result = v;
    }

    return result;
}

F32 vec2F32Dot(Vec2F32 a, Vec2F32 b) {
    return a.x * b.x + a.y * b.y;
}

F32 vec2F32Determinant(Vec2F32 a, Vec2F32 b) {
    return a.x * b.y - a.y * b.x;
}

B8 vec2F32EqualsApprox(Vec2F32 a, Vec2F32 b, F32 tolerance) {
    return (f32Abs(a.x - b.x) <= tolerance) && (f32Abs(a.y - b.y) <= tolerance);
}

B8 vec2F32Equals(Vec2F32 a, Vec2F32 b) {
    return f32Equals(a.x, b.x) && f32Equals(a.y, b.y);
}

F32 vec2F32Distance(Vec2F32 a, Vec2F32 b) {
    return vec2F32Length(vec2F32Sub(a, b));
}

F32 vec2F32DistanceSquared(Vec2F32 a, Vec2F32 b) {
    return vec2F32LengthSquared(vec2F32Sub(a, b));
}

Vec2F32 vec2F32Transform(Vec2F32 v, Mat4F32 m) {
    return (Vec2F32) { v.x* m.m0 + v.y* m.m4 + 0.0f* m.m8 + m.m12, v.x* m.m1 + v.y* m.m5 + 0.0f* m.m9 + m.m13 };
}

F32 vec2F32Angle(Vec2F32 a, Vec2F32 b) {
    return -f32Atan2(vec2F32Determinant(a, b), vec2F32Dot(a, b));
}

Vec2F32 vec2F32Clamp(Vec2F32 v, Vec2F32 minValue, Vec2F32 maxValue) {
    return (Vec2F32) { clamp(v.x, minValue.x, maxValue.x), clamp(v.y, minValue.y, maxValue.y) };
}

Vec2F32 vec2F32ClampScalar(Vec2F32 v, F32 minValue, F32 maxValue) {
    return (Vec2F32) { clamp(v.x, minValue, maxValue), clamp(v.y, minValue, maxValue) };
}

Vec2F32 vec2F32ClampLength(Vec2F32 v, F32 minValue, F32 maxValue) {
    F32 lengthSquared = vec2F32LengthSquared(v);
    if (lengthSquared < squared(minValue)) {
        return vec2F32MulScalar(vec2F32Normalize(v), minValue);
    } else if (lengthSquared > squared(maxValue)) {
        return vec2F32MulScalar(vec2F32Normalize(v), maxValue);
    }
    return v;
}

Vec2F32 vec2F32Lerp(Vec2F32 a, Vec2F32 b, F32 t) {
    return vec2F32Add(a, vec2F32MulScalar(vec2F32Sub(b, a), t));
}

Vec2F32 vec2F32Project(Vec2F32 a, Vec2F32 b) {
    return vec2F32MulScalar(b, vec2F32Dot(a, b) / vec2F32LengthSquared(b));
}

Vec2F32 vec2F32Reject(Vec2F32 a, Vec2F32 b) {
    return vec2F32Sub(a, vec2F32Project(a, b));
}

Vec2F32 vec2F32Reflect(Vec2F32 v, Vec2F32 normal) {
    return vec2F32Sub(v, vec2F32MulScalar(normal, 2.0f * vec2F32Dot(v, normal)));
}

Vec2F32 vec2F32Refract(Vec2F32 dirNormalized, Vec2F32 normal, F32 ratio) {
    Vec2F32 result = dirNormalized;

    F32 dot = vec2F32Dot(dirNormalized, normal);
    F32 d = 1.0f - ratio * ratio * (1.0f - dot * dot);

    if (d >= 0.0f) {
        d = f32Sqrt(d);

        result = vec2F32Sub(vec2F32MulScalar(dirNormalized, ratio), vec2F32MulScalar(normal, ratio * dot + d));
    }

    return result;
}

Vec2F32 vec2F32Rotate(Vec2F32 v, F32 angleRad) {
    F32 C = f32Cos(angleRad);
    F32 s = f32Sin(angleRad);

    return (Vec2F32) { v.x* C - v.y* s, v.x* s + v.y* C };
}

// BRIEF: Vector 3

Vec3F32 vec3F32Zero(void) {
    return (Vec3F32) { 0.0f, 0.0f, 0.0f };
}

Vec3F32 vec3F32One(void) {
    return (Vec3F32) { 1.0f, 1.0f, 1.0f };
}

Vec3F32 vec3F32Up(void) {
    return (Vec3F32) { 0.0f, 1.0f, 0.0f };
}

Vec3F32 vec3F32Down(void) {
    return (Vec3F32) { 0.0f, -1.0f, 0.0f };
}

Vec3F32 vec3F32Left(void) {
    return (Vec3F32) { -1.0f, 0.0f, 0.0f };
}

Vec3F32 vec3F32Right(void) {
    return (Vec3F32) { 1.0f, 0.0f, 0.0f };
}

Vec3F32 vec3F32Forward(void) {
    return (Vec3F32) { 0.0f, 0.0f, -1.0f };
}

Vec3F32 vec3F32Backward(void) {
    return (Vec3F32) { 0.0f, 0.0f, 1.0f };
}

Vec3F32 vec3F32Add(Vec3F32 a, Vec3F32 b) {
    return (Vec3F32) { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3F32 vec3F32AddScalar(Vec3F32 a, F32 scalar) {
    return (Vec3F32) { a.x + scalar, a.y + scalar, a.z + scalar };
}

Vec3F32 vec3F32Sub(Vec3F32 a, Vec3F32 b) {
    return (Vec3F32) { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vec3F32 vec3F32SubScalar(Vec3F32 a, F32 scalar) {
    return (Vec3F32) { a.x - scalar, a.y - scalar, a.z - scalar };
}

Vec3F32 vec3F32Mul(Vec3F32 a, Vec3F32 b) {
    return (Vec3F32) { a.x* b.x, a.y* b.y, a.z* b.z };
}

Vec3F32 vec3F32MulScalar(Vec3F32 v, F32 scalar) {
    return (Vec3F32) { v.x* scalar, v.y* scalar, v.z* scalar };
}

Vec3F32 vec3F32Div(Vec3F32 a, Vec3F32 b) {
    return (Vec3F32) { a.x / b.x, a.y / b.y, a.z / b.z };
}

Vec3F32 vec3F32DivScalar(Vec3F32 v, F32 scalar) {
    return (Vec3F32) { v.x / scalar, v.y / scalar, v.z / scalar };
}

Vec3F32 vec3F32Negate(Vec3F32 v) {
    return (Vec3F32) { -v.x, -v.y, -v.z };
}

Vec3F32 vec3F32Inverse(Vec3F32 v) {
    return (Vec3F32) { 1.0f / v.x, 1.0f / v.y, 1.0f / v.z };
}

F32 vec3F32LengthSquared(Vec3F32 v) {
    return squared(v.x) + squared(v.y) + squared(v.z);
}

F32 vec3F32Length(Vec3F32 v) {
    return f32Sqrt(vec3F32LengthSquared(v));
}

Vec3F32 Vec3Normalize(Vec3F32 v) {
    Vec3F32 result;

    F32 lengthSquared = vec3F32LengthSquared(v);

    if (lengthSquared != 1.0f && lengthSquared != 0.0f) {
        F32 lengthInv = 1.0f / f32Sqrt(lengthSquared);

        result = vec3F32MulScalar(v, lengthInv);
    } else {
        result = v;
    }

    return result;
}

F32 vec3F32Dot(Vec3F32 a, Vec3F32 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3F32 vec3F32Cross(Vec3F32 a, Vec3F32 b) {
    return (Vec3F32) { a.y* b.z - a.z* b.y, a.z* b.x - a.x* b.z, a.x* b.y - a.y* b.x };
}

B8 vec3F32EqualsApprox(Vec3F32 a, Vec3F32 b, F32 tolerance) {
    return (f32Abs(a.x - b.x) <= tolerance) && (f32Abs(a.y - b.y) <= tolerance) && (f32Abs(a.z - b.z) <= tolerance);
}

B8 vec3F32Equals(Vec3F32 a, Vec3F32 b) {
    return f32Equals(a.x, b.x) && f32Equals(a.y, b.y) && f32Equals(a.z, b.z);
}

F32 vec3F32Distance(Vec3F32 a, Vec3F32 b) {
    return vec3F32Length(vec3F32Sub(a, b));
}

F32 vec3F32DistanceSquared(Vec3F32 a, Vec3F32 b) {
    return vec3F32LengthSquared(vec3F32Sub(a, b));
}

Vec3F32 vec3F32Transform(Vec3F32 v, Mat4F32 m) {
    return (Vec3F32) {
               v.x* m.m0 + v.y* m.m4 + v.z* m.m8 + m.m12,
                      v.x* m.m1 + v.y* m.m5 + v.z* m.m9 + m.m13,
                      v.x* m.m2 + v.y* m.m6 + v.z* m.m10 + m.m14
    };
}

F32 vec3F32Angle(Vec3F32 a, Vec3F32 b) {
    return f32Atan2(vec3F32Length(vec3F32Cross(a, b)), vec3F32Dot(a, b));
}

QuatF32 vec3F32Quat(Vec3F32 a, Vec3F32 b) {
    QuatF32 result = {};

    Vec3F32 cross = vec3F32Cross(a, b);

    result.x = cross.x;
    result.y = cross.y;
    result.z = cross.z;
    result.w = 1.0f + vec3F32Dot(a, b);

    result = quatF32Normalize(result);

    return result;
}

Vec3F32 vec3F32Clamp(Vec3F32 v, Vec3F32 minValue, Vec3F32 maxValue) {
    return (Vec3F32) {
               clamp(v.x, minValue.x, maxValue.x), clamp(v.y, minValue.y, maxValue.y),
               clamp(v.z, minValue.z, maxValue.z)
    };
}

Vec3F32 vec3F32ClampScalar(Vec3F32 v, F32 minValue, F32 maxValue) {
    return (Vec3F32) { clamp(v.x, minValue, maxValue), clamp(v.y, minValue, maxValue), clamp(v.z, minValue, maxValue) };
}

Vec3F32 vec3F32ClampLength(Vec3F32 v, F32 minValue, F32 maxValue) {
    F32 lengthSquared = vec3F32LengthSquared(v);
    if (lengthSquared < squared(minValue)) {
        return vec3F32MulScalar(Vec3Normalize(v), minValue);
    } else if (lengthSquared > squared(maxValue)) {
        return vec3F32MulScalar(Vec3Normalize(v), maxValue);
    }
    return v;
}

Vec3F32 vec3F32Lerp(Vec3F32 a, Vec3F32 b, F32 t) {
    return vec3F32Add(a, vec3F32MulScalar(vec3F32Sub(b, a), t));
}

Vec3F32 vec3F32Project(Vec3F32 a, Vec3F32 b) {
    return vec3F32MulScalar(b, vec3F32Dot(a, b) / vec3F32LengthSquared(b));
}

Vec3F32 vec3F32Reject(Vec3F32 a, Vec3F32 b) {
    return vec3F32Sub(a, vec3F32Project(a, b));
}

Vec3F32 vec3F32Reflect(Vec3F32 v, Vec3F32 normal) {
    return vec3F32Sub(v, vec3F32MulScalar(normal, 2.0f * vec3F32Dot(v, normal)));
}

Vec3F32 vec3F32Refract(Vec3F32 dirNormalized, Vec3F32 normal, F32 ratio) {
    Vec3F32 result = dirNormalized;

    F32 dot = vec3F32Dot(dirNormalized, normal);
    F32 dSquared = 1.0f - squared(ratio) * (1.0f - squared(dot));

    if (dSquared >= 0.0f) {
        result = vec3F32Sub(
            vec3F32MulScalar(dirNormalized, ratio),
            vec3F32MulScalar(normal, ratio * dot + f32Sqrt(dSquared))
        );
    }

    return result;
}

Vec3F32 vec3F32Unproject(Vec3F32 source, Mat4F32 projection, Mat4F32 view) {
    Vec3F32 result;

    Mat4F32 mvpInv = mat4F32Inverse(mat4F32Mul(view, projection));
    QuatF32 q = quatF32Transform((QuatF32) { source.x, source.y, source.z, 1.0f }, mvpInv);

    result.x = q.x / q.w;
    result.y = q.y / q.w;
    result.z = q.z / q.w;

    return result;
}

Vec3F32 vec3F32RotateByQuat(Vec3F32 v, QuatF32 q) {
    Vec3F32 result = {};

    result.x = v.x * (squared(q.x) + squared(q.w) - squared(q.y) - squared(q.z)) +
               v.y * (2 * q.x * q.y - 2 * q.w * q.z) + v.z * (2 * q.x * q.z + 2 * q.w * q.y);

    result.y = v.x * (2 * q.w * q.z + 2 * q.x * q.y) +
               v.y * (squared(q.w) - squared(q.x) + squared(q.y) - squared(q.z)) +
               v.z * (-2 * q.w * q.x + 2 * q.y * q.z);

    result.z = v.x * (-2 * q.w * q.y + 2 * q.x * q.z) + v.y * (2 * q.w * q.x + 2 * q.y * q.z) +
               v.z * (squared(q.w) - squared(q.x) - squared(q.y) + squared(q.z));

    return result;
}

Vec3F32 vec3F32RotateByAxisAngle(Vec3F32 v, Vec3F32 axis, F32 angle) {
    // Euler-Rodrigues formula
    // https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula

    axis = Vec3Normalize(axis);

    angle *= 0.5f;

    Vec3F32 w = vec3F32MulScalar(axis, f32Sin(angle));
    Vec3F32 wv = vec3F32Cross(w, v);
    Vec3F32 wwv = vec3F32Cross(w, wv);

    wv = vec3F32MulScalar(wv, 2 * f32Cos(angle));
    wwv = vec3F32MulScalar(wwv, 2);

    return vec3F32Add(v, vec3F32Add(wv, wwv));
}

void vec3F32OrthoNormalize(Vec3F32* in_out_pLhs, Vec3F32* in_out_pRhs) {
    // Gram-Schmidt formula

    *in_out_pLhs = Vec3Normalize(*in_out_pLhs);
    *in_out_pRhs = vec3F32Cross(Vec3Normalize(vec3F32Cross(*in_out_pLhs, *in_out_pRhs)), *in_out_pLhs);
}

// BRIEF: Vector 4

Vec4F32 vec4F32Zero(void) {
    return (Vec4F32) { 0.0f, 0.0f, 0.0f, 0.0f };
}

Vec4F32 vec4F32One(void) {
    return (Vec4F32) { 1.0f, 1.0f, 1.0f, 1.0f };
}

Vec4F32 vec4F32Add(Vec4F32 a, Vec4F32 b) {
    return (Vec4F32) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

Vec4F32 vec4F32AddScalar(Vec4F32 v, F32 scalar) {
    return (Vec4F32) { v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar };
}

Vec4F32 vec4F32Sub(Vec4F32 a, Vec4F32 b) {
    return (Vec4F32) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

Vec4F32 vec4F32SubScalar(Vec4F32 v, F32 scalar) {
    return (Vec4F32) { v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar };
}

Vec4F32 vec4F32Mul(Vec4F32 a, Vec4F32 b) {
    return (Vec4F32) { a.x* b.x, a.y* b.y, a.z* b.z, a.w* b.w };
}

Vec4F32 vec4F32MulScalar(Vec4F32 v, F32 scalar) {
    return (Vec4F32) { v.x* scalar, v.y* scalar, v.z* scalar, v.w* scalar };
}

Vec4F32 vec4F32Div(Vec4F32 a, Vec4F32 b) {
    return (Vec4F32) { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
}

Vec4F32 vec4F32DivScalar(Vec4F32 v, F32 scalar) {
    return (Vec4F32) { v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar };
}

Vec4F32 vec4F32Negate(Vec4F32 a) {
    return (Vec4F32) { -a.x, -a.y, -a.z, -a.w };
}

Vec4F32 vec4F32Inverse(Vec4F32 a) {
    return (Vec4F32) { 1.0f / a.x, 1.0f / a.y, 1.0f / a.z, 1.0f / a.w };
}

F32 vec4F32LengthSquared(Vec4F32 v) {
    return squared(v.x) + squared(v.y) + squared(v.z) + squared(v.w);
}

F32 vec4F32Length(Vec4F32 v) {
    return f32Sqrt(vec4F32LengthSquared(v));
}

Vec4F32 vec4F32Normalize(Vec4F32 v) {
    Vec4F32 result;

    F32 lengthSquared = vec4F32LengthSquared(v);

    if (lengthSquared != 1.0f && lengthSquared != 0.0f) {
        F32 lengthInv = 1.0f / f32Sqrt(lengthSquared);

        result = vec4F32MulScalar(v, lengthInv);
    } else {
        result = v;
    }

    return result;
}

F32 vec4F32Dot(Vec4F32 a, Vec4F32 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

B8 vec4F32EqualsApprox(Vec4F32 a, Vec4F32 b, F32 tolerance) {
    return f32Abs(a.x - b.x) <= tolerance && f32Abs(a.y - b.y) <= tolerance && f32Abs(a.z - b.z) <= tolerance &&
           f32Abs(a.w - b.w) <= tolerance;
}

B8 vec4F32Equals(Vec4F32 a, Vec4F32 b) {
    return f32Equals(a.x, b.x) && f32Equals(a.y, b.y) && f32Equals(a.z, b.z) && f32Equals(a.w, b.w);
}

Vec4F32 vec4F32Clamp(Vec4F32 v, Vec4F32 minValue, Vec4F32 maxValue) {
    return (Vec4F32) {
               clamp(v.x, minValue.x, maxValue.x),
               clamp(v.y, minValue.y, maxValue.y),
               clamp(v.z, minValue.z, maxValue.z),
               clamp(v.w, minValue.w, maxValue.w)
    };
}

Vec4F32 vec4F32ClampScalar(Vec4F32 v, F32 minValue, F32 maxValue) {
    return (Vec4F32) {
               clamp(v.x, minValue, maxValue),
               clamp(v.y, minValue, maxValue),
               clamp(v.z, minValue, maxValue),
               clamp(v.w, minValue, maxValue)
    };
}

Vec4F32 vec4F32Lerp(Vec4F32 a, Vec4F32 b, F32 t) {
    return vec4F32Add(a, vec4F32MulScalar(vec4F32Sub(b, a), t));
}

// BRIEF: Matrix 4

Mat4F32 mat4F32Zero(void) {
    Mat4F32 result = {};
    return result;
}

Mat4F32 mat4F32Identity(void) {
    Mat4F32 result = {};
    result.data[0] = 1.0f;
    result.data[5] = 1.0f;
    result.data[10] = 1.0f;
    result.data[15] = 1.0f;
    return result;
}

Mat4F32 mat4F32Add(Mat4F32 a, Mat4F32 b) {
    Mat4F32 result;
    result.m0 = a.m0 + b.m0;
    result.m1 = a.m1 + b.m1;
    result.m2 = a.m2 + b.m2;
    result.m3 = a.m3 + b.m3;
    result.m4 = a.m4 + b.m4;
    result.m5 = a.m5 + b.m5;
    result.m6 = a.m6 + b.m6;
    result.m7 = a.m7 + b.m7;
    result.m8 = a.m8 + b.m8;
    result.m9 = a.m9 + b.m9;
    result.m10 = a.m10 + b.m10;
    result.m11 = a.m11 + b.m11;
    result.m12 = a.m12 + b.m12;
    result.m13 = a.m13 + b.m13;
    result.m14 = a.m14 + b.m14;
    result.m15 = a.m15 + b.m15;
    return result;
}

Mat4F32 mat4F32Sub(Mat4F32 a, Mat4F32 b) {
    Mat4F32 result;
    result.m0 = a.m0 - b.m0;
    result.m1 = a.m1 - b.m1;
    result.m2 = a.m2 - b.m2;
    result.m3 = a.m3 - b.m3;
    result.m4 = a.m4 - b.m4;
    result.m5 = a.m5 - b.m5;
    result.m6 = a.m6 - b.m6;
    result.m7 = a.m7 - b.m7;
    result.m8 = a.m8 - b.m8;
    result.m9 = a.m9 - b.m9;
    result.m10 = a.m10 - b.m10;
    result.m11 = a.m11 - b.m11;
    result.m12 = a.m12 - b.m12;
    result.m13 = a.m13 - b.m13;
    result.m14 = a.m14 - b.m14;
    result.m15 = a.m15 - b.m15;
    return result;
}

Mat4F32 mat4F32Mul(Mat4F32 a, Mat4F32 b) {
    Mat4F32 result;
    result.m0 = a.m0 * b.m0 + a.m1 * b.m4 + a.m2 * b.m8 + a.m3 * b.m12;
    result.m1 = a.m0 * b.m1 + a.m1 * b.m5 + a.m2 * b.m9 + a.m3 * b.m13;
    result.m2 = a.m0 * b.m2 + a.m1 * b.m6 + a.m2 * b.m10 + a.m3 * b.m14;
    result.m3 = a.m0 * b.m3 + a.m1 * b.m7 + a.m2 * b.m11 + a.m3 * b.m15;
    result.m4 = a.m4 * b.m0 + a.m5 * b.m4 + a.m6 * b.m8 + a.m7 * b.m12;
    result.m5 = a.m4 * b.m1 + a.m5 * b.m5 + a.m6 * b.m9 + a.m7 * b.m13;
    result.m6 = a.m4 * b.m2 + a.m5 * b.m6 + a.m6 * b.m10 + a.m7 * b.m14;
    result.m7 = a.m4 * b.m3 + a.m5 * b.m7 + a.m6 * b.m11 + a.m7 * b.m15;
    result.m8 = a.m8 * b.m0 + a.m9 * b.m4 + a.m10 * b.m8 + a.m11 * b.m12;
    result.m9 = a.m8 * b.m1 + a.m9 * b.m5 + a.m10 * b.m9 + a.m11 * b.m13;
    result.m10 = a.m8 * b.m2 + a.m9 * b.m6 + a.m10 * b.m10 + a.m11 * b.m14;
    result.m11 = a.m8 * b.m3 + a.m9 * b.m7 + a.m10 * b.m11 + a.m11 * b.m15;
    result.m12 = a.m12 * b.m0 + a.m13 * b.m4 + a.m14 * b.m8 + a.m15 * b.m12;
    result.m13 = a.m12 * b.m1 + a.m13 * b.m5 + a.m14 * b.m9 + a.m15 * b.m13;
    result.m14 = a.m12 * b.m2 + a.m13 * b.m6 + a.m14 * b.m10 + a.m15 * b.m14;
    result.m15 = a.m12 * b.m3 + a.m13 * b.m7 + a.m14 * b.m11 + a.m15 * b.m15;
    return result;
}

F32 mat4F32Trace(Mat4F32 m) {
    F32 result = (m.m0 + m.m5 + m.m10 + m.m15);
    return result;
}

F32 mat4F32Det(Mat4F32 m) {
    F32 result = 0.0f;

    F32 a00 = m.m0, a01 = m.m1, a02 = m.m2, a03 = m.m3;
    F32 a10 = m.m4, a11 = m.m5, a12 = m.m6, a13 = m.m7;
    F32 a20 = m.m8, a21 = m.m9, a22 = m.m10, a23 = m.m11;
    F32 a30 = m.m12, a31 = m.m13, a32 = m.m14, a33 = m.m15;

    result = a30 * a21 * a12 * a03 - a20 * a31 * a12 * a03 - a30 * a11 * a22 * a03 + a10 * a31 * a22 * a03 +
             a20 * a11 * a32 * a03 - a10 * a21 * a32 * a03 - a30 * a21 * a02 * a13 + a20 * a31 * a02 * a13 +
             a30 * a01 * a22 * a13 - a00 * a31 * a22 * a13 - a20 * a01 * a32 * a13 + a00 * a21 * a32 * a13 +
             a30 * a11 * a02 * a23 - a10 * a31 * a02 * a23 - a30 * a01 * a12 * a23 + a00 * a31 * a12 * a23 +
             a10 * a01 * a32 * a23 - a00 * a11 * a32 * a23 - a20 * a11 * a02 * a33 + a10 * a21 * a02 * a33 +
             a20 * a01 * a12 * a33 - a00 * a21 * a12 * a33 - a10 * a01 * a22 * a33 + a00 * a11 * a22 * a33;

    return result;
}

Mat4F32 mat4F32Transpose(Mat4F32 m) {
    Mat4F32 result;
    result.m0 = m.m0;
    result.m1 = m.m4;
    result.m2 = m.m8;
    result.m3 = m.m12;
    result.m4 = m.m1;
    result.m5 = m.m5;
    result.m6 = m.m9;
    result.m7 = m.m13;
    result.m8 = m.m2;
    result.m9 = m.m6;
    result.m10 = m.m10;
    result.m11 = m.m14;
    result.m12 = m.m3;
    result.m13 = m.m7;
    result.m14 = m.m11;
    result.m15 = m.m15;
    return result;
}

Mat4F32 mat4F32Inverse(Mat4F32 m) {
    Mat4F32 result;

    F32 a00 = m.m0, a01 = m.m1, a02 = m.m2, a03 = m.m3;
    F32 a10 = m.m4, a11 = m.m5, a12 = m.m6, a13 = m.m7;
    F32 a20 = m.m8, a21 = m.m9, a22 = m.m10, a23 = m.m11;
    F32 a30 = m.m12, a31 = m.m13, a32 = m.m14, a33 = m.m15;

    F32 b00 = a00 * a11 - a01 * a10;
    F32 b01 = a00 * a12 - a02 * a10;
    F32 b02 = a00 * a13 - a03 * a10;
    F32 b03 = a01 * a12 - a02 * a11;
    F32 b04 = a01 * a13 - a03 * a11;
    F32 b05 = a02 * a13 - a03 * a12;
    F32 b06 = a20 * a31 - a21 * a30;
    F32 b07 = a20 * a32 - a22 * a30;
    F32 b08 = a20 * a33 - a23 * a30;
    F32 b09 = a21 * a32 - a22 * a31;
    F32 b10 = a21 * a33 - a23 * a31;
    F32 b11 = a22 * a33 - a23 * a32;

    F32 det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

    F32 detInv = 1.0f / det;

    result.m0 = (a11 * b11 - a12 * b10 + a13 * b09) * detInv;
    result.m1 = (-a01 * b11 + a02 * b10 - a03 * b09) * detInv;
    result.m2 = (a31 * b05 - a32 * b04 + a33 * b03) * detInv;
    result.m3 = (-a21 * b05 + a22 * b04 - a23 * b03) * detInv;
    result.m4 = (-a10 * b11 + a12 * b08 - a13 * b07) * detInv;
    result.m5 = (a00 * b11 - a02 * b08 + a03 * b07) * detInv;
    result.m6 = (-a30 * b05 + a32 * b02 - a33 * b01) * detInv;
    result.m7 = (a20 * b05 - a22 * b02 + a23 * b01) * detInv;
    result.m8 = (a10 * b10 - a11 * b08 + a13 * b06) * detInv;
    result.m9 = (-a00 * b10 + a01 * b08 - a03 * b06) * detInv;
    result.m10 = (a30 * b04 - a31 * b02 + a33 * b00) * detInv;
    result.m11 = (-a20 * b04 + a21 * b02 - a23 * b00) * detInv;
    result.m12 = (-a10 * b09 + a11 * b07 - a12 * b06) * detInv;
    result.m13 = (a00 * b09 - a01 * b07 + a02 * b06) * detInv;
    result.m14 = (-a30 * b03 + a31 * b01 - a32 * b00) * detInv;
    result.m15 = (a20 * b03 - a21 * b01 + a22 * b00) * detInv;

    return result;
}

Mat4F32 mat4F32Translate(Vec3F32 position) {
    Mat4F32 result = mat4F32Identity();
    result.data[12] = position.x;
    result.data[13] = position.y;
    result.data[14] = position.z;
    return result;
}

Mat4F32 mat4F32Scale(Vec3F32 scale) {
    Mat4F32 result = mat4F32Identity();
    result.data[0] = scale.x;
    result.data[5] = scale.y;
    result.data[10] = scale.z;
    return result;
}

Mat4F32 mat4F32UniformScale(F32 scale) {
    Mat4F32 result = mat4F32Identity();
    result.data[0] = scale;
    result.data[5] = scale;
    result.data[10] = scale;
    return result;
}

Mat4F32 mat4F32Rotate(Vec3F32 axis, F32 angleRad) {
    Mat4F32 result;

    axis = Vec3Normalize(axis);

    F32 s = f32Sin(angleRad);
    F32 c = f32Cos(angleRad);
    F32 t = 1.0f - c;
    F32 x = axis.x;
    F32 y = axis.y;
    F32 z = axis.z;
    F32 xt = x * t;
    F32 yt = y * t;
    F32 zt = z * t;
    F32 xs = x * s;
    F32 ys = y * s;
    F32 zs = z * s;

    result.m0 = x * xt + c;
    result.m1 = y * xt + zs;
    result.m2 = z * xt - ys;
    result.m3 = 0.0f;
    result.m4 = x * yt - zs;
    result.m5 = y * yt + c;
    result.m6 = z * yt + xs;
    result.m7 = 0.0f;
    result.m8 = x * zt + ys;
    result.m9 = y * zt - xs;
    result.m10 = z * zt + c;
    result.m11 = 0.0f;
    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

Mat4F32 mat4F32RotateX(F32 angleRad) {
    Mat4F32 result = mat4F32Identity();

    F32 c = f32Cos(angleRad);
    F32 s = f32Sin(angleRad);

    result.m5 = c;
    result.m6 = s;
    result.m9 = -s;
    result.m10 = c;

    return result;
}

Mat4F32 mat4F32RotateY(F32 angleRad) {
    Mat4F32 result = mat4F32Identity();

    F32 c = f32Cos(angleRad);
    F32 s = f32Sin(angleRad);

    result.m0 = c;
    result.m2 = -s;
    result.m8 = s;
    result.m10 = c;

    return result;
}

Mat4F32 mat4F32RotateZ(F32 angleRad) {
    Mat4F32 result = mat4F32Identity();

    F32 c = f32Cos(angleRad);
    F32 s = f32Sin(angleRad);

    result.m0 = c;
    result.m1 = s;
    result.m4 = -s;
    result.m5 = c;

    return result;
}

Mat4F32 mat4F32RotateXyz(Vec3F32 angleRad) {
    Mat4F32 result = mat4F32Identity();

    F32 sinX = f32Sin(-angleRad.x);
    F32 cosX = f32Cos(-angleRad.x);
    F32 sinY = f32Sin(-angleRad.y);
    F32 cosY = f32Cos(-angleRad.y);
    F32 sinZ = f32Sin(-angleRad.z);
    F32 cosZ = f32Cos(-angleRad.z);

    result.m0 = cosZ * cosY;
    result.m1 = (cosZ * sinY * sinX) - (sinZ * cosX);
    result.m2 = (cosZ * sinY * cosX) + (sinZ * sinX);

    result.m4 = sinZ * cosY;
    result.m5 = (sinZ * sinY * sinX) + (cosZ * cosX);
    result.m6 = (sinZ * sinY * cosX) - (cosZ * sinX);

    result.m8 = -sinY;
    result.m9 = cosY * sinX;
    result.m10 = cosY * cosX;

    return result;
}

Mat4F32 mat4F32RotateZyx(Vec3F32 angleRad) {
    Mat4F32 result = mat4F32Identity();

    F32 sinX = sinf(angleRad.x);
    F32 cosX = cosf(angleRad.x);
    F32 sinY = sinf(angleRad.y);
    F32 cosY = cosf(angleRad.y);
    F32 sinZ = sinf(angleRad.z);
    F32 cosZ = cosf(angleRad.z);

    result.m0 = cosZ * cosY;
    result.m4 = (cosZ * sinY * sinX) - (cosX * sinZ);
    result.m8 = (sinZ * sinX) + (cosZ * cosX * sinY);

    result.m1 = cosY * sinZ;
    result.m5 = (cosZ * cosX) + (sinZ * sinY * sinX);
    result.m9 = (cosX * sinZ * sinY) - (cosZ * sinX);

    result.m2 = -sinY;
    result.m6 = cosY * sinX;
    result.m10 = cosY * cosX;

    return result;
}

Vec3F32 mat4F32Left(Mat4F32 m) {
    return Vec3Normalize((Vec3F32) { -m.m0, -m.m4, -m.m8 });
}

Vec3F32 mat4F32Right(Mat4F32 m) {
    return Vec3Normalize((Vec3F32) { m.m0, m.m4, m.m8 });
}

Vec3F32 mat4F32Down(Mat4F32 m) {
    return Vec3Normalize((Vec3F32) { -m.m1, -m.m5, -m.m9 });
}

Vec3F32 mat4F32Up(Mat4F32 m) {
    return Vec3Normalize((Vec3F32) { m.m1, m.m5, m.m9 });
}

Vec3F32 mat4F32Forward(Mat4F32 m) {
    return Vec3Normalize((Vec3F32) { -m.m2, -m.m6, -m.m10 });
}

Vec3F32 mat4F32Backward(Mat4F32 m) {
    return Vec3Normalize((Vec3F32) { m.m2, m.m6, m.m10 });
}

Mat4F32 mat4F32Frustum(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far) {
    Mat4F32 result = {};

    F32 rlDiff = right - left;
    F32 tbDiff = top - bottom;
    F32 fnDiff = far - near;

    result.m0 = (near * 2.0f) / rlDiff;
    result.m5 = (near * 2.0f) / tbDiff;
    result.m8 = (right + left) / rlDiff;
    result.m9 = (top + bottom) / tbDiff;
    result.m10 = -(far + near) / fnDiff;
    result.m11 = -1.0f;
    result.m14 = -(far * near * 2.0f) / fnDiff;

    return result;
}

Mat4F32 mat4F32Perspective(F32 fovRad, F32 aspect, F32 near, F32 far) {
    Mat4F32 result;

    F32 top = near * f32Tan(fovRad * 0.5);
    F32 bottom = -top;
    F32 right = top * aspect;
    F32 left = -right;

    result = mat4F32Frustum(left, right, bottom, top, near, far);
    return result;
}

Mat4F32 mat4F32Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far) {
    Mat4F32 result = {};

    F32 rlDiff = (right - left);
    F32 tbDiff = (top - bottom);
    F32 fnDiff = (far - near);

    result.m0 = 2.0f / rlDiff;
    result.m5 = 2.0f / tbDiff;
    result.m10 = -2.0f / fnDiff;
    result.m12 = -(left + right) / rlDiff;
    result.m13 = -(top + bottom) / tbDiff;
    result.m14 = -(far + near) / fnDiff;
    result.m15 = 1.0f;

    return result;
}

Mat4F32 mat4F32LookAt(Vec3F32 position, Vec3F32 target, Vec3F32 Up) {
    Mat4F32 result;

    Vec3F32 vz = Vec3Normalize(vec3F32Sub(position, target));
    Vec3F32 vx = Vec3Normalize(vec3F32Cross(Up, vz));
    Vec3F32 vy = vec3F32Cross(vz, vx);

    result.m0 = vx.x;
    result.m1 = vy.x;
    result.m2 = vz.x;
    result.m3 = 0.0f;
    result.m4 = vx.y;
    result.m5 = vy.y;
    result.m6 = vz.y;
    result.m7 = 0.0f;
    result.m8 = vx.z;
    result.m9 = vy.z;
    result.m10 = vz.z;
    result.m11 = 0.0f;
    result.m12 = -vec3F32Dot(vx, position);
    result.m13 = -vec3F32Dot(vy, position);
    result.m14 = -vec3F32Dot(vz, position);
    result.m15 = 1.0f;

    return result;
}

// BRIEF: Quaternion

QuatF32 quatF32Identity(void) {
    return (QuatF32) { 0.0f, 0.0f, 0.0f, 1.0f };
}

QuatF32 quatF32Add(QuatF32 a, QuatF32 b) {
    QuatF32 result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return result;
}

QuatF32 quatF32AddScalar(QuatF32 a, F32 scalar) {
    QuatF32 result = { a.x + scalar, a.y + scalar, a.z + scalar, a.w + scalar };
    return result;
}

QuatF32 quatF32Sub(QuatF32 a, QuatF32 b) {
    QuatF32 result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    return result;
}

QuatF32 quatF32SubScalar(QuatF32 q, F32 scalar) {
    QuatF32 result = { q.x - scalar, q.y - scalar, q.z - scalar, q.w - scalar };
    return result;
}

QuatF32 quatF32Mul(QuatF32 a, QuatF32 b) {
    QuatF32 result;

    result.x = a.x * b.w + a.w * b.x + a.y * b.z - a.z * b.y;
    result.y = a.y * b.w + a.w * b.y + a.z * b.x - a.x * b.z;
    result.z = a.z * b.w + a.w * b.z + a.x * b.y - a.y * b.x;
    result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

    return result;
}

QuatF32 quatF32MulScalar(QuatF32 q, F32 scalar) {
    QuatF32 result = { q.x * scalar, q.y * scalar, q.z * scalar, q.w * scalar };
    return result;
}

QuatF32 quatF32Div(QuatF32 a, QuatF32 b) {
    QuatF32 result = { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
    return result;
}

QuatF32 quatF32DivScalar(QuatF32 a, F32 scalar) {
    QuatF32 result = { a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar };
    return result;
}

F32 quatF32LengthSquared(QuatF32 q) {
    return squared(q.x) + squared(q.y) + squared(q.z) + squared(q.w);
}

F32 quatF32Length(QuatF32 q) {
    return f32Sqrt(quatF32LengthSquared(q));
}

QuatF32 quatF32Normalize(QuatF32 q) {
    QuatF32 result;

    F32 lengthSquared = quatF32LengthSquared(q);
    if (lengthSquared != 1.0f && lengthSquared != 0.0f) {
        F32 lengthInv = 1.0f / f32Sqrt(lengthSquared);

        result = quatF32MulScalar(q, lengthInv);
    } else {
        result = q;
    }
    return result;
}

QuatF32 quatF32Negate(QuatF32 q) {
    return (QuatF32) { -q.x, -q.y, -q.z, -q.w };
}

QuatF32 quatF32Conjugate(QuatF32 q) {
    return (QuatF32) { -q.x, -q.y, -q.z, q.w };
}

QuatF32 quatF32Inverse(QuatF32 q) {
    return quatF32Normalize(quatF32Conjugate(q));
}

QuatF32 quatF32Transform(QuatF32 q, Mat4F32 m) {
    QuatF32 result;

    result.x = m.m0 * q.x + m.m4 * q.y + m.m8 * q.z + m.m12 * q.w;
    result.y = m.m1 * q.x + m.m5 * q.y + m.m9 * q.z + m.m13 * q.w;
    result.z = m.m2 * q.x + m.m6 * q.y + m.m10 * q.z + m.m14 * q.w;
    result.w = m.m3 * q.x + m.m7 * q.y + m.m11 * q.z + m.m15 * q.w;

    return result;
}

B8 quatF32Equals(QuatF32 a, QuatF32 b) {
    return f32Equals(a.x, b.x) && f32Equals(a.y, b.y) && f32Equals(a.z, b.z) && f32Equals(a.w, b.w);
}

B8 quatF32Equivalent(QuatF32 a, QuatF32 b) {
    return quatF32Equals(quatF32Normalize(a), quatF32Normalize(b));
}

F32 quatF32Dot(QuatF32 a, QuatF32 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

QuatF32 quatF32Lerp(QuatF32 a, QuatF32 b, F32 t) {
    QuatF32 result = quatF32Add(a, quatF32MulScalar(quatF32Sub(b, a), t));
    return result;
}

QuatF32 quatF32Nlerp(QuatF32 a, QuatF32 b, F32 t) {
    QuatF32 result = quatF32Normalize(quatF32Lerp(a, b, t));
    return result;
}

QuatF32 quatF32Slerp(QuatF32 a, QuatF32 b, F32 t) {
    QuatF32 result;

    F32 cosHalfTheta = quatF32Dot(a, b);

    if (cosHalfTheta < 0) {
        b = quatF32Negate(b);
        cosHalfTheta = -cosHalfTheta;
    }

    if (f32Abs(cosHalfTheta) >= 1.0f) {
        result = a;
    } else if (cosHalfTheta > 0.95f) {
        result = quatF32Nlerp(a, b, t);
    } else {
        F32 halfTheta = f32Acos(cosHalfTheta);
        F32 sinHalfTheta = f32Sqrt(1.0f - squared(cosHalfTheta));

        if (f32Abs(sinHalfTheta) < 0.001f) {
            result = quatF32MulScalar(quatF32Add(a, b), 0.5f);
        } else {
            F32 ra = f32Sin((1 - t) * halfTheta) / sinHalfTheta;
            F32 rb = f32Sin(t * halfTheta) / sinHalfTheta;

            result = quatF32Add(quatF32MulScalar(a, ra), quatF32MulScalar(b, rb));
        }
    }

    return result;
}

QuatF32 quatF32FromMat4(Mat4F32 m) {
    QuatF32 result;

    F32 fourWSquaredMinus1 = m.m0 + m.m5 + m.m10;
    F32 fourXSquaredMinus1 = m.m0 - m.m5 - m.m10;
    F32 fourYSquaredMinus1 = m.m5 - m.m0 - m.m10;
    F32 fourZSquaredMinus1 = m.m10 - m.m0 - m.m5;

    I32 maxIndex = 0;
    F32 fourMaxSquaredMinus1 = fourWSquaredMinus1;

    if (fourXSquaredMinus1 > fourMaxSquaredMinus1) {
        fourMaxSquaredMinus1 = fourXSquaredMinus1;
        maxIndex = 1;
    }

    if (fourYSquaredMinus1 > fourMaxSquaredMinus1) {
        fourMaxSquaredMinus1 = fourYSquaredMinus1;
        maxIndex = 2;
    }

    if (fourZSquaredMinus1 > fourMaxSquaredMinus1) {
        fourMaxSquaredMinus1 = fourZSquaredMinus1;
        maxIndex = 3;
    }

    F32 maxValue = f32Sqrt(fourMaxSquaredMinus1 + 1.0f) * 0.5f;
    F32 mul = 0.25f / maxValue;

    switch (maxIndex) {
        case 0:
            result.w = maxValue;
            result.x = (m.m6 - m.m9) * mul;
            result.y = (m.m8 - m.m2) * mul;
            result.z = (m.m1 - m.m4) * mul;
            break;
        case 1:
            result.x = maxValue;
            result.w = (m.m6 - m.m9) * mul;
            result.y = (m.m1 + m.m4) * mul;
            result.z = (m.m8 + m.m2) * mul;
            break;
        case 2:
            result.y = maxValue;
            result.w = (m.m8 - m.m2) * mul;
            result.x = (m.m1 + m.m4) * mul;
            result.z = (m.m6 + m.m9) * mul;
            break;
        case 3:
            result.z = maxValue;
            result.w = (m.m1 - m.m4) * mul;
            result.x = (m.m8 + m.m2) * mul;
            result.y = (m.m6 + m.m9) * mul;
            break;
    }

    return result;
}

Mat4F32 quatF32ToMat4(QuatF32 q) {
    Mat4F32 result = mat4F32Identity();

    F32 xx = squared(q.x);
    F32 yy = squared(q.y);
    F32 zz = squared(q.z);
    F32 xz = q.x * q.z;
    F32 xy = q.x * q.y;
    F32 yz = q.y * q.z;
    F32 wx = q.w * q.x;
    F32 wy = q.w * q.y;
    F32 wz = q.w * q.z;

    result.m0 = 1 - 2 * (yy + zz);
    result.m1 = 2 * (xy + wz);
    result.m2 = 2 * (xz - wy);

    result.m4 = 2 * (xy - wz);
    result.m5 = 1 - 2 * (xx + zz);
    result.m6 = 2 * (yz + wx);

    result.m8 = 2 * (xz + wy);
    result.m9 = 2 * (yz - wx);
    result.m10 = 1 - 2 * (xx + yy);

    return result;
}

QuatF32 quatF32FromAxisAngle(Vec3F32 axis, F32 angleRad) {
    QuatF32 result = quatF32Identity();

    F32 lengthSquared = vec3F32LengthSquared(axis);

    if (lengthSquared != 0.0f) {
        angleRad *= 0.5f;

        if (lengthSquared != 1.0f) {
            axis = Vec3Normalize(axis);
        }

        F32 s = f32Sin(angleRad);
        F32 C = f32Cos(angleRad);

        result.x = axis.x * s;
        result.y = axis.y * s;
        result.z = axis.z * s;
        result.w = C;

        result = quatF32Normalize(result);
    }

    return result;
}

void quatF32ToAxisAngle(QuatF32 q, Vec3F32* out_pAxis, F32* out_pAngle) {
    if (f32Abs(q.w) > 1.0f) {
        q = quatF32Normalize(q);
    }

    Vec3F32 resultAxis = { 0.0f, 0.0f, 0.0f };
    F32 resultAngle = 2.0f * f32Acos(q.w);
    F32 den = f32Sqrt(1.0f - q.w * q.w);

    if (den > 0.0001f) {
        resultAxis.x = q.x / den;
        resultAxis.y = q.y / den;
        resultAxis.z = q.z / den;
    } else {
        // when the Angle is zero, set an arbitrary normalized axis
        resultAxis.x = 1.0f;
    }

    *out_pAxis = resultAxis;
    *out_pAngle = resultAngle;
}

// NOTE: rotation order is zyx
QuatF32 quatF32FromEuler(F32 pitch, F32 yaw, F32 roll) {
    QuatF32 result;

    F32 halfPitch = pitch * 0.5f;
    F32 halfYaw = yaw * 0.5f;
    F32 halfRoll = roll * 0.5f;

    F32 cosP = f32Cos(halfPitch);
    F32 sinP = f32Sin(halfPitch);
    F32 cosY = f32Cos(halfYaw);
    F32 sinY = f32Sin(halfYaw);
    F32 cosR = f32Cos(halfRoll);
    F32 sinR = f32Sin(halfRoll);

    result.x = sinP * cosY * cosR - cosP * sinY * sinR;
    result.y = cosP * sinY * cosR + sinP * cosY * sinR;
    result.z = cosP * cosY * sinR - sinP * sinY * cosR;
    result.w = cosP * cosY * cosR + sinP * sinY * sinR;

    return result;
}

Vec3F32 quatF32ToEuler(QuatF32 q) {
    Vec3F32 result;

    F32 x0 = 2.0f * (q.w * q.x + q.y * q.z);
    F32 x1 = 1.0f - 2.0f * (squared(q.x) + squared(q.y));

    F32 y0 = 2.0f * (q.w * q.y - q.z * q.x);
    y0 = y0 > 1.0f ? 1.0f : y0;
    y0 = y0 < -1.0f ? -1.0f : y0;

    F32 z0 = 2.0f * (q.w * q.z + q.x * q.y);
    F32 z1 = 1.0f - 2.0f * (squared(q.y) + squared(q.z));

    result.x = f32Atan2(x0, x1);
    result.y = f32Asin(y0);
    result.z = f32Atan2(z0, z1);

    return result;
}

// BRIEF: Colors

U32 u32FromColor3U32(Color3U32 c) {
    return ((c.r & 0x0FF) << 16) | ((c.g & 0x0FF) << 8) | (c.b & 0x0FF);
}

Color3U32 color3U32FromU32(U32 u) {
    Color3U32 result = (Color3U32) { (u >> 16) & 0x0FF, (u >> 8) & 0x0FF, (u) & 0x0FF };
    return result;
}

Color3F32 color3F32FromColor3U32(Color3U32 c) {
    Color3F32 result = (Color3F32) { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f };
    return result;
}

Color3U32 color3U32FromColor3F32(Color3F32 c) {
    Color3U32 result = (Color3U32) { c.r* 255, c.g* 255, c.b* 255 };
    return result;
}

ColorF32 colorF32FromColorU32(ColorU32 c) {
    ColorF32 result = (ColorF32) { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f };
    return result;
}

ColorU32 colorU32FromColorF32(ColorF32 c) {
    ColorU32 result = (ColorU32) { c.r* 255, c.g* 255, c.b* 255, c.a* 255 };
    return result;
}
