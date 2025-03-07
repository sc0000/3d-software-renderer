// Copyright 2025 Sebastian Cyliax

#include <memory.h>
#include <stdio.h>
#include <SDL_image.h>

#include "defs.h"
#include "texture.h"

void  load_texture(tex2_t* texture, const char* filepath) {
  SDL_Surface* tempSurface = IMG_Load(filepath);
  
  if (tempSurface == NULL) {
    printf("Error loading surface: %s\n", SDL_GetError());
    return;
  }

  SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(tempSurface, PIXELFORMAT, 0);
  
  if (formattedSurface == NULL) {
    printf("Error converting surface: %s\n", SDL_GetError());
    return;
  }

  SDL_LockSurface(formattedSurface);

  texture->size.x = (float)formattedSurface->w;
  texture->size.y = (float)formattedSurface->h;

  // texture->data = (color_t*)formattedSurface->pixels;

  color_t* pixels = (color_t*)formattedSurface->pixels;

  memcpy(texture->data, pixels, sizeof(color_t) * TEXTURE_SIZE * TEXTURE_SIZE/*TEXTURE_WIDTH * TEXTURE_HEIGHT*/);

  SDL_UnlockSurface(formattedSurface);
  SDL_FreeSurface(formattedSurface);
  SDL_FreeSurface(tempSurface);
}
