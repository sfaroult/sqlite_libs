/*
 * my_subtime.c - v1.0
 * 
 * Subtracts the first datetime/time from the second given datetime/time.  If
 * first is DateTime then second can be Date Time or just Time. If first is
 * Time then Second also has to be time Input: subtime("YYYY-MM-DD HH:MM:SS",
 * "DD HH:MM:SS") or subtime("YYYY-MM-DD HH:MM:SS", "HH:MM:SS") Output: The
 * answer given in the same format as the first argument.
 * 
 */

#include <ksu_common.h>
#include <ksu_my.h>

extern void my_subtime(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
  _ksu_null_if_null_param(argc, argv);
  my_time_op(context, "subtime", (char)0,
             (char *)sqlite3_value_text(argv[0]),
             (char *)sqlite3_value_text(argv[1]));
}
