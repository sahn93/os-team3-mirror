CC = arm-linux-gnueabi-gcc
CFLAGS = -W -Wall

all : gpsupdate file_loc

gpsupdate : gpsupdate.c
	$(CC) $(CFLAGS) -o gpsupdate gpsupdate.c -lm

file_loc : file_loc.c
	$(CC) $(CFLAGS) -o file_loc file_loc.c

clean : 
	rm gpsupdate file_loc
