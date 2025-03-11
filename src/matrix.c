// Copyright 2025 Sebastian Cyliax

#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "vector.h"
#include "darray.h"
#include "camera.h"

// TODO: Set up a proper memory structure for all 4x4 matrices

mat4_t mat4_identity()
{
  mat4_t m = {{
    { 1.f, 0.f, 0.f, 0.f },
    { 0.f, 1.f, 0.f, 0.f },
    { 0.f, 0.f, 1.f, 0.f },
    { 0.f, 0.f, 0.f, 1.f }
  }};

  return m;
}

mat4_t mat4_make_scale(const vec3_t* s)
{
  // |  x  0  0  0  |
  // |  0  y  0  0  |
  // |  0  0  z  0  |
  // |  0  0  0  1  |

  mat4_t m = mat4_identity();

  m.m[0][0] = s->x;
  m.m[1][1] = s->y;
  m.m[2][2] = s->z;

  return m;
}

mat4_t mat4_make_rot_x(const float a)
{
  // |  1  0  0  0  |
  // |  0  c  s  0  |
  // |  0 -s  c  0  |
  // |  0  0  0  1  |

  mat4_t m = mat4_identity();

  m.m[1][1] =  cosf(a);
  m.m[1][2] =  sinf(a);
  m.m[2][1] = -sinf(a);
  m.m[2][2] =  cosf(a);

  return m;
}

mat4_t mat4_make_rot_y(const float a)
{
  // |  c  0 -s  0  |
  // |  0  1  0  0  |
  // |  s  0  c  0  |
  // |  0  0  0  1  |

  mat4_t m = mat4_identity();

  m.m[0][0] =  cosf(a);
  m.m[0][2] = -sinf(a);
  m.m[2][0] =  sinf(a);
  m.m[2][2] =  cosf(a);

  return m;
}

mat4_t mat4_make_rot_z(const float a)
{
  // |  c  s  0  0  |
  // | -s  c  0  0  |
  // |  0  0  1  0  |
  // |  0  0  0  1  |

  mat4_t m = mat4_identity();

  m.m[0][0] =  cosf(a);
  m.m[0][1] =  sinf(a);
  m.m[1][0] = -sinf(a);
  m.m[1][1] =  cosf(a);

  return m;
}

mat4_t mat4_make_rotation(const rot3_t* r)
{
  if (!r) return mat4_identity();

  mat4_t m_rot_x = mat4_make_rot_x(r->x);
  mat4_t m_rot_y = mat4_make_rot_y(r->y);
  mat4_t m_rot_z = mat4_make_rot_z(r->z);

  mat4_t m_rot_tmp = mat4_mul_mat4(&m_rot_x, &m_rot_y);
  mat4_t m_rot = mat4_mul_mat4(&m_rot_tmp, &m_rot_z);

  return m_rot;
}

mat4_t mat4_make_translation(const vec3_t* t)
{
  // |  1  0  0  x  |
  // |  0  1  0  y  |
  // |  0  0  1  z  |
  // |  0  0  0  1  |

  mat4_t m = mat4_identity();

  m.m[0][3] = t->x;
  m.m[1][3] = t->y;
  m.m[2][3] = t->z;

  return m;
}

mat4_t mat4_look_at(const camera_t* camera)
{
  if (!camera) return mat4_identity();

  mat4_t t = camera_get_transform(camera);

  vec3_t cam_right = (vec3_t) { t.m[0][0], t.m[0][1], t.m[0][2] };
  vec3_t cam_up = (vec3_t) { t.m[1][0], t.m[1][1], t.m[1][2] };
  vec3_t cam_forward = (vec3_t) { t.m[2][0], t.m[2][1], t.m[2][2] };

  mat4_t m = mat4_identity();

  m.m[0][0] = cam_right.x;
  m.m[0][1] = cam_right.y;
  m.m[0][2] = cam_right.z;
  m.m[0][3] = -vec3_dot(&cam_right, &camera->translation);

  m.m[1][0] = cam_up.x;
  m.m[1][1] = cam_up.y;
  m.m[1][2] = cam_up.z;
  m.m[1][3] = -vec3_dot(&cam_up, &camera->translation);

  m.m[2][0] = cam_forward.x;
  m.m[2][1] = cam_forward.y;
  m.m[2][2] = cam_forward.z;
  m.m[2][3] = -vec3_dot(&cam_forward, &camera->translation);

  return m;
}

mat4_t mat4_make_projection(const float aspect_ratio, const float fov, const float z_near, const float z_far)
{
  mat4_t m = mat4_identity();

  m.m[0][0] = -1.f / tanf(fov / 2.f);
  m.m[1][1] = aspect_ratio * -1.f / tanf(fov / 2.f);
  m.m[2][2] = z_far / (z_far - z_near);
  m.m[2][3] = z_far * z_near / (z_far - z_near);
  m.m[3][2] = 1.0;
  m.m[3][3] = 0.f;

  return m;
}

vec4_t mat4_mul_vec4(const mat4_t* m, const vec4_t* v)
{
  return (vec4_t) {
    .x = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z + m->m[0][3] * v->w,
    .y = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z + m->m[1][3] * v->w,
    .z = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z + m->m[2][3] * v->w,
    .w = m->m[3][0] * v->x + m->m[3][1] * v->y + m->m[3][2] * v->z + m->m[3][3] * v->w
  };
}

mat4_t mat4_mul_mat4(const mat4_t* a, const mat4_t* b)
{
  mat4_t m = mat4_identity();

  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      m.m[i][j] = a->m[i][0] * b->m[0][j] + 
                  a->m[i][1] * b->m[1][j] +
                  a->m[i][2] * b->m[2][j] +
                  a->m[i][3] * b->m[3][j];
    }
  }

  return m;
}

mat4_t mat4_mul_mat4_multiple(const mat4_t* matrices)
{
  if (!matrices || darray_size((void*)matrices) == 0)
    return mat4_identity();

  mat4_t m = matrices[0];

  for (size_t i = 1; i < darray_size((void*)matrices); ++i)
      m = mat4_mul_mat4(&m, &matrices[i]);

  return m;
}
