#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void my_database(sqlite3_context  *context,
                        int               argc,
                        sqlite3_value   **argv){
  sqlite3      *thisdb; 
  char         *v;
  char         *p;

  thisdb = sqlite3_context_db_handle(context);
  if (thisdb) {
     v = (char *)sqlite3_db_filename(thisdb, (const char *)"main");
     if (v == NULL){
        sqlite3_result_text(context, ":memory:", -1, SQLITE_STATIC);
     } else {
        if ((p = strrchr(v, '/')) != (char *)NULL) {
           p++;
        } else {
           p = v;
        }
        v = (char *)sqlite3_malloc(strlen(p) + 1);
        strcpy(v, p);
        if ((p = strchr(v, '.')) != (char *)NULL) {
          *p = '\0';
        }
        sqlite3_result_text(context, v, -1, sqlite3_free);
     }
  } else {
    sqlite3_result_null(context);
  }
}
