#include <math.h>
#include <ksu_common.h>

extern void pg_pi(sqlite3_context  *context,
                  int               argc,
                  sqlite3_value   **argv){
  sqlite3_result_double(context, M_PI);
}
