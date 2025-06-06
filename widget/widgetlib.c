/*****************************************************************************
*  FILE:		widgetlib.c
*  DATE:		August 1988.
*  AUTHOR:		Richard A. Culshaw.
*  DESCRIPTION:	Contains the code to manipulate the widget structures
*			and the i/o. It forms a single library that should
*			be linked in with all code using these widgets.
* DISCLAIMER:		This file is deemed to be public-domain, on the simple
*			provisos that this section remains in this file and
*			that code using it do not do so for monetary gain.
*			Neither the author, nor the authors employees at the
*			time of developing this code, accept any liability or
*			responsibility for the use, abuse or misuse of this
*			code.
*****************************************************************************/

#include <widget.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


__EXTERN void *malloc __PROTO((size_t));

int		textwindowdefined = FALSE;
int    		widx, widy;	
int		maxwidy;
int    		scry;
int    		curid;
int		sizescreg;
int		screenborder = 0;

WINDOW    *screen;
WINDOW    *screenbox;
WINDOW    *edge1;
WINDOW	  *edge2;


void initialisewidgets ()
/* sets up screen and initialises global variables */
{
    int i;
    __EXTERN void endwidgets __PROTO((void));

    widx = widy = scry = 0;
    curid = 1;
    maxwidy = LINES - 3;
    initscr ();
    crmode ();
    noecho ();
    cmdlist = NULLCMD;
    lbllist = NULLLBL;
    tgllist = NULLTGL;
    inplist = NULLINP;
    actlist = NULLCH;
    for (i=0; i<NSIG; i++)
	signal (i, endwidgets);
}

void drawtextwindow (y)
/* draws the text window and draws a box around it */
int y;
{
    int i;

    if ((screenborder & VERTICAL) && (screenborder & HORIZONTAL)) {
        box (screenbox, '|', '-');
    	mvwaddch (screenbox, 0, 0, '.');
    	mvwaddch (screenbox, 0, COLS-2, '.');
    	mvwaddch (screenbox, LINES-y-1, 0, '`');
    	mvwaddch (screenbox, LINES-y-1, COLS-2, '\'');
    	touchwin (screenbox);
        wrefresh (screenbox);
    }
    else if (screenborder & HORIZONTAL) {
	waddstr (edge1, "-------------------------------------------------------------------------------");
        waddstr (edge2, "-------------------------------------------------------------------------------");
	wrefresh (edge1);
	wrefresh (edge2);
    }
    else if (screenborder & VERTICAL) {
	for (i=0; i<=sizescreg; i++) {
	    mvwaddch (edge1, i, 0, '|');
	    mvwaddch (edge2, i, 0, '|');
   	}
	wrefresh (edge1);
	wrefresh (edge2);
    }
    touchwin (screen);
    wrefresh (screen);
}

int opentextwindow (position, border)
/* define the text window from current position down + position * widget depth*/
int    position;
int    border;
{
    if ((position >= 0) && (!textwindowdefined) && (widy + 3*position < 21)) {
    	maxwidy = widy + position * 3 + 3;
	if ((border & VERTICAL) && (border & HORIZONTAL)) {
            screenbox = newwin (LINES-maxwidy, COLS-1, maxwidy, 0);
	    screen = newwin (LINES-2-maxwidy, COLS-3, maxwidy+1, 1);
	    sizescreg = LINES-2-maxwidy;
        }
	else if (border & HORIZONTAL) {
	    screen = newwin (LINES-2-maxwidy, COLS-1, maxwidy+1, 0);
    	    edge1 = newwin (1, COLS-1, maxwidy, 0);
            edge2 = newwin (1, COLS-1, LINES-1, 0);
	    sizescreg = LINES-2-maxwidy;
        }
	else if (border & VERTICAL) {
	    screen = newwin (LINES-maxwidy, COLS-3, maxwidy, 1);
	    edge1 = newwin (LINES-maxwidy, 1, maxwidy, 0);
	    edge2 = newwin (LINES-maxwidy, 1, maxwidy, COLS-2);
	    sizescreg = LINES-maxwidy;
        }
	else {
	    screen = newwin (LINES-maxwidy, COLS-1, maxwidy, 0);
	    sizescreg = LINES-maxwidy;
        }
        screenborder = border;
        scrollok (screen, TRUE);
	drawtextwindow (maxwidy);
        wmove (screen, scry, 0);
	wrefresh (screen);
	textwindowdefined = 1;
	return sizescreg;
    }
}

