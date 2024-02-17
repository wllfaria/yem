#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <time.h>
#include <unistd.h>

#include "yel.h"
#include "yem_cli.h"
#include "yem_fs.h"
#include "yem_ht.h"

#define USECOND 1000000

void yem_end_frame(time_t* last_frame, struct timespec* ts) {
    const time_t target = USECOND / 3;
    clock_gettime(CLOCK_MONOTONIC, ts);
    time_t current_frame = ts->tv_sec * USECOND + ts->tv_nsec / 1000;
    time_t delta = current_frame - *last_frame;

    if (delta < target) {
        usleep(target - delta);
    }

    *last_frame = current_frame + (target - delta);
}

int main(int argc, char** argv) {
    char* path = yem_parse_args(argc, argv);
    struct yem_fs_dir* dir = yem_fs_read_recurse(path);

    int fd = yem_fs_init();
    const int DIR_SIZE = dir->size * 5;
    struct yem_ht* ht = yem_ht_init(DIR_SIZE);
    yem_fs_watch_all(fd, dir, ht);

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_t last_frame = ts.tv_sec * USECOND + ts.tv_nsec / 1000;

    while (1) {
        struct yem_fs_event* event = yem_fs_poll_events(fd);
        struct yem_ht* build_items = yem_ht_init(DIR_SIZE);

        if (event->has_event) {
            if (event->event->mask == IN_CREATE) {
                yem_fs_ev_new(fd, ht, event, build_items);
            } else if (event->event->mask == IN_MODIFY) {
                yem_fs_ev_mod(fd, ht, event, build_items);
            } else if (event->event->mask == IN_DELETE) {
                yem_fs_ev_del(fd, ht, event, build_items);
            }
        }

        if (build_items->length > 0) {
            struct yem_ht_it it = yem_ht_iterator(build_items);
            while (yem_ht_it_next(&it) == 1) {
                pid_t pid = fork();
                if (pid < 0) {
                    YEL_CRIT("error\n");
                } else if (pid == 0) {
                    yem_fs_maybe_make_build_dir(path);
                    char* final_path = malloc(1024);
                    int len = strlen(it.key);
                    it.key[len - 1] = 'o';
                    snprintf(
                        final_path, 1024, "%s/%s/%s", path, "nbuild", it.key);
                    yem_fs_maybe_remove_file(final_path);
                    while (access(final_path, F_OK) != -1) {
                        YEL_INFO("attempting to remove: %s\n", final_path);
                        usleep(100000);
                    }
                    char* cmd[] = {
                        "gcc", "-c", it.path, "-o", final_path, NULL};
                    execvp(cmd[0], cmd);
                }
            }
        }

        free(build_items);
        yem_end_frame(&last_frame, &ts);
    }

    yem_fs_clean(fd, ht);
    return 0;
}
