// Copyright 2025 Sebastian Cyliax

#pragma once

#include "types.h"

vec2_t	vec2_add(const vec2_t* a, const vec2_t* b);
vec2_t	vec2_sub(const vec2_t* a, const vec2_t* b);
void	  vec2_scale(vec2_t* v, float f);
float	  vec2_dot(const vec2_t* a, const vec2_t* b);
float	  vec2_mag(vec2_t* v);
void	  vec2_normalize(vec2_t* v);
vec2_t  vec2_from_vec4(const vec4_t* v);

void	  vec2_swap(vec2_t* a, vec2_t* b);
void	  vec2_center(vec2_t* v);

void	  vec2_print(vec2_t* v);

uv_t    uv_scaled(const uv_t* uv, float f);
uv_t    uv_lerp(const uv_t* uv0, const uv_t* uv1, const float f);

vec3_t	vec3_add(const vec3_t* a, const vec3_t* b);
vec3_t	vec3_sub(const vec3_t* a, const vec3_t* b);
void	  vec3_scale(vec3_t* v, float f);
vec3_t  vec3_scaled(const vec3_t* v, float f);
float	  vec3_dot(const vec3_t* a, const vec3_t* b);
vec3_t	vec3_cross(const vec3_t* a, const vec3_t* b);
float	  vec3_mag(vec3_t* v);
void	  vec3_normalize(vec3_t* v);
vec3_t  vec3_invert(const vec3_t* v);

void	  vec3_rotate(vec3_t* v, const rot3_t* r);
void	  vec3_rotate_x(vec3_t* v, float a);
void	  vec3_rotate_y(vec3_t* v, float a);
void	  vec3_rotate_z(vec3_t* v, float a);

vec3_t  vec3_rotate_rodrigues(const vec3_t* v, const vec3_t* k, float a);

vec3_t	vec3_from_vec4(const vec4_t* v);
vec4_t	vec3_to_vec4(const vec3_t* v);

void	  vec3_print(const vec3_t* v);

vec4_t	vec4_from_vec3(const vec3_t* v);
vec3_t	vec4_to_vec3(const vec4_t* v);
void    vec4_apply_mat4(vec4_t* v, const mat4_t* m);

vec4_t	vec4_add(const vec4_t* a, const vec4_t* b);
vec4_t	vec4_sub(const vec4_t* a, const vec4_t* b);
void	  vec4_scale(vec4_t* v, float f);
float   vec4_dot(const vec4_t* a, const vec4_t* b);
vec4_t	vec4_cross(const vec4_t* a, const vec4_t* b);
float	  vec4_mag(vec4_t* v);
void	  vec4_normalize(vec4_t* v);

void	  vec4_rotate(vec4_t* v, const rot3_t* r);
void	  vec4_rotate_x(vec4_t* v, float a);
void	  vec4_rotate_y(vec4_t* v, float a);
void	  vec4_rotate_z(vec4_t* v, float a);

extern const vec2_t vec2_null;
extern const vec3_t vec3_null;
extern const vec4_t vec4_null;
