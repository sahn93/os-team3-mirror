#include <stdio.h>
#include <unistd.h>
int main() {
    long long int i;
    fork();
    fork();
    fork();
    fork();
    printf("Very long job PID: %d\n", getpid());
    while(1) {
        i++;
    }
    printf("PID %d finished!\n", getpid());
}
