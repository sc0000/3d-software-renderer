// Copyright 2025 Sebastian Cyliax

#include <stdio.h>
#include <string.h>
#include "darray.h"
#include "matrix.h"
#include "mesh.h"

void mesh_parse_obj(mesh_t* mesh, const char* filepath)
{
  if (!mesh || !filepath) return;

  mesh_free(mesh);

  FILE* file = fopen(filepath, "r");
  if (file == NULL) {
    perror("Error opening file");
    return;
  }

  char line[256];
  vec3_t v = { .x = 0.f, .y = 0.f, .z = 0.f };
  face_t f = { 
    .a = 0, 
    .b = 0, 
    .c = 0, 
    .color = 0xFF000000, 
    .tex_coords = { { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f } }, 
    .clipped_plane = -1 
  };
  
  uv_t* tex_coords = NULL;

  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "v ", 2) == 0) {
      if (sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z) == 3)
        darray_push(mesh->vertices, v);
    }

    if (strncmp(line, "vt ", 3) == 0) {
      uv_t uv;

      if (sscanf(line + 3, "%f %f", &uv.u, &uv.v) == 2) {
        uv.v = 1.f - uv.v;
        darray_push(tex_coords, uv);
      }
    }

    if (strncmp(line, "f ", 2) == 0) {
      int tex_idx[3];

      if (sscanf(line + 2, "%zd/%d/%*zd %zd/%d/%*zd %zd/%d/%*zd", 
        &f.a, &tex_idx[0], &f.b, &tex_idx[1], &f.c, &tex_idx[2]) == 6) {
        f.a -= 1;
        f.b -= 1;
        f.c -= 1;

        for (size_t i = 0; i < 3; ++i)
          f.tex_coords[i] = tex_coords[tex_idx[i] - 1];
        
        darray_push(mesh->faces, f);
      }
    }
  }

  fclose(file);
}

void mesh_free(mesh_t* mesh)
{
  if (!mesh) return;

  darray_clear(mesh->faces);
  darray_clear(mesh->vertices);
}

void mesh_init_transform(mesh_t* mesh) 
{
  mesh->scale = (vec3_t) {1.f, 1.f, 1.f};
  mesh->rotation = (rot3_t) {0.f, 0.f, 0.f };
  mesh->translation = (vec3_t) {0.f, 0.f, 0.f };
}

mat4_t mesh_get_transform(const mesh_t *mesh) 
{
  mat4_t mat_transform = mat4_identity();

  mat4_t mat_scale = mat4_make_scale(&mesh->scale);
  mat4_t rot = mat4_make_rotation(&mesh->rotation);
  mat4_t mat_translation = mat4_make_translation(&mesh->translation);

  mat_transform = mat4_mul_mat4(&mat_scale, &mat_transform);
  mat_transform = mat4_mul_mat4(&rot, &mat_transform);
  mat_transform = mat4_mul_mat4(&mat_translation, &mat_transform);

  return mat_transform;
}
