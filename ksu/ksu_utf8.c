#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sqlite3.h>

#include <ksu_utf8.h>
#include <ksu_unicode.h>

extern unsigned char *ksu_codepoint_to_utf8(const unsigned int  cp,
                                            unsigned char      *val) {

   if (cp <= 0x7f) {
      sprintf((char *)val, "%c", (char)cp);
   } else {
      if ((cp >= 0x80) && (cp <= 0x07ff)) {
         // 2-char UTF8 character
         val[0] = cp / 64 + 192;
         val[1] = cp % 64 + 128;
         val[2] = '\0';
      } else if (((cp >= 0x0800) && (cp <= 0xd7ff))
               || ((cp >= 0xe000) && (cp <= 0xffff))) {
         // 1110xxxx 10xxxxxx 10xxxxxx
         val[0] = cp / 4096 + 224;
         val[1] = (cp % 4096) / 64 + 128;
         val[2] = cp % 64 + 128;
         val[3] = '\0';
      } else {
         val[0] = cp / 262144 + 240;
         val[1] = (cp % 262144) / 4096 + 128;
         val[2] = (cp % 4096) / 64 + 128;
         val[3] = cp % 64 + 128;
         val[4] = '\0';
      } 
   }
   return val;
}

extern int ksu_is_utf8(const unsigned char *u) {
   // Validate utf8 character.
   // Returns the length, 0 if invalid.
   int len = 0;

   if (u) {
      if (*u < 0xc0) {
         len = 1;
      } else {
         if ((*u & 0xe0) == 0xc0) {
            // U-00000080 - U-000007FF : 110xxxxx 10xxxxxx
            len = 2;
         } else if ((*u & 0xf0) == 0xe0) {
            // U-00000800 - U-0000FFFF : 1110xxxx 10xxxxxx 10xxxxxx
            len = 3;
         } else if ((*u & 0xf8) == 0xf0) {
            // U-00010000 - U-001FFFFF : 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            len = 4;
         } else {
            // malformed UTF-8 character
            return 0;
         }
         // Check that the UTF-8 character is OK
         int i;
         for (i = 1; i < len; i++ ) {
            if ((u[i] & 0xC0) != 0x80) {
               return 0;
            }
         }
      }
   }
   return len;
}

extern unsigned char *ksu_decimal_to_utf8(const unsigned int  d,
                                          unsigned char      *val) {
    // Works like chr() in Oracle: convert to hex, then convert hex to
    // character. Returns NULL if invalid UTF-8 character.
    char   hex[50];
    int    len;
    int    i;

    sprintf(hex, "%X", d);
    len = strlen(hex);
    if ((len > 8) || (len % 2)){
        return NULL;  // Invalid UTF-8
    } else {
       for (i = 0; i < len/2; i++) {
          val[i] = 16 * (isdigit(hex[2*i]) ? hex[2*i] - '0' : 
                                 10 + hex[2*i] - 'A')
                   + (isdigit(hex[2*i+1]) ? hex[2*i+1] - '0' : 
                                 10 + hex[2*i+1] - 'A');
       }
       val[i] = '\0';
    }
    if (ksu_is_utf8(val) == 0) {
       return NULL;
    }
    return val;
}

extern unsigned int ksu_utf8_to_codepoint(const unsigned char *u,
                                          int                 *lenptr) {
   // Returns 0 if something goes wrong
   // Passes back the length
   unsigned int cp = 0;

   *lenptr = 0;
   if (u) {
      if (*u < 0xc0) {
         cp = (unsigned int)*u;
         *lenptr = 1;
      } else {
         *lenptr = ksu_is_utf8(u);
         if (*lenptr == 0) {
            return 0;
         }
         switch (*lenptr) {
            case 2:
                 cp = (u[0] - 192) * 64 + u[1] - 128;
                 break;
            case 3:
                 cp = (u[0] - 224) * 4096
                    + (u[1] - 128) * 64 + u[2] - 128;
                 break;
            default:
                 cp = (u[0] - 240) * 262144
                     + (u[1] - 128) * 4096
                     + (u[2] - 128) * 64 + u[3] - 128;
                 break;
         }
      }
   }
   return cp;
}

extern unsigned char *ksu_utf8_lower(const unsigned char *s) {
    unsigned char *ls = (unsigned char *)NULL;
    unsigned char *p;
    unsigned char *p2;

    if ((p = (unsigned char *)s) != (unsigned char *)NULL) {
                int   len = strlen((char *)s);
       unsigned int   u1;
       unsigned int   u2;
                int   charlen;
                int   i;
       unsigned char *n;
                int   nlen;
       unsigned char  utf8[5];

       if ((ls = (unsigned char *)sqlite3_malloc(1
                       + sizeof(unsigned char) * len * 4))
               != (unsigned char *)NULL) {
          p2 = ls;
          while (*p) {
             if (*p < 0xc0) {
                *p2 = tolower(*p);
                p++;
                p2++;
             } else {
                u1 = ksu_utf8_to_codepoint(p, &charlen);
                if (u1 == 0) {
                   // Invalid UTF-8. Copy as is
                   *p2 = *p;
                   p++;
                   p2++;
                } else {
                   u2 = ksu_unicode_lower(u1);
                   if (u1 == u2) {
                      // No change - copy character as is
                      for (i = 0; i < charlen; i++) {
                         *p2 = *p;
                         p++;
                         p2++;
                      }
                   } else {
                      *p2 = '\0';
                      n = ksu_codepoint_to_utf8(u2, utf8);
                      if (n) {
                         nlen = strlen((char *)n);
                         strcat((char *)p2, (char *)n);
                         p2 += nlen;
                      }
                      p += charlen;
                   }
                }
             }
          }
          *p2 = '\0';
       }
    }
    return ls;
}

