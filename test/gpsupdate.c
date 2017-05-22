#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "../include/linux/gps.h"
// syscall number 380: set_gps_location
int main(int argc, char *argv[])
{
    struct gps_location gpsloc;

    int lat_integer;
    int lat_fractional;
    int lng_integer;
    int lng_fractional;
    int accuracy;
    double temp;

    if (argc != 4) {
        printf("usage: ./gpsupdate <latitude> <longitude> <accuracy>");
        return -1;
    }
    
    accuracy = atoi(argv[3]);

    temp = atof(argv[1]);
    lat_integer = (int) floor(temp);
    lat_fractional = (int) ((temp - lat_integer)*1000000);

    temp = atof(argv[2]);
    lng_integer = (int) floor(temp);
    lng_fractional = (int) ((temp - lng_integer)*1000000);
    
    /*printf("%d %d, %d %d, %d\n", lat_integer, lat_fractional, lng_integer, lng_fractional, accuracy);*/

    gpsloc.lat_integer = lat_integer;
    gpsloc.lat_fractional = lat_fractional;
    gpsloc.lng_integer = lng_integer;
    gpsloc.lng_fractional = lng_fractional;
    gpsloc.accuracy = accuracy;

    if (syscall(380, &gpsloc) < 0)
        printf("Syscall Error!\n");
    else
        printf("GPS has updated.\n");
    
    return 0;
}
