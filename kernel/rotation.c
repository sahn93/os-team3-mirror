#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/rotation.h>
#include <linux/list.h>
#include <asm-generic/errno-base.h>
#include <linux/spinlock.h>

int dev_degree = -1;
// Spinlock for everything in rotation.c
DEFINE_SPINLOCK(g_lock);

struct rot_lock {
    int degree;
    int range;
    pid_t pid; // caller user process's pid.
    int is_read; // 1 for read lock, 0 for write lock.
};
// list of rotation locks that acquired lock.
struct rot_lock_acq {
    struct rot_lock lock;
    struct list_head acq_locks;
};
// list of rotation locks pending.
struct rot_lock_pend {
    struct rot_lock lock;
    struct list_head pend_locks;
};

struct rot_lock_acq acq_lock = {.acq_locks = LIST_HEAD_INIT(acq_lock.acq_locks)};
struct rot_lock_pend pend_lock = {.pend_locks = LIST_HEAD_INIT(pend_lock.pend_locks)};

int is_valid_input(int degree, int range) {
	// TODO : If input is valid, return 1. Otherwise, return 0.
	/* 0 <= degree < 360 , 0 < range < 180 */
	if(degree < 0 || degree >= 360)
		return 0;
	if(range <= 0 || range >= 180)
		return 0;	
	return 1;
}

struct rot_lock_acq *find_by_range(int degree, int range) {
	// TODO : If there is matching process with current pid and given degree range in running list,
    // return corresponding list_head. Otherwise (no matching node), return NULL.
    struct rot_lock_acq *alock;
    
    spin_lock(&g_lock);

    list_for_each_entry(alock, &acq_lock->acq_locks, acq_locks) {
       if (current->pid == alock->lock.pid
               && degree == alock->lock.degree
               && range == alock->lock.range) {
           spin_unlock(&g_lock);
           return alock;
       }
    } 

    spin_unlock(&g_lock);
	return NULL;
}

int read_lockable(struct rot_lock *r) {
	return 0;
}

// return 1 if a write lock is lockable.
int write_lockable(struct rot_lock *r) {
	return 0;
}

void exit_rotlock(void) {
	// TODO : 1. Acquire guard lock, 2. Remove elements in acquired list and pending list which has its pid, 
    // 3. If removed an element from acquired list, give lock for pending locks, 4. Release guard lock.
	return;
}

int range_overlap(struct rot_lock *r1, struct rot_lock *r2) {
	// TODO : Return 1 if two locks overlap, otherwise return 0.
	int distance = r1->degree - r2->degree;
	distance = (distance<0)?(-distance):distance;
	distance = (distance < 180)?distance:(360 - distance);
	if(distance <= r1->range + r2->range)
		return 1;
	return 0;
}

// dev: device degree
int dev_deg_in_range(struct rot_lock *r) {
    // TODO : Return 1 if rot_lock's range contains device's degree, else 0;	
	int distance = dev_degree - r->degree;
	distance = (distance<0)?(-distance):distance;
	distance = (distance < 180)?distance:(360 - distance);
	if(distance <= r->range)
		return 1;
	return 0;
}

int lock_lockables(int caller_is_readlock) {
    // TODO : lock pending locks that are available. If caller is a readlock, lock the first write lock.
    // else (caller is a writelock or set_rotation), FIFO.
    return 0;
}

asmlinkage int sys_set_rotation(int degree){
    // TODO : Set the current rotation as the input degree and let the pending locks
    // acquire their lock by calling lock_lockables(0).

	/* 0 <= degree < 360 */
	if (degree < 0 || degree >= 360)
		return -EINVAL;

	dev_degree = degree;
	printk("[set_rotation syscall] device degree : %d\n", dev_degree);
	return 0;
}

/* For 4 system calls below, 
   0 <= degree < 360, 0 < range < 180
   degree - range <= LOCK RANGE <= degree + range */

asmlinkage int sys_rotlock_read(int degree, int range){	
    // TODO : First, make a rot_lock struct. Then, put into the rot_lock_acq if available.
    // Else, put into the rot_lock_pend.
    
	if (!is_valid_input(degree, range))
		return -EINVAL;

	return 0;
}

asmlinkage int sys_rotlock_write(int degree, int range){
    // TODO : First, make a rot_lock struct. Then, put into the rot_lock_acq if available.
    // Else, put into the rot_lock_pend.
    
	if (!is_valid_input(degree, range)) 
		return -EINVAL;

	return 0;
}

asmlinkage int sys_rotunlock_read(int degree, int range){
    // TODO : First, find the rot_lock_acq struct by calling find_by_range function.
    // Then, remove this from the acqired locks list and let the locks in the pending
    // list to acquire their locks by calling lock_lockables(1).
	
	struct rot_lock_acq *to_unlock;  

	if (!is_valid_input(degree, range)) 
		return -EINVAL;
	
	spin_lock(&g_lock);
	to_unlock = find_by_range(degree, range);
	if (to_unlock == NULL || !to_unlock->lock.is_read) {
		spin_unlock(&g_lock);
		return -EINVAL;
	}

	list_del(&to_unlock->acq_locks);
	kfree(to_unlock);
	lock_lockables(1);
	
	spin_unlock(&g_lock);
	return 0;
}

asmlinkage int sys_rotunlock_write(int degree, int range){
    // TODO : First, find the rot_lock_acq struct by calling find_by_range function.
    // Then, remove this from the acqired locks list and let the locks in the pending
    // list to acquire their locks by calling lock_lockables(0).
	
    struct rot_lock_acq *to_unlock;

	if (!is_valid_input(degree, range)) 
		return -EINVAL;

	spin_lock(&g_lock);
	to_unlock = find_by_range(degree, range);
	if (to_unlock == NULL || to_unlock->lock.is_read) {
		spin_unlock(&g_lock);
		return -EINVAL;
	}

	list_del(&to_unlock->acq_locks);
	kfree(to_unlock);
	lock_lockables(0);

	spin_unlock(&g_lock);
	return 0;
}
