#include "internal.h"
#include <osbind.h>
#include <stdbool.h>
#include "vt52.h"

static int cursor = 0;

int
curs_set(int visibility)
{
    if (visibility < 0 || visibility > 2)
        return ERR;

    int prev = cursor;
    cursor = visibility;

    switch (visibility) {
    case 0:
        Cursconf(CURS_HIDE, 0);
        break;
    default:
        Cursconf(CURS_SHOW, 0);
        break;
    }

    return prev;
}
