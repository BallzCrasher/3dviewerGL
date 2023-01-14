# 3dviewerGL
3D model viewer that uses OpenGL. Written in C++.

## Motivation
This program was intended to be a game engine, and it still is. However, taking a 3D renderer to a full game engine takes a lot of time.
Even if this program may not run games just yet. It can still be used to view 3D models very efficiently as it uses OpenGL with GLFW, GLAD and assimp
without any headovers. That's why I'm publishing it early.
The source can also be used as a bases for a visual app or another 3d engine.

## Building and installation
This source code does not include GLFW or ASSIMP with it. So if you want to build this program you need to build GLFW and ASSIMP first 
from thier respected depositories and link them to this executable in the Makefile:
- GLFW: https://github.com/glfw/glfw
- assimp: https://github.com/assimp/assimp

if you want to use custom extentions that are built in with your GPU you may want to replace the glad.c file with another one:
- glad: https://github.com/Dav1dde/glad
alternatively, you can download it from a free web service:
- glad web service: http://glad.dav1d.de/

once all the dependencies are ready you can build the project by typing
```bash
git clone https://github.com/BallzCrasher/3dviewerGL.git
cd 3dviewerGL
make
```
the defualt behavior is to build and run in debug mode. If you are satisfied with the results then go ahead and build it in release mode:
```bash
make release
```
