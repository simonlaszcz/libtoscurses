.ND
.LP
.nr PS 12
.nr VS 13
.LP
.nh
.RP
.TL
A Window based Interactive Programming Utility On Unix:
A Library Package for C programmers
.AU
Richard A. Culshaw
.AI
GEC Hirst Research Centre
Data Systems Division
East Lane
Wembley
Middlesex HA9 7PP
.ND
.KS
.SH
INTRODUCTION

.PP
A Widget is a user definable 
.B "window gadget."
 They allow the user to interact with the program in specifying operations to be carried out. Widgetlib is a set of library functions which allows the user to create and interact with 
.B widgets.
A widget can be one of four types :
.LP
      1) 
.B Command 
Widget 
      2) 
.B Label 
Widget
      3) 
.B Toggle 
Widget
      4) 
.B Input 
Widget

.SH
Command Widgets
.PP
A command widget allows the user to select a function to be carried out, e.g in a paging program one such command could be 'Next Page'. In creating a command widget the programmer must pass it the function to be carried out when the widget is selected (activated). The widget is activated using a unique 'activation' character which is displayed in the widget for easy reference.

.SH
Label Widgets
.PP
A label widget is simply a widget which holds a character string such as a heading, which can be changed at will later. The programmer can supply parameters which can either left justify (default), right justify, or centre the message within the widget (which has a user definable width) as well as highlight (default) or dehighlight it.

.SH
Toggle Widgets
.PP
A toggle widget contains a message which indicates the current value of an entity which can be more than one value e.g, Yes/No/Don't-Know. The value is toggled when a given character is pressed. The activation character is also displayed in the widget. At any point the programmer can enquire as to the the current toggle value.

.SH
Input Widgets
.PP
An input widget allows the user to enter information into the program in the form of a string, e.g in a paging program one might want to supply a file name. There are two types of input widgets :

a) The widget is activated immediately on creation therefore an activation character is not required. E.g in a paging program one might have a command widget which activates this type of input widget in order to get a file name.
.LP
b) The widget has an associated activation character so therefore it may be activated at any point after creation. The character is displayed in the widget.

For ease of use the input widgets contain character strings which can explain
there use within the program.
.KE
.bp
.SH
Text Window
.PP
The library also allows the user to define a text window into which the program may report information. On creation the program has to pass a parameter which specifies how much room to leave before the top of the text window. E.g 0 means place the window immediately below the lowest widget, 1 means allow room for one more row of widgets before placing the window. The text window always stretches to the bottom of the screen. The window can be bordered on any 2 opposite sides or on all 4 (effectively b
it).

.SH
Widget Placement
.PP
The library maintains the current X,Y coordinate of the last widget created. When a new widget is created the program must pass a parameter 0 or 1. A 0 means put the new widget on the current row if possible, else go onto the next row. A 1 forces the new widget onto the next row. This means that widgets can only go after preceding ones and not before. But however widgets can be placed over widgets which have previously been destroyed.

.SH
Compiling Things
.PP
In order to use the library, it is necessary to have certain types and variables defined. Therefore, the program must have the line:
.PP
.B "#include <widgets.h>"
.LP
at the top of the source. The header file includes <curses.h>, <sgetty.h>, <stdio.h> so you don't have to include them also. Also compilations should have the following form:
.PP
.B "cc [flags] file ... -lwidgets -lcurses"

.SH
Type Definitions
.LP
The functions which create widgets return a type 
.B WIDGET
which is used for later identification of the widget.
.LP
If a function is required to return a widget identifier, then it will return 
.B NULLWIDGET
upon error.

.SH
Screen Refresh
.LP
The library supports a standard screen refresh invoked by ^L.
.KE
.bp
.KS
.SH
                                              USAGE
.SH
Getting Started
.LP
The first thing to do when using the library is set up the windowing system by calling 
.LP
.in 0.5i
initialisewidgets()
.in 0

.SH
Creating Widgets
.LP
.br

.in 0.5i
WIDGET mkcmdwidget (message, ch, function, row)
.br 
char message[];
.br 
char ch;
.br 
int 
(*func)();
.br 
int line;
.in 0
.LP
Creates a new command widget. message holds the message to be displayed in the command widget. ch is the activation character on which function is executed. row specifies where to place the widget (as described above). The returned value is the widget identifier for later use.

.LP
.in 0.5i
WIDGET mklblwidget (message, position, space, row)
.br
char message[];
.br
int  position, space, row;
.in 0
.LP
Creates a new label widget. Message holds the contents of the label, position specifies how the message is to appear. Using the constants CENTRE, LEFTJUST, RIGHTJUST, NOHIGH, pass a combination to position e.g CENTRE or LEFTJUST|NOHIGH etc.space indicates how large the label widget is to be. A positive value is the absolute length of the widget, whereas value <= 0 specifies how far the widget is to be from the righthand edge of the screen. row specifies where to place the widget (as described above).
eturned value is the widget identifier for later use.

