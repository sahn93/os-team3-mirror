#include <linux/spinlock.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm-generic/errno-base.h>

DEFINE_SPINLOCK(gps_lock);
