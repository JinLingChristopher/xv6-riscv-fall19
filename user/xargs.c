#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

void clean_params(char** params, int start) {
    for (int i = start; params[i]; ++i) {
        free(params[i]);
    }
}

void destroy_params(char** params) {
    for (int i = 0; params[i]; ++i) {
        free(params[i]);
    }
    free(params);
}

void xargs(int argc, char* argv[]) {
    char** params = (char**)malloc(sizeof(char*) * MAXARG);
    int j = 0;
    for (int i = 1; i < argc; ++i) {
        params[j] = (char*)malloc(sizeof(char*) * (strlen(argv[i])+1));
        strcpy(params[j++], argv[i]);
    }

    char line[512];
    while (gets(line, 512)) {
        if (strlen(line) == 0) {
            break;
        }
        int k = j;
        char* start = line;
        while (*start == ' ') {
            start++;
        }
        char* end = start;
        while (*end != '\0') {
            end++;
            if (*end == ' ') {
                *end++ = '\0';
                params[k] = malloc(sizeof(char) * (strlen(start) + 1));
                strcpy(params[k++], start);
                start = end;
            }
        }
        if (*start != '\0') {
            end--;
            *end = '\0';
            params[k] = malloc(sizeof(char) * (strlen(start) + 1));
            strcpy(params[k++], start);
        }
        params[k] = 0;

        int pid = fork();
        if (pid > 0) {
            wait();
        } else if (pid == 0) {
            exec(argv[1], params);
        } else {
            fprintf(2, "fork error...\n");
            destroy_params(params);
            exit();
        }
        clean_params(params, j);
    }
    destroy_params(params);
    exit();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(2, "usage: xargs <command>\n");
        exit();
    }

    xargs(argc, argv);
    exit();
}