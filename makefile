CPPFLAGS=-std=c++11
LDFLAGS=-g
LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system

sfml-app : main.o
	g++ $(LDFLAGS) -o sfml-app main.o $(LDLIBS)

main.o : main.cpp
	g++ $(CPPFLAGS) -c main.cpp
