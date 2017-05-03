#include "sched.h"
#include <linux/slab.h>

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
}

static struct task_struct *pick_next_task_wrr(struct rq *rq) {
	/*TODO*/
	return NULL;
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p) {
}

static void set_curr_task_wrr(struct rq *rq) {
}

static void task_tick_wrr(struct rq *rq, struct task_struct *curr, int queued) {
	/*TODO*/
}

static void task_fork_wrr(struct task_struct *p) {
	/*TODO*/
}

const struct sched_class wrr_sched_class = {
	.next				= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,

	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

#ifdef CONFIG_SMP
#endif
	
	.set_curr_task		= set_curr_task_wrr,
	.task_tick			= task_tick_wrr,
	.task_fork			= task_fork_wrr
};
