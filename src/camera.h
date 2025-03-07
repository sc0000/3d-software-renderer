// Copyright 2025 Sebastian Cyliax

#pragma once

#include "matrix.h"
#include "vector.h"

mat4_t camera_get_transform(const camera_t* camera);
void camera_update_dir_vectors(camera_t* camera);
void move_camera(camera_t* camera, const vec3_t* direction, const float delta_time);
void rotate_camera(camera_t* camera, const SDL_MouseMotionEvent* motion, const float delta_time);
