# 3D Software Renderer

This is a primitive, unfinished software renderer, mostly made to learn more about 3D graphics. It utilizes a little bit of [SDL](https://github.com/libsdl-org/SDL) for the window and pixel buffer, as well as png import, but is otherwise written completely from scratch. Right now, it can just display a single mesh in various ways (see controls), and allows the user to move around a bit.

## Demo

[Demo](https://youtu.be/3vDX2hut0-I).

## Build & Run

* GCC: Run `make all` to build, `make clean` to clean up.
* MSVC: Run `build.bat` from the VS command prompt to build, `clean.bat` to clean up.
* Run the resulting `3d_software_renderer.exe`.

## Controls

* W: Forward
* S: Backward
* A: Left
* D: Right
* E: Up
* Q: Down
* C: Toggle backface culling
* 1: Show only edges
* 2: Show edges with highlighted vertices
* 3: Show faces with randomized colors
* 4: Show faces and edges
* 5: Show texture
* 6: Show texture and edges
* 7: Show depth buffer
* Mouse: Look around

## Load Meshes and Textures

* Set `mesh_path` and `texture_path` at the top of `graphics.c` to the assets you want to display. Only obj and png files are supported.
* Set `TEXTURE_SIZE` in `defs.h` to the side-length your texture.

## Known Issues

* The frustum culling bugs out in the corners, and might even crash.
* Backface culling sometimes leads to missing faces.
* Mouse controls don't work in certain angles and camera positions.

## Note

The dynamic array implementation is taken from my little library [base](https://github.com/sc0000/base).
