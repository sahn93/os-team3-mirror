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
 
## High-level design
## Policies
### 1. Policies from original spec
### 2. Additional policy
We established an additional policy for this project.

## Implementation
## How to build kernel
## What we've learned
