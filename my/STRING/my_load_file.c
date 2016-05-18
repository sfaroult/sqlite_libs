#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ksu_common.h>

extern void  my_load_file(sqlite3_context * context,
                          int               argc,
                          sqlite3_value  ** argv) {

   struct stat    statbuf;
   char          *fname;
   unsigned char *blob;
   FILE          *fp;
   size_t         nread;

   _ksu_null_if_null_param(argc, argv);
   fname = (char *)sqlite3_value_text(argv[0]);
   if (stat((const char *)fname, &statbuf) == -1) {
     sqlite3_result_null(context);
     return;
   }
   if ((blob = (unsigned char *)sqlite3_malloc((int)statbuf.st_size))
            == (unsigned char *)NULL) {
     sqlite3_result_error_nomem(context);
     return;
   }
   if ((fp = fopen(fname, "r")) != (FILE *)NULL) {
      nread = fread((void *)blob, sizeof(unsigned char),
                                  (size_t)statbuf.st_size, fp);
      fclose(fp);
      sqlite3_result_blob(context, (const void *)blob,
                          (int)nread, sqlite3_free);
   } else {
      sqlite3_free(blob);
      sqlite3_result_null(context);
   }
}
