prog:main.o source.o
	gcc main.o source.o -o prog -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
		
source.o:source.c
	gcc -c source.c
	
main.o:main.c
	gcc -c main.c

