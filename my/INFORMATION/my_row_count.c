#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ksu_common.h>

extern void my_row_count(sqlite3_context  *context,
                         int               argc,
                         sqlite3_value   **argv){
  sqlite3      *thisdb; 

  thisdb = sqlite3_context_db_handle(context);
  if (thisdb) {
    sqlite3_result_int(context, sqlite3_changes(thisdb));
  } else {
    sqlite3_result_null(context);
  }
}
