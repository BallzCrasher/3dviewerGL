all:
	cmake -S . -B build
	cmake --build build
	./build/3dviewergl/3dviewergl
