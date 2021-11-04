#include <unistd.h>
#include <lisp.h>

int nanosleep(const struct timespec *req, struct timespec *rem);

void plt_sleep(int milliseconds)
{
    usleep(milliseconds*1000);
}
