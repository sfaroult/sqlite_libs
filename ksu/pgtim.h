#ifndef PGTIM_HEADER

#define PGTIM_HEADER

#define PGTIM_NOT_FOUND	-1
#define PGTIM_C	  0
#define PGTIM_CENTURIES	  1
#define PGTIM_CENTURY	  2
#define PGTIM_D	  3
#define PGTIM_DAY	  4
#define PGTIM_DAYS	  5
#define PGTIM_DECADE	  6
#define PGTIM_DECADES	  7
#define PGTIM_DOW	  8
#define PGTIM_DOY	  9
#define PGTIM_EPOCH	 10
#define PGTIM_H	 11
#define PGTIM_HOUR	 12
#define PGTIM_HOURS	 13
#define PGTIM_ISODOW	 14
#define PGTIM_ISOYEAR	 15
#define PGTIM_M	 16
#define PGTIM_MICROSECOND	 17
#define PGTIM_MICROSECONDS	 18
#define PGTIM_MILLENNIUM	 19
#define PGTIM_MILLISECOND	 20
#define PGTIM_MILLISECONDS	 21
#define PGTIM_MIN	 22
#define PGTIM_MINS	 23
#define PGTIM_MINUTE	 24
#define PGTIM_MINUTES	 25
#define PGTIM_MON	 26
#define PGTIM_MONS	 27
#define PGTIM_MONTH	 28
#define PGTIM_MONTHS	 29
#define PGTIM_QUARTER	 30
#define PGTIM_S	 31
#define PGTIM_SEC	 32
#define PGTIM_SECOND	 33
#define PGTIM_SECONDS	 34
#define PGTIM_SECS	 35
#define PGTIM_TIMEZONE	 36
#define PGTIM_TIMEZONE_HOUR	 37
#define PGTIM_TIMEZONE_MINUTE	 38
#define PGTIM_W	 39
#define PGTIM_WEEK	 40
#define PGTIM_WEEKS	 41
#define PGTIM_Y	 42
#define PGTIM_YEAR	 43
#define PGTIM_YEARS	 44

#define PGTIM_COUNT	45

extern int   pgtim_search(char *w);
extern char *pgtim_keyword(int code);

#endif
