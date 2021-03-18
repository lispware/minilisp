#include <unistd.h>
#include "../../lisp.h"
#include "../platform.h"


int nanosleep(const struct timespec *req, struct timespec *rem);


void plt_sleep(int milliseconds)
{
    usleep(milliseconds*1000);
}
