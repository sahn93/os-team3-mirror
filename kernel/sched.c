#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm-generic/errno-base.h>

asmlinkage int sys_sched_setweight(pid_t pid, int weight) {
	if (pid == 0)
		pid = current->pid;
	
	/* Error cases :
	     - task with given pid doesn't exist
	     - task's policy is not SCHED_WRR
		 - caller is neither the administrator, nor the owner of task
		 - the owner of task (not admin) tries to increase its weight
		 - weight < 1 || weight > 20
	*/

	// set task's weight here 

	return 0;
}

asmlinkage int sys_sched_getweight(pid_t pid) {
	if (pid == 0)
		pid = current->pid;

	/* Error cases :
	     - task with given pid doesn't exist
	     - task's policy is not SCHED_WRR
	*/

	// return task's weight below
	return 0;
}
