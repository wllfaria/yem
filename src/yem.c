#include <time.h>
#include <unistd.h>

#include "yem_cli.h"

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
    yem_parse_args(argc, argv);

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_t last_frame = ts.tv_sec * USECOND + ts.tv_nsec / 1000;

    while (1) {
        yem_end_frame(&last_frame, &ts);
    }

    return 0;
}
