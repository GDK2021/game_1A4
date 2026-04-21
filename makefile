jeu_lot2: main_lot2.o fonc_lot2.o
	gcc main_lot2.o fonc_lot2.o -o jeu_lot2 -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

fonc_lot2.o: fonc_lot2.c header_lot2.h
	gcc -c fonc_lot2.c -o fonc_lot2.o

main_lot2.o: main_lot2.c header_lot2.h
	gcc -c main_lot2.c -o main_lot2.o


