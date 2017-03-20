#include "ptree.h"

// convert_ts_prinfo(ts) : Returns the prinfo related to given task_struct 'ts'
struct prinfo convert_ts_prinfo (struct task_struct *ts) {
	struct prinfo new_pr;
	struct task_struct *tmp_ts;
	int i;
	
	new_pr.state = ts->state;
	new_pr.pid = ts->pid;
	new_pr.parent_pid = ts->parent->pid;

	if (!list_empty(&(ts->children))) {
		tmp_ts = list_entry(ts->children.next, struct task_struct, sibling);
		new_pr.first_child_pid = tmp_ts->pid;
	}
	else
		new_pr.first_child_pid = 0;
	
	// If 'ts' is the last process of current sibling list,
	// its next node will point back to its parent.
	if (ts->sibling.next != &(ts->parent->children)) {
		tmp_ts = list_entry(ts->sibling.next, struct task_struct, sibling);
		new_pr.next_sibling_pid = tmp_ts->pid;
	}
	else
		new_pr.next_sibling_pid = 0;

	new_pr.uid = task_uid(ts);
	for (i = 0; i < TASK_COMM_LEN; i++)
		new_pr.comm[i] = ts->comm[i];
	return new_pr;
}

// ptree_dfs(root, buf, n_entry, nr) : Traverse the subtree of process tree.
// root		: pointer to current subtree's root process
// buf		: pointer to the kernel-side buffer
// n_entry	: number of visited process
// nr		: initial size of buffer
void ptree_dfs(struct task_struct *root, struct prinfo *buf, int *n_entry, const int nr) {
	struct task_struct *child;

	// Ignore swapper process
	if (root->pid != 0) {
		// Do not store current prinfo if 'buf' is already full.
		if (*n_entry < nr) 
			buf[*n_entry] = convert_ts_prinfo(root);
		(*n_entry)++;
	}
	
	// recursive call for root's children
	list_for_each_entry(child, &(root->children), sibling)
		ptree_dfs(child, buf, n_entry, nr);
}

// sys_ptree(buf, nr) : Actual system call function for ptree
asmlinkage int sys_ptree(struct prinfo *buf, int *nr) {
	struct prinfo *kbuf;
	int knr, num_entry = 0;
	size_t buf_sz;
	
	unsigned long err;
  
	// Argument validity check
	if (buf == NULL || nr == NULL) return -EINVAL;

	err = get_user(knr, nr);
	if (err)	 return -EFAULT;
	if (knr < 1) return -EINVAL;
	buf_sz = knr * sizeof(struct prinfo);

	// Allocate buffer in kernel memory
	kbuf = (struct prinfo*)kmalloc(buf_sz, GFP_KERNEL);
	if (kbuf == NULL) return -ENOMEM;
	
	// Process tree traversal
	read_lock(&tasklist_lock);
	ptree_dfs(&init_task, kbuf, &num_entry, knr);
	read_unlock(&tasklist_lock);

	// Copy to user
	buf_sz = (num_entry > *nr ? *nr : num_entry) * sizeof(struct prinfo);
	err = copy_to_user(buf, kbuf, buf_sz);
	if (err) return -EFAULT;
	
	err = put_user(knr, nr);
	if (err) return -EFAULT;
	
	kfree(kbuf);
    return num_entry;
}
