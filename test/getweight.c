#include <stdio.h>
#include <unistd.h>

int main() {
    int pid, weight;
    printf("pid?\n");
    scanf("%d", &pid); 
    weight = syscall(381, pid);
    if(weight < 0) {
       printf("get weight fails...\n");
      return 0;
    } else {
       printf("PID %d's weight is %d\n", pid, weight);
    }
    return 0;
}
