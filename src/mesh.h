// Copyright 2025 Sebastian Cyliax

#pragma once

#include "types.h"

void mesh_parse_obj(mesh_t* mesh, const char* filepath);
void mesh_init_transform(mesh_t* mesh);
mat4_t mesh_get_transform(const mesh_t* mesh);
void mesh_apply_transform(mesh_t* mesh);

void mesh_free(mesh_t* mesh);
