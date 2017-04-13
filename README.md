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

## What we've learned
