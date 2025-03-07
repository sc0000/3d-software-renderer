// Copyright 2025 Sebastian Cyliax

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>

#include "camera.h"
#include "darray.h"
#include "graphics.h"
#include "light.h"
#include "matrix.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"

const char* mesh_path = "assets/cube.obj";
const char* texture_path = "assets/cube.png";

static struct {
	SDL_Renderer* renderer;
	SDL_Window* window;
	color_t* color_buffer;
  float* depth_buffer;
	SDL_Texture* color_buffer_texture;
	mat4_t mat_projection;
  mat4_t mat_view;
  plane_t frustum_planes[6];
  tri2_t* triangles_to_render;  
  // tri2_t* new_tris;
  vec4_t* transformed_vertices;
  size_t triangles_to_render_size;
  camera_t camera;
	uint32_t prev_frame_time;
  float delta_time;
} state;

camera_t* state_camera = &state.camera;
float* state_delta_time = &state.delta_time;

enum cull_method cull_method = CULL_NONE;
enum render_method render_method = RENDER_WIRE;

//==============================================
// Only for testing
static mesh_t curr_mesh;
static size_t num_faces;
static size_t num_vertices;
const light_t light = { { 1.f, 0.f, 0.f} };
//==============================================

static vec4_t camera_pos = {
	.x = 0.f,
	.y = 0.f,
	.z = 0.f,
  .w = 1.f
};

size_t screen_buffer_size(void)
{
	return (size_t)(WINDOW_WIDTH * WINDOW_HEIGHT);
}

bool init_graphics(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	};

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("Error initializing SDL_Image: %s\n", IMG_GetError());
		return false;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	const int fullScreenWidth = displayMode.w;
	const int fullScreenHeight = displayMode.h;

	state.window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		fullScreenWidth,
		fullScreenHeight,
		SDL_WINDOW_BORDERLESS
	);

	if (!state.window) {
		fprintf(stderr, "Error initializing window.\n");
		return false;
	}

	state.renderer = SDL_CreateRenderer(state.window, -1, 0);

	if (!state.renderer) {
		fprintf(stderr, "Error initializing renderer.\n");
		return false;
	}

	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
  SDL_ShowCursor(SDL_DISABLE);

  size_t buffer_size = screen_buffer_size();
	state.color_buffer = malloc(sizeof(color_t) * buffer_size);
  state.depth_buffer = malloc(sizeof(float) * buffer_size);

	if (!state.color_buffer) {
		fprintf(stderr, "Error initializing color buffer.\n");
		return false;
	}

	state.color_buffer_texture = SDL_CreateTexture(
		state.renderer, 
		PIXELFORMAT, 
		SDL_TEXTUREACCESS_STREAMING, 
		WINDOW_WIDTH, 
		WINDOW_HEIGHT
	);

	if (!state.color_buffer_texture) {
		fprintf(stderr, "Error initializing color buffer texture.\n");
		return false;
	}

	state.mat_projection = mat4_make_projection(
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
		DEG2RAD(90),
		0.1f,
		FOV_ANGLE);

  // populate face arrays; this can stay dynamic, since it only happens once
	mesh_parse_obj(&curr_mesh, mesh_path);

  num_faces = darray_size(curr_mesh.faces);
  num_vertices = darray_size(curr_mesh.vertices);

  // this needs to be adapted when working with several meshes
  state.triangles_to_render_size = num_faces;
  darray_reserve(state.triangles_to_render, state.triangles_to_render_size * 2);

	if (!curr_mesh.vertices || !curr_mesh.faces) {
		fprintf(stderr, "Failed to load mesh.\n");
		return false;
	}

	// seed random color generator and throwaway color for actual subsequent randomization
	srand((unsigned int)time(NULL));

	for (size_t i = 0; i < darray_size(curr_mesh.faces); ++i) {
		curr_mesh.faces[i].color = random_color();
	}

  load_texture(&curr_mesh.texture, texture_path);

	mesh_init_transform(&curr_mesh);

  state.camera = (camera_t) {
    .translation = { 0.f, 0.f, 0.f },
    .rotation = { 0.f, 0., 0.f }
  };

	return true;
}

