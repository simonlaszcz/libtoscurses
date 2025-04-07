/*****************************************************************************
/*  FILE:		wpage.c
/*  DATE:		August 1988.
/*  AUTHOR:		Richard A. Culshaw.
/*  DESCRIPTION:	A small demonstration program using the widgets.
/* DISCLAIMER:		This file is deemed to be public-domain, on the simple
/*			provisos that this section remains in this file and
/*			that code using it do not do so for monetary gain.
/*			Neither the author, nor the authors employees at the
/*			time of developing this code, accept any liability or
/*			responsibility for the use, abuse or misuse of this
/*			code.
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <widget.h>

WIDGET	title, nlcmd, npcmd, qcmd, finput, gfile, frac;
char	filename[14];
char    string[20];
int	nolines;
int	filepresent;
FILE	*f;
long	size, ftell();

int readline ()
{
    int   i;
    int	  space;
    char  buffer [80];
    int   count;
    int   ch;

	count = 0;
  	while ((count < 79) && (ch = getc(f)) && (ch != '\n') && (ch != EOF))
	    if (ch == '\t') {
		space = 8 - count % 8;
		for (i=0; i<space; i++)
		    buffer [count++] = ' ';
 	    }
	    else
	    	buffer [count++] = ch;
        buffer [count] = '\0';
        if ((count > 0) || (ch == '\n'));
	    report (buffer);
	return (ch); 
}

changefrac ()
{
    long  cur;
    char  data[5];

    cur = ftell (f);
    sprintf (data, "%ld%%", cur*100L/size);
    changelblwidget (frac, data, RIGHTJUST|NOHIGH);
}


nextline ()
{
    int ch;

    highlight (nlcmd);
    ch = readline ();
    dehighlight (nlcmd);
    if (ch == EOF) {
	deactivate (npcmd, BLANK);
	deactivate (nlcmd, BLANK);
    }
    changefrac ();
    widgetinput ();
}

nextpage ()
{
    int   line  = 0;
    int	  ch;

    highlight (npcmd);
    cleartextwindow ();

    while ((line++ != nolines) && ((ch = readline()) != EOF)) 
	;
	
    dehighlight (npcmd);
    if (ch == EOF) {
	deactivate (npcmd, BLANK);
	deactivate (nlcmd, BLANK);
    }

    changefrac ();
    widgetinput ();
}

quit ()
{
    endwidgets ();
}

getfile ()
{
    int i;

    if (!filepresent) {
        highlight (gfile);
    	killwidget (title);
    	changelblwidget (frac, "", RIGHTJUST|NOHIGH);
    	finput = mkinpwidget ("file", (int)NULL, filename, 14, 0, 0, EXEC);
    	killwidget (finput);
    }
    else
	filepresent = FALSE;
    for (i=5; i<=20; i++)
   	string [i] = filename [i-5]; 
    dehighlight (gfile);
    f = fopen (filename, "r");
    if (f == NULL) {
	cleartextwindow ();	
	report ("File does not exist. Try again");
	getfile ();
    }
    title = mklblwidget (string, CENTRE, 0, 0);
    fseek (f, 0L, 2);
    size = ftell (f);
    rewind (f);
    activate (nlcmd);
    activate (npcmd);
    nextpage ();
}

main (argc, argv)
int	argc;
char	*argv[];
{
    initialisewidgets ();
    if (argc > 1) {
	strcpy (filename, argv[1]);
	filepresent = TRUE;
    }
    strcpy (string, "File=");
    qcmd = mkcmdwidget ("Quit", 'q', quit, 0);
    nlcmd = mkcmdwidget ("Next line", 'n', nextline, 0);
    npcmd = mkcmdwidget ("Next page", 'N', nextpage, 0);
    gfile = mkcmdwidget ("Get file", 'f', getfile, 0);
    frac = mklblwidget ("", CENTRE, 4, 0);
    nolines = opentextwindow (0, HORIZONTAL);
    getfile ();
}	
