#ifndef KSU_TERR_HEADER

#define KSU_TERR_HEADER

#define KSU_TERR_NOT_FOUND	-1
#define KSU_TERR_AM	  0
#define KSU_TERR_AT	  1
#define KSU_TERR_AU	  2
#define KSU_TERR_BE	  3
#define KSU_TERR_BG	  4
#define KSU_TERR_BR	  5
#define KSU_TERR_BY	  6
#define KSU_TERR_CA	  7
#define KSU_TERR_CH	  8
#define KSU_TERR_CN	  9
#define KSU_TERR_CZ	 10
#define KSU_TERR_DE	 11
#define KSU_TERR_DK	 12
#define KSU_TERR_EE	 13
#define KSU_TERR_ES	 14
#define KSU_TERR_ET	 15
#define KSU_TERR_FI	 16
#define KSU_TERR_FR	 17
#define KSU_TERR_GB	 18
#define KSU_TERR_GR	 19
#define KSU_TERR_HK	 20
#define KSU_TERR_HR	 21
#define KSU_TERR_HU	 22
#define KSU_TERR_IE	 23
#define KSU_TERR_IL	 24
#define KSU_TERR_IN	 25
#define KSU_TERR_IS	 26
#define KSU_TERR_IT	 27
#define KSU_TERR_JP	 28
#define KSU_TERR_KR	 29
#define KSU_TERR_KZ	 30
#define KSU_TERR_LT	 31
#define KSU_TERR_NL	 32
#define KSU_TERR_NO	 33
#define KSU_TERR_NZ	 34
#define KSU_TERR_PL	 35
#define KSU_TERR_PT	 36
#define KSU_TERR_RO	 37
#define KSU_TERR_RU	 38
#define KSU_TERR_SE	 39
#define KSU_TERR_SI	 40
#define KSU_TERR_SK	 41
#define KSU_TERR_TR	 42
#define KSU_TERR_TW	 43
#define KSU_TERR_UA	 44
#define KSU_TERR_US	 45
#define KSU_TERR_YU	 46
#define KSU_TERR_ZA	 47

#define KSU_TERR_COUNT	48

extern int   ksu_terr_search(char *w);
extern char *ksu_terr_keyword(int code);

#endif