extern unsigned char *ksu_utf8_upper(const unsigned char *s) {
    unsigned char *us = (unsigned char *)NULL;
    unsigned char *p;
    unsigned char *p2;

    if ((p = (unsigned char *)s) != (unsigned char *)NULL) {
                int   len = strlen((char *)s);
       unsigned int   u1;
       unsigned int   u2;
                int   charlen;
                int   i;
       unsigned char *n;
                int   nlen;
       unsigned char  utf8[5];

       if ((us = (unsigned char *)sqlite3_malloc(1
                       + sizeof(unsigned char) * len * 4))
               != (unsigned char *)NULL) {
          p2 = us;
          while (*p) {
             if (*p < 0xc0) {
                *p2 = toupper(*p);
                p++;
                p2++;
             } else {
                u1 = ksu_utf8_to_codepoint(p, &charlen);
                if (u1 == 0) {
                   // Invalid UTF-8. Copy as is
                   *p2 = *p;
                   p++;
                   p2++;
                } else {
                   u2 = ksu_unicode_upper(u1);
                   if (u1 == u2) {
                      // No change - copy character as is
                      for (i = 0; i < charlen; i++) {
                         *p2 = *p;
                         p++;
                         p2++;
                      }
                   } else {
                      *p2 = '\0';
                      n = ksu_codepoint_to_utf8(u2, utf8);
                      if (n) {
                         nlen = strlen((char *)n);
                         strcat((char *)p2, (char *)n);
                         p2 += nlen;
                      }
                      p += charlen;
                   }
                }
             }
          }
          *p2 = '\0';
       }
    }
    return us;
}

extern unsigned char *ksu_utf8_unaccent(const unsigned char *s) {
    unsigned char *us = (unsigned char *)NULL;
    unsigned char *p;
    unsigned char *p2;

    if ((p = (unsigned char *)s) != (unsigned char *)NULL) {
                int   len = strlen((char *)s);
       unsigned int   u1;
       unsigned int   u2;
                int   charlen;
                int   i;
       unsigned char *n;
                int   nlen;
       unsigned char  utf8[5];

       if ((us = (unsigned char *)sqlite3_malloc(1
                       + sizeof(unsigned char) * len * 4))
               != (unsigned char *)NULL) {
          p2 = us;
          while (*p) {
             u1 = ksu_utf8_to_codepoint(p, &charlen);
             if (u1 == 0) {
                // Invalid UTF-8. Copy as is
                *p2 = *p;
                p++;
                p2++;
             } else {
                u2 = ksu_unicode_strip_accent(u1);
                if (u1 == u2) {
                   // No change - copy character as is
                   for (i = 0; i < charlen; i++) {
                      *p2 = *p;
                      p++;
                      p2++;
                   }
                } else {
                   *p2 = '\0';
                   n = ksu_codepoint_to_utf8(u2, utf8);
                   if (n) {
                      nlen = strlen((char *)n);
                      strcat((char *)p2, (char *)n);
                      p2 += nlen;
                   }
                   p += charlen;
                }
             }
          }
          *p2 = '\0';
       }
    }
    return us;
}

extern unsigned char *ksu_utf8_charlower(const unsigned char *s,
                                         unsigned char       *ls) {
    unsigned char *p;
    unsigned char *p2;

    ls[0] = '\0';
    if ((p = (unsigned char *)s) != (unsigned char *)NULL) {
       unsigned int   u1;
       unsigned int   u2;
                int   charlen;
                int   i;
       unsigned char *n;
                int   nlen;
       unsigned char  utf8[5];

       p2 = ls;
       if (*p) {
         if (*p < 0xc0) {
           *p2 = tolower(*p);
           *++p2 = '\0';
         } else {
           u1 = ksu_utf8_to_codepoint(p, &charlen);
           if (u1 == 0) {
             // Invalid UTF-8. Copy as is
             *p2 = *p;
             *++p2 = '\0';
           } else {
             u2 = ksu_unicode_lower(u1);
             if (u1 == u2) {
               // No change - copy character as is
               for (i = 0; i < charlen; i++) {
                 *p2 = *p;
                 p++;
                 p2++;
               }
               *p2 = '\0';
             } else {
               n = ksu_codepoint_to_utf8(u2, utf8);
               if (n) {
                 nlen = strlen((char *)n);
                 strncpy((char *)p2, (char *)n, 5);
               }
             }
           }
         }
       }
    }
    return ls;
}

