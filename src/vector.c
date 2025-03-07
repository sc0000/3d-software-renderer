// Copyright 2025 Sebastian Cyliax

#include <math.h>
#include <stdio.h>
#include "defs.h"
#include "vector.h"

vec2_t vec2_add(const vec2_t* a, const vec2_t* b)
{
  if (!a || !b) return vec2_null;

  return (vec2_t) {
    .x = a->x + b->x,
    .y = a->y + b->y
  };
}

vec2_t vec2_sub(const vec2_t* a, const vec2_t* b)
{
  if (!a || !b) return vec2_null;

  return (vec2_t) {
    .x = a->x - b->x,
    .y = a->y - b->y
  };
}

void vec2_scale(vec2_t* v, const float f)
{
  if (!v) return;

  v->x *= f;
  v->y *= f;
}

float vec2_dot(const vec2_t* a, const vec2_t* b)
{
  if (!a || !b) return 0.f;

  return a->x * b->x + a->y * b->y;
}

float vec2_mag(vec2_t* v)
{
  if (!v) return 0.f;

  return sqrtf(v->x * v->x + v->y * v->y);
}

void vec2_normalize(vec2_t* v)
{
  if (!v) return;

  float mag = vec2_mag(v);
  v->x /= mag;
  v->y /= mag;
}

vec2_t vec2_from_vec4(const vec4_t* v)
{
  return (vec2_t) {
    .x = v->x,
    .y = v->y
  };
}

void vec2_swap(vec2_t* a, vec2_t* b)
{
  if (!a || !b) return;

  vec2_t t = *a;
  a->x = b->x;
  a->y = b->y;
  b->x = t.x;
  b->y = t.y; 
}

void vec2_center(vec2_t* v)
{
  if (!v) return;

  v->x += (float)WINDOW_WIDTH / 2.f;
  v->y += (float)WINDOW_HEIGHT / 2.f;
}

void vec2_print(vec2_t* v)
{
  if (!v) return;

  printf("x: %f, y: %f\n", v->x, v->y);
}

uv_t uv_scaled(const uv_t* uv, const float f)
{
  return (uv_t) {
    .u = uv->u * f,
    .v = uv->v * f
  };
}

uv_t uv_lerp(const uv_t* uv0, const uv_t* uv1, const float f)
{
  if (!uv0 || !uv1) 
    return (uv_t) { 0.f, 0.f };

  return (uv_t) {
    .u = uv0->u + (uv1->u - uv0->u) * f,
    .v = uv0->v + (uv1->v - uv0->v) * f
  };
}

vec3_t vec3_add(const vec3_t* a, const vec3_t* b)
{
  if (!a || !b) return vec3_null;

  return (vec3_t) {
    .x = a->x + b->x,
    .y = a->y + b->y,
    .z = a->z + b->z
  };
}

vec3_t vec3_sub(const vec3_t* a, const vec3_t* b)
{
  if (!a || !b) return vec3_null;

  return (vec3_t) {
    .x = a->x - b->x,
    .y = a->y - b->y,
    .z = a->z - b->z
  };
}

void vec3_scale(vec3_t* v, const float f)
{
  if (!v) return;

  v->x *= f;
  v->y *= f;
  v->z *= f;
}

vec3_t vec3_scaled(const vec3_t* v, float f)
{
  if (!v) return vec3_null;

  return (vec3_t) {
    .x = v->x * f,
    .y = v->y * f,
    .z = v->z * f
  };
}

float vec3_dot(const vec3_t* a, const vec3_t* b)
{
  if (!a || !b) return 0.f;

  return a->x * b->x + a->y * b->y + a->z * b->z;
}

vec3_t vec3_cross(const vec3_t* a, const vec3_t* b)
{
  if (!a || !b) return vec3_null;

  return (vec3_t) {
    .x = a->y * b->z - a->z * b->y,
    .y = a->z * b->x - a->x * b->z,
    .z = a->x * b->y - a->y * b->x
  };
}

