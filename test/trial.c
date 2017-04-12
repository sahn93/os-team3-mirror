#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int is_prime(int n){
	// returns 1 if prime, 0 if not
	int i;
	if(n<2)
		return -1;
	for(i=2;i*i<=n;i++){
		if(n%i==0)
			return 0;
	}
	return 1;
}
int main(int argc, int **argv){
	int id, num, i;
	FILE *fp;
	char *str;
	str = malloc(1024+1);
	if(argc <= 1){
		printf("Integer identifier needed.\nProgram terminated.\n"); 
		return -1;
	}
	id = atoi(argv[1]);
	while(1){
		/* acquire lock */
		syscall(381, 90, 90);
		fp = fopen("integer", "r");
		if(fp == NULL){
			printf("Exception: null file pointer.\n");
			syscall(383, 90, 90);
			continue;
		}
		
		fgets(str, 1024, fp);
		num = atoi(str);
		printf("trial-%d: %d = ", id, num);
		while(is_prime(num)!=1){
			for(i=2;i*i<=num;i++){
				if(num%i==0 && is_prime(i)==1){
					num = num/i;
					printf("%d * ",i);
					break;
				}
			}
		}
		if(is_prime(num)==1)
			printf("%d\n", num);
		fclose(fp);
		/* release lock */
		syscall(383, 90, 90);
	}
	return 0;
}
