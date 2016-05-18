#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <ksu_common.h>

#define MAXKEY_LEN      64
#define CHUNK            5

typedef struct node {
           unsigned char key[MAXKEY_LEN + 1];
           unsigned char keylen;
           char *val;
           struct node *left;
           struct node *right;
          } LISTAGG_NODE_T;

typedef struct {
  char           *sep;  // Null if default separator
  char           *ord;
  int             ordlen;
  LISTAGG_NODE_T *tree;
} LISTAGG_CONTEXT_T;


static LISTAGG_NODE_T *new_node(unsigned char *key,
                                unsigned char  keylen,
                                char          *val) {
    LISTAGG_NODE_T *n = (LISTAGG_NODE_T *)NULL;

    if (val && strlen(val)) {
      if ((n = sqlite3_malloc(sizeof(LISTAGG_NODE_T)))
                             != (LISTAGG_NODE_T *)NULL) {
        int len = 1 + strlen(val);

        if ((n->val = (char *)sqlite3_malloc(len)) == (char *)NULL) {
          sqlite3_free(n);
          return (LISTAGG_NODE_T *)NULL;
        }
        (void)strcpy(n->val, val);
        if (keylen) {
          (void)memcpy((char *)n->key, (char *)key, (int)keylen);
          n->keylen = keylen;
        } else {
          n->keylen = 0;
        }
        n->left = (LISTAGG_NODE_T *)NULL;
        n->right = (LISTAGG_NODE_T *)NULL;
      }
    }
    return n;
}

static void free_node(LISTAGG_NODE_T *n) {
    if (n) {
      free_node(n->right);
      free_node(n->left);
      if (n->val) {
        sqlite3_free(n->val);
      }
      sqlite3_free(n);
    }
}

static void add_node(LISTAGG_NODE_T **t_ptr, LISTAGG_NODE_T *n) {
    if (t_ptr && n) {
      LISTAGG_NODE_T *t = *t_ptr;

      if (t == (LISTAGG_NODE_T *)NULL) {
        *t_ptr = n;
      } else {
        int cmp;
       
        if (t->keylen) {
          cmp = memcmp(t->key, n->key,
                       (int)(t->keylen<n->keylen?t->keylen:n->keylen));
        } else {
          cmp = strcmp(t->val, n->val);
        }
        if (cmp == 0) {
          if (n->keylen < t->keylen) {
            add_node(&(t->left), n);
          } else {
            add_node(&(t->right), n);
          }
        } else if (cmp > 0) {
          // First greater than second
          add_node(&(t->left), n);
        } else {
          add_node(&(t->right), n);
        }
      }
    }
}

static char *dupl_value(sqlite3_value *arg) {
    int   len;
    char *v;
    char *r = (char *)NULL;
    int   typ;

    if (arg) {
      typ = sqlite3_value_type(arg);
      if ((typ != SQLITE_NULL) && (typ != SQLITE_BLOB)) {
        v = (char *)sqlite3_value_text(arg);
        len = strlen(v);
        if ((r = (char *)sqlite3_malloc(len + 1)) != (char *)NULL) {
          (void)strcpy(r, v);
        }
      }
    }
    return r;
}

static char is_order_spec(char *s, int cols) {
   // cols is the number of columns to order.
   // Cannot be smaller than the ordering spec.
   char ret = 0;
   int  len = 0;
   int  i;

   if (s) {
     len = strlen(s);
     if (len <= cols) {
       ret = 1;
       for (i = 0; i < len; i++) {
         if ((toupper(s[i]) != 'A')
             && (toupper(s[i]) != 'D')) {
           ret = 0;
           break;
         }
       }
     } 
   }
   return ret;
}

