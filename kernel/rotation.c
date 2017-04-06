#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/rotation.h>
#include <linux/list.h>
#include <asm-generic/errno-base.h>

int dev_degree = -1;

int is_valid_input(int degree, int range) {
	// TODO : If input is valid, return 1. Otherwise, return 0.
	return 0;
}

struct list_head *find_with_range(int degree, int range) {
	// TODO : If there is matching process with current pid and given degree range, return corresponding list_head
	// Otherwise (no matching node), return NULL
	return NULL;
}

asmlinkage int sys_set_rotation(int degree){
	/* 0 <= degree < 360 */
	if (degree < 0 || degree >= 360)
		return -1;

	dev_degree = degree;
	printk("[set_rotation syscall] device degree : %d\n", dev_degree);
	return 0;
}

/* For 4 system calls below, 
   0 <= degree < 360, 0 < range < 180
   degree - range <= LOCK RANGE <= degree + range */

asmlinkage int sys_rotlock_read(int degree, int range){	
	// printk("Hello from rotlock_read\n");
	if (!is_valid_input(degree, range))
		return -1;

	return 0;
}

asmlinkage int sys_rotlock_write(int degree, int range){
	// printk("Hello from rotlock_write\n");
	if (!is_valid_input(degree, range)) 
		return -1;

	return 0;
}

asmlinkage int sys_rotunlock_read(int degree, int range){
	// printk("Hello from rotunlock_read\n");
	struct list_head *to_unlock;

	if (!is_valid_input(degree, range)) 
		return -1;
	
	to_unlock = find_with_range(degree, range);
	if (to_unlock == NULL)
		return -1;

	return 0;
}

asmlinkage int sys_rotunlock_write(int degree, int range){
	// printk("Hello from rotunlock_write\n");
	struct list_head *to_unlock;

	if (!is_valid_input(degree, range)) 
		return -1;

	to_unlock = find_with_range(degree, range);
	if (to_unlock == NULL)
		return -1;

	return 0;
}
