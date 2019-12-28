#include "kernel/types.h"
#include "user/user.h"

int main() {

    int buf[10];
    
    int p2c[2], c2p[2], nbytes;
    if (pipe(p2c) < 0) {
        fprintf(2, "create pipe for parent to child failed ...");
        exit();
    }
    if (pipe(c2p) < 0) {
        fprintf(2, "create pipe for child -> parent failed ...");
        exit();
    }
    
    int pid = fork();
    if (pid > 0) {          //parent
        write(p2c[1], buf, 10);
        if ((nbytes = read(c2p[0], buf, 10))) {
            printf("%d: received pong\n", getpid());
        }
        wait();
    } else if (pid == 0) {
        if ((nbytes = read(p2c[0], buf, 10))) {
            printf("%d: received ping\n", getpid());
        }
        write(c2p[1], buf, 10);
    } else {
        fprintf(2, "fork error.");
        exit();
    }

    close(p2c[0]);
    close(p2c[1]);
    close(c2p[0]);
    close(c2p[1]);

    exit();
}