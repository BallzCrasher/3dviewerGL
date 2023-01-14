CXXFLAGS= -std=c++17 -lglfw3 -lGL -lX11 -lpthread -ldl -lassimp -Iincludes 
OPTIMIZE= -O2
CXX=g++
CC=gcc

debug: target/debug/program
	./target/debug/program

target/debug/program: target/debug/obj/main.o target/debug/obj/glad.o \
	target/debug/obj/helper_functions.o target/debug/obj/model.o target/debug/obj/shader.o | target
	$(CXX) -g $^ $(CXXFLAGS) -o target/debug/program

target/debug/obj/main.o: src/main.cpp includes/callbacks.hpp | target
	$(CXX) -g src/main.cpp $(CXXFLAGS) -c -o target/debug/obj/main.o

target/debug/obj/glad.o: src/glad.c | target
	$(CC) -g src/glad.c -c -o target/debug/obj/glad.o

target/debug/obj/helper_functions.o: src/helper_functions.cpp includes/helper_functions.hpp | target
	$(CXX) -g src/helper_functions.cpp $(CXXFLAGS) -c -o target/debug/obj/helper_functions.o

target/debug/obj/shader.o: src/shader.cpp includes/shader.hpp | target
	$(CXX) -g src/shader.cpp $(CXXFLAGS) -c -o target/debug/obj/shader.o

target/debug/obj/model.o: src/model.cpp includes/model.hpp | target
	$(CXX) -g src/model.cpp $(CXXFLAGS) -c -o target/debug/obj/model.o

release: target/release/program | target
	./target/release/program

target/release/program: target/release/obj/main.o target/release/obj/glad.o \
	target/release/obj/helper_functions.o target/release/obj/model.o target/release/obj/shader.o | target
	$(CXX) $^ $(CXXFLAGS) $(OPTIMIZE) -o target/release/program

target/release/obj/main.o: src/main.cpp | target
	$(CXX) src/main.cpp $(CXXFLAGS) $(OPTIMIZE) -c -o target/release/obj/main.o

target/release/obj/glad.o: src/glad.c | target
	$(CC) src/glad.c -c $(OPTIMIZE) -o target/release/obj/glad.o

target/release/obj/helper_functions.o: src/helper_functions.cpp includes/helper_functions.hpp | target
	$(CXX) src/helper_functions.cpp $(CXXFLAGS) $(OPTIMIZE) -c -o target/release/obj/helper_functions.o

target/release/obj/shader.o: src/shader.cpp includes/shader.hpp | target
	$(CXX) src/shader.cpp $(CXXFLAGS) $(OPTIMIZE) -c -o target/release/obj/shader.o

target/release/obj/model.o: src/model.cpp includes/model.hpp | target
	$(CXX) src/model.cpp $(CXXFLAGS) $(OPTIMIZE) -c -o target/release/obj/model.o

target:
	mkdir -p target/debug/obj
	mkdir -p target/release/obj

clean:
	rm target/debug/obj/*
	rm target/release/obj/*
