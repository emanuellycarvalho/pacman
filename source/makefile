PASTINHA_CONJUNTA=C:\Users\Administrador\Documents\UFMG\PDS\pacman




CC = $(PASTINHA_CONJUNTA)\mingw\bin\gcc
PATH_ALLEGRO=$(PASTINHA_CONJUNTA)\allegro

all: pacman.exe
	
pacman.exe: pacman.o
	$(CC) -O2 -o pacman.exe pacman.o $(PATH_ALLEGRO)\lib\liballegro-5.0.10-monolith-mt.a


pacman.o: pacman.c 
	$(CC) -O2 -I $(PATH_ALLEGRO)\include -c pacman.c 	

ex: pacman.exe
	pacman