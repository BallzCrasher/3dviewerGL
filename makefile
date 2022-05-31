all:
	g++ -g main.cpp includes/glad.c -ldl -lglfw -o test
	./test
clean:
	rm test
