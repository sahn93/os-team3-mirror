#include "ptree.h"

asmlinkage int sys_ptree(struct prinfo *buf, int *nr);
{
    printk( "[sys_ptree] Hello World\n" );
    return 0;
}
