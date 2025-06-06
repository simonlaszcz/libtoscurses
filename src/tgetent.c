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
 *	tgetent   load buffer with entry for specified terminal
 *
 *  KEY WORDS
 *
 *	termcap functions
 *	utility routines
 *
 *  SYNOPSIS
 *
 *	int tgetent(bp,name)
 *	char *bp;
 *	char *name;
 *
 *  DESCRIPTION
 *
 *	Extracts the entry for terminal <name> from the termcap file
 *	and places it in the character buffer <bp>.   It is currently
 *	assumed that bp is at least 1024 characters.  If the entry in
 *	the termcap file is larger than 1023 characters the excess
 *	characters will be discarded and appropriate status will
 *	be returned.
 *
 *	Also note that since bp is used by other termcap
 *	routines, the storage associated with the termcap entry
 *	cannot be freed until all termcap calls are completed.
 *
 *	Tgetent can be directed to look in a file other than
 *	the default (/etc/termcap) by defining an environment
 *	variable called TERMCAP to be the pathname of the desired
 *	termcap file.  This is useful for debugging new entries.
 *	NOTE: the pathname MUST begin with a '/' character.
 *      (Atari ST specific change: the pathname may begin with '\',
 *       or with a drive letter followed by ':').
 *
 *	Also, if the string assigned to TERMCAP does not begin with
 *	a '/' and if the environment variable TERM matches <name> then
 *	the string assigned to TERMCAP is copied to buffer <bp> 
 *	instead of reading a termcap file.
 *      (Atari ST specific change: TERM is no longer checked (the
 *       check was buggy).
 *
 *	Modification by ERS: if no termcap file can be found, then
 *	a default termcap is used (this is for GEMDOS).
 *
 *      Further mods by MJ: original routines fail to proces valid
 *      termcap files - replaced with new versions.
 *      Atari specific: default termcap used when nothing better is
 *      around reads a number of rows and colums from Line-A variables.
 *	
 *  RETURNS
 *
 *	-1  if the termcap file cannot be opened
 *	 0  if no entry in termcap file matches <name>
 *	 1  if extraction is successful with no errors
 *	 2  if extraction is successful but entry truncated
 *
 *  SEE ALSO
 *
 *	tgetnum   extract numeric type capability
 *	tgetflag  test boolean type capability
 *	tgetstr   get string value of capability
 *
 *  AUTHOR
 *
 *	Fred Fish
 *
 */

#include <stdio.h>
#include <string.h>
#include <termcap.h>
#include <ctype.h>

#ifndef _COMPILER_H
#  include <compiler.h>
#endif

#if __STDC__
#  include<stdlib.h>
#endif

#define TRUE 1
#define FALSE 0
#define BUFSIZE 1024			/* Assumed size of external buffer */

#define NO_FILE	 -1			/* Returned if can't open file */
#define NO_ENTRY  0			/* Returned if can't find entry */
#define SUCCESS   1			/* Returned if entry found ok */
#define TRUNCATED 2			/* Returned if entry found but trunc */

# ifdef DGK
# define DEFAULT_ROOT "termcap"		/* name without path component */
/**
  static FILE *fopenp __PROTO((char *name, char *mode, char *pathname));
**/
# define DEFAULT_FILE "\\etc\\termcap"
# else
# define DEFAULT_FILE "/etc/termcap"	/* default termcap filename */
# endif
/* __EXTERN char *fgetlr __PROTO((char *bp, int bpsize, FILE *fp)); */
static int build_entry __PROTO((char *bp, char *stop, FILE *fp, char *name));
static FILE *find_file __PROTO((char *));
/**
static int gotcha __PROTO((char *bp, char *name));
**/

char *_tcpbuf;				/* Place to remember buffer pointer */

/*
 *  PSEUDO CODE
 *
 *	Begin tgetent
 *	    Erase any previous buffer contents.
 *	    Remember the buffer pointer.
 *	    If termcap file is not found then
 *		If buffer was filled anyway then
 *		    Return SUCCESS.
 *		Else
 *		    Return NO_FILE.
 *		End if
 *	    Else
 *		While records left to process
 *		    If this is entry is what we want then
 *			Close the termcap file.
 *			If entry was truncated then
 *			    Return TRUNCATED status
 *			Else
 *			    Return SUCCESS status.
 *			End if
 *		    End if
 *		End while
 *		Return NO_ENTRY status.
 *	    End if
 *	End tgetent
 *
 *	Modification by Michal Jagerman (April of 1992):
 *      The "While records left to process" is too simple minded
 *      since termcap entry in a buffer can be spliced from
 *	multiple pieces by using "tc" capability. Therefore
 *      we delegete the task of termcap buffer filling to
 *      a specialized internal function 'build_entry()'. This
 *      function also solves a problem of opening a termcap file
 *      in a 'wrong' mode by cleaning a termcap buffer from all
 *      leftovers.
 *
 *  
 */

int tgetent(bp,name)
char *bp;				/* Pointer to buffer (1024 char min) */
char *name;				/* Pointer to terminal entry to find */
{
    FILE *fp;

    *bp = '\0';
    _tcpbuf = bp;
    if ((fp = find_file(bp)) == NULL) {
	if (*bp != '\0') {
	    return(SUCCESS);
	} else {
	    return(NO_FILE);
	}
    } else {
	*bp++ = ':';
	return (build_entry(bp, bp + BUFSIZE - 1, fp, name));
    }
}

/*
 *  INTERNAL FUNCTION
 *
 *	build_entry    construct termcap entry in a given buffer
 *
 *  SYNOPSIS
 *
 *	static int build_entry(bp, stop, fp, name)
 *	char *bp, *stop, *name;
 *      FILE *fp;
 *
 *  DESCRIPTION
 *
 *      For a given name build in a buffer bp a termcap description
 *      using a contents of file fp.  Continue this until the entry
 *      is complete or we reached stop.  Concatenate entries if
 *      required by tc capability.   White space characters and
 *      backslashes escaping newlines are not copied into bp.
 *      Returns SUCCESS if there was no problems, NO_ENTRY if an
 *      entry with given name was not found and TRUNCATED if we
 *      run out of a buffer space or continuation entry specified
 *      with tc was not found
 *
 *  BUGS
 *
 *      Termcap specifications require for tc to be the last capability
 *      for the given name.  This is not enforced but anything which
 *      follows tc in the same description will be discarded.
 *      It is not entirely clear what we should return when continuation
 *      specified with tc failed.
 *      Other stuff which goes beyond termcap specs can be accepted.
 *      Terminal names starting with '#' are not accepted.
 *      Continuation with names over 127 characters long will likely bomb!
 *
 *  AUTHOR
 *
 *	Michal Jaegermann
 *
 */
static int build_entry(bp, stop, fp, name)
char *bp, *stop, *name;
FILE *fp;
{
    int c;
    int so_far, skip_all = 0;
    char *np;
    char nbuf[128];
    static int _tgetc __PROTO((FILE *fp));

    /* rewind file - we may seek for a continuation entry */
    rewind(fp);

    /*
     * this is FSM - sort of 
     */
    while (EOF != (c = getc(fp))) {
	 /*
	  * we are looking at a comment - skip it
	  */
	if ('#' == c) {
	    do {
		if (EOF == (c = getc(fp)))
		    return NO_ENTRY;
	    } while ('\n' != c);
	}
	/*
	 * empty line or we finished comment traversal;
	 * a little bit to good - but valid termcap file will be
	 * stil accepted
	 */
	if (isspace(c))
	    continue;
	/*
	 * try matching name
	 */
	np = name;
	while (*np == c) {
	    np += 1;
	    c = _tgetc(fp);
	}
	/*
	 * we finished traversing our name - is this really a match ?
	 */
	if (*np == '\0') {
	    if (c == '|' || c == ':')
		break; /* we have a match */
	    if  (c == EOF)
		return (TRUNCATED); /* match - but we wanted more */
	}
	/*
	 * no match - skip until next name or next entry
	 * if we are past all possible names here 
	 */
	skip_all = 0;
	while ('\n' != c) {
	    if (':' == c)
		skip_all = 1; /* we are past all valid names for this entry */
	    if ('|' == c && 0 == skip_all)
		break;
	    c = _tgetc(fp);
	}
    }
    if (EOF == c)
	return (NO_ENTRY);
    while (':' != c)	/* skip the remainig variants of terminal names */
	c = _tgetc(fp); /* we do not want any mixups later             */

    /*
     * at last we got it - copy terminal description into bp
     */
    so_far = 0;  /* rough indicator how far we are into a capability */
    while ('\n' != (c = _tgetc(fp))) {
	if (0 == so_far && !isalpha(c))
	    continue;	 /* do not bother with all kind of spurious stuff */
	so_far++;
	if (1 == so_far && 't' == c ) {
	    /* a special case - maybe we have "tc=" string? */
	    if ((bp + 3) > stop) {
		ungetc(c, fp);
		continue;
		/* cheating with so_far, but we want to skip this case! */
	    }
	    *bp++ = c;
	    c = _tgetc(fp);
	    if ('c' == c) {
		*bp++ = c;
		c = _tgetc(fp);
		if ('=' == c) {
		    /* we will continue with a name which follows */
		    bp -= 2;
		    /* copy new name to nbuf */
		    np = nbuf;
		    while (':' != (c = _tgetc(fp))) {
			if ('\n' == c || EOF == c)
			    break;
			*np++ = c;
		    }
		    *np = '\0';
		    return (SUCCESS == build_entry(bp, stop, fp, nbuf) ?
			    SUCCESS : TRUNCATED);
		}
	    }
	}  /* end of 'tc=' check */
	if (':' == c) /* literal colon cannot occur in capabilities strings -
		       * one has to use '\072' instead */
	    so_far =  0;
	*bp++ = c;
	if (bp >= stop)
	    return(TRUNCATED);
    }
    if (bp < stop)
	*bp = '\0';
    return(SUCCESS);
}

/*
 * Auxilary function to read a character from a text file
 * with skipping escaped line terminators; any escaped
 * '\n' will be replaced by a character which follows.
 * After escape any number of ^M's will vanish,
 * i.e a string of three characters '\\', 0x0d, 'a' will read
 * as a string of two characters '\\', 'a' and so on...
 * We do not tolerate such garbage in text files. :-)
 */
static int _tgetc(fp)
FILE *fp;
{
    int c;

    if ('\\' == (c = getc(fp))) {
	while ('\r' == (c = getc(fp)))
	    ;				/* Messy stuff - go away */
	if (c != '\n') {
	    ungetc(c, fp);
	    return ('\\');
	}
	c = getc(fp);
    }
    return(c);
}



/*
 *  INTERNAL FUNCTION
 *
 *	find_file    find the termcap file and open it if possible
 *
 *  KEY WORDS
 *
 *	internal functions
 *	find_file
 *
 *  SYNOPSIS
 *
 *	static FILE *find_file(bp)
 *	char *bp;
 *
 *  DESCRIPTION
 *
 *	Attempts to locate and open the termcap file.  Also handles
 *	using the environment TERMCAP string as the actual buffer
 *	(that's why bp has to be an input parameter).
 *
 *	If TERMCAP is defined an begins with a '/' character then
 *	it is taken to be the pathname of the termcap file and
 *	an attempt is made to open it.  If this fails then
 *	the default termcap file is used instead.
 *
 *	If TERMCAP is defined but does not begin with a '/' then
 *	it is assumed to be the actual buffer contents provided
 *	that <name> matches the environment variable TERM.
 *
 *  BUGS
 *
 *	There is currently no way to be sure which termcap
 *	file was opened since the default will always be
 *	tried.
 *
 */

/*
 *  PSEUDO CODE
 *
 *	Begin find_file
 *	    If there is a TERMCAP environment string then
 *		If the string is not null then
 *		    If the string is a pathname then
 *			If that file is opened successfully then
 *			    Return its pointer.
 *			End if
 *		    Else
 *			If there is a TERM environment string then
 *			    If TERM matches <name> then
 *				Copy TERMCAP string to buffer.
 *				Return NULL for no file.
 *			    End if
 *			End if
 *		    End if
 *		End if
 *	    End if
 *	    Open default termcap file and return results.
 *	End find_file
 *
 */

#ifdef GEMDOS
/*
 * we do not really need the following part once the stuff is in
 * our termcap  buffer
 */
/* "df|default|Atari default"  */
/*
 * this values we will try to fill to fit a given display
 */
/* :co#80:li#25:\  */

static char term_default[] = "\
:al=\\EL:am:bs:cd=\\EJ:ce=\\EK:cl=\\EE:cm=\\EY%+ %+ :dl=\\EM\
:do=\\EB:eo:ho=\\EH:is=\\Eq\\EE\\Ee\\Ev:it#8:pt:kb=^H:ll=\\EY9!\
:me=\\Eq:mr=\\Ep:le=\\ED:nd=\\EC:rc=\\Ek:sc=\\Ej:se=\\Eq:so=\\Ep:ta=^I\
:up=\\EA:ve=\\Ee:vi=\\Ef:km:bl=^G:cr=^M:ti=\\Ev\\Ee:sr=\\EI:sf=^J";

#include <linea.h>
#include <support.h>
#endif

static FILE *find_file(bp)
char *bp;
{
    FILE *fp;
    char *cp, *ncp;
    __EXTERN char *getenv __PROTO((const char *));

    if ((cp = getenv("TERMCAP")) != NULL) {
	if (*cp != '\0') {
	    if (*cp == '/' || *cp == '\\' || (cp[1] == ':')) {
		if ((fp = fopen(cp,"r")) != NULL) {
		    return(fp);
		}
	    } else {
		if ((ncp = getenv("TERM")) != NULL) {
			strcpy(bp,cp);
			return((FILE *)NULL);
		}
	    }
	}
    }
# ifdef DGK
	/* Try current directory, then /etc/termcap, then along the path
	 */
	if (fp = fopen(DEFAULT_ROOT, "r"))
		return fp;
	else if (fp = fopen(DEFAULT_FILE, "r"))
		return fp;
        else if (NULL !=
		 (cp = findfile(DEFAULT_ROOT, getenv("PATH"), (char **)0)) &&
		 (NULL != (fp = fopen(cp,"r"))))
		       return fp;
	        
/** this replaced by the above **
	else if (fp = fopenp(DEFAULT_ROOT, "r", NULL))
		return fp;
**/
	else {
#  ifdef GEMDOS
		/* 
		 * if we do not have any better information, then
                 * first try to glimpse screen size from the env
                 * failing which
		 * we will try to glimpse screen sizes from Line-A variables
		 */
                char *rows, *cols = getenv("COLUMNS");

                if(!(rows = getenv("LINES")))
                    rows = getenv("ROWS");
                
                if((!rows) || (!cols))
                    linea0();

		strcpy (bp, ":co#");
                if(!cols)
                {
                    (void) _ultoa((unsigned long) (V_CEL_MX + 1),
                                  bp + sizeof(":co#") - 1, 10);
                }
                else
                    strcat(bp, cols);
                
		strcat (bp, ":li#");
                if(!rows)
                {
                    (void) _ultoa((unsigned long) (V_CEL_MY + 1),
                                  bp + strlen(bp), 10);
                }
                else
                    strcat(bp, rows);
                
		strcat (bp, term_default);
#  endif
		return (FILE *) NULL;
	}
# else
    return(fopen(DEFAULT_FILE,"r"));
# endif
}
#if 0  /* this is not used anymore */

/*
 *  INTERNAL FUNCTION
 *
 *	gotcha   test to see if entry is for specified terminal
 *
 *  SYNOPSIS
 *
 *	gotcha(bp,name)
 *	char *bp;
 *	char *name;
 *
 *  DESCRIPTION
 *
 *	Tests to see if the entry in buffer bp matches the terminal
 *	specified by name.  Returns TRUE if match is detected, FALSE
 *	otherwise.
 *
 */

/*
 *  PSEUDO CODE
 *
 *	Begin gotcha
 *	    If buffer character is comment character then
 *		Return FALSE since remainder is comment
 *	    Else
 *		Initialize name scan pointer.
 *		Compare name and buffer until end or mismatch.
 *		If valid terminators for both name and buffer strings
 *		    Return TRUE since a match was found.
 *		Else
 *		    Find next non-name character in buffer.
 *		    If not an alternate name separater character
 *			Return FALSE since no more names to check.
 *		    Else
 *			Test next name and return results.
 *		    End if
 *		End if
 *	    End if
 *	End gotcha
 *
 */

static int gotcha(bp,name)
char *bp;
char *name;
{
    char *np;
 
    if (*bp == '#') {
	return(FALSE);
    } else {
	np = name;
	while (*np == *bp && *np != '\0') {np++; bp++;}
	if (*np == '\0' && (*bp == '\0' || *bp == '|' || *bp == ':')) {
	    return(TRUE);
	} else {
	    while (*bp != '\0' && *bp != ':' && *bp != '|') {bp++;}
	    if (*bp != '|') {
		return(FALSE);
	    } else {
		return(gotcha(++bp,name));
	    }
	}
    }
}

#ifdef DGK
# ifdef MSDOS
# define PATHSEP ';'
# endif
# ifdef GEMDOS
# define PATHSEP ','
# endif

/* Follow the PATH, trying to fopen the file.  Takes one additional
 * argument which can be NULL.  Otherwise this argument gets filled
 * in the full path to the file.  Returns as does fopen().
 */

/* On Atari ST use library routine findfile() instead */

static FILE *
fopenp(name, mode, pathname)
char *name, *mode, *pathname;
{
	char buffer[BUFSIZ], *buf, *bufp, *pathp, lastch;
	FILE *fp;
	__EXTERN char *getenv __PROTO((const char *));

	/* If pathname is given, use it instead of buf so the calling
	 * process knows the path we found name under
	 */
	if (pathname)
		buf = pathname;
	else
		buf = buffer;

	strcpy(buf, name);
	pathp = getenv("PATH");
	while (pathp && *pathp) {
		bufp = buf;
		while (*pathp && *pathp != PATHSEP)
			lastch = *bufp++ = *pathp++;
		if (lastch != '\\')
			*bufp++ = '\\';
		strcpy(bufp, name);
		if (fp = fopen(buf, mode))
			return fp;
		if (*pathp)
			pathp++;
	}
	return NULL;
}
#endif
#endif /* 0 */
