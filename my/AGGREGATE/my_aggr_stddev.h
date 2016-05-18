#ifndef MY_AGGR_STDDEV_H

#define MY_AGGR_STDDEV_H

typedef struct {
  double       K;
  double       sum;
  double       sum_sqr;
  unsigned int n;
} CONTEXT_T;

extern void my_stddev_step(sqlite3_context *context,
                           int             argc,
                           sqlite3_value **argv);

#endif
