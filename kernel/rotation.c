#include "rotation.h"

asmlinkage int set_rotation(int degree){
	printk("Hello from set_rotation\n");
	return 0;
}

int rotlock_read(int degree, int range){	
	/* 0 <= degree < 360 , 0 < range < 180 */
	printk("Hello from rotlock_read\n");
	return 0;
}

int rotlock_write(int degree, int range){
	/* degree - range <= LOCK RANGE <= degree + range */
	printk("Hello from rotlock_write\n");
	return 0;
}

int rotunlock_read(int degree, int range){
	/* 0 <= degree < 360 , 0 < range < 180 */
	printk("Hello from rotunlock_read\n");
	return 0;
}

int rotunlock_write(int degree, int range){
	/* degree - range <= LOCK RANGE <= degree + range */
	printk("Hello from rotunlock_write\n");
	return 0;
}
