#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define __NR_sched_setweight 380
#define __NR_sched_getweight 381

int is_prime(int n)
{
	int i;
	if(n<2)
		return -1;
	for(i=2;i<n;i++){
		if(n%i==0)
			return 0;
	}
	return 1;
}
int main(int argc, char *argv[])
{
	int n, weight, sys_ret, i;
	time_t t1, t2;
	if(argc <= 2){
		printf("\nPlease enter an integer and its weight.\nProgram terminated.\n");
		return -1;
	}
	n = atoi(argv[1]);
	weight = atoi(argv[2]);
	t1 = clock();
	sys_ret = syscall(__NR_sched_setweight, 0, weight);
	if(n<=1){
		printf("\nInvalid input.\nProgram terminated.\n");
		return -1;
	}
	while(is_prime(n)!=1){
		for(i=2;i<n;i++){
			if(n%i==0 && is_prime(i)==1){
				n = n/i;
				printf("%d * ", i);
				break;
			}
		}
	}
	t2 = clock();
	printf("%d\n", n);
	printf("Elapsed time(s):\t%lf\tWeight:\t%d\n", (double)(t2-t1)/1000000., weight);
	return 0;
}		
