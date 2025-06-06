#include "internal.h"
#include <osbind.h>

int
beep(void)
{
    trace1();
    Cconout('\a');
    return OK;
}
