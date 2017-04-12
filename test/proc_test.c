#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define __NR_set_rotation		380
#define __NR_rotlock_read		381
#define __NR_rotlock_write		382
#define __NR_rotunlock_read		383
#define __NR_rotunlock_write	385

int main(int argc, char *argv[]) {
	int my_pid, com, degree, range;
	int sys_ret;

	if (argc != 2) {
		printf("One argument expected.\n");
		return -1;
	}
	my_pid = atoi(argv[1]);
	printf("Process %d starts!\n", my_pid);
	printf("Usage : \n");
	printf("  - set_rotation : 0 [degree]\n");
	printf("  - rotlock_read : 1 [degree] [range]\n");
	printf("  - rotlock_write : 2 [degree] [range]\n");
	printf("  - rotunlock_read : 3 [degree] [range]\n");
	printf("  - rotunlock_write : 4 [degree] [range]\n");
	printf("  - exit : -1\n");
	printf("=======================================\n");

	scanf("%d", &com);
	while (com != -1) {
		switch (com) {
			case 0 :
				scanf("%d", &degree);
				sys_ret = syscall(__NR_set_rotation, degree);
				printf("[Process %d] set_rotation returns %d\n", my_pid, sys_ret);
				break;
			case 1 :
				scanf("%d%d", &degree, &range);
				sys_ret = syscall(__NR_rotlock_read, degree, range);
				printf("[Process %d] rotlock_read returns %d\n", my_pid, sys_ret);
				break;
			case 2 :
				scanf("%d%d", &degree, &range);
				sys_ret = syscall(__NR_rotlock_write, degree, range);
				printf("[Process %d] rotlock_write returns %d\n", my_pid, sys_ret);
				break;
			case 3 :
				scanf("%d%d", &degree, &range);
				sys_ret = syscall(__NR_rotunlock_read, degree, range);
				printf("[Process %d] rotunlock_read returns %d\n", my_pid, sys_ret);
				break;
			case 4 :
				scanf("%d%d", &degree, &range);
				sys_ret = syscall(__NR_rotunlock_write, degree, range);
				printf("[Process %d] rotunlock_write returns %d\n", my_pid, sys_ret);
				break;
		}
		scanf("%d", &com);
	}
	printf("Process %d terminated!\n", my_pid);
	return 0;
}
