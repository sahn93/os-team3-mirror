# Project 1 - Team 3

### High-level design

Our `ptree` system call can be divided into three phases.

#### 1. Error handling & Memory allocation
Before `ptree` starts traversing process tree, it checks its two arguments' validity (`buf` and `nr`).
There are two possible cases of erroneous input. 
- `buf` or `nr` is NULL pointer.
- `nr` is less than 1. (no actual space in `buf`)

Simultaneously, `ptree` copies `nr`'s value from user space to kernel space, and allocate some memory
in kernel space to store `prinfo`s. During this phase, `ptree` captures failure of memory allocation and
memory copying APIs.

#### 2. Traverse process tree in DFS manner
When all memory are ready, `ptree` locks tasklists first. And then, it calls `ptree_dfs` to traverse
current process tree. During traverse, `ptree_dfs` stores `prinfo` struct corresponding to each process 
to `buf` in pre-order. When `ptree_dfs` returns, `ptree` unlocks tasklist.

#### 3. Copy back to user space
After `ptree_dfs` finishes, `ptree` copies the `prinfo` data stored in `buf` and new `nr` value to user space.



### Implementation

### How to build our kernel

### Investigation of the process tree

### What we've learned

In this very first project, we have learned some basic knowledge about linux kernel as well as how to use git in the real world. First of all, we have figured out the structure of `task_struct`, and the means to changing mode from the kernel mode to the user mode and vice versa. Also, we could understand why we have to make sure to lock task list before traversing along the process tree. Thanks to TAs, we have lessons with `ctags` and `cscope`, which were excessively helpful in this project. On the other hand, some of us used git for the first time, and it was a great time to learn how to use git in a team project.
