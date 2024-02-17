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
void yem_fs_watch_all(int fd, struct yem_fs_dir* dir, struct yem_ht* ht);
struct yem_fs_event* yem_fs_poll_events(int fd);
int yem_fs_maybe_make_build_dir(char* path);
int yem_fs_maybe_remove_file(char* path);
void yem_fs_clean(int fd, struct yem_ht* ht);
void yem_fs_ev_new(int fd, struct yem_ht* ht, struct yem_fs_event* event,
    struct yem_ht* build);
void yem_fs_ev_mod(int fd, struct yem_ht* ht, struct yem_fs_event* event,
    struct yem_ht* build);
void yem_fs_ev_del(int fd, struct yem_ht* ht, struct yem_fs_event* event,
    struct yem_ht* build);