bool init_meshes(void)
{
	// TODO: Setup data structure for all meshes...
	return true;
}

void update(void)
{
	const uint32_t ticks = SDL_GetTicks();
	const uint32_t time_to_wait = (uint32_t)FRAME_TIME - (ticks - state.prev_frame_time);
	
	if (ticks < state.prev_frame_time + (uint32_t)FRAME_TIME)
		SDL_Delay(time_to_wait);

	state.delta_time = (float)(ticks - state.prev_frame_time) / 1000.f;
	
	// transform, project
	curr_mesh.scale = (vec3_t) {
		.x = 0.5f,
		.y = 0.5f,
		.z = 0.5f
	};

	curr_mesh.rotation.y = PI * sinf(0.0001f * (float)ticks);
	curr_mesh.rotation.x = PI * sinf(0.0001f * (float)ticks);
	curr_mesh.translation.z = 3.f;

  state.mat_view = mat4_look_at(&state.camera);

	project_mesh(&curr_mesh);

	state.prev_frame_time = SDL_GetTicks();
}

void render(void)
{
	if (!state.renderer) return;

	// SDL_RenderClear(state.renderer);
	clear_color_buffer(0x00000000);
  clear_depth_buffer();
	// draw_grid();

  size_t tris_current_size = darray_size(state.triangles_to_render);

  // printf("num tris: %zd\n", tris_current_size);
	
	// draw projections
	if (state.triangles_to_render) {
    for (size_t i = 0; i < tris_current_size; ++i) {
      tri2_t* triangle = &state.triangles_to_render[i];
      tri2_round(triangle);
      tri2_draw_texture(triangle, &curr_mesh.texture);
    }

		if (render_method == RENDER_WIRE ||
			render_method == RENDER_WIRE_VERTEX ||
			render_method == RENDER_FILL_TRIANGLE_WIRE ||
			render_method == RENDER_TEXTURE_WIRE) {
			for (size_t i = 0; i < tris_current_size; ++i) {
				tri2_t* triangle = &state.triangles_to_render[i];
				tri2_round(triangle);
				draw_triangle_edges(triangle, 0xFFFFFFFF);

				if (render_method == RENDER_WIRE_VERTEX)
					draw_triangle_vertices(&state.triangles_to_render[i], 0xFF00FF00);
			}
		}
	}
	
	render_color_buffer();
	SDL_RenderPresent(state.renderer);
}

