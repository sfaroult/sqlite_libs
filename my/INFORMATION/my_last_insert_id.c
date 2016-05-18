#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ksu_common.h>

extern void my_last_insert_id(sqlite3_context  *context,
                              int               argc,
                              sqlite3_value   **argv){
  sqlite3      *thisdb; 

  thisdb = sqlite3_context_db_handle(context);
  if (thisdb) {
    sqlite3_result_int64(context, sqlite3_last_insert_rowid(thisdb));
  } else {
    sqlite3_result_null(context);
  }
}
