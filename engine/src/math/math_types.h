#pragma once

#include "base/base.h"

typedef union Vec2F32 {
    F32 data[2];
    struct {
        union {
            F32 x, r, s, u;
        };
        union {
            F32 y, g, t, v;
        };
    };
} Vec2F32;

typedef union Vec3F32 {
    F32 data[3];
    struct {
        union {
            F32 x, r, s, u;
        };
        union {
            F32 y, g, t, v;
        };
        union {
            F32 z, b, p, w;
        };
    };
} Vec3F32;

typedef union Vec4F32 {
    F32 data[4];
    struct {
        union {
            F32 x, r, s;
        };
        union {
            F32 y, g, t;
        };
        union {
            F32 z, b, p;
        };
        union {
            F32 w, a, q;
        };
    };
} Vec4F32;

typedef union Vec2I32 {
    I32 data[2];
    struct {
        I32 x, y;
    };
} Vec2I32;

typedef union Vec3I32 {
    I32 data[3];
    struct {
        I32 x, y, z;
    };
} Vec3I32;

typedef union Vec4I32 {
    I32 data[4];
    struct {
        I32 x, y, z, w;
    };
} Vec4I32;

typedef union Vec2U32 {
    U32 data[2];
    struct {
        U32 x, y;
    };
} Vec2U32;

typedef union Vec3U32 {
    U32 data[3];
    struct {
        union {
            U32 x, r;
        };
        union {
            U32 y, g;
        };
        union {
            U32 z, b;
        };
    };
} Vec3U32;

typedef union Vec4U32 {
    U32 data[4];
    struct {
        union {
            U32 x, r;
        };
        union {
            U32 y, g;
        };
        union {
            U32 z, b;
        };
        union {
            U32 w, a;
        };
    };
} Vec4U32;

typedef Vec3F32 Color3F32;
typedef Vec4F32 ColorF32;
typedef Vec3U32 Color3U32;
typedef Vec4U32 ColorU32;
typedef Vec4F32 QuatF32;

// NOTE: right handed, column major
typedef union Mat4F32 {
    F32 data[16];
    Vec4F32 cols[4];
    struct {
        Vec4F32 col0, col1, col2, col3;
    };
    struct {
        F32 m0, m4, m8, m12;
        F32 m1, m5, m9, m13;
        F32 m2, m6, m10, m14;
        F32 m3, m7, m11, m15;
    };
} Mat4F32;

typedef union Extents1DF32 {
    F32 data[2];
    F32 coords[2];
    struct {
        union {
            F32 min, start;
        };
        union {
            F32 max, end;
        };
    };
} Extents1DF32;

typedef union Extents2DF32 {
    F32 data[4];
    F32 coords[4];
    Vec2F32 points[2];
    struct {
        union {
            Vec2F32 min, start;
        };
        union {
            Vec2F32 max, end;
        };
    };
    struct {
        union {
            F32 x0, xMin, xStart;
        };
        union {
            F32 y0, yMin, yStart;
        };
        union {
            F32 x1, xMax, xEnd;
        };
        union {
            F32 y1, yMax, yEnd;
        };
    };
} Extents2DF32;

typedef union Extents3DF32 {
    F32 data[6];
    F32 coords[6];
    Vec3F32 points[2];
    struct {
        union {
            Vec3F32 min, start;
        };
        union {
            Vec3F32 max, end;
        };
    };
    struct {
        union {
            F32 x0, xMin, xStart;
        };
        union {
            F32 y0, yMin, yStart;
        };
        union {
            F32 z0, zMin, zStart;
        };
        union {
            F32 x1, xMax, xEnd;
        };
        union {
            F32 y1, yMax, yEnd;
        };
        union {
            F32 z1, zMax, zEnd;
        };
    };
} Extents3DF32;

typedef Extents1DF32 IntervalF32;
typedef Extents2DF32 RectF32;
typedef Extents2DF32 AABBF32;
typedef Extents2DF32 Segment2DF32;
typedef Extents3DF32 Box3DF32;
typedef Extents3DF32 Segment3DF32;
