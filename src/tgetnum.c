/************************************************************************
 *									*
 *			Copyright (c) 1982, Fred Fish			*
 *			    All Rights Reserved				*
 *									*
 *	This software and/or documentation is released for public	*
 *	distribution for personal, non-commercial use only.		*
 *	Limited rights to use, modify, and redistribute are hereby	*
 *	granted for non-commercial purposes, provided that all		*
 *	copyright notices remain intact and all changes are clearly	*
 *	documented.  The author makes no warranty of any kind with	*
 *	respect to this product and explicitly disclaims any implied	*
 *	warranties of merchantability or fitness for any particular	*
 *	purpose.							*
 *									*
 ************************************************************************
 */


/*
 *  LIBRARY FUNCTION
 *
 *	tgetnum   extract numeric option from termcap entry
 *
 *  KEY WORDS
 *
 *	termcap
 *	ce functions
 *
 *  SYNOPSIS
 *
 *	tgetnum(id)
 *	char *id;
 *
 *  DESCRIPTION
 *
 *	Returns numeric value of capability <id>, or -1 if <id>
 *	is not found.   Knows about octal numbers, which
 *	begin with 0.
 *
 */

#ifndef _COMPILER_H
#  include <compiler.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <termcap.h>
#if __STDC__
#  include <stdlib.h>
#endif

# ifdef MSDOS
# define index strchr
# endif

extern char *_tcpbuf;		/* Termcap entry buffer pointer */

/*
 *  PSEUDO CODE
 *
 *	Begin tgetnum
 *	    Initialize pointer to the termcap entry buffer.
 *	    While there is a field to process
 *		Skip over the field separator character.
 *		If this is the entry we want then
 *		    If the entry is not a numeric then
 *			Return failure value.
 *		    Else
 *			Initialize value to zero.
 *			If number begins with zero then
 *			    Set accumulation base to 8.
 *			Else
 *			    Set accumulation base to 10.
 *			End if
 *			While there is a numeric character
 *			    Accumulate the value.
 *			End while
 *			Return value.
 *		    End if
 *		End if
 *	    End while
 *	    Return failure value.
 *	End tgetnum
 *
 *  Atari ST specific change, when this fails, we chech to see if we are 
 *     trying to get the entry for "li" = lines, ro "co" = columns, if 
 *     this is the case try the environment variables "ROWS" "LINES" and 
 *     "COLUMNS", before returning error. This is great for window 
 *     managers like "Gemini" or "TOSwin". 
 *
 */

#if defined(GEMDOS)
/* get line /col from the env */
static int getlinecol(id)
char *id;
{
    char *p;

    if (id[0] == 'l' && id[1] =='i' )
    {
        if(!(p = getenv("LINES")))
            p = getenv("ROWS");
        if(p)
            return(atoi(p));
    }
    else if (id[0] == 'c' && id[1] =='o' )
    {
        if((p = getenv("COLUMNS")))
            return(atoi(p));
    }
    return(-1);
}
#endif

int tgetnum(id)
char *id;
{
    int value, base;
    char *bp;

    bp = _tcpbuf;
    while ((bp = index(bp,':')) != NULL) {
	bp++;
	if (*bp++ == id[0] && *bp != '\0' && *bp++ == id[1]) {
	    if (*bp != '\0' && *bp++ != '#') {
#if !defined(GEMDOS)
		return(-1);
#else 
                return getlinecol(id);
#endif 

	    } else {
		value = 0;
		if (*bp == '0') {
		    base = 8;
		} else {
		    base = 10;
		}
		while (isdigit(*bp)) {
		    value *= base;
		    value += (*bp++ - '0');
		}
		return(value);
	    }
	}
      }
#if !defined(GEMDOS)
      return(-1);
#else 
      return getlinecol(id);
#endif 
}
