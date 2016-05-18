#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void ora_user(sqlite3_context  *context,
                     int               argc,
                     sqlite3_value   **argv){
  char  *usr;

  usr = getenv("USER");
  if (usr) {
    sqlite3_result_text(context, usr, -1, SQLITE_STATIC);
  } else {
    sqlite3_result_null(context);
  }
}