int addtochlist (int ch, int id, WIDGETTYPE type)
{
    struct chentry  *chlist;
    struct chentry  *newentry;
    /* search list to see char already entered in the list */	
    chlist = actlist;
    if (chlist != NULLCH) {
	while (chlist -> next != NULLCH) 
	   if (chlist -> ch == ch)
		return FALSE;           /* char exists => error */
	    else
		chlist = chlist -> next;
    	newentry = (struct chentry*)malloc((size_t)sizeof(struct chentry));
	chlist -> next = newentry;
    }
    else {
    	newentry = (struct chentry*)malloc((size_t)sizeof(struct chentry));
	actlist = newentry;
    }
    /* char not there so add to list */
    newentry -> ch = ch;
    newentry -> id = id;
    newentry -> type = type;
    newentry -> next = NULLCH;
    return TRUE;
}


WIDGETTYPE widgettype (WIDGET ptr)
{
    struct cmdwid    *clist;
    struct tglwid    *tlist;
    struct lblwid    *llist;
    struct inpwid    *ilist;

    /* is it a command widget ? */
    for (clist = cmdlist; clist != NULLCMD; clist = clist -> next)       
    	if ((clist != NULLCMD) && (clist -> id == ptr))
    		return CMD;

    /* is it a toggle widget ? */
    for (tlist = tgllist; tlist != NULLTGL; tlist = tlist -> next)       
    	if ((tlist != NULLTGL) && (tlist -> id == ptr))
    		return TGL;

    /* is it a label widget ? */
    for (llist = lbllist; llist != NULLLBL; llist = llist -> next)       
    	if ((llist != NULLLBL) && (llist -> id == ptr))
    		return LBL;

    /* is in an input widget ? */
    for (ilist = inplist; ilist != NULLINP; ilist = ilist -> next)       
    	if ((ilist != NULLINP) && (ilist -> id == ptr))
    		return INP;

    /* doesn't exist, return error */
    return FALSE;             
}

void boxwidget (widget, length)
WINDOW	*widget;
int	length;
{
    int i;

    wmove (widget, 0, 1);
    for (i=1; i < length; i++)
       	waddch (widget, '-');
    wmove (widget, 2, 1);
    for (i=1; i < length; i++)
       	waddch (widget, '-');
    mvwaddch (widget, 1, 0, '|');
    mvwaddch (widget, 1, length, '|');
    mvwaddch (widget, 0, 0, '.');
    mvwaddch (widget, 0, length, '.');
    mvwaddch (widget, 2, 0, '`');
    mvwaddch (widget, 2, length, '\'');
}

void drawcmdwidget (widget, info, ch, length, boolean)
WINDOW	*widget;
char	info[];
int	ch;
int	length;
int	boolean;
{
    boxwidget (widget, length-1);
    wmove (widget, 1, 1);
    wstandout (widget);
    waddch (widget, ch);
    if (!boolean)
        wstandend (widget);
    wprintw (widget, ":%s", info);
    if (boolean)
        wstandend (widget);
    wrefresh (widget);
}

void drawlblwidget (widget, info, pos, difference, space, boolean)
WINDOW	*widget;
char	info[];
int	pos;
int	difference;
int	space;
int	boolean;
{
    boxwidget (widget, space+1);

    /* work out justification */
    if ((pos & LEFTJUST) || (difference == 0))
	wmove (widget, 1, 1);
    else if (pos & RIGHTJUST)
	wmove (widget, 1, 1+difference);
    else if (pos & CENTRE)
	wmove (widget, 1, difference / 2 + 1);
    if (boolean)
        wstandout (widget);
    wprintw (widget, "%s", info);
    if (boolean)
        wstandend (widget);
    wrefresh (widget);
}
  

void drawtglwidget (widget, info, tgl, toggle, length, boolean)
WINDOW	*widget;
char	info[];
char	tgl[];
int	toggle;
int	length;
int	boolean;
{
    boxwidget (widget, length-1);
    wmove (widget, 1, 1);
    wstandout (widget);
    wprintw (widget, "%c", toggle);
    if (!boolean)
        wstandend (widget);
    wprintw (widget, ":%s:", info);
    if (!boolean)
    	wstandout (widget);
    wprintw (widget, "%s", tgl);
    wstandend (widget);
    wrefresh (widget);
}

void drawinpwidget (widget, info, ch, length, boolean, exec)
WINDOW	*widget;
char	info[];
int	ch;
int	length;
int	boolean;
{
    boxwidget (widget, length-1);
    wmove (widget, 1, 1);
    if (boolean)
    	wstandout (widget);
    if (exec)
 	wprintw (widget, "%s", info);
    else
    	wprintw (widget, "%c:%s", ch, info);
    if (boolean)
    	wstandend (widget);
    waddch (widget, '>');
    wrefresh (widget);
}

