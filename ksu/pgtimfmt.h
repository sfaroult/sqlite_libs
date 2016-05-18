#ifndef PGTIMFMT_HEADER

#define PGTIMFMT_HEADER

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

extern int   pgtimfmt_search(char *w);
extern int   pgtimfmt_best_match(char *w);
extern int   pgtimfmt_abbrev(char *w);
extern char *pgtimfmt_keyword(int code);

#endif
