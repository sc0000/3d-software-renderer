// Copyright 2025 Sebastian Cyliax

#pragma once

#include <stdint.h>
#include <math.h>

#define PI 3.14159265f
#define TWO_PI 6.28318530f
#define E 2.71828182f

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FOV_ANGLE 120
#define NUM_RAYS WINDOW_WIDTH

#define DIST_PROJ_PLANE ((0.5 * WINDOW_WIDTH) / tan(0.5 * FOV_ANGLE))

#define FPS 120
#define FRAME_TIME (1000 / FPS)

#define TEXTURE_SIZE 64
#define PIXELFORMAT SDL_PIXELFORMAT_ARGB8888

#define DEG2RAD(x) (x * PI / 180.f)
#define RAD2DEG(x) (x * 180.f / PI)
#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))

#define MOUSE_SENSITIVITY 1

#define SWAP(type, a, b) do { \
    type temp = *(a); \
    *(a) = *(b); \
    *(b) = temp; \
} while (0)
