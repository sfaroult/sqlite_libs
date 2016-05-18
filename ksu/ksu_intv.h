#ifndef KSU_INTV_HEADER

#define KSU_INTV_HEADER

#define KSU_INTV_NOT_FOUND	-1
#define KSU_INTV_CENTURY	  0
#define KSU_INTV_DAY	  1
#define KSU_INTV_HOUR	  2
#define KSU_INTV_MINUTE	  3
#define KSU_INTV_MONTH	  4
#define KSU_INTV_SECOND	  5
#define KSU_INTV_WEEK	  6
#define KSU_INTV_YEAR	  7

#define KSU_INTV_COUNT	8

extern int   ksu_intv_search(char *w);
extern char *ksu_intv_keyword(int code);

#endif
