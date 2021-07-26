all_files=C:\Users\Administrador\Documents\UFMG\PDS\pacman




CC = $(all_files)\mingw\bin\gcc
path_allegro=$(all_files)\allegro

all: pacman.exe
	
pacman.exe: pacman.o
	$(CC) -O2 -o pacman.exe pacman.o $(path_allegro)\lib\liballegro-5.0.10-monolith-mt.a


pacman.o: pacman.c 
	$(CC) -O2 -I $(path_allegro)\include -c pacman.c

ex: pacman.exe
	pacman