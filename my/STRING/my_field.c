#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void  my_field(sqlite3_context * context, 
                      int               argc, 
                      sqlite3_value  ** argv) {
    int    i;
    int    typ;
    int    curtyp;
    char   found = 0;
    int    len;
                
    _ksu_check_arg_cnt(argc, 2, -1, "field");
    typ = sqlite3_value_type(argv[0]);
    // Doesn't return null but 0 when the first parameter is null
    // Other nulls are ignored
    if (typ == SQLITE_NULL) {
      sqlite3_result_int(context, 0);
      return;
    }
    len = sqlite3_value_bytes(argv[0]);
    for (i = 1; i < argc; i++) {
      curtyp = sqlite3_value_type(argv[i]);
      if ((curtyp == typ)
          || ((curtyp == SQLITE_INTEGER) && (typ == SQLITE_FLOAT))
          || ((curtyp == SQLITE_FLOAT) && (typ == SQLITE_INTEGER))) {
        switch(typ) {
          case SQLITE_INTEGER:
          case SQLITE_FLOAT:
               found = (sqlite3_value_double(argv[i])
                        == sqlite3_value_double(argv[0]));
               break;
          case SQLITE_BLOB:
               found = ((len == sqlite3_value_bytes(argv[1]))
                       && (memcmp(sqlite3_value_blob(argv[0]),
                                  sqlite3_value_blob(argv[1]),
                                  (size_t)len) == 0));
               break; 
          default: 
               found = (strcmp((char *)sqlite3_value_text(argv[i]),
                         (char *)sqlite3_value_text(argv[0])) == 0);
               break; 
      }
      if (found) {
        sqlite3_result_int(context, i);
        return;
      } 
    } 
  } 
  sqlite3_result_int(context, 0);
} 
