/* 
 * ++jrb, turn it into a fn
 */

static char ret[3] = { '\0', '\0', '\0' };

char *
unctrl(int ch)
{
    int c = ch & 0377;          /* Note 8bit chars get mapped to themselves */

    if ((c < ' ') || (c == 0177)) {
        ret[0] = '^';
        ret[1] = (c == 0177) ? '?' : c | 0100;
    }
    else {
        ret[0] = c;
        ret[1] = '\0';
    }

    return ret;
}
