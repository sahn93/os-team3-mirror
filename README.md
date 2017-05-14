# Team 3 - Project 3

In this project, we will build our own CPU scheduler in the Tizen Linux kernel. Then, we test the scheduler with the ARTIK device.

This project consists of 3 parts: implementing, testing, and improving WRR scheduler. 


### 1. Implementing WRR Scheduler
* Each task has its weight ranging from 1 to 20.
* Each task has its time slice with its `weight * 10ms`.
* Each task is assigned to the CPU which has the smallest weight sum.
* If there exists a task in maximum weight CPU such that it could be moved to the minimum weight CPU without reversing the two CPU weights' order, it should be moved to minimum weight CPU. This load balancing should be executed every 2000ms.

### 2. Testing WRR Scheduler
WRR Scheduler test consissts of 2 parts.
1. Measuring execution time of a process according to its weight.
2. Measuring execution time of a process according to # of other processes on the cpu.

In WRR policy, the execution time of a process will gets longer as its weight decreases and # of processes in the same core increses.


### 3. Improving WRR Scheduler
* 

## High-level design

We added `wrr_rq` struct in the struct `rq` along with `cfs_rq` and `rt_rq`. We added an integer variable named `wrr_total_weight` in `wrr_rq` struct which stores the sum of every tasks' weight in that `wrr_rq`. With `wrr_total_weight`, we can glance the weight without iterating all elements in `wrr_rq` when load balancing. 
Also, we added `active` array which consists of twenty `list_head`s which is head of `weight_list`. 


We detailed further explanation in following Implementation section. 

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

Also, we added `sched_wrr_entity` struct in `task_struct`. Each entity has its `weight`, `time_slice` and `time_left`. This struct forms linked lists named `run_list` and `weight_list`. `weight_list` exists only if there are two or more CPUs.

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
Most of all, we learned how to hack the linux scheduler. Now we know how to implement a new scheduler class and a run queue that follow a brand new policy. We also learned some useful debugging and testing skills. We have read much of `core.c`, `rt.c` and `fair.c` and we come to be familiar with reading long kernel codes.
