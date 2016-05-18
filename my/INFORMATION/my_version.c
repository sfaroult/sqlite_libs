#include <stdio.h>
#include <ksu_common.h>

/*
 *   Returns the running version of sqlite
 */
extern void my_version(sqlite3_context  *context,
                       int argc,
                       sqlite3_value   **argv) {

   sqlite3_result_text(context,
                       sqlite3_libversion(),
                       -1,           // Length - -1 means terminated by \0
                       NULL); // Function for freeing memory
}
