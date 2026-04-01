/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * Clean things up before exiting
 */

#include "internal.h"
#include <stdio.h>
#include "vt52.h"

int
endwin(void)
{
    restore_getch();
    restore_color();

    SANE();
    PUT('\r');
    PUT('\n');

#ifdef DEBUG
    if (xyz_trace_file != NULL)
        fclose(xyz_trace_file);
#endif

    return OK;
}
