// Copyright 2025 Sebastian Cyliax

#include <stdio.h>

#include <SDL.h>

#include "camera.h"
#include "defs.h"
#include "graphics.h"
#include "vector.h"
#include "matrix.h"

mat4_t camera_get_transform(const camera_t* camera)
{
  if (!camera) return mat4_identity();

  mat4_t rot = mat4_make_rotation(&camera->rotation);
	mat4_t mat_translation = mat4_make_translation(&camera->translation);
  mat4_t mat_transform = mat4_mul_mat4(&mat_translation, &rot);

	return mat_transform;
}

void camera_update_dir_vectors(camera_t* camera)
{
  if (!camera) return;

  const mat4_t t = camera_get_transform(camera);

  camera->dir_vectors[RIGHT] = (vec3_t) { t.m[0][0], t.m[0][1], t.m[0][2] };
  camera->dir_vectors[UP] = (vec3_t) { t.m[1][0], t.m[1][1], t.m[1][2] };
  camera->dir_vectors[FORWARD] = (vec3_t) { t.m[2][0], t.m[2][1], t.m[2][2] };
}

void move_camera(camera_t* camera, const vec3_t* direction, const float delta_time)
{
  mat4_t t = camera_get_transform(camera);

  vec3_t cam_right = (vec3_t) { t.m[0][0], t.m[0][1], t.m[0][2] };
  vec3_scale(&cam_right, direction->x * 10.f * delta_time);

  vec3_t cam_up = (vec3_t) { t.m[1][0], t.m[1][1], t.m[1][2] };
  vec3_scale(&cam_up, direction->y * 10.f * delta_time);

  vec3_t cam_forward = (vec3_t) { t.m[2][0], t.m[2][1], t.m[2][2] };
  vec3_scale(&cam_forward, direction->z * 10.f * delta_time);

  vec3_t translation_input = vec3_add(&cam_right, &cam_up);
  translation_input = vec3_add(&translation_input, &cam_forward);
  camera->translation = vec3_add(&camera->translation, &translation_input);

  update_frustum_planes();
}

void rotate_camera(camera_t* camera, const SDL_MouseMotionEvent* motion, const float delta_time)
{
  if (!motion) return;

  camera->rotation.y += (motion->xrel * delta_time * MOUSE_SENSITIVITY);
  camera->rotation.x += (motion->yrel * delta_time * MOUSE_SENSITIVITY);

  if (camera->rotation.x < (float)(-M_PI / 2 + 0.05)) {
      camera->rotation.x = (float)(-M_PI / 2 + 0.05);
  }

  if (camera->rotation.x > (float)(M_PI / 2 - 0.05)) {
      camera->rotation.x = (float)(M_PI / 2 - 0.05);
  }

  // printf("x: %f y: %f z: %f\n", camera->rotation.x, camera->rotation.y, camera->rotation.z);

  update_frustum_planes();
}
