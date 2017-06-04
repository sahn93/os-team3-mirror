#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../include/linux/gps.h"

int main(int argc, char *argv[]) {
	struct gps_location gpsloc;
	double lat, lng;
    int acc, err;

	if (argc != 2) {
		printf("Error : ./file_loc [file path]\n");
		return -1;
	}

	if (strlen(argv[1]) >= 200) {
		printf("Error : Too long file path\n");
		return -1;
	}

	err = syscall(381, argv[1], &gpsloc);
	if (err) {
		printf("Error on syscall: ");
		switch (errno) {
			case EINVAL:
				printf("Invalid arguments.\n");
				break;
			case EACCES:
				printf("Access denied.\n");
				break;
			case ENODEV:
				printf("No GPS coordinate embedded.\n");
				break;
			default:
                printf("%d\n", errno);
				printf("Unknown error.\n");

		}
		return -1;
	}
	
	lat = gpsloc.lat_integer + gpsloc.lat_fractional / 1000000.;
	lng = gpsloc.lng_integer + gpsloc.lng_fractional / 1000000.;
	acc = gpsloc.accuracy;

	printf("File path : %s\n", argv[1]);
	printf("GPS Coordinate : (%.6lf, %.6lf)\n", lat, lng);
	printf("accuracy : %d\n", acc);
	printf("Google Maps link : https://www.google.com/maps/@%.6lf,%.6lf,13z\n", lat, lng);
	
	return 0;
}
