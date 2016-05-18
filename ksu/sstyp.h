#ifndef SSTYP_HEADER

#define SSTYP_HEADER

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
