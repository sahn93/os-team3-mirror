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

static void update_curr_wrr(struct rq *rq) {
	struct task_struct *curr = rq->curr;
	u64 delta_exec;

	if (curr->sched_class != &wrr_sched_class)
		return;

	delta_exec = rq->clock_task - curr->se.exec_start;
	if (unlikely((s64)delta_exec <= 0))
		return;

	schedstat_set(curr->statistics.exec_max,
			  max(delta_exec, curr->se.statistics.exec_max));

	curr->se.sum_exec_runtime += delta_exec;
	account_group_exec_runtime(curr, delta_exec);

	curr->se.exec_start = rq->clock_task;
	cpuacct_charge(curr, delta_exec);
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct list_head *wq;

	list_add_tail(&wrr_se->run_list, &rq->wrr.rq);
	
#ifdef CONFIG_SMP
	wq = &rq->wrr.active.queue[wrr_se->weight-1];
	list_add(&wrr_se->weight_list, wq);
	rq->wrr.wrr_total_weight += wrr_se->weight;
#endif
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	/*TODO*/
	struct sched_wrr_entity *wrr_se = &p->wrr;
	list_del_init(&wrr_se->run_list);

#ifdef CONFIG_SMP
	list_del_init(&wrr_se->weight_list);
	rq->wrr.wrr_total_weight -= wrr_se->weight;
#endif
}

static void yield_task_wrr(struct rq *rq) {
	struct wrr_rq *wrr_rq = &rq->wrr;
	if (wrr_rq) {
		struct list_head *head = &wrr_rq->rq;
		struct list_head *first = &head->next;
		if(first!=head) {
			list_del(first);
			list_add_tail(first, head);
		}
	}
}
static struct task_struct *pick_next_task_wrr(struct rq *rq) {
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct task_struct *p;
	if(!wrr_rq || (&wrr_rq->rq == wrr_rq->rq.next))
		return NULL;
	return container_of(wrr_rq->rq.next, struct task_struct, wrr);
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p) {
	update_curr_wrr(rq);
}

	/*TODO*/
#ifdef CONFIG_SMP
static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags) {
	int cpu = task_cpu(p);
	struct rq *rq;
	struct wrr_rq *wrr_rq;
	int min_wsum = 1987654321, min_cpu = -1, i;

	if (p->nr_cpus_allowed == 1)
		return cpu;

	if (sd_flag & SD_BALANCE_WAKE)
		return cpu;

	rcu_read_lock();
	for_each_cpu(i, tsk_cpus_allowed(p)) {
		rq = cpu_rq(i);
		wrr_rq = &rq->wrr;
		if (wrr_rq->wrr_total_weight < min_wsum) {
			min_wsum = wrr_rq->wrr_total_weight;
			min_cpu = i;
		}
	}
	rcu_read_unlock();
	return min_cpu;
}
#endif

static void set_curr_task_wrr(struct rq *rq) {
	struct task_struct *p = rq->curr;
	p->se.exec_start = rq->clock_task;
	return;
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued) {
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct wrr_rq *wrr_rq = &rq->wrr;

	update_curr_wrr(rq);

	if (--p->wrr.time_left)
		return;

	p->wrr.time_left = p->wrr.time_slice;

	list_move_tail(&wrr_se->run_list, &wrr_rq->rq);
	set_tsk_need_resched(p);
}

/*
 * We switched to the sched_wrr class.
 */
static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
	if (!p->on_rq)
		return;
	/*
	 * We were most likely switched from sched_rt, so
	 * kick off the schedule if running, otherwise just see
	 * if we can still preempt the current task.
	 */
	if (rq->curr == p)
		resched_task(rq->curr);
	else
		check_preempt_curr(rq, p, 0);
}

const struct sched_class wrr_sched_class = {
	.next				= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,
	.yield_task			= yield_task_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr,
#endif
	.set_curr_task		= set_curr_task_wrr,
	.task_tick			= task_tick_wrr,
	.switched_to 		= switched_to_wrr
};
