#include "kernel/types.h"
#include "user/user.h"


void generate(int to) {
    int start = 2, stop = 100;
    for (int i = start; i <= stop; ++i) {
        write(to, &i, sizeof(i));
    }
    close(to);
}

void filter(int from, int to, int prime) {
    int num;
    while (read(from, &num, sizeof(num))) {
        if ((num % prime) != 0) {
            write(to, &num, sizeof(num));
        }
    }
    close(from);
    close(to);
}

int main() {
    int in[2];
    if (pipe(in) < 0) {
        fprintf(2, "create pipe failed...\n");
        exit();
    }
    generate(in[1]);
    int prime = 0;
    int to[2];
    while (1) {
        if (read(in[0], &prime, sizeof(prime))) {
            printf("prime %d\n", prime);
            int pid = fork();
            if (pid > 0) {
                wait();
            } else if (pid == 0) {
                if (pipe(to) < 0) {
                    fprintf(2, "create pipe to failed...\n");
                    exit();
                }
                filter(in[0], to[1], prime);
                in[0] = to[0];
            } else {
                fprintf(2, "fork failed...\n");
                exit();
            }
        } else {
            close(to[0]);
            break;
        }
    }
    exit();
}
