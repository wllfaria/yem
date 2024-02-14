#pragma once
#include <sys/inotify.h>

#include "yem_ht.h"

struct yem_fs_dir {
    char** paths;
    int size;
};

struct yem_fs_event {
    struct inotify_event* event;
    int has_event;
};

int yem_fs_init();
struct yem_fs_dir* yem_fs_read_recurse(char* path);
struct yem_ht* yem_fs_watch_all(int fd, struct yem_fs_dir* dir);
struct yem_fs_event* yem_fs_poll_events(int fd);
