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
 *	tgetflag   extract boolean termcap capability
 *
 *  KEY WORDS
 *
 *	termcap
 *
 *  SYNOPSIS
 *
 *	tgetflag(id)
 *	char *id;
 *
 *  DESCRIPTION
 *
 *	Returns TRUE if specified id is present in terminal
 *	entry, FALSE otherwise.
 *
 */

#include <stdio.h>
#include <string.h>
#include <termcap.h>

#ifndef _COMPILER_H
#  include <compiler.h>
#endif

#define TRUE 1
#define FALSE 0
# ifdef MSDOS
# define index strchr
# endif

extern char *_tcpbuf;		/* Termcap entry buffer pointer */

/*
 *  PSEUDO CODE
 *
 *	Begin tgetflag
 *	    Initialize pointer to the termcap entry buffer.
 *	    While there is a field to process
 *		Skip over the field separator character.
 *		If this is the entry we want then
 *		    If entry is identifier only then
 *			Return TRUE
 *		    Else
 *			Return FALSE
 *		    End if
 *		End if
 *	    End while
 *	    Return FALSE as default.
 *	End tgetflag
 *
 */

int tgetflag(id)
char *id;
{
    char *bp;

    bp = _tcpbuf;
    while ((bp = index(bp,':')) != NULL) {
	bp++;
	if (*bp++ == id[0] && *bp != '\0' && *bp++ == id[1]) {
	    if (*bp == '\0' || *bp++ == ':') {
		return(TRUE);
	    } else {
		return(FALSE);
	    }
	}
    }
    return(FALSE);
}
