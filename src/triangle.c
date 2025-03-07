// Copyright 2025 Sebastian Cyliax

#include <stdio.h>
#include <float.h>
#include "defs.h"
#include "triangle.h"

// TODO: Move drawing functions into own files
#include "graphics.h"

void face_print(face_t* face)
{
	if (!face) return;
	printf("a: %zd, b: %zd, c: %zd\n", face->a, face->b, face->c);
}

void face_fix_obj_indices(face_t* face)
{
  if (!face) return;
  face->a -= 1;
  face->b -= 1;
  face->c -= 1;
}

tri2_t tri2_from_tri4(const tri4_t* t)
{
  return (tri2_t) {
    .vertices = { 
      {t->vertices[0].x, t->vertices[0].y},
      {t->vertices[1].x, t->vertices[1].y},
      {t->vertices[2].x, t->vertices[2].y} 
    },

    .color = t->color,
    .tex_coords = {
      {t->tex_coords[0].u, t->tex_coords[0].v},
      {t->tex_coords[1].u, t->tex_coords[1].v},
      {t->tex_coords[2].u, t->tex_coords[2].v}
    }
  };
}

void tri2_fill(const tri2_t* t, color_t color)
{
  const tri2_t sorted = tri2_sort_y(t);
  const vec2_t a = sorted.vertices[0];
  const vec2_t b = sorted.vertices[1];
  const vec2_t c = sorted.vertices[2];

  const vec2_t m = {
    .x = (b.y - a.y) * (c.x - a.x) / (c.y - a.y) + a.x,
    .y = b.y
  };

  tri2_t flat_bottom = {
    .vertices = { a, b, m }
  };

  tri2_t flat_top = {
    .vertices = { c, m, b }
  };
  
  tri2_flat_bottom(&flat_bottom, color);

  if (c.y != m.y)
    tri2_flat_top(&flat_top, color);
}

void tri2_flat_bottom(tri2_t* t, color_t color)
{
  const vec2_t a = t->vertices[0];
  const vec2_t b = t->vertices[1];
  const vec2_t c = t->vertices[2];

  for (int y = (int)a.y; y <= (int)b.y; ++y) {
    if (b.y - a.y == 0.f) continue;

    const float alpha = (y - a.y) / (b.y - a.y);
    const int x0 = (int)(a.x + alpha * (b.x - a.x));
    const int x1 = (int)(a.x + alpha * (c.x - a.x));

    draw_line_bresenham(x0, y, x1, y, color);
  }
}

void tri2_flat_top(tri2_t* t, color_t color)
{
  const vec2_t a = t->vertices[0];
  const vec2_t b = t->vertices[1];
  const vec2_t c = t->vertices[2];

  for (int y = (int)a.y; y >= (int)b.y; --y) {
    if (b.y - a.y == 0.f) continue;

    const float alpha = (y - a.y) / (b.y - a.y);

    const int x0 = (int)(a.x + alpha * (b.x - a.x));
    const int x1 = (int)(a.x + alpha * (c.x - a.x));

    draw_line_bresenham(x0, y, x1, y, color);
  }
}

void tri2_draw_texture(const tri2_t* t, const tex2_t* texture)
{
  if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX)
    return;

  const tri2_t sorted = tri2_sort_y(t);
  const vec2_t a = sorted.vertices[0];
  const vec2_t b = sorted.vertices[1];
  const vec2_t c = sorted.vertices[2];

  float alpha0 =  b.y - a.y != 0.f ? (b.x - a.x) / fabsf(b.y - a.y) : 0.f;
  float alpha1 =  c.y - a.y != 0.f ? (c.x - a.x) / fabsf(c.y - a.y) : 0.f;

  if (b.y - a.y != 0.f) {
    for (int y = (int)a.y; y < (int)b.y; ++y) {
      int x0 = (int)(b.x + alpha0 * (y - b.y));
      int x1 = (int)(a.x + alpha1 * (y - a.y));

      if (x1 < x0)
        SWAP(int, &x0, &x1);

      for (int x = x0; x < x1; ++x)
        draw_texel(x, y, texture, t);
        // draw_pixel(x, y, x % 2 == 0 && y % 2 == 0 ? 0xFF000000 : 0xFFFFFFFF);
    }
  }

  alpha0 = c.y - b.y != 0.f ? (c.x - b.x) / fabsf(c.y - b.y) : 0.f;
  alpha1 = c.y - a.y != 0.f ? (c.x - a.x) / fabsf(c.y - a.y) : 0.f;

  if (c.y - b.y != 0.f) {
    for (int y = (int)b.y; y <= (int)c.y; ++y) {
      int x0 = (int)(b.x + alpha0 * (y - b.y));
      int x1 = (int)(a.x + alpha1 * (y - a.y));

      if (x1 < x0)
        SWAP(int, &x0, &x1);

      for (int x = x0; x < x1; ++x)
        draw_texel(x, y, texture, t);
        // draw_pixel(x, y, x % 2 == 0 && y % 2 == 0 ? 0xFF000000 : 0xFFFFFFFF);
    }
  }
}

