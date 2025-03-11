// Copyright 2025 Sebastian Cyliax

#pragma once

#include <stdbool.h>

#include "defs.h"
#include "types.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"

enum cull_method {
  CULL_NONE,
  CULL_BACKFACE,
  CULL_DEFAULT_MAX
} extern cull_method;

enum render_method {
  RENDER_WIRE,
  RENDER_WIRE_VERTEX,
  RENDER_FILL_TRIANGLE,
  RENDER_FILL_TRIANGLE_WIRE,
  RENDER_TEXTURE,
  RENDER_TEXTURE_WIRE,
  RENDER_DEPTH_BUFFER,
  RENDER_DEFAULT_MAX
} extern render_method;

typedef uint32_t color_t;

extern camera_t* state_camera;
extern float* state_delta_time;

bool init_graphics(void);

// preliminary
bool init_meshes(void);

void update(void);
void render(void);
void destroy_window(void);
size_t screen_buffer_size(void);
void clear_color_buffer(color_t color);
void clear_depth_buffer();
void render_color_buffer(void);
void project_vertex(const vec4_t* vert_3d, vec2_t* vert_2d, float* inv_depth);
void project_mesh(mesh_t* mesh);
void sort_triangles(tri2_t* triangles);

void draw_pixel(const unsigned int x, const unsigned int y, color_t color);
void draw_texel(const unsigned int x, const unsigned int y, const tex2_t* tex, const tri2_t* tri);
void draw_line_dda(const unsigned int x0, const unsigned int y0, const unsigned int x1, const unsigned int y1, color_t color);
void draw_line_bresenham(unsigned int x0, unsigned int y0, const unsigned int x1, const unsigned int y1, color_t color);
void draw_triangle_vertices(const tri2_t* triangle, color_t color);
void draw_triangle_edges(const tri2_t* triangle, color_t color);
void draw_rect(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h, color_t color);

void draw_grid(void);

void draw_depth_buffer(void);

color_t random_color(void);

// separate files?
void update_frustum_planes(void);

void handle_single_outside_vertex(face_t* face, vec4_t* tri_verts, const size_t* vertex_indices, const size_t outside_idx, 
  const vec4_t* plane_location, const vec4_t* plane_normal, const int plane_idx, mesh_t* mesh);
void handle_two_outside_vertices(face_t* face, vec4_t* vertices, const size_t* vertex_indices, const size_t inside_idx, const plane_t* plane);

void move_outside_vertex_to_plane(vec4_t* outside_vert, const size_t outside_idx, const vec4_t* inside_vert, const float inside_dot, 
  const vec4_t* plane_location, const vec4_t* plane_normal, float* out_lerp_factor);
vec4_t create_vertex_at_plane(const vec4_t* outside_vert, const vec4_t* inside_vert, const float inside_dot, const vec4_t* plane_location, 
  const vec4_t* plane_normal, float* out_lerp_factor);
const float calc_lerp_factor(const vec4_t* outside_vert, const float inside_dot, const vec4_t* plane_location, const vec4_t* plane_normal);

const bool vertex_outside_plane(const vec4_t* vert, const vec4_t* plane_location, const vec4_t* plane_normal);
void clip_triangle_against_plane(vec4_t* verts, unsigned int* clip_mask, const vec4_t* plane_location, const vec4_t* plane_normal);

void clip_face_against_frustum_planes(face_t* face, vec4_t* transformed_vertices, const size_t* vertex_indices, 
  unsigned int* clip_mask, mesh_t* mesh);
