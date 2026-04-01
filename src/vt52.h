#ifndef VT52_H
#define VT52_H

#include "internal.h"
#include <osbind.h>

#define PUT(n)          Bconout(DEV_CONSOLE, n)
#define ESC(n)          (PUT(27), PUT(n))

#define FG(c)           (ESC('b'), PUT(c + 32))
#define BG(c)           (ESC('c'), PUT(c + 32))
#define CLS()           ESC('E')
#define WRAP(bf)        ESC(bf ? 'v' : 'w')
#define REVERSE(bf)     ESC(bf ? 'p' : 'q')
#define CLREOL()        ESC('K')
#define CLRSOL()        ESC('o')
#define CURSOR(bf)      ESC(bf ? 'e' : 'f')
#define MVCURSOR(y,x)   (ESC('Y'), PUT(y + 32), PUT(x + 32))

#define SANE()          (CURSOR(1), FG(15), BG(0), REVERSE(0), CLS(), WRAP(1))

#endif
