#ifndef KSU_UTF8_H
#define KSU_UTF8_H

// --- UTF-8 ---

#ifndef SQLITE_SKIP_UTF8
// Macro copied from the sqlite code
#define SQLITE_SKIP_UTF8(zIn) {                        \
      if ((*(zIn++)) >= 0xc0) {                        \
         while ((*zIn & 0xc0) == 0x80) { zIn++; }      \
      }                                                \
   }

#endif

// If SQLITE_SKIP_UTF8 above is an utf8-aware p++,
// this is p--
#define _ksu_utf8_decr(p) { while ((*(--p) & 0xc0) == 0x80); }

#define _ksu_utf8_copychar(pin, pout) {      \
      if ((*pout = *pin) >= 0xc0) {                    \
         pin++;                              \
         pout++;                             \
         while ((*pin & 0xc0) == 0x80) {     \
           *pout = *pin;                     \
           pin++;                            \
           pout++;                           \
         }                                   \
      } else {                               \
         if (*pout) {                        \
           pin++;                            \
           pout++;                           \
         }                                   \
      }                                      \
   }

extern          int   ksu_charlen(unsigned char *p);
extern          int   ksu_bytes_to_charpos(unsigned char *s, int pos);
// ksu_charpos_to_bytes returns -1 if s isn't a valid UTF-8 string
extern          int   ksu_charpos_to_bytes(unsigned char *s, int pos);
// ksu_lax_charpos_to_bytes doesn't validate UTF-8 characters
extern          int   ksu_lax_charpos_to_bytes(unsigned char *s, int pos);
extern unsigned int   ksu_utf8_to_codepoint(const unsigned char *u,
                                            int *lenptr);
// Returns the length in bytes, 0 if invalid
extern          int   ksu_is_utf8(const unsigned char *u);

// A "letter" is defined as something that has a capital and
// small variant
extern          int   ksu_is_letter(const unsigned char *s);

// strchr-like function for utf8 characters. Returns NULL if
// "needle" is an invalid utf8 character.
extern unsigned char *ksu_uft8search(const unsigned char *haystack, 
                                     const unsigned char *needle);
// ------------------------------------------------------
// The second argument to these functions must be an 
// array of at least 5 elements (it will store a single
// utf8 character). A pointer to this array will be 
// returned if functions succeed.
// ------------------------------------------------------
extern unsigned char *ksu_decimal_to_utf8(const unsigned int d,
                                          unsigned char *utf8);
extern unsigned char *ksu_codepoint_to_utf8(const unsigned int cp,
                                            unsigned char *utf8);
extern unsigned char *ksu_utf8_charlower(const unsigned char *s,
                                         unsigned char *utf8);
extern unsigned char *ksu_utf8_charupper(const unsigned char *s,
                                         unsigned char *utf8);
// See comment below about ksu_utf8_unaccent()
extern unsigned char *ksu_utf8_charunaccent(const unsigned char *s,
                                            unsigned char *utf8);

// ------------------------------------------------------
// All the following functions return a malloc'ed string
// that must be freed by sqlite3_free
// ------------------------------------------------------
//  ksu_utf8_lower() and ksu_utf8_upper() apply to a whole
//  string
extern unsigned char *ksu_utf8_lower(const unsigned char *s);
extern unsigned char *ksu_utf8_upper(const unsigned char *s);
// Unaccent is very crude. It doesn't do a proper conversion,
// such as replacing ü by ue - the accent goes and it becomes u
extern unsigned char *ksu_utf8_unaccent(const unsigned char *s);

#endif