float vec3_mag(vec3_t* v)
{
  if (!v) return 0.f;

  return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

void vec3_normalize(vec3_t* v)
{
  if (!v) return;

  float mag = vec3_mag(v);
  v->x /= mag;
  v->y /= mag;
  v->z /= mag;
}

vec3_t vec3_invert(const vec3_t* v)
{
  return (vec3_t) {
    .x = v->x * -1.f,
    .y = v->y * -1.f,
    .z = v->z * -1.f
  };
}

void vec3_rotate(vec3_t* v, const rot3_t* r)
{
  if (!v) return;

	vec3_rotate_x(v, r->x);
	vec3_rotate_y(v, r->y);
	vec3_rotate_z(v, r->z);
}

void vec3_rotate_x(vec3_t* v, const float a)
{
  if (!v) return;

  float tmp_y = v->y;
  v->y = v->y * cosf(a) - v->z * sinf(a);
  v->z = tmp_y * sinf(a) + v->z * cosf(a);
}

void vec3_rotate_y(vec3_t* v, const float a)
{
  if (!v) return;

  float tmp_x = v->x;
  v->x = v->x * cosf(a) + v->z * sinf(a);
  v->z = -tmp_x * sinf(a) + v->z * cosf(a);
}

void vec3_rotate_z(vec3_t* v, const float a)
{
  if (!v) return;

  float tmp_x = v->x;
  v->x = v->x * cosf(a) - v->y * sinf(a);
  v->y = tmp_x * sinf(a) + v->y * cosf(a);
}

vec3_t vec3_rotate_rodrigues(const vec3_t* v, const vec3_t* k, float a)
{
  float cos_theta = cosf(a);
  float sin_theta = sinf(a);

  vec3_t v_k_cross = vec3_cross(v, k);

  vec3_t term1 = vec3_scaled(v, cos_theta);
  vec3_t term2 = vec3_scaled(&v_k_cross, sin_theta);
  vec3_t term3 = vec3_scaled(k, vec3_dot(k, v) * (1.f - cos_theta));

  vec3_t v_rot = vec3_add(&term1, &term2);
  v_rot = vec3_add(&v_rot, &term3);

  return v_rot;
}

vec3_t vec3_from_vec4(const vec4_t* v)
{
  if (!v) return vec3_null;

  return (vec3_t) {
    .x = v->x,
    .y = v->y,
    .z = v->z
  };
}

vec4_t vec3_to_vec4(const vec3_t* v)
{
  if (!v) return vec4_null;

  return (vec4_t) {
    .x = v->x,
    .y = v->y,
    .z = v->z,
    .w = 1.f
  };
}

void vec3_print(const vec3_t* v)
{
  if (!v) return;

  printf("x: %f, y: %f, z: %f\n", v->x, v->y, v->z);
}

vec4_t vec4_from_vec3(const vec3_t* v)
{
  return vec3_to_vec4(v);
}

vec3_t vec4_to_vec3(const vec4_t* v)
{
  return vec3_from_vec4(v);
}

void vec4_apply_mat4(vec4_t* v, const mat4_t* m)
{
  if (!v || !m) return;

  vec4_t tmp = *v;

  *v = (vec4_t) {
    .x = m->m[0][0] * tmp.x + m->m[0][1] * tmp.y + m->m[0][2] * tmp.z + m->m[0][3] * tmp.w,
    .y = m->m[1][0] * tmp.x + m->m[1][1] * tmp.y + m->m[1][2] * tmp.z + m->m[1][3] * tmp.w,
    .z = m->m[2][0] * tmp.x + m->m[2][1] * tmp.y + m->m[2][2] * tmp.z + m->m[2][3] * tmp.w,
    .w = m->m[3][0] * tmp.x + m->m[3][1] * tmp.y + m->m[3][2] * tmp.z + m->m[3][3] * tmp.w
  };
}

vec4_t vec4_add(const vec4_t* a, const vec4_t* b)
{
  if (!a || !b) return vec4_null;

  return (vec4_t) {
    .x = a->x + b->x,
      .y = a->y + b->y,
      .z = a->z + b->z,
      .w = 1.f
  };
}

vec4_t vec4_sub(const vec4_t* a, const vec4_t* b)
{
  if (!a || !b) return vec4_null;

  return (vec4_t) {
      .x = a->x - b->x,
      .y = a->y - b->y,
      .z = a->z - b->z,
      .w = 1.f
  };
}

void vec4_scale(vec4_t* v, const float f)
{
  if (!v) return;

  v->x *= f;
  v->y *= f;
  v->z *= f;
}

float vec4_dot(const vec4_t* a, const vec4_t* b)
{
  if (!a || !b) return 0.f;

  return a->x * b->x + a->y * b->y + a->z * b->z;
}

vec4_t vec4_cross(const vec4_t* a, const vec4_t* b)
{
  if (!a || !b) return vec4_null;

  return (vec4_t) {
    .x = a->y * b->z - a->z * b->y,
    .y = a->z * b->x - a->x * b->z,
    .z = a->x * b->y - a->y * b->x,
    .w = 1.f
  };
}

float vec4_mag(vec4_t* v)
{
  if (!v) return 0.f;

  return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

void vec4_normalize(vec4_t* v)
{
  if (!v) return;

  float mag = vec4_mag(v);
  v->x /= mag;
  v->y /= mag;
  v->z /= mag;
}

void vec4_rotate(vec4_t* v, const rot3_t* r)
{
  if (!v || !r) return;
}

void vec4_rotate_x(vec4_t* v, float a)
{
  if (!v) return;

  float tmp_y = v->y;
  v->y = v->y * cosf(a) - v->z * sinf(a);
  v->z = tmp_y * sinf(a) + v->z * cosf(a);
}

void vec4_rotate_y(vec4_t* v, float a)
{
  if (!v) return;

  float tmp_x = v->x;
  v->x = v->x * cosf(a) + v->z * sinf(a);
  v->z = -tmp_x * sinf(a) + v->z * cosf(a);
}

void vec4_rotate_z(vec4_t* v, float a)
{
  if (!v) return;

  float tmp_x = v->x;
  v->x = v->x * cosf(a) - v->y * sinf(a);
  v->y = tmp_x * sinf(a) + v->y * cosf(a);
}

const vec2_t vec2_null = { 0.f, 0.f };
const vec3_t vec3_null = { 0.f, 0.f, 0.f };
const vec4_t vec4_null = { 0.f, 0.f, 0.f, 1.f };

void vec4_print(const vec4_t* v)
{
  if (!v) return;

  printf("x: %f, y: %f, z: %f, w: %f\n", v->x, v->y, v->z, v->w);
}
