#ifndef __GPS_H
#define __GPS_H

struct gps_location {
    int lat_integer;
    int lat_fractional;
    int lng_integer;
    int lng_fractional;
    int accuracy;
};

#endif
