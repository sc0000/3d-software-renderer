set COMP_FLAGS=/std:c11 /TC /Zi /W4 /WX /wd4100 /D_CRT_SECURE_NO_WARNINGS

set src_dir=src
set include_dir=include
set libs=lib\SDL2main.lib lib/SDL2.lib lib/SDL2_image.lib

@echo off
setlocal enabledelayedexpansion

set "src="
for %%f in (%src_dir%\*.c) do (
  set "src=!src! %%f"
)

CL %COMP_FLAGS% /I include %src% /link %libs% /OUT:3d_software_renderer.exe
