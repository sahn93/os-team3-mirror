#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm-generic/errno-base.h>
#include "sched/sched.h"

int is_curr_admin(void) {
	return current_uid() == 0 || current_euid() == 0;
}

int is_curr_task_owner(struct task_struct *ts) {
	return current_uid() == task_uid(ts) || current_euid() == task_uid(ts);
}

asmlinkage int sys_sched_setweight(pid_t pid, int weight) {
	struct task_struct *ts;
	struct sched_wrr_entity *wrr_se;
	struct rq *rq;

	if (weight < 1 || weight > 20)
		return -EINVAL;

	if (pid == 0)
		pid = current->pid;

	ts = find_task_by_vpid(pid);
	if (!ts)
		return -EINVAL;

	rq = task_rq(ts);
	raw_spin_lock(&rq->lock);
	wrr_se = &ts->wrr;

	if (ts->policy != SCHED_WRR) {
		raw_spin_unlock(&rq->lock);
		return -EINVAL;
	}

	if (!is_curr_admin()) {
		if (!is_curr_task_owner(ts)
				|| wrr_se->weight < weight) {
			raw_spin_unlock(&rq->lock);
			return -EPERM;
		}
	}

	if (wrr_se->weight != weight) {
		wrr_se->weight = weight;
		list_move(&wrr_se->weight_list, &rq->wrr.active.queue[weight-1]);
	}
	raw_spin_unlock(&rq->lock);
	return 0;
}

asmlinkage int sys_sched_getweight(pid_t pid) {
	struct task_struct *ts;
	int ret = -EINVAL;

	if (pid == 0)
		pid = current->pid;

	ts = find_task_by_vpid(pid);
	if (!ts)
		return ret;

	raw_spin_lock(&ts->pi_lock);
	if ((ts->policy != SCHED_WRR)) {
		raw_spin_unlock(&ts->pi_lock);
		return ret;
	}

	ret = ts->wrr.weight;
	raw_spin_unlock(&ts->pi_lock);
	return ret;
}
