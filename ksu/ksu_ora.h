#ifndef KSU_ORA_HEADER

#define KSU_ORA_HEADER

// Time formats
#define ORATIM_AMBIGUOUS	-2
#define ORATIM_NOT_FOUND	-1
#define ORATIM_YYYY	  0
#define ORATIM_YYY	  1
#define ORATIM_YY	  2
#define ORATIM_YEAR	  3
#define ORATIM_Y_YYY	  4
#define ORATIM_Y	  5
#define ORATIM_X	  6
#define ORATIM_WW	  7
#define ORATIM_W	  8
#define ORATIM_TZR	  9
#define ORATIM_TZM	 10
#define ORATIM_TZH	 11
#define ORATIM_TZD	 12
#define ORATIM_TS	 13
#define ORATIM_THSP	 14
#define ORATIM_TH	 15
#define ORATIM_SYYYY	 16
#define ORATIM_SYEAR	 17
#define ORATIM_SSSSS	 18
#define ORATIM_SS	 19
#define ORATIM_SPTH	 20
#define ORATIM_SP	 21
#define ORATIM_SCC	 22
#define ORATIM_RRRR	 23
#define ORATIM_RR	 24
#define ORATIM_RM	 25
#define ORATIM_Q	 26
#define ORATIM_PM	 27
#define ORATIM_P_M_	 28
#define ORATIM_MONTH	 29
#define ORATIM_MON	 30
#define ORATIM_MM	 31
#define ORATIM_MI	 32
#define ORATIM_J	 33
#define ORATIM_IYYY	 34
#define ORATIM_IYY	 35
#define ORATIM_IY	 36
#define ORATIM_IW	 37
#define ORATIM_I	 38
#define ORATIM_HH24	 39
#define ORATIM_HH12	 40
#define ORATIM_HH	 41
#define ORATIM_FX	 42
#define ORATIM_FM	 43
#define ORATIM_FF	 44
#define ORATIM_EE	 45
#define ORATIM_E	 46
#define ORATIM_DY	 47
#define ORATIM_DS	 48
#define ORATIM_DL	 49
#define ORATIM_DDD	 50
#define ORATIM_DD	 51
#define ORATIM_DAY	 52
#define ORATIM_D	 53
#define ORATIM_CC	 54
#define ORATIM_BC	 55
#define ORATIM_B_C_	 56
#define ORATIM_AM	 57
#define ORATIM_AD	 58
#define ORATIM_A_M_	 59
#define ORATIM_A_D_	 60

#define ORATIM_COUNT	61

extern int   oratim_search(char *w);
extern int   oratim_best_match(char *w);
extern int   oratim_abbrev(char *w);
extern char *oratim_keyword(int code);

// Number formats (more than one character)

#define ORANUM_AMBIGUOUS	-2
#define ORANUM_NOT_FOUND	-1
#define ORANUM_RN_L	  0
#define ORANUM_TM	  1
#define ORANUM_RN_U	  2
#define ORANUM_PR	  3
#define ORANUM_MI	  4
#define ORANUM_FM	  5
#define ORANUM_EEEE	  6

#define ORANUM_COUNT	7

extern int   oratim_search(char *w);
extern int   oratim_best_match(char *w);
extern int   oratim_abbrev(char *w);
extern char *oratim_keyword(int code);

extern int   oranum_search(char *w);
extern int   oranum_best_match(char *w);
extern int   oranum_abbrev(char *w);
extern char *oranum_keyword(int code);

#endif
