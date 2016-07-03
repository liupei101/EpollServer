#makefile for "c/s Server"
CC = gcc
CFLAGS = -g

ALL: S SC MC
	

S: Server.c unity.h
	$(CC) -o S Server.c

SC: Sgclient.c unity.h
	$(CC) -o SC Sgclient.c

MC: Mtclient.c unity.h
	$(CC) -o MC Mtclient.c

clean:
	rm -f S SC MC

#END