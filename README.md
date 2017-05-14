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

Also, we added `sched_wrr_entity` struct in `task_struct`. Each entity has its `weight`, `time_slice` and `time_left`. This struct forms linked lists named `run_list` and `weight_list`. `weight_list` exists only if there are two or more cpus.

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

We added several functions for WRR scheduling. 

* `void init_wrr_rq(struct wrr_rq *wrr_rq)` initializes `wrr_rq`.

* `static void update_curr_wrr(struct rq *rq)` 

* `static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)` enqueues given `task_sturct` to `rq`'s wrr run queue. If `CONFIG_SMP`, it also enqueues given `task_struct` to corresponding wrr weight queue and updates wrr run queue's total weight. 

* `static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)` dequeues given `task_struct` from `rq`'s wrr run queue. If `CONFIG_SMP`, it also dequeues given `task_struct` from corresponding wrr weight queue. If `CONFIG_SMP`, it also dequeues given `task_struct` from corresponding wrr weight queue and updates wrr run queue's total weight.

* `static void yield_task_wrr(struct rq *rq)` moves the first task in the `wrr_rq` to the rear end so that the second one have eligibility to run.

* `static struct task_struct *pick_next_task_wrr(struct rq *rq)` returns the second task in the `wrr_rq`. If `wrr_rq` is empty or has only one task, returns 0.

* `static void put_prev_task_wrr(struct rq *rq, struct task_struct *p)`

* `static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags)`

* `static void set_curr_task_wrr(struct rq *rq)`

* `static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)`

* `static void task_fork_wrr(struct task_struct *p)`

* `static void switched_to_wrr(struct rq *rq, struct task_struct *p)`

* `static void check_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)`

* `static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)`

* `static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)`

* `void print_wrr_stats(struct seq_file *m, int cpu)`

* `void trigger_wrr_load_balance()`



## How to build kernel & Test
### Build & flash kernel
1. Type `build` on the root directory to build kernel.
2. Type `flash` to upload kernel to the ARTIK.

### Compile & push test code
1. Type `arm-linux-gnueabi-gcc <source file> -o <output name>` to compile test code.
2. Connect SDB by typing `direct_set_debug.sh --sdb-set` on Artik.
3. Type `push <source> <destination>` to send a file to Artik.

### Run the test
Test consists of 4 files.
* `fork16`: Starts 16 very long identical tasks.
* `trial`: Run prime factorization as specific weight.
* `exetime_per_weight.sh`: Executes `fork16` to activate every cores and executes `trial` as weight 20 to 1 consecutively.
* `exetime_per_procnum.sh`: Executes `trial` 4 times with each 16, 32, 48, 64 tasks on cores.

#### Execution time according to weight change
hit `./exetime_per_weight.sh ./fork16 ./trial` on the home directory.

#### Execution time according to # of processes
hit `./exetime_per_procnum.sh ./fork16 ./trial` on the home directory.

## What we have learned

