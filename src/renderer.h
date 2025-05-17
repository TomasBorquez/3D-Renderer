#pragma once
#include "base.h"

typedef struct {
  f64 x;
  f64 y;
} Vec2;

typedef struct {
  i32 x;
  i32 y;
} VecI2;

typedef struct {
  f64 x;
  f64 y;
  f64 z;
} Vec3;

typedef struct {
  i32 x;
  i32 y;
  i32 z;
} VecI3;

typedef struct {
  f32 a[3 * 3];
} Mat3;

typedef struct {
  f32 a[4 * 4];
} Mat4;

VEC_TYPE(VectorVec2, Vec2);
VEC_TYPE(VectorVec3, Vec3);

VEC_TYPE(VectorVecI2, VecI2);
VEC_TYPE(VectorVecI3, VecI3);

VEC_TYPE(VectorI8, i8);
VEC_TYPE(VectorI32, i32);

// TODO: Change to u8
typedef struct {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
} Color;

#define BLACK (Color){10, 10, 10, 255}
#define WHITE (Color){240, 240, 240, 255}
#define GREEN (Color){0, 255, 0, 255}
#define RED (Color){255, 0, 0, 255}
#define BLUE (Color){64, 128, 255, 255}
#define YELLOW (Color){255, 200, 0, 255}

Mat3 Mat3Create();
u16 Mat3Get(u16 x, u16 y);
void Mat3Set(Mat3 *mat, Mat3 o);
Mat3 Mat3Mul(Mat3 a, Mat3 b);
Mat3 Mat3Translate(Vec2 v);

Mat4 Mat4Create();
u16 Mat4Get(u16 x, u16 y);
Mat4 Mat4Mul(Mat4 a, Mat4 b);
Mat4 Mat4Translate(Vec3 v);
Mat4 Mat4Scale(Vec3 v);
Mat4 Mat4Transpose(Mat4 a);
// Mat4 Mat4Ortho(f32 left, f32 right, f32 top, f32 bottom, f32 near, f32 far);
// Mat4 Mat4Perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far);
// Mat4 Mat4RotX(f32 deg);
// Mat4 Mat4RotY(f32 deg);
// Mat4 Mat4RotZ(f32 deg);

Vec3 Vec3Add(Vec3 a, Vec3 b);
Vec3 Vec3Sub(Vec3 a, Vec3 b);
Vec3 Vec3Mul(Vec3 a, Vec3 b);
Vec3 Vec3Scale(Vec3 a, f32 s);
Vec3 Vec3Cross(Vec3 a, Vec3 b);
f64 Vec3Dot(Vec3 a, Vec3 b);
Vec3 Vec3Normalize(Vec3 v);
f64 Vec3Norm(Vec3 v);

Vec3 M2V(Mat4 m);
Mat4 V2M(Vec3 v);
