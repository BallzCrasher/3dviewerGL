all: program
	./program

program: .main.obj .glad.obj
	g++ -g .main.obj .glad.obj -ldl -lglfw -o program

.main.obj: main.cpp
	g++ -g main.cpp -ldl -lglfw -o .main.obj -c

.glad.obj: includes/glad.c
	gcc -g includes/glad.c -ldl -lglfw -c -o .glad.obj

clean:
	rm .main.obj .glad.obj
