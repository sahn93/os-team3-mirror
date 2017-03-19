#ifndef PTREE_H_
#define PTREE_H_

#include <linux/prinfo.h> /* prinfo */
#include <asm-generic/errno-base.h> /* error codes */
#include <linux/sched.h> /* task_struct */
#include <linux/slab_def.h> /* kmalloc */
#include <linux/list.h> /* list */
#include <linux/kernel.h> /* printk */
#include <linux/unistd.h> /* system call number */
#include <asm/uaccess.h> /* copy from user, etc. */
