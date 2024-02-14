#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <time.h>
#include <unistd.h>

#include "yel.h"

volatile sig_atomic_t keep_running = 1;
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

void signal_handler(int signum) {
    keep_running = 0;
}

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

int main() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_t last_frame = ts.tv_sec * USECOND + ts.tv_nsec / 1000;

    char buffer[EVENT_BUF_LEN];
    int fd = inotify_init();
    int wd = inotify_add_watch(
        fd, "/home/wiru/code/personal/yem", IN_MODIFY | IN_CREATE | IN_DELETE);

    while (keep_running) {
        int i = 0;
        ssize_t length = read(fd, buffer, EVENT_BUF_LEN);

        if (!keep_running) {
            break;
        }

        // TODO: this thing is a mess and I don't understand
        // it clearly, so I have to modify in order to understand
        const struct inotify_event* event;
        for (char* ptr = buffer; ptr < buffer + length;
             ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event*)ptr;

            if (event->len) {
                char full_path[PATH_MAX];
                snprintf(full_path, PATH_MAX, "%s/%s - %s",
                    "/home/wiru/code/personal/yem", event->name, event->cookie);

                if (event->mask & IN_CREATE) {
                    printf("The file %s was created.\n", full_path);
                } else if (event->mask & IN_DELETE) {
                    printf("The file %s was deleted.\n", full_path);
                } else if (event->mask & IN_MODIFY) {
                    printf("The file %s was modified.\n", full_path);
                }
            }
        }

        yem_end_frame(&last_frame, &ts);
    }

    return 0;
}
