#include <linux/limits.h>
#include <sys/inotify.h>
#include <time.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define MAX_READ_EVENTS 1024
#define EVENT_BUF_LEN (MAX_READ_EVENTS * (EVENT_SIZE + 16))

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

    while (1) {
        yem_end_frame(&last_frame, &ts);
    }

    return 0;
}