void screenrefresh ()
/* clear the screen and redraw all the widgets and the text screen if present */
{
    struct cmdwid *clist;
    struct lblwid *llist;
    struct tglwid *tlist;
    struct inpwid *ilist;

    /* clear the entire screen */
    clear();
    refresh();

    /* redraw all the command widgets */
    clist = cmdlist;
    while (clist != NULLCMD) {
	touchwin (clist -> widget);
	wrefresh (clist -> widget);
	clist = clist -> next;
    }

    /* redraw all the label widgets */
    llist = lbllist;
    while (llist != NULLLBL) {
	touchwin (llist -> widget);
	wrefresh (llist -> widget);
	llist = llist -> next;
    }

    /* redraw all the toggle widgets */
    tlist = tgllist;
    while (tlist != NULLTGL) {
	touchwin (tlist -> widget);
	wrefresh (tlist -> widget);
	tlist = tlist -> next;
    }

    /* redraw all the input widgets */
    ilist = inplist;
    while (ilist != NULLINP) {
	touchwin (ilist -> widget);
	wrefresh (ilist -> widget);
	ilist = ilist -> next;
    }

    /* redraw text window if present */
    if (textwindowdefined) {
	touchwin (screen);
	touchwin (screenbox);
    	wrefresh (screenbox);
    	wrefresh (screen);
    }
}

WIDGET widgetinput ()
/* scan standard input for chars and act on them */
{
    struct cmdwid *list;	
    struct chentry *chlist;
    char	  ch;
    int		  i;
    int		  j;

    for (;;)
    {
    	ch = getchar ();

	if (ch == '\014') {
	    screenrefresh ();
	    continue;
	}
    
	chlist = actlist;
	while ((chlist != NULLCH) && (chlist -> ch != ch))
	    chlist = chlist -> next; 
	
	if (chlist == NULLCH)
	    continue;

    	if (chlist -> type == 1) {  /* does char activate a command widget ? */
    	    for (list = cmdlist; list != NULLCMD; list = list->next)
    		if (list -> id == chlist -> id)
		    if (list -> active)
    	 	     	return (*(list -> func))();
    	}
    	else if (chlist -> type == 2) /* does char activate a toggle widget ?*/
    	    togglewidget (chlist -> id);
    	else 
    	    return (getinput (chlist -> id)); /* must activate input widget */	
    }
}

    
void togglewidget (WIDGET id)
{
    struct tglwid *list;
    int 	  i=0;
    int 	  x,y;
    int 	  pos;


    list = tgllist;	
    /* find toggle widget with given id */
    while (list -> id != id)
    	list = list -> next;

    if (!list -> active)
	return;

    /* increase its toggle value */
    pos = ++(list -> cur);
    if (pos == list -> total)   /* do we need to wrap round values */
    {	pos = 0;
    	list -> cur = 0;
    }

    /* output new toggle value */
    wstandout (list -> widget);
    mvwaddstr (list -> widget, 1, list -> xtgl, list -> tgl[pos]);
    wstandend (list -> widget);
    getyx (list -> widget, y, x);
    for (; x < list -> length-1; x++)
    	waddch (list -> widget, ' ');
    wrefresh (list -> widget);
    if (textwindowdefined)
    	wrefresh (screen);
}


int getinput (WIDGET id)
{
    struct inpwid *list;	
    int 	  i = 0;
    int		 j;
    int		 cursor = 0;
    int		 index = 0;
    WINDOW 	 *widgy;
    int 	 max;
    char	 ch = '\0';
    int		 flag = 0;
    int		 sofi;

    highlight (id);
    list = inplist;
    /* find the input widget with the given id */
    while (list -> id != id)
    	list = list -> next;

    if (!list -> active)
	/* return; */
	return (int)NULLWIDGET;		/* is this correct? ++jrb */

    widgy = list -> widget;
    sofi = list -> sofi;
    max = list -> length - sofi - 1;
    wmove (widgy, 1, sofi);
    wrefresh (widgy);

    /* repeatidly get chars from stdin until carriage return is pressed */
    while ((ch = getchar()) != '\n') {
    	if ((ch > 31) || (ch == '\025') || (ch == '\014'))
    	{
    	switch (ch) {
	
	/* delete ? */
    	case '\177' : if ((!flag) && (cursor > 0)){ /* chars don't spill over */
    			   cursor -= 1;
    			   mvwaddch (widgy, 1, cursor + sofi, ' ');
    			   wmove (widgy, 1, cursor + sofi);
    			   index -= 1;
    			   wrefresh (widgy);
    			   break;
    		      }
    		      else if (cursor > 0){  /* chars spill over */
    		          index -= 1;
    			  if (index == max-1) {
    			      flag = 0;
    			      mvwaddch (widgy, 1, sofi-1, '>');
    			  }
    			  for (j=0; j < max-1; j++)
    			      mvwaddch (widgy, 1, j+sofi, list -> input [index-max+j+1]);
    			      wmove (widgy, 1, cursor + sofi);
    			      wrefresh (widgy);
    			      break;
    		      }

    		      else        /* nothing to delete ! */	
    			  break;

	/* ^U pressed (delete all) */
    	case '\025' : index = cursor = 0;
    		      wmove (widgy, 1, sofi);
    		      for (j=0; j < max; j++)
    			  waddch (widgy, ' ');
    		      mvwaddch (widgy, 1, sofi-1, '>');
    		      wrefresh (widgy);
    		      break;

	/* ^L pressed (screen refresh) */
	case '\014' : screenrefresh ();
    		      wmove (widgy,1,cursor+sofi);
		      wrefresh (widgy);
		      break;

	/* default = all other chars except remaining control chars */
    	default :    if (index < list -> lofs) {  /* space still remaining ? */
    			 list -> input [index++] = ch;
    	          	 if (cursor == (max-1)) { /* about to spill over ? */
    			    mvwaddch (widgy, 1, sofi-1, '<');
    			    for (j=0; j < max-2; j++)
    				mvwaddch (widgy, 1, j+sofi, list ->input [index-max+j+1]);
    			    mvwaddch (widgy, 1, cursor+sofi-1, ch);
    			    flag = 1;
    		  	  }			
    		  	  else    /* not spilt over */
    			      mvwaddch (widgy, 1, (cursor++) +sofi, ch);
    		  	  wrefresh (widgy);
    		  	  break;
    		          }
    		     else         /* no room left ! */
    			 break;
    		     }
    	}
        }		
    /* terminate input */
    list -> input [index] = '\0';

    /* clean up input line */
    wmove (widgy, 1, sofi);
    for (j=0; j < max; j++)
    	waddch (widgy, ' ');
    mvwaddch (widgy, 1, sofi-1, '>');
    wrefresh (widgy);
    if (textwindowdefined)
    	wrefresh (screen);
    dehighlight (id);
    return id;
}

