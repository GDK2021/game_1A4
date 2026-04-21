jeu: main.o fonc_lot2.o source.o
	gcc main.o fonc_lot2.o source.o -o jeu -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

main.o: main.c header.h
	gcc -c main.c -o main.o

fonc_lot2.o: fonc_lot2.c header.h
	gcc -c fonc_lot2.c -o fonc_lot2.o

source.o: source.c header.h
	gcc -c source.c -o source.o

clean:
	rm -f *.o jeu
