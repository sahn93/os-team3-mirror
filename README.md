Project 1 - Team 3
==================

## High-level design

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

## Implementation
`ptree.c` consists of 3 functions.
- `struct prinfo convert_ts_prinfo (struct task_struct *ts)`

Converts `task_struct` into `prinfo` struct.
- `void ptree_dfs(struct task_struct *root, struct prinfo *buf, int *n_entry, const int nr)`

Traverse the process tree by calling `ptree_dfs` recursively for a process's children. It does not store current prinfo if 'buf' is already full.
- `asmlinkage int sys_ptree(struct prinfo *buf, int *nr)`

Actual system call function for ptree. It validates the arguments first and traverse the process tree from the root(swapper) after allocates buffer in kernel memory. after traversal, copy the kernel buffer into the user buffer.

## How to build our kernel
1) type `build` on the root directory.
2) type `flash` to upload kernel to the ARTIK.

## Investigation of the process tree
1) compile `os-team3/test/test_proj1.c` by typing `arm-linux-gnueabi-gcc test_proj1.c -o test_proj1`.
2) push test program to the ARTIK by typing `push test_proj1 /root/test_proj1`.
3) execute test program by typing `/root/test_proj1` on artik console.

#### Result (partial)
    systemd,1,1,0,2152,2,0
        systemd-journal,2152,0,1,0,2183,0
        systemd-udevd,2183,1,1,0,3438,0
        dlog_logger,3438,1,1,0,3472,1901
        key-manager,3472,1,1,0,3499,444
        murphyd,3499,1,1,0,3507,451
        resourced,3507,1,1,0,3509,0
        sdbd,3509,1,1,0,3532,0
        device-policy-m,3532,1,1,0,3535,402
        media-server,3535,1,1,0,3546,451
full result can be found on [here](https://github.com/swsnu/os-team3/blob/proj1/test/test_proj1_result.txt).

## What we've learned
In this very first project, we have learned some basic knowledge about linux kernel as well as how to use git in the real world. First of all, we have figured out the structure of `task_struct`, and the means to changing mode from the kernel mode to the user mode and vice versa. Also, we could understand why we have to make sure to lock task list before traversing along the process tree. Thanks to TAs, we have lessons with `ctags` and `cscope`, which were excessively helpful in this project. On the other hand, some of us used git for the first time, and it was a great time to learn how to use git in a team project.
In addition, we learned the right way of using Git. We will review the pull request beforehand merging it from now on.
