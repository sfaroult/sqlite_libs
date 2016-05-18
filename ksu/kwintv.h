#ifndef KWINTV_HEADER

#define KWINTV_HEADER

#define KWINTV_AMBIGUOUS	-2
#define KWINTV_NOT_FOUND	-1
#define KWINTV_YEARS	  0
#define KWINTV_YEAR	  1
#define KWINTV_Y	  2
#define KWINTV_WEEKS	  3
#define KWINTV_WEEK	  4
#define KWINTV_W	  5
#define KWINTV_SECS	  6
#define KWINTV_SECONDS	  7
#define KWINTV_SECOND	  8
#define KWINTV_SEC	  9
#define KWINTV_S	 10
#define KWINTV_MONTHS	 11
#define KWINTV_MONTH	 12
#define KWINTV_MONS	 13
#define KWINTV_MON	 14
#define KWINTV_MINUTES	 15
#define KWINTV_MINUTE	 16
#define KWINTV_MINS	 17
#define KWINTV_MIN	 18
#define KWINTV_M	 19
#define KWINTV_HOURS	 20
#define KWINTV_HOUR	 21
#define KWINTV_H	 22
#define KWINTV_DECADES	 23
#define KWINTV_DECADE	 24
#define KWINTV_DAYS	 25
#define KWINTV_DAY	 26
#define KWINTV_D	 27
#define KWINTV_CENTURY	 28
#define KWINTV_CENTURIES	 29
#define KWINTV_C	 30

#define KWINTV_COUNT	31

extern int   kwintv_search(char *w);
extern int   kwintv_best_match(char *w);
extern int   kwintv_abbrev(char *w);
extern char *kwintv_keyword(int code);

#endif
