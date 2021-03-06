#include "../../lisp.h"
#include "../platform.h"

int nanosleep(const struct timespec *req, struct timespec *rem);

void plt_sleep(int milliseconds)
{
    Sleep(milliseconds*1000);
}