tri2_t tri2_sort_y(const tri2_t* t)
{
  vec2_t a = t->vertices[0];
  vec2_t b = t->vertices[1];
  vec2_t c = t->vertices[2];

  if (a.y > b.y) vec2_swap(&a, &b);
  if (b.y > c.y) vec2_swap(&b, &c);
  if (a.y > b.y) vec2_swap(&a, &b);

  return (tri2_t) {
    .vertices = { a, b, c }
  };
}

void tri2_sort(tri2_t* t)
{
  vec2_t a = t->vertices[0];
  vec2_t b = t->vertices[1];
  vec2_t c = t->vertices[2];

  if (a.y > b.y) vec2_swap(&a, &b);
  if (b.y > c.y) vec2_swap(&b, &c);
  if (a.y > b.y) vec2_swap(&a, &b);

  t->vertices[0] = a;
  t->vertices[1] = b;
  t->vertices[2] = c;
}

void tri2_swap(tri2_t* a, tri2_t* b)
{
  const tri2_t tmp = *a;
  *a = *b;
  *b = tmp;
}

void tri2_round(tri2_t* t)
{
  if (!t) return;

  t->vertices[0].x = roundf(t->vertices[0].x);
  t->vertices[0].y = roundf(t->vertices[0].y);
  t->vertices[1].x = roundf(t->vertices[1].x);
  t->vertices[1].y = roundf(t->vertices[1].y);
  t->vertices[2].x = roundf(t->vertices[2].x);
  t->vertices[2].y = roundf(t->vertices[2].y);
}

vec3_t tri2_barycentric_weights(const tri2_t* t, vec2_t p)
{
  const float even_split = 1.f / 3.f;
  if (!t) return (vec3_t) { even_split, even_split, even_split };

  const vec2_t* vs = t->vertices;

  const vec2_t ab = vec2_sub(&vs[1],&vs[0]);
  const vec2_t ac = vec2_sub(&vs[2],&vs[0]);
  const vec2_t pa = vec2_sub(&vs[0],&p);
  const vec2_t pb = vec2_sub(&vs[1],&p);
  const vec2_t pc = vec2_sub(&vs[2],&p);

  const float tri_area_doubled = fabsf(ac.x * ab.y - ac.y * ab.x);

  const float alpha = fabsf(pc.x * pb.y - pc.y * pb.x) / tri_area_doubled;
  const float beta  = fabsf(pa.x * pc.y - pa.y * pc.x) / tri_area_doubled;
  const float gamma = fabsf(pb.x * pa.y - pb.y * pa.x) / tri_area_doubled; // = 1.f - alpha - beta;

  return (vec3_t) {
    .x = alpha,
    .y = beta,
    .z = gamma
  };
}

const tri2_t tri2_null = {
  .vertices = { 
    { .x = 0.f, .y = 0.f },
    { .x = 0.f, .y = 0.f },
    { .x = 0.f, .y = 0.f }
  },

  .tex_coords = {
    { .u = 0.f, .v = 0.f },
    { .u = 0.f, .v = 0.f },
    { .u = 0.f, .v = 0.f }
  },

  .color = 0x00000000,
  .inv_depth = 1.f / FLT_MAX
};
