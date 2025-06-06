/*
 * History : Written by tjalk.cs.vu.nl  ( I got no name from the article) 
 *
 * Hacked to use termcap by: Josh Siegel (josh@hi.unm@hc.dspo.gov) 
 *
 *
 * Please note, The hack that handles loading the atributes into the system
 * is exactly that... a hack.  I had little time and little want to make
 * it better. 
 *
 *
 * --Josh Siegel 
 *
 */
#define MaxNrOfWorms 32		/* You can't alter this without altering
				 * the zoo!  (Zoo: see somewhere else in
				 * this program)      */
#define MaxLength    48		/* This really is long enough! */
long _stksize=65536;
int		ScreenWidth;
int		ScreenHeigth;
char            cm[16], cl[16];


#include <stdio.h>


int		wormlength = 20;/* 20 by default, can be redefined by
				 * -l<nr> */
int		nrofworms = 3;	/* 3 by default, can be redefined by
				 * -n<nr> */
char           *message = "				W O R M S ! ! !";
/* message can be redifined by -m<string> */

int             x_direction[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int             y_direction[8] = {0, 1, 1, 1, 0, -1, -1, -1};

int	screen[100][100];



main(argc, argv)
  char          **argv;

{
  char	wormx[MaxNrOfWorms][MaxLength], wormy[MaxNrOfWorms][MaxLength];
  int             dir[MaxNrOfWorms], *direction;
  int		ptr1, ptr2, x, y, i, j;
  long            dummy;
  char           *pos();


  setbuf(stdout, NULL);
  /* First test for any flags... */

  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'L':
	argv[i][1]='l';
      case 'l':
	if (
	    sscanf(argv[i], "-l%d", &wormlength) == 0 ||
	    wormlength < 2 || wormlength > MaxLength) {
	  puts("Bad length.");
	  exit(1);
	} break;
      case 'N':
	argv[i][1]='n';
      case 'n':
	if (
	    sscanf(argv[i], "-n%d", &nrofworms) == 0 ||
	    nrofworms < 1 || nrofworms > MaxNrOfWorms) {
	  puts("Bad number of worms.");
	  exit(1);
	} break;
      case 'M':
      case 'm':
	message = argv[i] + 2;
	break;
      default:
	printf(" -%c: bad option.\n", argv[i][1]);
	exit(1);
      }
    } else {
      printf("Syntax: %s [ -l<nr> ] [ -n<nr> ] \
[ -m<message> ]\n", argv[0]);
      exit(1);
    }

  /* Now let's initialize !! */

  initsys();
  clear_screen();
  printf("%s%s", pos(0, 22), message);

  for (j = 0; j < nrofworms; j++) {
    for (i = 0; i < wormlength; i++)
      wormx[j][i] = wormy[j][i] = 0;
    dir[j] = 1;			/* direction of worm j */
  }

  screen[0][0] = nrofworms * wormlength;
  ptr1 = ptr2 = 0;
  /* They point to the front and the back of a worm, respectively. */


  /* init randomizer... I hope this will work, it works on UNIX */
  srand((int) time(&dummy));



  while (1) {
    ptr2 = (ptr1 + 1) % wormlength;
    /* Take the next part of each worm. */

    for (i = 0; i < nrofworms; i++) {
      register int	enough = 0;

      x = wormx[i][ptr2];
      y = wormy[i][ptr2];
      /*
       * x and y are the coordinates of the tail of worm i, which has
       * to be removed.			 
       */
      if ((--screen[x][y]) == 0)
	printf("%s ", pos(x, y));
      /*
       * It will only be removed when there are no other "parts" of
       * worms on that spot on the screen.  
       */

      /* Now put a head somewhere. */
      /*
       * We'll modify the direction just until we've got a correct
       * direction (so that the worm will not cross or  overlap another
       * worm or itself, nor will run of the screen),  or that we've had
       * enough,  since than there probably is no such direction. 
       */

      direction = &dir[i];

      do
	*direction = (*direction + (rand()) % 3 + 7) % 8;
      while ((x = wormx[i][ptr1] + x_direction[*direction]) < 0 ||
	     x > ScreenWidth - 1 ||
	     (y = wormy[i][ptr1] + y_direction[*direction]) < 0 ||
	     y > ScreenHeigth - 2 ||
	     ((screen[x][y] != 0 || cross(x, y, *direction))
	      && enough++ < 19));

      if (enough >= 19) {	/* We can't move this poor worm */
	x = wormx[i][ptr1];	/* old coordinates  */
	y = wormy[i][ptr1];
      }
      /*
       * x and y now contain the new coordinates of the worms head. Let's
       * just place it on the screen (and in memory) 
       */
      wormx[i][ptr2] = x;
      wormy[i][ptr2] = y;
      printf("%s%c", pos(x, y),
	     "*#Ox@~.+!&%$'`=-oXwW()^~*#x0@+./"[i]);
      /* Welcome to the zoo!! ^^^^ */
      screen[x][y]++;
    }
    ptr1 = ptr2;
  }
}




cross(x, y, d)
{				/* keeps two worms from crossing */
  if (d & 1 == 0)
    return 0;			/* worm moving diagonally? */

  return (screen[x][y - y_direction[d]] && screen[x - x_direction[d]][y]);
}

/*
 * I assume there are betters ways of doing this but since I had no
 * interest in spending more then 5 minutes making this use termcap ... 
 */

initsys()
{
  static char     hasrun = 0, bp[1024], bp2[1024], *tgetstr(), *p1;
  char           *tmp, *getenv();

  if((tmp=getenv("TERM"))==NULL)
	tmp="st52";
  tgetent(bp, tmp);
  strcpy(bp2, bp);
  p1 = bp2;
  tmp = tgetstr("cl", &p1);
  strcpy(cl, tmp);
  strcpy(bp2, bp);
  p1 = bp2;
  tmp = tgetstr("cm", &p1);
  strcpy(cm, tmp);
  strcpy(bp2, bp);
  p1 = bp2;
  if ((ScreenHeigth = tgetnum("li", &p1) - 1) > 99) {
    printf("I cannot handle a terminal that has more then 99 lines\n");
    exit(0);
  }
  strcpy(bp2, bp);
  p1 = bp2;
  if ((ScreenWidth = tgetnum("co", &p1) - 1) > 99) {;
    printf("I cannot handle a terminal that has more then 99 rows\n");
    exit(0);
  }
}

char           *
pos(x, y)
{
  char           *ret, *tgoto();

  ret = tgoto(cm, x, y);
  return ret;
}



clear_screen()
{
  puts(cl);
}
