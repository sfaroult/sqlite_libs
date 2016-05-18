#ifndef PGNUM_HEADER

#define PGNUM_HEADER

#define PGNUM_AMBIGUOUS	-2
#define PGNUM_NOT_FOUND	-1
#define PGNUM_TM	  0
#define PGNUM_PR	  1
#define PGNUM_MI	  2
#define PGNUM_FM	  3
#define PGNUM_EEEE	  4

#define PGNUM_COUNT	5

extern int   pgnum_search(char *w);
extern int   pgnum_best_match(char *w);
extern int   pgnum_abbrev(char *w);
extern char *pgnum_keyword(int code);

#endif