.LP
.in 0.5i
WIDGET mktglwidget (message, number, tgl, tglch, row)
.br
char message[];
.br
int  number;
.br
char tgl[10][20];
.br
char tglch;
.br
int  row;
.in 0
.LP
Creates a new toggle widget. message contains a label for the widget, and number holds the number of toggle values. tgl is used for holding actual toggle values which are toggled between using the activation character tglch. At present there is a limit of 10 toggle values, each being a maximum length of 20 characters. row specifies where to place the widget (as described above). The returned value is the widget identifier for later use.
.LP
.KE
.bp
.KS
.in 0.5i
WIDGET mkinpwidget (message, character, input, lengthstr, lengthwid, row, exec)
.br
char message[], character, input[];
.br
int  lengthstr, lengthwid, row, exec; 
.in 0
.LP
Creates a new input widget. message contains the label for the widget, and character is the activation character for the widget. input is the variable in which the user wishes the data obtained from the input widget to be placed, lengthstr is the length of input. lengthwid specifies how big to make the widget. A positive value indicates the absolute length of the widget, where as a value <= 0 indicates how far from the righthand edge the widget is to be. exec is used to indicate whether the widget sh
e activated on creation by (passing EXEC), or should remain dormant until the activation character is used (passing NOEXEC). (n.b when passing EXEC an activation character is not needed therefore one could pass NULL for character.)

.SH
Text Window Management
.LP
.br

.in 0.5i
int opentextwindow (position, border)
.br
int position, border;
.in 0
.LP
Opens and displays the text window. position specifies where the window is to go. i.e position = 0 will place the window immediately below the lowest widget, position = 1 will allow room for 1 more row of widgets below the current lowest before placing the window etc. border is used to indicate whether the window should be bordered. i.e using the parameters VERTICAL & HORIZONTAL, one can specify whether the window should be bordered vertically (passing VERTICAL), horizontally (passing HORIZONTAL), bo
ssing VERTICAL|HORIZONTAL), or neither (passing 0). The function returns the number of lines within the window.

.in 0.5i
report (message)
.br
char *message;
.in 0
.LP
Displays the given message in the text window. Scrolling is done automatically when the bottom of the window is reached and is therefore of no concern for the user.

.in 0.5i
cleartextwindow ()
.in 0
.LP
Clears the text window and returns its cursor to the top lefthand corner. 

.in 0.5i
killtextwindow ()
.in 0
.LP
Destroys the text window. The text window can be redefined at a later date.

.SH
Killing Widgets

.LP
.br
.in 0.5i
int killwidget (ptr)
.br
WIDGET ptr;
.in 0
.LP
Kills the given widget. the function returns TRUE if successfull or FALSE if the widget doesn't exist.
.KE
.bp
.KS
.SH
Interaction

.LP
.br
.in 0.5i
WIDGET widgetinput ()
.in 0
.LP
Wait for input from widgets. Program control is passed back when :-
.br
a) A Command widget is activated and control is passed to its associated function.
.br
b) An input widget is activated and after subsequent input, control is passed to the calling function with the widget pointer of the activated input widget.
.br
Activating a toggle widget just toggles the widget, control is only passed back upon one of the above conditions.

.LP
.br
.in 0.5i
int tsttglwidget (ptr)
.br
WIDGET ptr;
.in 0
.LP
Returns the index value of the current state of the given toggle widget (0 < index < number toggle values - 1). Returns FALSE if the given widget does not exist.

.LP
.br
.in 0.5i
int activate (ptr)
.br
WIDGET ptr;
.in 0
.LP
Reactivates a widget which was previously deactivated. Returns TRUE if successful otherwise FALSE.

.LP
.br
.in 0.5i
int deactivate (ptr, blank)
.br
WIDGET ptr;
.br
int    blank;
.in 0
.LP
Deactivates the given active widget. The widget remains on screen but it cannot be activated. blank can be BLANK or NOBLANK which indicates whether the contents of the widget should be blanked out or not while it is inactive. The function returns TRUE if successful or FALSE if the given widget does not exist.

.LP
.br
.in 0.5i
int highlight (ptr)
.br
WIDGET ptr;
.in 0
.LP
Highlights the given widget. Returns TRUE if successful else FALSE if the widget does not exist.

.LP
.br
.in 0.5i
int dehighlight (ptr)
.br
WIDGET ptr;
.in 0
.LP
Turns off the highlighting of the given widget. Returns TRUE if successful else FALSE if the widget does not exist.

.LP
.br
.in 0.5i
int changelblwidget (ptr, message, position)
.br
WIDGET ptr;
.br
char   message[];
.br
int    position;
.in 0
.LP
Changes the contents of the given label widget to that given in message. position specifies how the message is to appear (see mklblwidget). The function returns TRUE if successful else FALSE.
.KE
.KS
.LP
.br
.in 0.5i
WIDGETTYPE widgettype (ptr)
.br
WIDGET ptr;
.in 0
.LP
Returns the type of widget pointed too by ptr. The returned type value is one of CMD, LBL, TGL, INP or FALSE if the widget does not exist.

.SH
Finishing Up

.LP
.br
.in 0.5i
endwidgets ()
.in 0
.LP
Closes down the windowing system and should be used at the end. This function is also automatically called on an interrupt (e.g ^C).
.KE
