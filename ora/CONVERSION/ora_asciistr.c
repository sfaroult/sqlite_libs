/*
 * An adaptation of Oracle's database's asciistr function.
 *
 * http://docs.oracle.com/cd/B14117_01/server.101/b10759/functions006.htm
 * (Note the difference described below)
 *
 * Takes text as the only argument.
 * In the text returned ...
 *  - ASCII characters are not modified (including control characters such as
 *    newline \n)
 *  - Anything else is changed to "\xx" where xx is the hex representation of
 *    the byte.
 *
 * given: "ABÄCDE"
 * return: "AB\c3\84CDE"
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>

// typedef to abstract away what a "byte" is.
typedef unsigned char byte;

typedef struct Array_t {
   unsigned int size;
   unsigned int index;
   byte *data;
} Array;

static void Array_init(Array *a) {
   if (a) {
      a->size = 32; // some smallish size for starters
      a->index = 0;
      a->data = (byte *)sqlite3_malloc(sizeof(byte) * a->size);
      if (a->data) {
         a->data[a->index] = '\0'; // make data look like a zero-length string
      }
   }
}

static void Array_add(Array *a, byte b) {
   // first check if the array has run out of room
   // if so, double its size and reallocate
   if (a && a->data) {
      if (a->index == a->size - 1) {
         a->size *= 2;
         a->data = sqlite3_realloc(a->data, sizeof(byte) * a->size);
      }
      // then, always check if the byte is a normal ascii char
      // if so, add it as-is to data
      if (b < 127) {
         a->data[a->index++] = b;
      }
      // otherwise, we need to add "\xx" to data
      // where xx is the hex representation of a byte
      else {
         // make sure there is enough room for 3 more bytes in data
         // "\xx" is three bytes
         if (a->index + 3 >= a->size - 1) {
            a->size *= 2;
            a->data = sqlite3_realloc(a->data, sizeof(byte) * a->size);
         }
         if (a->data) {
            // now for magic to get the hex representation of a byte
            // hexChars is just all the hex digits in order
            static const char *hexChars = "0123456789abcdef";
            // add a \ to data as is policy
            a->data[a->index++] = '\\';
            // bit shift right 4 in order to get the first hex digit
            a->data[a->index++] = hexChars[b >> 4 & 0xF];
            // and then get the other hex digit
            a->data[a->index++] = hexChars[b & 0xF];
         }
      }
      if (a->data) {
         // always append a '\0' to data in order to keep it looking like
         // a string; do not increment index here so that the '\0' will be
         // overwritten if Array_add is called again
         a->data[a->index] = '\0';
      }
   }
}

static void Array_delete(Array *a) {
   if (a) {
      if (a->data) {
         sqlite3_free(a->data);
         a->data = NULL;
      }
      a->size = 0;
      a->index = 0;
   }
}

extern void ora_asciistr(sqlite3_context *context,
                         int              argc,
                         sqlite3_value  **argv) {

    _ksu_null_if_null_param(argc, argv);
   const unsigned char *string = sqlite3_value_text(argv[0]);
   Array array;
   Array_init(&array);
   {
      // loop over all the bytes fetched
      byte *p;
      for (p = (byte *)string; *p != '\0'; p++) {
         Array_add(&array, *p);
      }
   }
   // extract the string from the array so the array can be disposed of
   unsigned char *ret = (unsigned char *)sqlite3_malloc(sizeof(byte)
                                                        * array.size);
   ret = (unsigned char *)strcpy((char *)ret, (char *)array.data);
   Array_delete(&array);
   // return the string
   sqlite3_result_text(context, (char *)ret, -1, sqlite3_free);
}
