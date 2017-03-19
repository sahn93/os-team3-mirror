#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <asm-generic/errno-base.h>

asmlinkage int sys_ptree(struct prinfo *buf, int *nr);
{
    printk( "[sys_ptree] Hello World\n" );
    return 0;
}