void destroy_window(void)
{
  IMG_Quit();

	free(state.triangles_to_render);
	free(state.color_buffer);
  free(state.depth_buffer);
	
  SDL_DestroyTexture(state.color_buffer_texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
}

void clear_color_buffer(color_t color)
{
  size_t buffer_size = screen_buffer_size();

	for (int i = 0; i < buffer_size; ++i)
		state.color_buffer[i] = color;
}

void clear_depth_buffer()
{
  size_t buffer_size = screen_buffer_size();

  for (int i = 0; i < buffer_size; ++i)
		state.depth_buffer[i] = 0.f;
}

void render_color_buffer(void)
{
	SDL_UpdateTexture(
		state.color_buffer_texture,
		NULL,
		state.color_buffer,
		sizeof(color_t) * WINDOW_WIDTH
	);

	SDL_RenderCopyEx(state.renderer, state.color_buffer_texture, NULL, NULL, 0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderPresent(state.renderer);
}

void project_vertex(const vec4_t* vert_3d, vec2_t* vert_2d, float* inv_depth)
{
	if (!vert_3d || !vert_2d) return;

	vec4_t projected_vertex = mat4_mul_vec4(&state.mat_projection, vert_3d);

	if (projected_vertex.w != 0.f) {
		projected_vertex.x /= projected_vertex.w;
		projected_vertex.y /= projected_vertex.w;
		projected_vertex.z /= projected_vertex.w;
	}

	vert_2d->x = projected_vertex.x;
	vert_2d->y = projected_vertex.y;

	vert_2d->x *= (float)WINDOW_WIDTH / 2.f;
	vert_2d->y *= (float)WINDOW_HEIGHT / 2.f;

  vert_2d->x += (float)WINDOW_WIDTH / 2.f;
  vert_2d->y += (float)WINDOW_HEIGHT / 2.f;

  *inv_depth = 1.f / projected_vertex.w;
}

void project_mesh(mesh_t* mesh)
{
	if (!mesh || !mesh->faces || !mesh->vertices) 
		return;

  darray_clear(state.triangles_to_render);

	const mat4_t mat_transform = mesh_get_transform(mesh);
  const vec4_t camera_location = vec4_from_vec3(&state.camera.translation);

  // All original vertices to world space
  for (size_t i = 0; i < num_vertices; ++i) {
    vec4_t vec4 = vec4_from_vec3(&mesh->vertices[i]);
    vec4 = mat4_mul_vec4(&mat_transform, &vec4);

    darray_push(state.transformed_vertices, vec4);
  }

	for (size_t i = 0; i < num_faces; ++i) {
    // printf("cl: %d ", mesh->faces[i].clipped_plane);
    // face_print(&mesh->faces[i]);

		const size_t vertex_indices[3] = {
			mesh->faces[i].a, 
			mesh->faces[i].b, 
			mesh->faces[i].c 
		};

    face_t current_face = mesh->faces[i];
		vec4_t transformed_vertices[3];

    for (size_t j = 0; j < 3; ++j) {
        const size_t vertex_index = vertex_indices[j];
        transformed_vertices[j] = state.transformed_vertices[vertex_index];
      }
      
    unsigned int clip_mask = 0;
      
    clip_face_against_frustum_planes(&current_face, transformed_vertices,
      vertex_indices, &clip_mask, mesh);

    if (clip_mask >= 0b111) 
      continue;

    for (size_t j = 0; j < 3; ++j) 
      transformed_vertices[j] = mat4_mul_vec4(&state.mat_view, &transformed_vertices[j]);

    vec4_t ab = vec4_sub(&transformed_vertices[1], &transformed_vertices[0]);
		vec4_t ac = vec4_sub(&transformed_vertices[2], &transformed_vertices[0]);

		vec4_t normal = vec4_cross(&ab, &ac);

		vec4_t camera_dir = vec4_sub(&transformed_vertices[0], &camera_location);

		vec4_normalize(&normal);
		vec4_normalize(&camera_dir);

		if (cull_method == CULL_BACKFACE && vec4_dot(&normal, &camera_dir) > 0) {
      continue;
    }

    tri2_t triangle = tri2_null;

		triangle.tex_coords[0] = current_face.tex_coords[0];
		triangle.tex_coords[1] = current_face.tex_coords[1];
		triangle.tex_coords[2] = current_face.tex_coords[2];
		
    // Lighting, only for colored triangles so far...
		vec4_t light_dir = vec4_from_vec3(&light.direction);
		vec4_normalize(&light_dir);
		const float light_factor = (vec4_dot(&normal, &light_dir) + 1.f) / 2.f;
		triangle.color = light_shade_flat(&mesh->faces[i], light_factor);
    // triangle.color = mesh->faces[i].color;
    // triangle.color = 0xFFAAAA00;

		for (size_t j = 0; j < 3; ++j) {
			vec4_t v = transformed_vertices[j];
			project_vertex(&v, &triangle.vertices[j], &triangle.inv_depth[j]);
		}
    
    darray_push(state.triangles_to_render, triangle);
	}

  darray_reset_size(mesh->faces, num_faces);
  darray_reset_size(mesh->vertices, num_vertices);
  darray_clear(state.transformed_vertices);
}

void draw_pixel(const unsigned int x, const unsigned int y, color_t color)
{
	if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) 
		return;

	state.color_buffer[WINDOW_WIDTH * y + x] = color;
}

void draw_texel(const unsigned int x, const unsigned int y, const tex2_t* tex, const tri2_t* tri)
{
	if (!tex || !tri) return;

	const vec2_t p = { (float)x, (float)y };
  
	const vec3_t weights = tri2_barycentric_weights(tri, p);

	const float alpha = weights.x;
	const float beta = weights.y;
	const float gamma = weights.z;

	const uv_t* tex_coords = tri->tex_coords;
  const float* inv_depth = tri->inv_depth;

	float interp_u = tex_coords[0].u * inv_depth[0] * alpha + 
    tex_coords[1].u * inv_depth[1]* beta + 
    tex_coords[2].u * inv_depth[2] * gamma;

	float interp_v = tex_coords[0].v * inv_depth[0] * alpha + 
    tex_coords[1].v * inv_depth[1] * beta + 
    tex_coords[2].v * inv_depth[2] * gamma;

  float interp_inv_depth = inv_depth[0] * alpha + inv_depth[1] * beta + inv_depth[2] * gamma;

  interp_inv_depth = interp_inv_depth > 1.f ? 1.f : interp_inv_depth;
  interp_inv_depth = interp_inv_depth < 0.f ? 0.f : interp_inv_depth;

  interp_u /= interp_inv_depth;
  interp_v /= interp_inv_depth;

  const size_t depth_buffer_index = WINDOW_WIDTH * y + x;

  if (interp_inv_depth > state.depth_buffer[depth_buffer_index]) {
    state.depth_buffer[depth_buffer_index] = interp_inv_depth;

    color_t color = 0;

    if (tex && (render_method == RENDER_TEXTURE || render_method == RENDER_TEXTURE_WIRE)) {
      const int tex_x = (int)(interp_u * tex->size.x) % (int)tex->size.x;
	    const int tex_y = (int)(interp_v * tex->size.y) % (int)tex->size.y;
      color = tex->data[(int)tex->size.x * tex_y + tex_x];
    }

    if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
      color = tri->color;
   
    if (render_method == RENDER_DEPTH_BUFFER) {
      const color_t a = color & 0xFF000000;
      const color_t r = (color_t)((0xFFFFFFFF & 0x00FF0000) * interp_inv_depth);
      const color_t g = (color_t)((0xFFFFFFFF & 0x0000FF00) * interp_inv_depth);
      const color_t b = (color_t)((0xFFFFFFFF & 0x000000FF) * interp_inv_depth);

      color = a | r & 0x00FF0000 | g & 0x0000FF00 | b & 0x000000FF;
    }

    draw_pixel(x, y, color); 
  }
}

void draw_line_dda(const unsigned int x0, const unsigned int y0, 
  const unsigned int x1, const unsigned int y1, color_t color)
{
	const int dx = abs(x1 - x0);
	const int dy = abs(y1 - y0);
	const int sx = x0 < x1 ? 1 : -1;
	const int sy = y0 < y1 ? 1 : -1;
	const int sidelength = dx > dy ? dx : dy;

	const float inc_x = (float)dx * sx / (float)sidelength;
	const float inc_y = (float)dy * sy / (float)sidelength;

	float cx = (float)x0;
	float cy = (float)y0;

	for (;;) {
		const unsigned int rcx = (unsigned int)round(cx);
		const unsigned int rcy = (unsigned int)round(cy);

		draw_pixel(rcx, rcy, color);

		if (rcx == x1 && rcy == y1)
			break;

		cx += inc_x;
		cy += inc_y;
	}
}

void draw_line_bresenham(unsigned int x0, unsigned int y0, 
  const unsigned int x1, const unsigned int y1, const color_t color)
{
	int dx =  abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;

	for (;;) {
		draw_pixel(x0, y0, color);
		if (x0 == x1 && y0 == y1) 
			break;

		const int err2 = err << 1;

		if (err2 >= dy) {
			err += dy;
			x0 += sx;
		}

		if (err2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void draw_triangle_vertices(const tri2_t* triangle, const color_t color)
{
	for (size_t i = 0; i < 3; ++i) {
		const int x = (int)roundf(triangle->vertices[i].x);
		const int y = (int)roundf(triangle->vertices[i].y);

		draw_rect(x - 2, y - 2, 4, 4, color);
	}
}

void draw_triangle_edges(const tri2_t* triangle, const color_t color)
{
	draw_line_bresenham(
		(int)triangle->vertices[0].x, 
		(int)triangle->vertices[0].y, 
		(int)triangle->vertices[1].x, 
		(int)triangle->vertices[1].y,
		color
	);

	draw_line_bresenham(
		(int)triangle->vertices[1].x, 
		(int)triangle->vertices[1].y, 
		(int)triangle->vertices[2].x, 
		(int)triangle->vertices[2].y,
		color
	);

	draw_line_bresenham(
		(int)triangle->vertices[2].x, 
		(int)triangle->vertices[2].y, 
		(int)triangle->vertices[0].x, 
		(int)triangle->vertices[0].y,
		color
	);
}

void draw_rect(const unsigned int x, const unsigned int y, 
  const unsigned int w, const unsigned int h, const color_t color)
{
	for (unsigned int i = x; i < x + w; ++i) {
		for (unsigned int j = y; j < y + h; ++j) 
			draw_pixel(i, j, color);
	}
}

void draw_grid(void)
{
	for (int i = 1; i < WINDOW_WIDTH; ++i) {
		for (int j = 1; j < WINDOW_HEIGHT; ++j) {
			if (i % 10 == 0 || j % 10 == 0)
				draw_pixel(i, j, 0xFFFFFFFF);
		}
	}
}

void draw_depth_buffer(void)
{
  size_t buffer_size = screen_buffer_size();

  for (size_t i = 0; i < buffer_size; ++i) 
    state.color_buffer[i] = (color_t)(0xFF000000 * state.depth_buffer[i]);
}

color_t random_color()
{
	unsigned char a = 255;
	unsigned char r = rand() % 256;
	unsigned char g = rand() % 256;
	unsigned char b = rand() % 256;

	return (a << 24) | (r << 16) | (g << 8) | b;
}

void update_frustum_planes(void)
{
  mat4_t t = camera_get_transform(&state.camera);

  vec3_t cam_right = (vec3_t) { t.m[0][0], t.m[0][1], t.m[0][2] };
  vec3_t cam_left = vec3_invert(&cam_right);
  vec3_t cam_up = (vec3_t) { t.m[1][0], t.m[1][1], t.m[1][2] };
  vec3_t cam_down = vec3_invert(&cam_up);
  vec3_t cam_forward = (vec3_t) { t.m[2][0], t.m[2][1], t.m[2][2] };
  vec3_t cam_backward = vec3_invert(&cam_forward);

  float half_fov = (float)DEG2RAD(90) / 2.f;
  float half_fov_scaled = half_fov * (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH; 

  // TODO: Check if you could rotate without creating a copy...
  vec3_t right_normal = vec3_rotate_rodrigues(&cam_left, &cam_up, -1.f * half_fov);
  vec3_t left_normal = vec3_rotate_rodrigues(&cam_right, &cam_up, half_fov);

  vec3_t top_normal = vec3_rotate_rodrigues(&cam_up, &cam_left, half_fov_scaled);
  vec3_t bottom_normal = vec3_rotate_rodrigues(&cam_down, &cam_left, -1.f * half_fov_scaled);

  vec3_t front_offset = vec3_scaled(&cam_forward, 0.1f);
  vec3_t back_offset = vec3_scaled(&cam_forward, 1200.f);

  vec3_t front_location = vec3_add(&state.camera.translation, &front_offset);
  vec3_t front_normal = cam_forward;

  vec3_t back_location = vec3_add(&state.camera.translation, &back_offset);
  vec3_t back_normal = cam_backward;

  // printf("raw rot x: %f y: %f z: %f\n", cosf(half_fov), 0, sinf(half_fov));
  // printf("left_normal  %f y: %f z: %f\n", left_normal.x, left_normal.y, left_normal.z);

  state.frustum_planes[PLANE_RIGHT] = (plane_t) {
    .location = state.camera.translation,
    .normal = right_normal
  };

  state.frustum_planes[PLANE_LEFT] = (plane_t) {
    .location = state.camera.translation,
    .normal = left_normal
  };

  state.frustum_planes[PLANE_TOP] = (plane_t) {
    .location = state.camera.translation,
    .normal = top_normal
  };

  state.frustum_planes[PLANE_BOTTOM] = (plane_t) {
    .location = state.camera.translation,
    .normal = bottom_normal
  };

  state.frustum_planes[PLANE_FRONT] = (plane_t) {
    .location = front_location,
    .normal = front_normal
  };

  state.frustum_planes[PLANE_BACK] = (plane_t) {
    .location = back_location,
    .normal = back_normal
  };
}

// ? What else is contained in face ?
void handle_single_outside_vertex(face_t* face, vec4_t* tri_verts, const size_t* vertex_indices, 
  const size_t outside_idx, const vec4_t* plane_location, const vec4_t* plane_normal, const int plane_idx, mesh_t* mesh)
{
  size_t inside_idx0 = (outside_idx + 2) % 3;
  size_t inside_idx1 = (outside_idx + 1) % 3;

  // TODO: Get this value as input?
  vec4_t* outside_vert = &tri_verts[outside_idx];
  // const vec4_t outside_to_plane_location = vec4_sub(plane_location, outside_vert);
  // const float outside_dot = vec4_dot(&outside_to_plane_location, plane_normal);

  const vec4_t* inside_vert0 = &tri_verts[inside_idx0];
  const vec4_t* inside_vert1 = &tri_verts[inside_idx1];

  const vec4_t inside_to_plane_location0 = vec4_sub(plane_location, inside_vert0);
  const vec4_t inside_to_plane_location1 = vec4_sub(plane_location, inside_vert1);

  const float inside_dot0 = vec4_dot(&inside_to_plane_location0, plane_normal);
  const float inside_dot1 = vec4_dot(&inside_to_plane_location1, plane_normal);

  float new_vert_lerp_factor = 0.f;
  float old_vert_lerp_factor = 0.f;

  uv_t orig_outside_tex_coords = face->tex_coords[outside_idx];

  vec4_t new_vert = create_vertex_at_plane(outside_vert, inside_vert1, 
    inside_dot1, plane_location, plane_normal, &new_vert_lerp_factor);

  move_outside_vertex_to_plane(outside_vert, vertex_indices[outside_idx], inside_vert0, inside_dot0,
    plane_location, plane_normal, &old_vert_lerp_factor);

  face->tex_coords[outside_idx] = 
    uv_lerp(&face->tex_coords[outside_idx], &face->tex_coords[inside_idx0], old_vert_lerp_factor); 

  const size_t new_vert_idx = darray_size(state.transformed_vertices);

  if (new_vert_idx < 0) return;

  const size_t new_vertex_indices[3] = {
    new_vert_idx,
    vertex_indices[outside_idx],
    vertex_indices[inside_idx1]
  };

  face_t new_face = (face_t) {
    .a = new_vertex_indices[0],
    .b = new_vertex_indices[1],
    .c = new_vertex_indices[2],
    .tex_coords = { { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f } },
    .color = 0xFFFFFF00,
    .clipped_plane = plane_idx
  };

  vec4_t new_transformed_vertices[3] = {
    new_vert,
    *outside_vert,
    *inside_vert1
  };

  unsigned int clip_mask = 0;

  clip_face_against_frustum_planes(&new_face, new_transformed_vertices, new_vertex_indices, &clip_mask,
    mesh);

  if (clip_mask >= 0b111) 
    return;

  for (size_t j = 0; j < 3; ++j) 
    new_transformed_vertices[j] = mat4_mul_vec4(&state.mat_view, &new_transformed_vertices[j]); 

  vec4_t ab = vec4_sub(&new_transformed_vertices[1], &new_transformed_vertices[0]);
  vec4_t ac = vec4_sub(&new_transformed_vertices[2], &new_transformed_vertices[0]);

  vec4_t normal = vec4_cross(&ab, &ac);

  const vec4_t camera_location = vec4_from_vec3(&state.camera.translation);
  vec4_t camera_dir = vec4_sub(&new_transformed_vertices[0], &camera_location);

  vec4_normalize(&normal);
  vec4_normalize(&camera_dir);

  if (cull_method == CULL_BACKFACE && vec4_dot(&normal, &camera_dir) < 0) {
    return;
  }

  tri2_t triangle = tri2_null;
  triangle.color = 0xFFAAAA00;
  triangle.tex_coords[0] = uv_lerp(&orig_outside_tex_coords, &face->tex_coords[inside_idx1], new_vert_lerp_factor);
  triangle.tex_coords[1] = face->tex_coords[outside_idx];
  triangle.tex_coords[2] = face->tex_coords[inside_idx1];

  for (size_t j = 0; j < 3; ++j) {
    vec4_t v = new_transformed_vertices[j];
    project_vertex(&v, &triangle.vertices[j], &triangle.inv_depth[j]);
  }
  
  darray_push(state.triangles_to_render, triangle);
}

void handle_two_outside_vertices(face_t* face, vec4_t* vertices, const size_t* vertex_indices, 
  const size_t inside_idx, const plane_t* plane)
{
  const vec4_t plane_location = vec4_from_vec3(&(plane->location));
  const vec4_t plane_normal = vec4_from_vec3(&(plane->normal));

  const vec4_t* inside_vert = &vertices[inside_idx];
  const vec4_t inside_to_plane_location = vec4_sub(&plane_location, inside_vert);
  const float inside_dot = vec4_dot(&inside_to_plane_location, &plane_normal);

  const size_t outside_idx0 = (inside_idx + 2) % 3;
  const size_t outside_idx1 = (inside_idx + 1) % 3;

  vec4_t* outside_vert0 = &vertices[outside_idx0];
  vec4_t* outside_vert1 = &vertices[outside_idx1];

  float out_lerp_factor0 = 0.f;
  float out_lerp_factor1 = 0.f;

  move_outside_vertex_to_plane(outside_vert0, vertex_indices[outside_idx0],
    inside_vert, inside_dot, &plane_location, &plane_normal, &out_lerp_factor0);
  
  move_outside_vertex_to_plane(outside_vert1, vertex_indices[outside_idx1], 
    inside_vert, inside_dot, &plane_location, &plane_normal, &out_lerp_factor1);

  // printf("out_lerp_factor0: %f\n", out_lerp_factor0);

  face->tex_coords[outside_idx0] = 
    uv_lerp(&face->tex_coords[outside_idx0], &face->tex_coords[inside_idx], out_lerp_factor0);
  face->tex_coords[outside_idx1] = 
    uv_lerp(&face->tex_coords[outside_idx1], &face->tex_coords[inside_idx], out_lerp_factor1);
}

void move_outside_vertex_to_plane(vec4_t* outside_vert, const size_t outside_idx, const vec4_t* inside_vert, 
  const float inside_dot, const vec4_t* plane_location, const vec4_t* plane_normal, float* out_lerp_factor)
{
 vec4_t outside_to_inside = vec4_sub(inside_vert, outside_vert);
  *out_lerp_factor = calc_lerp_factor(outside_vert, inside_dot, plane_location, plane_normal);
  vec4_scale(&outside_to_inside, *out_lerp_factor);

  *outside_vert = vec4_add(outside_vert, &outside_to_inside);
  // state.transformed_vertices[outside_idx] = *outside_vert;
}

vec4_t create_vertex_at_plane(const vec4_t* outside_vert, const vec4_t* inside_vert, const float inside_dot,
  const vec4_t* plane_location, const vec4_t* plane_normal, float* out_lerp_factor)
{
  vec4_t outside_to_inside = vec4_sub(inside_vert, outside_vert);
  *out_lerp_factor = calc_lerp_factor(outside_vert, inside_dot, plane_location, plane_normal);
  vec4_scale(&outside_to_inside, *out_lerp_factor);

  return vec4_add(outside_vert, &outside_to_inside);
}

const float calc_lerp_factor(const vec4_t* outside_vert, const float inside_dot, 
  const vec4_t* plane_location, const vec4_t* plane_normal)
{
  const vec4_t outside_to_plane_location = vec4_sub(plane_location, outside_vert);
  const float outside_dot = vec4_dot(&outside_to_plane_location, plane_normal);
  
  return outside_dot / (outside_dot - inside_dot);
}

const bool vertex_outside_plane(const vec4_t* vert, const vec4_t* plane_location, const vec4_t* plane_normal)
{
  const vec4_t to_plane_location = vec4_sub(plane_location, vert);
  const float dot = vec4_dot(&to_plane_location, plane_normal);
  return dot > 0;
}

void clip_triangle_against_plane(vec4_t* verts, unsigned int* clip_mask, 
  const vec4_t* plane_location, const vec4_t* plane_normal)
{
  for (size_t i = 0; i < 3; ++i) {
    if (vertex_outside_plane(&verts[i], plane_location, plane_normal)) 
      *clip_mask |= (1 << i);
  }
}

void clip_face_against_frustum_planes(face_t* face, vec4_t* transformed_vertices, 
  const size_t* vertex_indices, unsigned int* clip_mask, mesh_t* mesh)
{
  for (int k = 0 ; k < PLANE_COUNT; ++k) {
      if (*clip_mask >= 0b111) return;

      if (face->clipped_plane == k) continue;

      const plane_t* plane = &state.frustum_planes[k];
      const vec4_t plane_location = vec4_from_vec3(&plane->location); 
      const vec4_t plane_normal = vec4_from_vec3(&plane->normal);

      *clip_mask = 0;

      clip_triangle_against_plane(transformed_vertices, 
        clip_mask, &plane_location, &plane_normal);

      switch (*clip_mask) {
        case 0b000:
          break;

        case 0b001:
          handle_single_outside_vertex(face, transformed_vertices, vertex_indices, 0U, &plane_location, &plane_normal, k, mesh);
          break;

        case 0b010:
          handle_single_outside_vertex(face, transformed_vertices, vertex_indices, 1U, &plane_location, &plane_normal, k, mesh);
          break;

        case 0b100:
          handle_single_outside_vertex(face, transformed_vertices, vertex_indices, 2U, &plane_location, &plane_normal, k, mesh);
          break;

        case 0b110:
          handle_two_outside_vertices(face, transformed_vertices, vertex_indices, 0U, plane);
          break;

        case 0b101:
          handle_two_outside_vertices(face, transformed_vertices, vertex_indices, 1U, plane);
          break;

        case 0b011:
          handle_two_outside_vertices(face, transformed_vertices, vertex_indices, 2U, plane);
          break;

        case 0b111:
          break;
      }
    }
}