WIDGET mkcmdwidget (info, acpt, func, line)
/* make a new command widget */
char   info[];
int    acpt;
int    (*func)();
int    line;
{
    struct cmdwid 	*list;
    struct cmdwid 	*temp;
    int 		length = 0;

    while (info [length++] != '\0')   /* work out length of command */
    	;	
    length += 3;
    if ((line == 1) || ((length + widx) > COLS-1) || widx == COLS-1)
    	if (widy + 3 == maxwidy) /* go onto next line */
    	    return NULLWIDGET;
	else {
    	    widy += 3;
    	    widx = 0;          
	}
    if (!addtochlist (acpt, curid, 1))  /* activation char in use ? */
    	return NULLWIDGET;
    
    /* find end of list and allocate memory for new entry */
    temp = cmdlist; 
    if (temp != NULLCMD) {
    	while (temp -> next != NULLCMD)
    	    temp = temp -> next;
    	list = (struct cmdwid *)malloc((size_t)sizeof(struct cmdwid));
    	temp -> next = list;
    }
    else {
    	list = (struct cmdwid *)malloc((size_t)sizeof(struct cmdwid));
    	cmdlist = list;
    }

    /* create and display new widget */
    list -> widget = newwin (3, length, widy, widx);

    drawcmdwidget (list -> widget, info, acpt, length, FALSE);

    /* assign information to be withheld */
    list -> x = widx;
    list -> y = widy;
    list -> id = curid;
    strcpy (list -> msg ,info);
    list -> func = func;
    list -> acpt = acpt;
    list -> length = length;
    list -> next = NULLCMD;
    list -> active = TRUE;
    curid += 1;
    widx += length;
    if (textwindowdefined)
    	wrefresh (screen);
    return (curid-1);
}

WIDGET mklblwidget (info, pos, space, line)
/* make a new label widget */
char    info[];
int     pos;
int     space;
int     line;
{
    struct lblwid	*list;
    struct lblwid	*temp;
    int 		length = 0;
    int 		difference;

    if (!((pos & CENTRE) || (pos & LEFTJUST) || (pos & RIGHTJUST)))
	pos = pos|LEFTJUST;

    /* calculate length of label widget */
    while (info [length++] != '\0')    
    	;	
    length -= 1;
    if ((space < length) && (space > 0))    /* not given enough room so error */
    	return NULLWIDGET; 
    
    if (space <= 0)
        if ((line == 1) || (widx == COLS-1))
	    space += COLS - 3;
	else
	    space += COLS - 3 - widx;

    difference = space - length;

    if ((line == 1) || ((space + 2 + widx) > COLS-1) || (widx == COLS-1))
    /* not enough room on existing line so go onto next */
    	if (widy + 3 == maxwidy)
    	    return NULLWIDGET;
	else {
	    widx = 0;
	    widy += 3;
 	}

    /* find end of list and alllocate memory for new entry */
    temp = lbllist;
    if (temp != NULLLBL) {
    	while (temp -> next != NULLLBL)
    	    temp = temp -> next;
    	list = (struct lblwid *)malloc((size_t)sizeof(struct lblwid));
    	temp -> next = list;
    }
    else {
    	list = (struct lblwid *)malloc((size_t)sizeof(struct lblwid));
    	lbllist = list;
    }

    /* create new and display new widget */
    list -> widget = newwin (3, space+2, widy, widx);

    drawlblwidget (list -> widget, info, pos, difference, space, !(pos & NOHIGH));

    /* assign information to be withheld */
    list -> x = widx;
    list -> y = widy;
    list -> id = curid;
    strcpy (list -> msg ,info);
    list -> pos = pos;
    list -> length = space+2;
    list -> difference = difference;
    list -> next = NULLLBL;
    curid += 1;
    widx += space+2;
    if (textwindowdefined)
        wrefresh (screen);
    return (curid - 1);
}

