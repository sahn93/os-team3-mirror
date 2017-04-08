#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/unistd.h>

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

int is_valid_input(int degree, int range);
struct rot_lock_acq *find_by_range(int degree, int range);
int read_lockable(struct rot_lock *r);
int write_lockable(struct rot_lock *r);
void exit_rotlock(void);
int range_overlap(struct rot_lock *r1, struct rot_lock *r2);
int dev_deg_in_range(struct rot_lock *r);
int lock_lockables(int caller_is_readlock);

