
all: main

main: main.c mainHeader.h
	gcc main.c -o main -Wall -lpthread
