
CFLAGS=-Wall -Werror -std=gnu11 -O0 -g -fgnu-tm 

mutex.o:

%.o: %.c
	gcc -c $(CFLAGS) $^



