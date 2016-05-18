#ifndef MYTIM_HEADER

#define MYTIM_HEADER

#define MYTIM_NOT_FOUND	-1
#define MYTIM_DAY	  0
#define MYTIM_DAY_HOUR	  1
#define MYTIM_DAY_MICROSECOND	  2
#define MYTIM_DAY_MINUTE	  3
#define MYTIM_DAY_SECOND	  4
#define MYTIM_HOUR	  5
#define MYTIM_HOUR_MICROSECOND	  6
#define MYTIM_HOUR_MINUTE	  7
#define MYTIM_HOUR_SECOND	  8
#define MYTIM_MICROSECOND	  9
#define MYTIM_MINUTE	 10
#define MYTIM_MINUTE_MICROSECOND	 11
#define MYTIM_MINUTE_SECOND	 12
#define MYTIM_MONTH	 13
#define MYTIM_QUARTER	 14
#define MYTIM_SECOND	 15
#define MYTIM_SECOND_MICROSECOND	 16
#define MYTIM_WEEK	 17
#define MYTIM_YEAR	 18
#define MYTIM_YEAR_MONTH	 19

#define MYTIM_COUNT	20

extern int   mytim_search(char *w);
extern char *mytim_keyword(int code);

#endif
