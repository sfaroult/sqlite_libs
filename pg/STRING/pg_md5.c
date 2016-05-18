#include <stdio.h>
#include <openssl/md5.h>

#include <ksu_common.h>

extern void pg_md5(sqlite3_context *context,
                   int              argc,
                   sqlite3_value  **argv) {
   unsigned char  md[MD5_DIGEST_LENGTH];
   char           mdstr[MD5_DIGEST_LENGTH * 2 + 1];
   int            len;
   int            i;

   _ksu_null_if_null_param(argc, argv);
   len = sqlite3_value_bytes(argv[0]);
   (void)MD5((const unsigned char *)sqlite3_value_text(argv[0]),
             (unsigned long)len, md);
   for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
     sprintf(&(mdstr[2*i]), "%02x", md[i]);
   }
   sqlite3_result_text(context, mdstr, -1, SQLITE_TRANSIENT);
}