WIDGET mktglwidget (info, num, tgl, toggle, line)
/* make a new toggle widget */
char  info[];
int   num;	
char  *tgl[10];
int   toggle;
int   line;
{
    int 		length = 0;
    struct tglwid	*list;
    struct tglwid	*temp;
    int 		xtgl;
    int 		max = 0;
    int 		i, j, x;
    
    /* calculate maximum length of toggle */
    for (i=0; i < num; i++)  
    	{
    	for (j=0, x=0; tgl[i][j] != '\0'; j++)
       	    x++;
    	if ((x-1)>max)
    	    max = x-1;
    	}
    
    while (info [length++] != '\0') 
    	;	
    xtgl = length + 3;
    length = length + max + 5;

    if ((line == 1) || ((length + widx) > COLS-1) || (widx == COLS-1))
        /* not enough room on existing line so go onto next */
    	if (widy + 3 == maxwidy)
    	    return NULLWIDGET;
	else {
	    widx = 0;
	    widy += 3;
	}

    /* add char to activation list if not already in use */
    if (!addtochlist (toggle, curid, 2)) 
    	return NULLWIDGET;

    /* find end of list and allocate memory for new entry */
    temp = tgllist;
    if (temp != NULLTGL) {
    	while (temp -> next != NULLTGL)
    	    temp = temp -> next;
    	list = (struct tglwid *)malloc((size_t)sizeof(struct tglwid));
    	temp -> next = list;
    }
    else {
    	list = (struct tglwid *)malloc((size_t)sizeof(struct tglwid));
    	tgllist = list;
    }

    /* create and display new widget */
    list -> widget = newwin (3, length, widy, widx);

    drawtglwidget (list -> widget, info, tgl[0], toggle, length, FALSE);

    /* assign information to be withheld */
    list -> x = widx;
    list -> y = widy;
    list -> id = curid;
    strcpy (list -> msg ,info);
    list -> xtgl = xtgl;
    list -> total = num;
    for (i=0; i<num; i++) 
	strcpy (list -> tgl[i], tgl[i]);
    list -> cur = 0;
    list -> toggle = toggle;
    list -> length = length;
    list -> next = NULLTGL;
    list -> active = TRUE;
    curid += 1;
    widx += length;
    if (textwindowdefined)
        wrefresh (screen);
    return (curid-1);
}

WIDGET mkinpwidget (info, acpt, input, lofs, length, line, exec)
/* make a new input widget */
char	info[];
int     acpt;
char    input[];
int     lofs;
int     length;
int     line;
int	exec;
{
    struct inpwid	*list;
    struct inpwid	*temp;
    int			count = 0;
    int			offset;

    if (exec)
      	offset = 3;
    else
	offset = 5;

    while (info [count++] != '\0')
	;

    if (length <= 0)
	if ((line == 1) || (widx == COLS-1))
	    length += COLS-offset-count;
	else
	    length += COLS-offset-widx-count;
    length += offset-1+count;
    if ((line == 1) || ((length + widx) > COLS-1) || (widx == COLS -1))
        /* if no room on existing line go onto next */
    	if (widy + 3 == maxwidy)
    	    return NULLWIDGET;
	else {
	    widx = 0;
	    widy += 3;
	}

    /* add activation char to list if not already in use */
    if (!addtochlist (acpt, curid, 3)) 
    	return NULLWIDGET;

    /* find end of list and allocate memory for new entry */
    temp = inplist;
    if (temp != NULLINP) {
    	while (temp -> next != NULLINP)
    	    temp = temp -> next;
    	list = (struct inpwid *)malloc((size_t)sizeof(struct inpwid));
    	temp -> next = list;
    }
    else {
    	list = (struct inpwid *)malloc((size_t)sizeof(struct inpwid));
    	inplist = list;
    }

    /* create and display new widget */
    list -> widget = newwin (3, length, widy, widx);

    drawinpwidget (list -> widget, info, acpt, length, FALSE, exec);

    /* assign information to be withheld */
    list -> x = widx;
    list -> y = widy;
    list -> id = curid;
    strcpy (list -> msg ,info);
    list -> acpt = acpt;
    list -> input = input;
    list -> lofs = lofs;

    if (exec)
	list -> sofi = count+1;
    else
    	list -> sofi = count+3;

    list -> length = length;
    list -> next = NULLINP;
    list -> exec = exec;
    list -> active = TRUE;
    curid += 1;
    widx += length;
    if (textwindowdefined)
    	wrefresh (screen);
    if (exec)
	getinput (curid-1);
    return (curid-1);
}

