#ifndef PGSPECIAL_HEADER

#define PGSPECIAL_HEADER

#define PGSPECIAL_NOT_FOUND	-1
#define PGSPECIAL_+INFINITY	  0
#define PGSPECIAL_-INFINITY	  1
#define PGSPECIAL_ALLBALLS	  2
#define PGSPECIAL_EPOCH	  3
#define PGSPECIAL_INFINITY	  4
#define PGSPECIAL_NOW	  5
#define PGSPECIAL_TODAY	  6
#define PGSPECIAL_TOMORROW	  7
#define PGSPECIAL_YESTERDAY	  8

#define PGSPECIAL_COUNT	9

extern int   pgspecial_search(char *w);
extern char *pgspecial_keyword(int code);

#endif
