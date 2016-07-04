#makefile for "c/s Server"
CC = gcc
CFLAGS = -g

ALL: obj/S obj/SC obj/MC
	

obj/S: Server.c unity.h
	$(CC) -o obj/S Server.c

obj/SC: Sgclient.c unity.h
	$(CC) -o obj/SC Sgclient.c

obj/MC: Mtclient.c unity.h
	$(CC) -o obj/MC Mtclient.c -lpthread

clean:
	rm -f obj/S obj/SC obj/MC

#END
