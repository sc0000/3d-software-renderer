// Copyright 2025 Sebastian Cyliax

#pragma once

#include "types.h"

mat4_t mat4_identity();

mat4_t mat4_make_scale(const vec3_t* s);
mat4_t mat4_make_rot_x(const float a);
mat4_t mat4_make_rot_y(const float a);
mat4_t mat4_make_rot_z(const float a);
mat4_t mat4_make_rotation(const rot3_t* r);
mat4_t mat4_make_translation(const vec3_t* t);

mat4_t mat4_look_at(const camera_t* camera);

mat4_t mat4_make_projection(float aspect_ratio, float fov, float z_near, float z_far);

vec4_t mat4_mul_vec4(const mat4_t* m, const vec4_t* v);
mat4_t mat4_mul_mat4(const mat4_t* a, const mat4_t* b);
mat4_t mat4_mul_mat4_multiple(const mat4_t* matrices);
