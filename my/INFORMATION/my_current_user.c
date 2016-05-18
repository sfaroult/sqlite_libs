#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void my_current_user(sqlite3_context  *context,
                        int               argc,
                        sqlite3_value   **argv){
  char  *usr;
  char  *p;
  int    len;

  usr = getenv("USER");
  if (usr) {
    len = strlen(usr) + strlen("@localhost") + 1;
    if ((p = (char *)sqlite3_malloc(len)) != NULL) {
      strcpy(p, usr);
      strcat(p, "@localhost");
      sqlite3_result_text(context, p, -1, sqlite3_free);
    } else {
      sqlite3_result_error_nomem(context);
    }
  } else {
      sqlite3_result_null(context);
  }
}
