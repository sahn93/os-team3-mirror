# Team 3 - Project 2

This assignment includes implementing a new kernel synchronization primitive in Linux. It provides reader-writer lock based on device rotation.

This project consists of 3 parts in kernel and test respectively.

### 1. Kernel part
* `set_rotation`: Sets device rotation as `degree`.
* `rotlock_read` & `rotlock_write`: Acquires rotation read/write lock.
* `rotunlock_read` & `rotunlock_write`: Releases rotation read/write lock.

### 2. Test part
* `rotd`: Updates device rotation every 2 seconds in background.
* `selector` : Grabs and releases write lock to increase `integer`.
* `trial`: Grabs and releases read lock to factorize `integer`.
 
## Policies
### 1. Policies from original specs
1. Locker have to contain current degree in its range.
2. A reader can acquire lock even if other readers already acquired lock within its range.
3. Both reader and writer can't acquire lock if a writer already acquired within its range.
4. Writer starvation should be avoided.

### 2. Additional policy
By the policy 1-4, readers have to wait if a reader occupying a lock and a writer is waiting for the range. After the reader unlocks, we decided to give lock for the waiting writer.

## High-level design

To synchronize processes that uses rotation locks, we introduced a `spinlock_t` variable, which name is `g_lock`. Every system call have to take this lock to start their tasks, and also have to unlock when finishing their jobs. Therefore, our system calls never be executed concurrently between multiple processes.

We also instroduced a structure `rot_lock` that contains pid of the lock owner to distinguish a lock from locks with same degree and range.

We managed rotation locks with 2 lists, `rot_lock_acq` stands for the list of `rot_lock`s that occupying a lock and `rot_lock_pend` that stands for the list of `rot_lock`s that pending for a lock.

When a reader or writer failed to acquire `rot_lock`, we put them into the `rot_lock_pend` and stop the process. Else, it takes a lock and run its task.

There are 2 situations that let the processes in pending acquire `rot_lock`.

* When degree is updated
* When a lock is unlocked

In both situation, we will check whether there are available locks in pending list and put them into the acquired list and wake up the processes.

## Implementation

### 1. Data Structure
First, we made `rot_lock` struct as below. This struct has `degree`, `range`, `pid`, and `is_read` to indicate the lock is read or write. 

```c
struct rot_lock {
    int degree;
    int range;
    pid_t pid; // caller user process's pid.
    int is_read; // 1 for read lock, 0 for write lock.
};
```
Next, we made two independent lists which have acquired locks and pending locks respectively. In `rot_lock_acq`, we added one integer variable called `committed` which indicates whether the lock finished or not. 

```c
struct rot_lock_acq {
    struct rot_lock lock;
    struct list_head acq_locks;
	int committed; // Set 1 if rot_lock_acq's corresponding lock system call is finished. Otherwise 0
};
struct rot_lock_pend {
    struct rot_lock lock;
    struct list_head pend_locks;
};
```
### 2. Functions

* `int is_valid_input(int degree, int range)` checks if the input `degree` and `range` are valid or not.
* `struct rot_lock_acq *find_by_range(int degree, int range, int committed)` gets `degree`, `range`, `committed` and finds the node with such values in the list, and returns the pointer of the node.
* `int read_lockable(struct rot_lock *r)` returns whether the input read lock can grab lock or not.
* `int write_lockable(struct rot_lock *r)` returns whether the input write lock can grab lock or not.
* `void exit_rotlock(void)` handles when the current process terminates. It deletes any nodes with that process in both lists and call `lock_lockables` so that it can grab any other locks if available. 
* `int range_overlap(struct rot_lock *r1, struct rot_lock *r2)` detects if two locks have overlap ranges.
* `int dev_deg_in_range(struct rot_lock *r)` returns 1 if `dev_degree` is included in the lock's range.
* `int lock_lockables(int caller_is_readlock)` gets whether the caller is readlock and lock any other locks if available.

## How to build kernel & Test
### Build & flash kernel
1. Type `build` on the root directory to build kernel.
2. Type `flash` to upload kernel to the ARTIK.

### Compile & push test code
1. Type `arm-linux-gnueabi-gcc <source file> -o <output name>` to compile test code.
2. Connect SDB by typing `direct_set_debug.sh --sdb-set` on Artik.
3. Type `push <source> <destination>` to send a file to Artik.

### Run the test
1. Type `sdb shell` to use Artik's shell.
2. Run `rotd` first.
3. Run `selector` and `trial` according to various scenarios.

We have tested and passed for all scenarios in this [issue](https://github.com/swsnu/osspr2017/issues/68).

## What we have learned

* We have learned the concept of reader-writer lock
* We have learned how to use `schedule` and `set_current_state(TASK_INTERRUPTIBLE)` and `wake_up_process`
* We have learned about how to manage extreme cases such as unexpectedly terminating processes.
