#include <linux/gps.h>
#include <linux/uaccess.h>
#include <linux/gpscommon.h>

DEFINE_SPINLOCK(gps_lock);
struct gps_location gpsloc;

asmlinkage int sys_set_gps_location(struct gps_location __user *loc) {
    
    struct gps_location *kbuf;

    if (loc == NULL)
        return -EINVAL;

    kbuf = (struct gps_location*)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
    if (kbuf == NULL)
        return -ENOMEM;

    if (copy_from_user(kbuf, loc, sizeof(struct gps_location))) {
        kfree(kbuf);
        return -EFAULT;
    }

    if (
            (kbuf->lat_fractional < 0 || kbuf->lat_fractional >= 1000000) ||
            (kbuf->lng_fractional < 0 || kbuf->lng_fractional >= 1000000) ||
            (kbuf->lat_integer > 90 || kbuf->lat_integer < -90) ||
            (kbuf->lng_integer > 180 || kbuf->lng_integer < -180) ||
            (kbuf->lat_integer == 90 && kbuf->lat_fractional !=0) ||
            (kbuf->lng_integer == 180 && kbuf->lng_fractional !=0) ||
            (kbuf->accuracy < 0)
       ) {
        kfree(kbuf);
        return -EINVAL;
    }

    spin_lock(&gps_lock);
        gpsloc.lat_integer = kbuf->lat_integer;
        gpsloc.lat_fractional = kbuf->lat_fractional;
        gpsloc.lng_integer = kbuf->lng_integer;
        gpsloc.lng_fractional = kbuf->lng_fractional;
        gpsloc.accuracy = kbuf->accuracy;
    spin_unlock(&gps_lock);

    kfree(kbuf);
    return 0;
}
