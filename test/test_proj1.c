#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include "../include/linux/prinfo.h"

#define BUFFER_NUM 100 /* number of buffers */

void print_prinfo(struct prinfo p) {
    printf("%s,%d,%ld,%d,%d,%d,%ld\n", p.comm, p.pid, p.state,
                p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
}

void print_tree(struct prinfo *buf, int nr) {
    int stack[100];
    int depth=0;
    stack[0] = 0;
    stack[1] = -1;
    for (int i=0; i<nr; i++) {
        while (buf[i].parent_pid != stack[depth]) {
            depth--;
        }
        depth++;
        stack[depth] = buf[i].pid;

        for (int j=0; j<depth-1; j++)
            printf("\t");
        print_prinfo(buf[i]);
    }
}

int main() {
    int nr=BUFFER_NUM;
    int num;
    struct prinfo *buf;
    buf = (struct prinfo*) malloc(nr*sizeof(struct prinfo));

    if (buf == NULL) {
        printf("malloc failed.\n");
        return -1;
    }

    num = syscall(380, buf, &nr);

    if (num<0) {
        printf("syscall failed.\n");
        return -1;
    }
    if (num < nr) {
        print_tree(buf, num);
    } else {
        print_tree(buf, nr);
    }
    printf("total # of processes: %d\n", num);

    free(buf);
    return 0;
}
