#include "sched.h"
#include <linux/slab.h>

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
}

static struct task_struct *pick_next_task_wrr(struct rq *rq) {
	return NULL;
}

const struct sched_class wrr_sched_class = {
	.next				= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,

	.pick_next_task		= pick_next_task_wrr

#ifdef CONFIG_SMP
#endif
	
};
