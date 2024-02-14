#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

struct yem_ht* yem_fs_watch_all(int fd, struct yem_fs_dir* dir) {
    struct yem_ht* ht = yem_ht_init(dir->size * 2);
    for (int i = 0; i < dir->size; ++i) {
        char* path = dir->paths[i];
        int wd = yem_fs_watch(fd, path);
        yem_ht_push(ht, path, wd);
    }
    return ht;
}

struct yem_fs_event* yem_fs_poll_events(int fd) {
    char buf[4096];
    const size_t buffer_len = sizeof(buf);

    struct inotify_event* in_ev = NULL;
    int in_ev_size = sizeof(struct inotify_event);

    struct yem_fs_event* ev = malloc(sizeof(struct yem_fs_event));
    ev->event = in_ev;
    ev->has_event = 0;

    ssize_t len = read(fd, buf, buffer_len);
    if (len < 0 && errno != EINTR) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    for (char* ptr = buf; ptr < buf + len; ptr += in_ev_size + ev->event->len) {
        ev->event = (struct inotify_event*)ptr;
        if (!ev->event->len) {
            continue;
        }

        ev->has_event = 1;
        return ev;
    }

    return ev;
}
