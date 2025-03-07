// Copyright 2025 Sebastian Cyliax

#include "light.h"

color_t light_shade_flat(const face_t* face, const float factor)
{
    const color_t a = (color_t)(face->color & 0xFF000000);
    const color_t r = (color_t)((face->color & 0x00FF0000) * factor);
    const color_t g = (color_t)((face->color & 0x0000FF00) * factor);
    const color_t b = (color_t)((face->color & 0x000000FF) * factor);

    return a | r & 0x00FF0000 | g & 0x0000FF00 | b & 0x000000FF;
}