void cleartextwindow ()
{
    werase (screen);
    wrefresh (screen);
    scry = 0;
}

void report (info)
/* place given info into data screen */
char    *info;
{
    int i, count = 0;

    if (textwindowdefined) {
    	if (scry == sizescreg)    /* scroll window if reached bottom */
    	{
    	    scroll (screen);
    	    scry -= 1;
    	}
	for (i=0; info[i] != '\0'; i++)
	    if (info[i] == '\n')
		count += 1;

    	mvwaddstr (screen, scry, 0, info);
    	scry += 1+count;
    	wrefresh (screen);
    }
}

int tsttglwidget (WIDGET ptr)
{
    struct tglwid *list;
    int		  i=0;

    /* locate the record of the given toggle widget pointer */
    list = tgllist;
    while ((list != NULLTGL) && (list -> id != ptr)) 
    	list = list -> next;

    /* if it exists return index value else return error FALSE */
    if (list -> id == ptr)
    	return list -> cur;
    else
    	return FALSE;
}

void deletechentry (ch)
/* remove the entry from character list for killed widget */
int ch;
{
    struct chentry    *chlist;
    struct chentry    *previous;
   
    chlist = actlist;
    if (chlist -> ch == ch) {        /* is required entry at head of list ? */
	actlist = chlist -> next;    
	free ((char*)chlist);        /* deallocate memory for deleted entry */
    }
    else {
	while (chlist -> ch != ch) {  /* locate required entry in the list */
	    previous = chlist;
	    chlist = chlist -> next;
	}
	previous -> next = chlist -> next;  /* remove entry by skiping it */
	free ((char*)chlist);        /* deallocate memory for deleted entry */
    }
}

void killcmdwidget (WIDGET ptr)
{
    struct cmdwid    *clist;
    struct cmdwid    *previous;

    clist = cmdlist;
    if (clist -> id == ptr) {         /* required widget at head of list ? */
	cmdlist = clist -> next;
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	deletechentry (clist -> acpt);
	if ((clist -> x + clist -> length) == widx)   /* if last widget added */
		widx -= clist -> length;              /* pull back widx       */
	free ((char*)clist);
    }
    else {
	while (clist -> id != ptr) {    /* find widget in list */
	   previous = clist;
	   clist = clist -> next;
	}
	previous -> next = clist -> next;   /* skip unrequired widget in list */
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	deletechentry (clist -> acpt);
	if ((clist -> x + clist -> length) == widx)   /* if last widget added */
		widx -= clist -> length;              /* pull back widx       */
	free ((char*)clist);
    }
}

void killlblwidget (WIDGET ptr)
{
    struct lblwid    *clist;
    struct lblwid    *previous;

    clist = lbllist;
    if (clist -> id == ptr) {         /* is widget at head of list ? */
	lbllist = clist -> next;
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	if ((clist -> x + clist -> length) == widx)  /* pull back widx if */
		widx -= clist -> length;             /* last widget added */
	free ((char*)clist);
    }
    else {
	while (clist -> id != ptr) {    /* find widget in linked list */
	   previous = clist;
	   clist = clist -> next;
	}
	previous -> next = clist -> next;   /* skip widget in list */
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	if ((clist -> x + clist -> length) == widx)  /* pull back widx if */
		widx -= clist -> length;             /* last widget added */
	free ((char*)clist);
    }
}

void killtglwidget (WIDGET ptr)
{
    struct tglwid    *clist;
    struct tglwid    *previous;

    clist = tgllist;
    if (clist -> id == ptr) {       /* is widget at head of list ? */
	tgllist = clist -> next;
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	deletechentry (clist -> toggle);
	if ((clist -> x + clist -> length) == widx)  /* pull back widx if */
		widx -= clist -> length;             /* last widget added */
	free ((char*)clist);
    }
    else {
	while (clist -> id != ptr) {    /* find widget in linked list */
	   previous = clist;
	   clist = clist -> next;
	}
	previous -> next = clist -> next;   /* skip widget in list */
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	deletechentry (clist -> toggle);
	if ((clist -> x + clist -> length) == widx)   /* pull back widx if */
		widx -= clist -> length;              /* last widget added */
	free ((char*)clist);
    }
}

