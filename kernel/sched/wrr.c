#include "sched.h"
#include <linux/slab.h>

void init_wrr_rq(struct wrr_rq *wrr_rq) {
	struct wrr_weight_array *array;
	int i;

	INIT_LIST_HEAD(&wrr_rq->rq);
#ifdef CONFIG_SMP
	array = &wrr_rq->active;
	for (i = 0; i < MAX_WRR_WEIGHT; i++) {
		INIT_LIST_HEAD(array->queue + i);
		__clear_bit(i, array->bitmap);
	}
	__set_bit(MAX_WRR_WEIGHT, array->bitmap);
	wrr_rq->wrr_total_weight = 0;

#endif
	raw_spin_lock_init(&wrr_rq->wrr_runtime_lock);
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct list_head *wq;

	list_add_tail(&wrr_se->run_list, &rq->wrr.rq);
	
#ifdef CONFIG_SMP
	wq = &rq->wrr.active.queue[wrr_se->weight-1];
	list_add(&wrr_se->weight_list, wq);
#endif
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
	struct sched_wrr_entity *wrr_se = &p->wrr;
	list_del_init(&wrr_se->run_list);

#ifdef CONFIG_SMP
	list_del_init(&wrr_se->weight_list);
#endif
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
