#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#include "yem_fs.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define MAX_READ_EVENTS 1024
#define EVENT_BUF_LEN (MAX_READ_EVENTS * (EVENT_SIZE + 16))

int yem_fs_init() {
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int yem_fs_watch(int fd, char* path) {
    int wd = inotify_add_watch(fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    return wd;
}

void yem_fs_dir_walk(struct yem_fs_dir* dir, char* path) {
    struct dirent* d;
    struct stat stat_buf;
    DIR* stream = opendir(path);

    if (!stream) {
        exit(EXIT_FAILURE);
    }

    while ((d = readdir(stream)) != NULL) {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 ||
            strcmp(d->d_name, ".git") == 0) {
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, d->d_name);

        if (stat(full_path, &stat_buf) == -1) {
            continue;
        }

        if (S_ISDIR(stat_buf.st_mode)) {
            dir->paths = realloc(dir->paths, (dir->size + 1) * sizeof(char*));
            dir->paths[dir->size] = strdup(full_path);
            dir->size++;
            yem_fs_dir_walk(dir, full_path);
        }
    }

    closedir(stream);
}

struct yem_fs_dir* yem_fs_read_recurse(char* path) {
    int end = strlen(path) - 1;
    if (path[end] == '/') {
        path[end] = '\0';
    }

    struct yem_fs_dir* dirs = malloc(sizeof(struct yem_fs_dir));
    dirs->paths = malloc(sizeof(char*));
    dirs->paths[0] = strdup(path);
    dirs->size = 1;

    yem_fs_dir_walk(dirs, path);

    return dirs;
}

int lol() {
    char* watched_dir = "/home/wiru/code/personal/yem";
    int fd = yem_fs_init();
    int wd = yem_fs_watch(fd, watched_dir);

    char buffer[4096];
    const size_t buffer_len = sizeof(buffer);

    while (1) {
        ssize_t length = read(fd, buffer, buffer_len);
        if (length < 0 && errno != EINTR) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        const struct inotify_event* event;
        int offset = sizeof(struct inotify_event) + event->len;

        for (char* ptr = buffer; ptr < buffer + length; ptr += offset) {
            event = (const struct inotify_event*)ptr;

            if (event->len) {
                char full_path[PATH_MAX];

                if (event->mask & IN_CREATE) {
                    printf("The file %s was created.\n", full_path);
                } else if (event->mask & IN_DELETE) {
                    printf("The file %s was deleted.\n", full_path);
                } else if (event->mask & IN_MODIFY) {
                    printf("The file %s was modified.\n", full_path);
                }
            }
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);

    return 0;
}
