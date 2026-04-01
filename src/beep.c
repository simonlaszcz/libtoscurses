#include "internal.h"
#include "vt52.h"

int
beep(void)
{
    trace1();
    PUT('\a');
    return OK;
}
