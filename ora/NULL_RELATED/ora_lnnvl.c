#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ksu_common.h>

extern void ora_lnnvl(sqlite3_context * context,
                      int               argc,
                      sqlite3_value  ** argv) {
        int    type;
        int    i;

        type = sqlite3_value_type(argv[0]);
        switch(type) {
          case SQLITE_INTEGER:
               i = sqlite3_value_int(argv[0]);
               if (i == 0) {
                 i = 1;
               } else {
                 i = 0;
               }
               sqlite3_result_int(context, i);
               break;
          case SQLITE_NULL:
               sqlite3_result_int(context, 1);
               break;
          default: // Invalid
               ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "lnnvl");
               break;
        }
}
