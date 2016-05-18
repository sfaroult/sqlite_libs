#ifndef KSU_SS_HEADER

#define KSU_SS_HEADER

#define KSU_SS_NOT_FOUND	-1
#define KSU_SS_D	  0
#define KSU_SS_DAY	  1
#define KSU_SS_DAYOFYEAR	  2
#define KSU_SS_DD	  3
#define KSU_SS_DW	  4
#define KSU_SS_DY	  5
#define KSU_SS_HH	  6
#define KSU_SS_HOUR	  7
#define KSU_SS_ISOWK	  8
#define KSU_SS_ISOWW	  9
#define KSU_SS_ISO_WEEK	 10
#define KSU_SS_M	 11
#define KSU_SS_MCS	 12
#define KSU_SS_MI	 13
#define KSU_SS_MICROSECOND	 14
#define KSU_SS_MILLISECOND	 15
#define KSU_SS_MINUTE	 16
#define KSU_SS_MM	 17
#define KSU_SS_MONTH	 18
#define KSU_SS_MS	 19
#define KSU_SS_N	 20
#define KSU_SS_NANOSECOND	 21
#define KSU_SS_NS	 22
#define KSU_SS_Q	 23
#define KSU_SS_QQ	 24
#define KSU_SS_QUARTER	 25
#define KSU_SS_S	 26
#define KSU_SS_SECOND	 27
#define KSU_SS_SS	 28
#define KSU_SS_W	 29
#define KSU_SS_WEEK	 30
#define KSU_SS_WEEKDAY	 31
#define KSU_SS_WK	 32
#define KSU_SS_WW	 33
#define KSU_SS_Y	 34
#define KSU_SS_YEAR	 35
#define KSU_SS_YY	 36
#define KSU_SS_YYYY	 37

#define KSU_SS_COUNT	38

extern int   ksu_ss_search(char *w);
extern char *ksu_ss_keyword(int code);

#define SSTYP_NOT_FOUND	-1
#define SSTYP_BIGINT	  0
#define SSTYP_BINARY	  1
#define SSTYP_BIT	  2
#define SSTYP_CHAR	  3
#define SSTYP_DATE	  4
#define SSTYP_DATETIME	  5
#define SSTYP_DATETIME2	  6
#define SSTYP_DATETIMEOFFSET	  7
#define SSTYP_DECIMAL	  8
#define SSTYP_FLOAT	  9
#define SSTYP_HIERARCHYID	 10
#define SSTYP_IMAGE	 11
#define SSTYP_INT	 12
#define SSTYP_MONEY	 13
#define SSTYP_NCHAR	 14
#define SSTYP_NTEXT	 15
#define SSTYP_NUMERIC	 16
#define SSTYP_NVARCHAR	 17
#define SSTYP_REAL	 18
#define SSTYP_SMALLDATETIME	 19
#define SSTYP_SMALLINT	 20
#define SSTYP_SMALLMONEY	 21
#define SSTYP_SQL_VARIANT	 22
#define SSTYP_TEXT	 23
#define SSTYP_TIME	 24
#define SSTYP_TINYINT	 25
#define SSTYP_UNIQUEIDENTIFIER	 26
#define SSTYP_VARBINARY	 27
#define SSTYP_VARCHAR	 28
#define SSTYP_XML	 29

#define SSTYP_COUNT	30

extern int   sstyp_search(char *w);
extern char *sstyp_keyword(int code);

#endif
