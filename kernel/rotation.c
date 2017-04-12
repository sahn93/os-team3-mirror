#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/rotation.h>
#include <linux/list.h>
#include <asm-generic/errno-base.h>
#include <linux/spinlock.h>
#include <linux/sched.h>

int dev_degree = -1;
// Spinlock for everything in rotation.c
DEFINE_SPINLOCK(g_lock);

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

    list_for_each_entry(alock, &(acq_lock.acq_locks), acq_locks) {
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
    // 1. Check if a write lock is blocking the range. 
    // 2. Check if more than a write lock is in pending list.
    struct rot_lock_acq *alock;
    struct rot_lock_pend *plock;

    spin_lock(&g_lock);
    list_for_each_entry(alock, &(acq_lock.acq_locks), acq_locks) {
        if (range_overlap(r, &(alock->lock))
                && alock->lock.is_read == 0) {
           spin_unlock(&g_lock);
           return 0;
        } 
    }
    list_for_each_entry(plock, &(pend_lock.pend_locks), pend_locks) {
        if (range_overlap(r, &(plock->lock))
                && dev_deg_in_range(&(plock->lock))
                && plock->lock.is_read == 0) {
            spin_unlock(&g_lock);
            return 0;
        }
    }
    spin_unlock(&g_lock);
    return 1;
}

// return 1 if a write lock is lockable.
int write_lockable(struct rot_lock *r) {
    struct rot_lock_acq *alock;
    spin_lock(&g_lock);
    list_for_each_entry(alock, &(acq_lock.acq_locks), acq_locks) {
        if (range_overlap(r, &(alock->lock))) {
            spin_unlock(&g_lock);
            return 0;
        }
    }
    spin_unlock(&g_lock);
	return 1;
}

void exit_rotlock(void) {
	// TODO : 1. Acquire guard lock, 2. Remove elements in acquired list and pending list which has its pid, 
	// 3. If removed an element from acquired list, give lock for pending locks, 4. Release guard lock.

	struct rot_lock_acq *alock;
	struct rot_lock_pend *plock;

	spin_lock(&g_lock);

	list_for_each_entry(alock, &(acq_lock.acq_locks), acq_locks){
		if(current->pid == alock->lock.pid){
			int alock_is_read = alock->lock.is_read;
			list_del(&alock->acq_locks);
			kfree(alock);
			lock_lockables(alock_is_read);			
			spin_unlock(&g_lock);
			return;
		}
	}

	list_for_each_entry(plock, &(pend_lock.pend_locks), pend_locks){
		if(current->pid == plock->lock.pid){
			int plock_is_read = plock->lock.is_read;
			list_del(&plock->pend_locks);
			kfree(plock);
			lock_lockables(plock_is_read);
			spin_unlock(&g_lock);
			return;
		}
	}
	spin_unlock(&g_lock);
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
    
    struct rot_lock_pend *plock, *tmp;
    int ignore_writelock = 0;
    int count=0;
    struct rot_lock_acq *alock;

    if (caller_is_readlock) {
        list_for_each_entry_safe(plock, tmp, &(pend_lock.pend_locks), pend_locks) {
           if (plock->lock.is_read==0
                   && write_lockable(&(plock->lock))) {
               // lock the first write lock in the pending list.
               // make an alock element and put it into the acq_lock list.
               alock = kmalloc(sizeof(*alock), GFP_KERNEL);
               if (alock == NULL) {
                   spin_unlock(&g_lock);
                   return -ENOMEM;
               }
               alock->lock = plock->lock;
               // Then, free this plock.
               list_add_tail(&(alock->acq_locks), &acq_lock.acq_locks);
               list_del(&plock->pend_locks);
               kfree(plock);
               count++;
               break;
           }
        }
    } else {
        // FIFO
        list_for_each_entry_safe(plock, tmp, &(pend_lock.pend_locks), pend_locks) {
            if (plock->lock.is_read==0
                    && ignore_writelock==0
                    && write_lockable(&(plock->lock))) {
                // lock the write lock if it is the first lockable lock in pending list.
                // make an alock element and put it into the acq_lock list.
                alock = kmalloc(sizeof(*alock), GFP_KERNEL);
                if (alock == NULL) {
                   spin_unlock(&g_lock);
                   return -ENOMEM;
                }
                alock->lock = plock->lock;
                list_add_tail(&(alock->acq_locks), &acq_lock.acq_locks);
                // Then, free this plock.
                list_del(&plock->pend_locks);
                kfree(plock);
                count++;
                // In this case, no more pending one acquires a lock.
                break;
            } else if (read_lockable(&(plock->lock))) {
                // If the first lockable lock is a read lock, lock every lockable read locks.
                // Since write lock can't acqure a lock, make a flag.
                ignore_writelock=1;
                // make an alock element and put it into the acq_lock list.
                alock = kmalloc(sizeof(*alock), GFP_KERNEL);
                if (alock == NULL) {
                   spin_unlock(&g_lock);
                   return -ENOMEM;
                }
                alock->lock = plock->lock;
                list_add_tail(&(alock->acq_locks), &acq_lock.acq_locks);
                // Then, free this plock.
                list_del(&plock->pend_locks);
                kfree(plock);
                count++;
                // Iterate.
            }
        }
    }

    // return # of locks that acqured a lock.
    return count;
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