void killinpwidget (WIDGET ptr)
{
    struct inpwid    *clist;
    struct inpwid    *previous;

    clist = inplist;
    if (clist -> id == ptr) {        /* is widget at head of list ? */
	inplist = clist -> next;
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	deletechentry (clist -> acpt);
	if ((clist -> x + clist -> length) == widx)   /* pull back widx if */
		widx -= clist -> length;              /* last widget added */
	free ((char*)clist);
    }
    else {
	while (clist -> id != ptr) {   /* locate widget in linked list */
	   previous = clist;
	   clist = clist -> next;
	}
	previous -> next = clist -> next;   /* skip entry in linked list */
	werase (clist -> widget);
	wrefresh (clist -> widget);
	delwin (clist -> widget);
	deletechentry (clist -> acpt);
	if ((clist -> x + clist -> length) == widx)   /* pull back widx if */
		widx -= clist -> length;              /* last widget added */
	free ((char*)clist);
    }
}
	
int killwidget (WIDGET ptr)
{
    switch (widgettype (ptr)) {
        case CMD : killcmdwidget (ptr);    /* command widget */
		 home ();
		 return TRUE;
	case TGL : killtglwidget (ptr);    /* toggle widget */
		 home ();
		 return TRUE;
	case LBL : killlblwidget (ptr);    /* label widget */
		 home ();
		 return TRUE;
	case INP : killinpwidget (ptr);    /* input widget */
		 home ();
		 return TRUE;
	case FALSE : return FALSE;           /* doesn't exist */
    }
}

int chactive (WIDGET ptr, int boolean, int blank)
{
    struct cmdwid *clist;
    struct tglwid *tlist;
    struct inpwid *ilist;
    int		  i;

    switch (widgettype (ptr)) {
	case CMD: for (clist = cmdlist; clist != NULLCMD; clist = clist -> next)
		  if ((clist != NULLCMD) && (clist -> id == ptr)) {
			if ((!boolean) && (blank)) {
			    wmove (clist->widget, 1, 1);
			    for (i=0; i < clist->length-2; i++)
				waddch (clist->widget, ' ');
			    wrefresh (clist->widget);
			}
		  	if (textwindowdefined)
	    		    wrefresh (screen);
			return(clist -> active = boolean);
		  }
		  break;
	case TGL: for (tlist = tgllist; tlist != NULLTGL; tlist = tlist -> next)
		  if ((tlist != NULLTGL) && (tlist -> id == ptr)) {
			if ((!boolean) && (blank)) {
			    wmove (tlist->widget, 1, 1);
			    for (i=0; i < tlist->length-2; i++)
				waddch (tlist->widget, ' ');
			    wrefresh (tlist->widget);
			}
		  	if (textwindowdefined)
	    		    wrefresh (screen);
			return(tlist -> active = boolean);
		  }
		  break;
	case INP: for (ilist = inplist; ilist != NULLINP; ilist = ilist -> next)
		  if ((ilist != NULLINP) && (ilist -> id == ptr)) {
			if ((!boolean) && (blank)) {
			    wmove (ilist->widget, 1, 1);
			    for (i=0; i < ilist->length-2; i++)
				waddch (ilist->widget, ' ');
			    wrefresh (ilist->widget);
			}
		  	if (textwindowdefined)
	    		    wrefresh (screen);
			return (ilist -> active = boolean);
		  }
		  break;
	default : return FALSE;
	}
}

int activate (WIDGET ptr)
{
    int boolean;

    boolean = (chactive (ptr, TRUE, FALSE));
    light (ptr, FALSE);
    return boolean;
}

int deactivate (WIDGET ptr, int blank)
{
    return (chactive (ptr, FALSE, blank));
}

int light (WIDGET ptr, int boolean)
{
    struct cmdwid *clist;
    struct tglwid *tlist;
    struct lblwid *llist;
    struct inpwid *ilist;

    switch (widgettype (ptr)) {
	case CMD: for (clist = cmdlist; clist != NULLCMD; clist = clist -> next)
		  if ((clist != NULLCMD) && (clist -> id == ptr)) {
		      if (clist -> active)
                  	drawcmdwidget (clist->widget,clist->msg,clist->acpt,clist->length,boolean);
		  if (textwindowdefined)
	    		wrefresh (screen);
		  return (clist -> active);
		  }
		  break;

	case TGL: for (tlist = tgllist; tlist != NULLTGL; tlist = tlist -> next)
		  if ((tlist != NULLTGL) && (tlist -> id == ptr)) {
		      if (tlist -> active)
    		  	  drawtglwidget (tlist->widget,tlist->msg,tlist->tgl[tlist->cur],tlist->toggle,tlist->length,boolean);
		  if (textwindowdefined)
	    		wrefresh (screen);
		  return (tlist -> active);
		  }
		  break;

	case LBL: for (llist = lbllist; llist != NULLLBL; llist = llist -> next)
		  if ((llist != NULLLBL) && (llist -> id == ptr))
    		  drawlblwidget (llist->widget,llist->msg,llist->pos,llist->difference,llist->length-2,boolean);
		  if (textwindowdefined)
	    		wrefresh (screen);
		  return TRUE;
		  break;

	case INP: for (ilist = inplist; ilist != NULLINP; ilist = ilist -> next)
		  if ((ilist != NULLINP) && (ilist -> id == ptr)) {
		      if (ilist -> active)
    		  	  drawinpwidget (ilist->widget,ilist->msg,ilist->acpt,ilist->length,boolean,ilist->exec);
		  if (textwindowdefined)
	    		wrefresh (screen);
		  return (ilist -> active);
		  }
		  break;

	}
	return FALSE;
}