extern unsigned char *ksu_utf8_charupper(const unsigned char *s,
                                         unsigned char       *us) {
    unsigned char *p;
    unsigned char *p2;

    us[0] = '\0';
    if ((p = (unsigned char *)s) != (unsigned char *)NULL) {
       unsigned int   u1;
       unsigned int   u2;
                int   charlen;
                int   i;
       unsigned char *n;
                int   nlen;
       unsigned char  utf8[5];

       p2 = us;
       if (*p) {
         if (*p < 0xc0) {
           *p2 = toupper(*p);
           *++p2 = '\0';
         } else {
           u1 = ksu_utf8_to_codepoint(p, &charlen);
           if (u1 == 0) {
             // Invalid UTF-8. Copy as is
             *p2 = *p;
             *++p2 = '\0';
           } else {
             u2 = ksu_unicode_upper(u1);
             if (u1 == u2) {
               // No change - copy character as is
               for (i = 0; i < charlen; i++) {
                 *p2 = *p;
                 p++;
                 p2++;
               }
               *p2 = '\0';
             } else {
               n = ksu_codepoint_to_utf8(u2, utf8);
               if (n) {
                 nlen = strlen((char *)n);
                 strncpy((char *)p2, (char *)n, 5);
               }
             }
           }
         }
       }
    }
    return us;
}

extern unsigned char *ksu_utf8_charunaccent(const unsigned char *s,
                                            unsigned char       *us) {
    unsigned char *p;
    unsigned char *p2;

    us[0] = '\0';
    if ((p = (unsigned char *)s) != (unsigned char *)NULL) {
       unsigned int   u1;
       unsigned int   u2;
                int   charlen;
                int   i;
       unsigned char *n;
                int   nlen;
       unsigned char  utf8[5];

       p2 = us;
       if (*p) {
         u1 = ksu_utf8_to_codepoint(p, &charlen);
         if (u1 == 0) {
           // Invalid UTF-8. Copy as is
           *p2 = *p;
           *++p2 = '\0';
         } else {
           u2 = ksu_unicode_strip_accent(u1);
           if (u1 == u2) {
             // No change - copy character as is
             for (i = 0; i < charlen; i++) {
               *p2 = *p;
               p++;
               p2++;
             }
             *p2 = '\0';
           } else {
             n = ksu_codepoint_to_utf8(u2, utf8);
             if (n) {
               nlen = strlen((char *)n);
               strncpy((char *)p2, (char *)n, 5);
             }
           }
         }
       }
    }
    return us;
}

extern int ksu_is_letter(const unsigned char *s) {
    // The definition of "letter" is basically whether
    // a lower case or upper case version of it exists ...
             int ret = 0;
             int charlen = 0;
    unsigned int u1;
    unsigned int u2;

    if (s) {
       if (*s < 0xc0) {
          ret = isalpha(*s);
       } else {
          u1 = ksu_utf8_to_codepoint(s, &charlen);
          if (u1 != 0) {
             u2 = ksu_unicode_upper(u1);
             if (u2 != u1) {
                ret = 1;
             } else {
                u2 = ksu_unicode_lower(u1);
                if (u2 != u1) {
                   ret = 1;
                }
             }
          }
       }
    }
    return ret; 
}
//
//  Returns the length of s in CHARACTERS, not bytes
//
extern int ksu_charlen(unsigned char *s) {
   int len = 0;
   unsigned char *p = s;

   while (*p != '\0') {
      len++;
      // Beware that the macro increments p
      SQLITE_SKIP_UTF8(p);
   }
   return len;
}

extern int ksu_bytes_to_charpos(unsigned char *s, int pos) {
  unsigned char *p = s;
  int charcnt = 0;

  while (p < &(s[pos])) {
     SQLITE_SKIP_UTF8(p);
     charcnt++;
  }
  return charcnt;
}

extern int ksu_charpos_to_bytes(unsigned char *s, int pos) {
  int bytecnt = 0;
  int charcnt = 0;
  int len;

  while (charcnt < pos) {
     len = ksu_is_utf8((const unsigned char *)&(s[bytecnt]));
     if (len == 0) {
        return -1; // Invalid UTF-8
     }
     bytecnt += len; 
     charcnt++;
  }
  return bytecnt;
}

extern int ksu_lax_charpos_to_bytes(unsigned char *s, int pos) {
  int            bytecnt = 0;
  int            charcnt = 0;
  unsigned char *p = s;

  if (p && *p) {
    while (*p && (charcnt < pos)) {
      SQLITE_SKIP_UTF8(p);
      charcnt++;
    }
    bytecnt = (int)(p - s);
  }
  return bytecnt;
}

// strchr-like function for utf8 characters. Returns NULL if
// "needle" is an invalid utf8 character.
extern unsigned char *ksu_uft8search(const unsigned char *haystack,
                                     const unsigned char *needle) {
    unsigned char *p = (unsigned char *)NULL;

    if (haystack
        && needle
        && ksu_is_utf8(needle)) {
      p = (unsigned char *)strstr((char *)haystack, (char *)needle);
    }
    return p;
} 
