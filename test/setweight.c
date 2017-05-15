#include <stdio.h>
#include <unistd.h>

int main() {
    int pid, weight;
    printf("pid weight?\n");
    scanf("%d %d", &pid, &weight); 
    if(syscall(380, pid, weight) != 0) {
       printf("set weight fails...\n");
      return 0;
    } else {
       printf("PID %d's weight is %d\n", pid, weight);
    }
    return 0;
}
