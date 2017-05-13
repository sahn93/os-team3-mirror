#include "sched.h"
#include <linux/slab.h>

#define WRR_INF 1987654321

DEFINE_SPINLOCK(wrr_lb_lock);
unsigned long wrr_next_balance = 0;

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

	schedstat_set(curr->se.statistics.exec_max,
			  max(delta_exec, curr->se.statistics.exec_max));

	curr->se.sum_exec_runtime += delta_exec;
	account_group_exec_runtime(curr, delta_exec);

	curr->se.exec_start = rq->clock_task;
	cpuacct_charge(curr, delta_exec);
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct list_head *wq;

	list_add_tail(&wrr_se->run_list, &rq->wrr.rq);
	
#ifdef CONFIG_SMP
	wq = &rq->wrr.active.queue[wrr_se->weight-1];
	list_add(&wrr_se->weight_list, wq);
	rq->wrr.wrr_total_weight += wrr_se->weight;
#endif

	inc_nr_running(rq);
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->wrr;
	list_del_init(&wrr_se->run_list);

#ifdef CONFIG_SMP
	list_del_init(&wrr_se->weight_list);
	rq->wrr.wrr_total_weight -= wrr_se->weight;
#endif
	
	dec_nr_running(rq);
}

static void yield_task_wrr(struct rq *rq) {
	struct wrr_rq *wrr_rq = &rq->wrr;
	if (wrr_rq) {
		struct list_head *head = &wrr_rq->rq;
		struct list_head *first = head->next;
		if(first!=head) {
			list_del(first);
			list_add_tail(first, head);
		}
	}
}
static struct task_struct *pick_next_task_wrr(struct rq *rq) {
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct sched_wrr_entity *wrr_se;

	if(!wrr_rq || (&wrr_rq->rq == wrr_rq->rq.next))
		return NULL;

	wrr_se = list_entry(wrr_rq->rq.next, struct sched_wrr_entity, run_list);
	return container_of(wrr_se, struct task_struct, wrr);
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p) {
	update_curr_wrr(rq);
}

#ifdef CONFIG_SMP
static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags) {
	int cpu = task_cpu(p);
	struct rq *rq;
	struct wrr_rq *wrr_rq;
	int min_wsum = WRR_INF, min_cpu = -1, i;

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

static void task_fork_wrr(struct task_struct *p) {
	p->wrr.time_left = p->wrr.time_slice;
}

/*
 * We switched to the sched_wrr class.
 */
static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
	if (!p->on_rq)
		return;
	/*
	 * Case 1: rt -> wrr
	 *  1): curr is RT: Do nothing.
	 *  2): curr is WRR: Reschedule.
	 *  3): curr is CFS: Impossible.
	 * Case 2: cfs -> wrr
	 *  1): curr is RT: Do nothing.
	 *  2): curr is WRR: Do nothing.
	 *  3): curr is CFS: Preempt!
	 */

	if (rq->curr == p)
		resched_task(rq->curr);
	else
		check_preempt_curr(rq, p, 0);
}

/*
 * Preempt the current task with a newly woken task if needed:
 */
static void check_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
	if (rq->curr->sched_class == &fair_sched_class) {
		resched_task(rq->curr);
		return;
	}
}

// This function is empty, since WRR have nothing to do with prio.
static void
prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)
{
}

static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)
{
	return task->wrr.time_left;
}

#ifdef CONFIG_SCHED_DEBUG
extern void print_wrr_rq(struct seq_file *m, int cpu, struct wrr_rq *wrr_rq);

void print_wrr_stats(struct seq_file *m, int cpu)
{
	struct wrr_rq *wrr_rq = &cpu_rq(cpu)->wrr;
	rcu_read_lock();
	print_wrr_rq(m, cpu, wrr_rq);
	rcu_read_unlock();
}
#endif

const struct sched_class wrr_sched_class = {
	.next				= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,
	.yield_task			= yield_task_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

	.check_preempt_curr	= check_preempt_wakeup,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr,
#endif
	.set_curr_task		= set_curr_task_wrr,
	.task_tick			= task_tick_wrr,
	.task_fork			= task_fork_wrr,

	.prio_changed 		= prio_changed_wrr,
	.switched_to 		= switched_to_wrr,

	.get_rr_interval	= get_rr_interval_wrr
};

#ifdef CONFIG_SMP
void trigger_wrr_load_balance() {
	struct rq *rq, *min_rq, *max_rq;
	struct sched_wrr_entity *wrr_se;
	struct task_struct *to_move_ts;
	int min_cpu = 1, max_cpu = 1;
	int min_wsum = WRR_INF, max_wsum = -1, curr_wsum;
	int max_movable_w;
	int i, endflag = 0;

	// If there's only one working cpu, just ignore load balancing
	if (num_online_cpus() <= 1)
		return;

	spin_lock(&wrr_lb_lock);

	// Check whether it's time for load balancing or not.
	if (time_after_eq(jiffies, wrr_next_balance)) {
		rcu_read_lock();

		// Find the maximum-weighted cpu and minimum-weighted cpu.
		for_each_cpu(i, cpu_online_mask) {
			rq = cpu_rq(i);
			curr_wsum = rq->wrr.wrr_total_weight;
			if (curr_wsum < min_wsum) {
				min_wsum = curr_wsum;
				min_cpu = i;
			}
			if (curr_wsum > max_wsum) {
				max_wsum = curr_wsum;
				max_cpu = i;
			}
		}
		rcu_read_unlock();

		if (min_cpu != max_cpu) {
			min_rq = cpu_rq(min_cpu);
			max_rq = cpu_rq(max_cpu);

			double_rq_lock(min_rq, max_rq);
			max_movable_w = min_wsum + (max_wsum - min_wsum) / 2;
			max_movable_w = (max_movable_w > 20 ? 20 : max_movable_w);

			// Search movable task from highest weight queue
			for (i = max_movable_w; !endflag && i >= 1; i--) {
				list_for_each_entry(wrr_se, &max_rq->wrr.active.queue[i-1], weight_list) {
					to_move_ts = container_of(wrr_se, struct task_struct, wrr);

					// 1. If task is not currently running, and
					// 2. if the task can be migrated to min_cpu
					if ((to_move_ts != max_rq->curr)
							&& cpumask_test_cpu(min_cpu, tsk_cpus_allowed(to_move_ts))) {
						// Migrate to_move_ts from max_rq to min_rq
						dequeue_task_wrr(max_rq, to_move_ts, 0);
						set_task_cpu(to_move_ts, min_cpu);
						enqueue_task_wrr(min_rq, to_move_ts, 0);
						endflag = 1;
						break;
					}
				}
			}
			double_rq_unlock(min_rq, max_rq);
		}
		wrr_next_balance = jiffies + 2 * HZ;
	}
	spin_unlock(&wrr_lb_lock);
}
#endif
