#ifndef __GPS_COMMON_H
#define __GPS_COMMON_H

#include <linux/spinlock.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm-generic/errno-base.h>

extern spinlock_t gps_lock;
extern struct gps_location gpsloc;

#endif
