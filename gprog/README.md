# ways to in Sokol
1. Include platform specific shader code inside c source witch ""
EXAMPLE: 
    metal/triangle-metal.c

2. Use sokol-shdc to compile annotated GLSL source file into a C header (or other output formats) for use with sokol-gfx - manage backend integration for specific os
EXAMPLE: 
    metal/triangle-metal2.c

3. same as 2. but use sokol_app.h to (A minimal cross-platform application-wrapper library) get the background translation done
EXAMPLE: 
    sapp/triangle-bufferless-sapp-new.c
