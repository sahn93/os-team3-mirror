#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define __NR_rotlock_write 382
#define __NR_rotunlock_write 385

int main(int argc, char *argv[]) {
    FILE *fout;
    int value;
    if (argc==2) {
        printf("The argument supplied is %s\n", argv[1]);
        value = atoi(argv[1]);
    } else {
        printf("One argument expected.\n");
        return 0;
    }
    
    while(1) {
        syscall(__NR_rotlock_write, 90, 90);
        printf("selector: %d\n", value);

        fout = fopen("integer", "w");
        fprintf(fout, "%d\n", value);
        fclose(fout);

        value++;
        syscall(__NR_rotunlock_write, 90, 90);
    }
    return 0;
}
