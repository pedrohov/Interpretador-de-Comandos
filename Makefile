all:
	gcc -o vesh.o -c vesh.c
	gcc -o main vesh.o main.c

run:
	./main