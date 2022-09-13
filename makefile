CPPFLAGS= -ldl -lglfw -lassimp -I ./includes

all: program
	./program
program: .main.obj .glad.obj
	g++ -g .main.obj .glad.obj $(CPPFLAGS)  -o program

.main.obj: main.cpp ./includes/*
	g++ -g main.cpp $(CPPFLAGS) -o .main.obj -c

.glad.obj: includes/glad.c
	gcc -g includes/glad.c $(CPPFLAGS) -c -o .glad.obj

release:	
	g++ -g main.cpp $(CPPFLAGS) -O2 -o .main.obj -c
	gcc -g includes/glad.c $(CPPFLAGS) -O2 -c -o .glad.obj
	g++ -g .main.obj .glad.obj $(CPPFLAGS) -O2 -o program

clean:
	rm .main.obj .glad.obj
