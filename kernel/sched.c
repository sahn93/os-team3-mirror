#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <asm-generic/errno-base.h>

asmlinkage int sys_sched_setweight(pid_t pid, int weight) {
	/*TODO*/
	return 0;
}

asmlinkage int sys_sched_getweight(pid_t pid) {
	/*TODO*/
	return 0;
}
