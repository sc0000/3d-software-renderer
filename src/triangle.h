// Copyright 2025 Sebastian Cyliax

#pragma once

#include "types.h"

void    face_print(face_t* face);
void    face_fix_obj_indices(face_t* face);

tri2_t  tri2_from_tri4(const tri4_t* t);
void    tri2_fill(const tri2_t* t, color_t color);
void    tri2_flat_bottom(tri2_t* t, color_t color);
void    tri2_flat_top(tri2_t* t, color_t color);
void    tri2_draw_texture(const tri2_t* t, const tex2_t* texture);
tri2_t  tri2_sort_y(const tri2_t* t);
void    tri2_sort(tri2_t* t);
void    tri2_swap(tri2_t* a, tri2_t* b);
void    tri2_round(tri2_t* t);
vec3_t  tri2_barycentric_weights(const tri2_t* t, vec2_t p);

extern const tri2_t tri2_null;

void    tri4_fill(const tri4_t* t, color_t color);
void    tri4_flat_bottom(tri4_t* t, color_t color);
void    tri4_flat_top(tri4_t* t, color_t color);
void    tri4_draw_texture(const tri4_t* t, const tex2_t* texture);
tri2_t  tri4_sort_y(const tri4_t* t);
void    tri4_swap(tri4_t* a, tri4_t* b);
void    tri4_round(tri4_t* t);
vec3_t  tri4_barycentric_weights(const tri4_t* t, vec2_t p);
