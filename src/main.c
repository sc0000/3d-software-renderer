// Copyright 2025 Sebastian Cyliax

#include "graphics.h"
#include "input.h"

#define SDL_MAIN_HANDLED

static bool is_running = false;

int main(int argc, char* argv[])
{
  is_running = init_graphics();

  if (!is_running) return 0;

  while (is_running) {
    process_input(&is_running);
    update();
    render();
  }

  destroy_window();

  return 0;
}
