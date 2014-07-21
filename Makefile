AS	=gas
LD	=gld
CC	=gcc
CFLAGS	=

all:	server
objects = menu.o log.o comm.o server.o main.o thread_pool.o print.o cmd_pro.o openssl.o package.o
	
server:	$(objects)
	$(CC)	$(objects) -lpthread  -lssl -o server


comm.o: package.h util.h cmd_pro.h sys_param.h

menu.o: config.h print.h log.h

server.o: util.h config.h log.h thread_pool.h comm.h

main.o: util.h server.h menu.h
	$(CC) -c main.c
	
clean:
	rm server *.o
