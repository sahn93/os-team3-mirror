# Team 3 - Project 3

In this project, we will build our own CPU scheduler in the Tizen Linux kernel. Then, we test the scheduler with the ARTIK device.

This project consists of 3 parts: implementing, testing, and improving WRR scheduler. 

### 1. Implementing WRR Scheduler
* The WRR scheduling policy 

### 2. Testing WRR Scheduler

### 3. Improving WRR Scheduler
* 
## Policies

### 1. Policies from original specs

### 2. Additional Policy


## High-level design



## Implementation

### 1. Data Structure

First, we made `wrr_rq` struct as below. This struct is included in the struct `rq` with `cfs_rq` and `rt_rq`.

```c
struct wrr_rq {
  struct list_head rq;

#ifdef CONFIG_SMP
  struct wrr_prio_array active;
  unsigned int wrr_total_weight;
#endif

  raw_spinlock_t wrr_runtime_lock;
  };
```

Also, we add `sched_wrr_entity` struct in `task_struct`. This struct forms two linked lists.


```c
struct sched_wrr_entity {
  struct list_head run_list;
#ifdef CONFIG_SMP
  struct list_head weight_list;
#endif
  unsigned int weight;
  unsigned int time_slice;
  unsigned int time_left;
};
```
### 2. Functions



## How to build kernel & Test
### Build & flash kernel
1. Type `build` on the root directory to build kernel.
2. Type `flash` to upload kernel to the ARTIK.

### Compile & push test code
1. Type `arm-linux-gnueabi-gcc <source file> -o <output name>` to compile test code.
2. Connect SDB by typing `direct_set_debug.sh --sdb-set` on Artik.
3. Type `push <source> <destination>` to send a file to Artik.

### Run the test


## What we have learned

