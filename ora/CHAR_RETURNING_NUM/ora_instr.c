#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
/*
 * This function takes a series of arguments and returns the position
 * specified by the arguments Same Encoding REQUIRED*/
#include <ksu_common.h>

extern void ora_instr(sqlite3_context  *context,
                      int               argc,
                      sqlite3_value   **argv) {
   int start = 1;
   int occ = 1;

   // checks for correct number of arguments
   _ksu_check_arg_cnt(argc, 2, 4, "instr");
   if (ksu_prm_ok(context, argc, argv, "instr",
                  KSU_PRM_TEXT, KSU_PRM_TEXT,
                  KSU_PRM_INT_NOT_0, KSU_PRM_INT_GT_0)) {
      if (argc >= 3) {
         start = sqlite3_value_int(argv[2]);
      }
      if (argc == 4) {
         occ = sqlite3_value_int(argv[3]);
      }

      //VARIABLES!!!!
      int            pos;
      int            count = 0;
      int            i;
      unsigned char *line = (unsigned char *)sqlite3_value_text(argv[0]);
      unsigned char *comp = (unsigned char *)sqlite3_value_text(argv[1]);
      int            len = strlen((char *)comp);
      //end variables...
      // Goes character by character and checks whether it matches
      // the string to match. if it does, it increments count, if
      // count=the occurrence variable, it returns position
      if (start > 0) {
         int comp2;
         int startbyte;

         startbyte = ksu_charpos_to_bytes(line, start - 1);
         if (startbyte == -1) {
            ksu_err_msg(context, KSU_ERR_INV_UTF8, "instr");
            return;
         }
         line = &(line[startbyte]);
         pos = start;
         for (i = 0; i < strlen((char *)line); i++) {
            comp2 = strncmp((char *)&(line[i]), (char *)comp, len);
            if (comp2 == 0) {
               count++;
               if (count == occ) {
                  pos = ksu_charpos_to_bytes((unsigned char *)line, pos);
                  sqlite3_result_int(context, pos);
                  return;
               }
            }
            pos++;
         }
      } else {
         unsigned char *p = &(line[strlen((char *)line) - 1 ]);
         i = 0;
         start = -1 * start;
         while ((p > line) && (i < start)) {
            _ksu_utf8_decr(p);
            i++;
         }
         if (i == start) {
            pos = p - line;
            for (i = p - line; i > 0; i--) {
               if (!strncmp((char *)&(line[i]), (char *)comp, len)) {
                  count++;
                  if (count == occ) {
                     pos = ksu_charpos_to_bytes((unsigned char *)line, pos);
                     pos++;
                     sqlite3_result_int(context, pos);
                     return;
                  }
               }
               pos--;
            }
         }
      }
      sqlite3_result_int(context, 0);
    }
}
