#ifndef ORANUM_HEADER

#define ORANUM_HEADER

#define ORANUM_AMBIGUOUS	-2
#define ORANUM_NOT_FOUND	-1
#define ORANUM_RN	  0
#define ORANUM_TM	  1
#define ORANUM_RN	  2
#define ORANUM_PR	  3
#define ORANUM_MI	  4
#define ORANUM_FM	  5
#define ORANUM_EEEE	  6

#define ORANUM_COUNT	7

extern int   oranum_search(char *w);
extern int   oranum_best_match(char *w);
extern int   oranum_abbrev(char *w);
extern char *oranum_keyword(int code);

#endif
