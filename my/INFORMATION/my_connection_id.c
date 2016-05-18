#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ksu_common.h>

extern void my_connection_id(sqlite3_context  *context,
                             int               argc,
                             sqlite3_value   **argv){
  sqlite3_result_int(context, getpid());
}
