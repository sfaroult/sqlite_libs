#ifndef KSU_PG_HEADER

#define KSU_PG_HEADER

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

#define PGSPECIAL_NOT_FOUND     	-1
#define PGSPECIAL_PLUSINFINITY	    0
#define PGSPECIAL_MINUSINFINITY	    1
#define PGSPECIAL_ALLBALLS	        2
#define PGSPECIAL_EPOCH	            3
#define PGSPECIAL_INFINITY	        4
#define PGSPECIAL_NOW	            5
#define PGSPECIAL_TODAY	            6
#define PGSPECIAL_TOMORROW	        7
#define PGSPECIAL_YESTERDAY	        8

#define PGSPECIAL_COUNT	9

#define PGTIMFMT_AMBIGUOUS	-2
#define PGTIMFMT_NOT_FOUND	-1
#define PGTIMFMT_YYYY	  0
#define PGTIMFMT_YYY	  1
#define PGTIMFMT_YY	  2
#define PGTIMFMT_Y_YYY	  3
#define PGTIMFMT_Y	  4
#define PGTIMFMT_WW	  5
#define PGTIMFMT_W	  6
#define PGTIMFMT_US	  7
#define PGTIMFMT_TZ	  8
#define PGTIMFMT_TM	  9
#define PGTIMFMT_TH	 10
#define PGTIMFMT_SSSS	 11
#define PGTIMFMT_SS	 12
#define PGTIMFMT_SP	 13
#define PGTIMFMT_RM	 14
#define PGTIMFMT_Q	 15
#define PGTIMFMT_PM	 16
#define PGTIMFMT_P_M_	 17
#define PGTIMFMT_OF	 18
#define PGTIMFMT_MS	 19
#define PGTIMFMT_MONTH	 20
#define PGTIMFMT_MON	 21
#define PGTIMFMT_MM	 22
#define PGTIMFMT_MI	 23
#define PGTIMFMT_J	 24
#define PGTIMFMT_IYYY	 25
#define PGTIMFMT_IYY	 26
#define PGTIMFMT_IY	 27
#define PGTIMFMT_IW	 28
#define PGTIMFMT_IDDD	 29
#define PGTIMFMT_ID	 30
#define PGTIMFMT_I	 31
#define PGTIMFMT_HH24	 32
#define PGTIMFMT_HH12	 33
#define PGTIMFMT_HH	 34
#define PGTIMFMT_FX	 35
#define PGTIMFMT_FM	 36
#define PGTIMFMT_DY	 37
#define PGTIMFMT_DDD	 38
#define PGTIMFMT_DD	 39
#define PGTIMFMT_DAY	 40
#define PGTIMFMT_D	 41
#define PGTIMFMT_CC	 42
#define PGTIMFMT_BC	 43
#define PGTIMFMT_B_C_	 44
#define PGTIMFMT_AM	 45
#define PGTIMFMT_AD	 46
#define PGTIMFMT_A_M_	 47
#define PGTIMFMT_A_D_	 48

#define PGTIMFMT_COUNT	49

#define PGNUM_AMBIGUOUS	-2
#define PGNUM_NOT_FOUND	-1
#define PGNUM_TM	  0
#define PGNUM_TH	  1
#define PGNUM_SG	  2
#define PGNUM_S 	  3
#define PGNUM_PR	  4
#define PGNUM_PL	  5
#define PGNUM_MI	  6
#define PGNUM_FM	  7
#define PGNUM_EEEE	  8

#define PGNUM_COUNT	9

extern int   pgtim_search(char *w);
extern char *pgtim_keyword(int code);

extern int   pgspecial_search(char *w);
extern char *pgspecial_keyword(int code);

extern int   pgtimfmt_search(char *w);
extern int   pgtimfmt_best_match(char *w);
extern int   pgtimfmt_abbrev(char *w);
extern char *pgtimfmt_keyword(int code);

extern int   pgnum_search(char *w);
extern int   pgnum_best_match(char *w);
extern int   pgnum_abbrev(char *w);
extern char *pgnum_keyword(int code);

#endif