//
//    SQLite3 LISTAGG usage:
//       LISTAGG(col, ['sep',], 'order spec', col, ...)
//
extern void ora_listagg_step(sqlite3_context *context,
                             int              argc,
                             sqlite3_value  **argv) {
  LISTAGG_CONTEXT_T *ctx;
  int                typ;
  int                i;
  int                j;

  _ksu_check_arg_cnt(argc, 1, -1, "listagg");
  typ = sqlite3_value_type(argv[0]);
  if (typ != SQLITE_NULL) {
    if (typ == SQLITE_BLOB) {
      ksu_err_msg(context, KSU_ERR_INV_DATATYPE , "listagg");
      return;
    }
    ctx = (LISTAGG_CONTEXT_T *)sqlite3_aggregate_context(context,
                              sizeof(LISTAGG_CONTEXT_T));
    if (ctx) {
      LISTAGG_NODE_T *n;
      unsigned char   key[MAXKEY_LEN];
      unsigned char   keylen = 0;
      unsigned char   prev_keylen = 0;
      char           *val;
      int             len;
      int             colnum;

      if (!ctx->tree) {
        char *ord;

        switch(argc) {
          case 1:
               break;
          case 2:
               // Separator is necessarily the 2nd parameter
               ctx->sep = dupl_value(argv[1]);
               break;
          case 3:
               // 'A' or 'D' is necessarily the 2nd parameter
               ord = (char *)sqlite3_value_text(argv[1]);
               if (!is_order_spec(ord, 1)) {
                 // Syntax error
                 ksu_err_msg(context, KSU_ERR_INV_FIELD, ord, "listagg");
                 return;
               }
               ctx->ord = dupl_value(argv[1]);
               break;
          default:
               // order spec is either the 2nd parameter (default
               // separator) or the 3rd one 
               ord = (char *)sqlite3_value_text(argv[1]);
               if (!is_order_spec(ord, argc - 2)) {
                 ord = (char *)sqlite3_value_text(argv[2]);
                 if (!is_order_spec(ord, argc - 3)) {
                   // Syntax error
                   ksu_err_msg(context, KSU_ERR_INV_FIELD, ord, "listagg");
                   return;
                 } else {
                   ctx->ord = dupl_value(argv[2]);
                   ctx->sep = dupl_value(argv[1]);
                 }
               } else {
                 ctx->ord = dupl_value(argv[1]);
               }
               break;
        }
        if (ctx->ord) {
          ctx->ordlen = strlen(ctx->ord);
        } else {
          ctx->ordlen = 0;
        }
      }
      for (i = (ctx->sep ? 3 : 2); i < argc; i++) {
        colnum = i - (ctx->sep ? 3 : 2); // Starts from 0
        typ = sqlite3_value_type(argv[i]);
        switch (typ) {
          case SQLITE_NULL:
               break; // Ignore
          case SQLITE_BLOB:
               ksu_err_msg(context, KSU_ERR_INV_DATATYPE , "listagg");
               return;
          default:
               // All datatypes are stored as chars
               // to avoid endianness issues with sorting
               // (we negate bytes for descending order)
               val = (char *)sqlite3_value_text(argv[i]);
               len = strlen(val);
               if (len > (int)(MAXKEY_LEN - (int)keylen)) {
                 len = (int)(MAXKEY_LEN - (int)keylen);
               }
               if (len) {
                 (void)memcpy(&(key[keylen]), val, len);
                 keylen += (unsigned char)len;
               }
               break;
         }
         if ((colnum < ctx->ordlen)
             && (toupper(ctx->ord[colnum]) == 'D')) {
           for (j = prev_keylen; j < keylen; j++) {
             key[j] = ~key[j];
           }
         }
         prev_keylen = keylen; 
      }
      if ((n = new_node(key, keylen, (char *)sqlite3_value_text(argv[0])))
                != (LISTAGG_NODE_T *)NULL) {
        add_node(&(ctx->tree), n);
      } else {
        free_node(ctx->tree);
        ctx->tree = (LISTAGG_NODE_T *)NULL;
        sqlite3_result_error_nomem(context);
        return;
      }      
    }
  }
}

static void create_aggr(char           **s_ptr,
                        int             *sz_ptr,
                        LISTAGG_NODE_T  *t,
                        char            *sep) {
   char *s;

   if (s_ptr && t && t->val) {
     create_aggr(s_ptr, sz_ptr, t->left, sep);
     if ((s = *s_ptr) == (char *)NULL) {
       if (*sz_ptr == 0) { // Otherwise we have already had a memory problem
         *sz_ptr = (strlen(t->val) + (sep ? strlen(sep) : 1)) * CHUNK;
         if ((s = (char *)sqlite3_malloc(*sz_ptr + 1)) != (char *)NULL) {
           *s = '\0';
           *s_ptr = s;
         }
       }
     } else {
       if (*sz_ptr < (strlen(s) + strlen(t->val) + (sep ? strlen(sep) : 1))) {
         // Too small. Resize.
         if ((*s_ptr = (char *)sqlite3_realloc(s,
                                   1 + *sz_ptr
                                     + (strlen(t->val)
                                        + (sep ? strlen(sep) : 1)) * CHUNK))
                                    != (char *)NULL) {
           s = *s_ptr;
           *sz_ptr += (strlen(t->val)
                       + (sep ? strlen(sep) : 1)) * CHUNK;
         }
       }
     }
     if (s) {
       if (*s) {
         strcat(s, (sep? sep : ","));
       }
       (void)strcat(s, t->val);
       // Free memory ASAP
       sqlite3_free(t->val);
       t->val = (char *)NULL; // To avoid accidents
       create_aggr(s_ptr, sz_ptr, t->right, sep);
     }
   }
}

extern void ora_listagg_final(sqlite3_context *context) {
  LISTAGG_CONTEXT_T *ctx;
  char              *result = (char *)NULL;
  int                result_sz = 0;

  ctx = (LISTAGG_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->tree) {
      create_aggr(&result, &result_sz, ctx->tree, ctx->sep);
      // Now the result string should be built. Free whatever we can.
      if (ctx->sep) {
        sqlite3_free(ctx->sep);
        ctx->sep = (char *)NULL;
      }
      if (ctx->ord) {
        sqlite3_free(ctx->ord);
        ctx->ord = (char *)NULL;
      }
      free_node(ctx->tree);
      ctx->tree = (LISTAGG_NODE_T *)NULL;
      // Pass to SQLite
      if (result_sz) {
        sqlite3_result_text(context, result, -1, sqlite3_free);
      } else {
        sqlite3_result_error_nomem(context);
      }
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
