#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int check_filename(const char* path, const char* filename) {
    int n1 = strlen(path);
    int n2 = strlen(filename);
    int start = n1 - n2;
    if (start < 0) {
        return 0;
    }
    int i = 0;
    while (i < n2) {
        if (path[start + i] != filename[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

void find(const char* path, const char* filename) {
    int fd;
    struct stat st;
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_FILE) {
        if (check_filename(path, filename)) {
            printf("%s\n", path);
        }
    } else if (st.type == T_DIR) {
        char buf[512];
        struct dirent de;
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) {
                continue;
            }
            if ((strcmp(de.name, ".") == 0) || (strcmp(de.name, "..") == 0)) {
                continue;
            }
            memset(buf, 0, 512);
            strcpy(buf, path);
            char* p = buf + strlen(buf);
            *p++ = '/';
            uint size = strlen(de.name);
            memmove(p, de.name, size);
            p += size;
            *p = '\0';
            find(buf, filename);
        }
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(2, "usage: find <pattern>\n");
        exit();
    }
    find(argv[1], argv[2]);
    exit();
}