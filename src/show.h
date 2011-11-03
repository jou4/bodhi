#ifndef _show_h_
#define _show_h_

#define SHOW_COLS 30
#define INDENT(n, d)  n += bd_show_indent(d)
#define PRINT(n, s) n += printf(s)
#define PRINT1(n, t, p1) n += printf(t, p1)
#define PRINT2(n, t, p1, p2) n += printf(t, p1, p2)
#define BREAKLINE(n, d) \
    if(n > SHOW_COLS){ \
        printf("\n"); n = bd_show_indent(++d); \
    }
#define DOBREAKLINE(n, d) printf("\n"); n = bd_show_indent(++d);
#define DOBREAKLINE_NOSHIFT(n, d) printf("\n"); n = bd_show_indent(d);

int bd_show_indent(int depth);

#endif
