// Copyright 2025 Sebastian Cyliax

#include <SDL.h>

#include "camera.h"
#include "graphics.h"
#include "input.h"

static vec3_t direction = { 0.f, 0.f, 0.f };

void process_input(bool* is_running)
{
  SDL_Event event;

  /*
    TODO: while (SDL_PollEvent) {...}

  */

  SDL_PollEvent(&event);

  switch (event.type) {
    case SDL_QUIT:
      *is_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        *is_running = false;
        break;
      }
      
      if (event.key.keysym.sym == SDLK_c) {
        if (cull_method == CULL_NONE)
          cull_method = CULL_BACKFACE;

        else
          cull_method = CULL_NONE;
        break;
      }

      if (event.key.keysym.sym == SDLK_1) {
        render_method = RENDER_WIRE;
        break;
      }

      if (event.key.keysym.sym == SDLK_2) {
        render_method = RENDER_WIRE_VERTEX;
        break;
      }

      if (event.key.keysym.sym == SDLK_3) {
        render_method = RENDER_FILL_TRIANGLE;
        break;
      }

      if (event.key.keysym.sym == SDLK_4) {
        render_method = RENDER_FILL_TRIANGLE_WIRE;
        break;
      }

      if (event.key.keysym.sym == SDLK_5) {
        render_method = RENDER_TEXTURE;
        break;
      }

      if (event.key.keysym.sym == SDLK_6) {
        render_method = RENDER_TEXTURE_WIRE;
        break;
      }

      if (event.key.keysym.sym == SDLK_7) {
        render_method = RENDER_DEPTH_BUFFER;
        break;
      }

      if (event.key.keysym.sym == SDLK_w) {
        direction.z = 1.f;
        move_camera(state_camera, &direction, *state_delta_time);
        break;
      }

      if (event.key.keysym.sym == SDLK_s) {
        direction.z = -1.f;
        move_camera(state_camera, &direction, *state_delta_time);
        break;
      }

      if (event.key.keysym.sym == SDLK_a) {
        direction.x = -1.f;
        move_camera(state_camera, &direction, *state_delta_time);
        break;
      }

      if (event.key.keysym.sym == SDLK_d) {
        direction.x = 1.f;
        move_camera(state_camera, &direction, *state_delta_time);
        break;
      }
      
      if (event.key.keysym.sym == SDLK_q) {
        direction.y = -1.f;
        move_camera(state_camera, &direction, *state_delta_time);
        break;
      }
      
      if (event.key.keysym.sym == SDLK_e) {
        direction.y = 1.f;
        move_camera(state_camera, &direction, *state_delta_time);
        break;
      }

    case SDL_KEYUP:
      if (event.key.keysym.sym == SDLK_w) {
          direction.z = 0.f;
          break;
        }

        if (event.key.keysym.sym == SDLK_s) {
          direction.z = 0.f;
          break;
        }

        if (event.key.keysym.sym == SDLK_a) {
          direction.x = 0.f;
          break;
        }

        if (event.key.keysym.sym == SDLK_d) {
          direction.x = 0.f;
          break;
        }

        if (event.key.keysym.sym == SDLK_q) {
          direction.y = 0.f;
          break;
        }
        
        if (event.key.keysym.sym == SDLK_e) {
          direction.y = 0.f;
          break;
        }

        else break;

    case SDL_MOUSEMOTION:
      rotate_camera(state_camera, &event.motion, *state_delta_time);
      break;
  }
}