int highlight (WIDGET ptr)
{
    return (light (ptr, TRUE));
}

int dehighlight (WIDGET ptr)
{
    return (light (ptr, FALSE));
}

void home ()
/* scans the current state of the screen and places the x,y cursor coordinates
   at the end of widget furthest down and to the right */
{
    struct cmdwid *clist = cmdlist;
    struct lblwid *llist = lbllist;
    struct tglwid *tlist = tgllist;
    struct inpwid *ilist = inplist;
    int           x, y;

    x = y = 0;

    /* look through command widgets */
    while (clist != NULLCMD) {
	if (clist -> y > y) {
	    y = clist -> y;
	    x = clist->x + clist->length;
	}
	else if ((clist->x + clist->length > x) && (clist -> y == y)) {
	    x = clist->x + clist->length;
}
   	clist = clist -> next;
    }

    /* look through toggle widgets */
    while (tlist != NULLTGL) {
	if (tlist -> y > y) {
	    y = tlist -> y;
	    x = tlist->x + tlist->length;
	}
	else if ((tlist->x + tlist->length > x) && (tlist -> y == y)) {
	    x = tlist->x + tlist->length;
}
	tlist = tlist -> next;
    }

    /* look through label widgets */
    while (llist != NULLLBL) {
	if (llist -> y > y) {
	    y = llist -> y;
	    x = llist->x + llist->length;
	}
	else if ((llist->x + llist->length > x) && (llist -> y == y)) {
	    x = llist->x + llist->length;
}
	llist = llist -> next;
    }

    /* look through input widgets */
    while (ilist != NULLINP) {
	if (ilist -> y > y) {
	    y = ilist -> y;
	    x = ilist->x + ilist->length;
	}
	else if ((ilist->x + ilist->length > x) && (ilist -> y == y)) {
	    x = ilist->x + ilist->length;
}
	ilist = ilist -> next;
    }
   
    /* update screen x,y coordinates */
    widx = x;
    widy = y;
}

int changelblwidget (WIDGET ptr, char info[], int pos)
{
    struct lblwid *list = lbllist;
    int		  length = 0;

    while ((list != NULLLBL) && (list->id != ptr))
	list = list -> next;

    if (list == NULLLBL)
	return FALSE;

    if (!((pos & CENTRE) || (pos & LEFTJUST) || (pos & RIGHTJUST)))
	pos = pos|LEFTJUST;

    /* calculate length of label widget */
    while (info [length++] != '\0')    
    	;	
    length -= 1;
    if (list->length-2 < length) /* not given enough room so error */
    	return FALSE;
    
    list -> difference = list->length-2 - length;

    strcpy (list -> msg, info);
    list -> pos = pos;
    werase (list->widget);
    drawlblwidget (list->widget,info,pos,list->difference,list->length-2,!(pos & NOHIGH));

    if (textwindowdefined)
	wrefresh (screen);
    return (TRUE);
}

void endwidgets ()
/* tidy up at the end */
{
    struct cmdwid *clist = cmdlist;
    struct lblwid *llist = lbllist;
    struct tglwid *tlist = tgllist;
    struct inpwid *ilist = inplist;

    while (clist != NULLCMD) {
	killcmdwidget (clist -> id);
        clist = cmdlist;
    }

    while (llist != NULLLBL) {
	killlblwidget (llist -> id);
	llist = lbllist;
    }

    while (tlist != NULLTGL) {
	killtglwidget (tlist -> id);
	tlist = tgllist;
    }

    while (ilist != NULLINP) {
	killinpwidget (ilist -> id);
	ilist = inplist;
    }

    killtextwindow ();
    endwin ();
    exit (0);
}

void killtextwindow ()
/* destroy screen and any borders */
{
    if (textwindowdefined) {
	werase (screen);
	wrefresh (screen);
	delwin (screen);
	textwindowdefined = FALSE;
	if ((screenborder & VERTICAL) && (screenborder & HORIZONTAL)) {
	    werase (screenbox);
	    wrefresh (screenbox);
	    delwin (screenbox);
 	}
	else if ((screenborder & VERTICAL) || (screenborder & HORIZONTAL)) {
	    werase (edge1);
	    werase (edge2);
	    wrefresh (edge1);
	    wrefresh (edge2);
	    delwin (edge1);
	    delwin (edge2);
        }
	maxwidy = LINES - 2;
	screenborder = 0;
    }
}
