# Team 3 - Project 3

In this project, we will build our own CPU scheduler in the Tizen Linux kernel. Then, we test the scheduler with the ARTIK device.

This project consists of 3 parts: implementing, testing, and improving WRR scheduler. 


### 1. Implementing WRR Scheduler
#### Policies
* Each task has its weight ranging from 1 to 20.
* Each task has its time slice with its `weight * 10ms`.
* Each task is assigned to the CPU which has the smallest weight sum.
* If there exists a task in maximum weight CPU such that it could be moved to the minimum weight CPU without reversing the two CPU weights' order, it should be moved to minimum weight CPU. This load balancing should be executed every 2000ms.

#### `wrr_rq`
We made a run queue for wrr scheduler. this run queue consists of a circular list of tasks and an array of lists for each weight from 1 to 20.

#### `sched_class_wrr`
We declared `wrr_sched_class` that contains pointers for functions that are essential for a `sched_class` and implemented the functions.

#### `trigger_wrr_load_balance()`
We implemented this function to perform load balancing that moves a task from a wrr_rq with the highest weight sum to one with the lowest weight sum.

### 2. Testing WRR Scheduler
WRR Scheduler test consissts of 2 parts.
1. Measuring execution time of a process according to its weight.
2. Measuring execution time of a process according to # of other processes on the cpu.

In WRR policy, the execution time of a process will gets longer as its weight decreases and # of processes in the same core increses.


### 3. Improving WRR Scheduler
In order to improve the performance of original WRR scheduler, we focused on load balancing and tried to improve it. 
The key concept of our improved load balancing is **Retry migration with different MIN_CPU on failure**. 
Pseudo code of our load balancing is following:

```
Sort active CPUs in ascending order with their weight sums, store the result in MIN_CPUS[];
MAX_CPU <- MIN_CPUS[NR_CPUS-1];
for (i = 0; i < NR_CPUS-1; i++) {
    MIN_CPU <- MIN_CPUS[i];
    
    if (there exists a movable task from MAX_CPU to MIN_CPU) {
        Migrate the task from MAX_CPU to MIN_CPU;
        break;
    }
}
```

Previous load balancing policy offers only one change to pick MIN_CPU and MAX_CPU, but our policy gives MAX_CPU more chances to push its workload to other CPU. Therefore, our policy minimizes the maximum weight among every CPUs.

Moreover, Our new policy works well for workloads using CPU affinity (e.g. `taskset` in linux). For example, assume there are some tasks in CPU 0 and 1, which are only allowed to run on CPU 0 and 1. Also assume that CPU 0 has maximum weight sum, and there is no tasks in CPU 2 ~ 7. Since original policy pick a MIN_CPU among CPU 2 ~ 7, there is no chance for load balancing from CPU 0 to CPU 1, even though there might be some tasks could migrate. On the contrary, our new policy tries every CPU (2~7) and then select CPU 1 as MIN_CPU. So it can achieve load balance between CPU 0 and CPU 1.


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

* `void init_wrr_rq(struct wrr_rq *wrr_rq)`: Initializes `wrr_rq`.

* `static void update_curr_wrr(struct rq *rq)`: Updates run queue statistics such as execution time.

* `static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)`: enqueues given `task_sturct` to `rq`'s wrr run queue. If `CONFIG_SMP`, it also enqueues given `task_struct` to corresponding wrr weight queue and updates wrr run queue's total weight. 

* `static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)`: dequeues given `task_struct` from `rq`'s wrr run queue. If `CONFIG_SMP`, it also dequeues given `task_struct` from corresponding wrr weight queue. If `CONFIG_SMP`, it also dequeues given `task_struct` from corresponding wrr weight queue and updates wrr run queue's total weight.

* `static void yield_task_wrr(struct rq *rq)`: moves the first task in the `wrr_rq` to the rear end so that the second one have eligibility to run.

* `static struct task_struct *pick_next_task_wrr(struct rq *rq)`: returns the second task in the `wrr_rq`. If `wrr_rq` is empty or has only one task, returns 0.

* `static void put_prev_task_wrr(struct rq *rq, struct task_struct *p)`: We have nothing to do with this function.

* `static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags)`: Select cpu the task should be switched to.

* `static void set_curr_task_wrr(struct rq *rq)`: Set start time for current task.

* `static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)`: Updates remaining time slice for current task.

* `static void task_fork_wrr(struct task_struct *p)`: Initialize left time as time slice.

* `static void switched_to_wrr(struct rq *rq, struct task_struct *p)`: Reschedule when a only rq task switched to wrr and preempt when a cfs task switched to wrr and curr task is in cfs.

* `static void check_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)`: Preempt the current task with a newly woken task if needed.

* `static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)`: This function is empty, since WRR have nothing to do with prio.

* `static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)`: Returns a task's remaining time slice.

* `void print_wrr_stats(struct seq_file *m, int cpu)`: Prints out current state of wrr_rq in a cpu.

* `void trigger_wrr_load_balance()`: move a task from the most weighted core to the least weighted core.

There are two `trigger_wrr_load_balance()` functions. 
First one in `#ifndef CONFIG_WRR_RETRY_MIN` block is the function regarding to original load balancing policy. 
We used `jiffies` to check whether it is time for load balancing or not.
In order to pick a task to migrate from MAX_CPU to MIN_CPU, we calculated `max_movable_w`, the maximum movable weight. And then we searched from `queue[max_movable_w]`, and try next biggest weight queue, and so on.

Another one in `#else` block is the function regarding to our improved load balancing policy.
We first sort CPUs in ascending order with their weight sums, and try to migrate a task until it is succeeded. The rest parts are the same as former `trigger_wrr_load_balance()`.


## How to build kernel & Test
### Activate our improved load balancing policy
1. To use our load balancing policy, define `CONFIG_WRR_RETRY_MIN` in `kernel/sched/wrr.c`, line 234.
2. To use original policy, comment out that line.

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
Most of all, we learned how to hack the linux scheduler. Now we know how to implement a new scheduler class and a run queue that follow a brand new policy. We also learned some useful debugging and testing skills. 
