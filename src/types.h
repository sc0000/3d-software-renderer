// Copyright 2025 Sebastian Cyliax

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "defs.h"

enum plane_type {
  PLANE_RIGHT,
  PLANE_BOTTOM,
  PLANE_LEFT,
  PLANE_TOP,
  PLANE_BACK,
  PLANE_FRONT,
  PLANE_COUNT
};

enum directions {
  RIGHT,
  UP,
  FORWARD,
  DIRECTIONS_COUNT
};

typedef uint32_t color_t;

typedef struct vec2 {
	float x;
	float y;
} vec2_t;

typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3_t;

typedef vec3_t rot3_t;

typedef struct vec4 {
	float x;
	float y;
	float z;
	float w;
} vec4_t;

typedef struct mat4 {
	float m[4][4];
} mat4_t;

typedef struct uv {
	float u;
	float v;
} uv_t;

// vertex indices of the corresponding mesh
typedef struct face {
	size_t a;
	size_t b;
	size_t c;
	uv_t tex_coords[3]; // [0] = tex coords of a, etc.
	color_t color;
  int clipped_plane;
} face_t;

// triangle 2d projection
typedef struct tri2 {
	vec2_t vertices[3];
	uv_t tex_coords[3];
  float inv_depth[3];
	color_t color;
} tri2_t;

typedef struct tri4 {
  vec4_t vertices[3];
	uv_t tex_coords[3];
	float avg_z;
	color_t color;
} tri4_t;

typedef struct tex2 {
	color_t data[TEXTURE_SIZE * TEXTURE_SIZE];
	vec2_t size;
} tex2_t;

typedef struct mesh {
	vec3_t* vertices;
	face_t* faces;
	vec3_t scale;
	rot3_t rotation;
	vec3_t translation;
	tex2_t texture;
} mesh_t;

typedef struct plane {
  vec3_t location;
  vec3_t normal;
} plane_t;

typedef struct light {
	vec3_t direction;
} light_t;

typedef struct camera {
  vec3_t translation;
  rot3_t rotation;
  mat4_t mat_transform;
  vec3_t dir_vectors[3];
} camera_t;
