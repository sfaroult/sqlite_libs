#include <stdio.h>
#include <strings.h>
#include <ksu_common.h>

#define CHUNK         100

typedef struct {
  char *aggr;  // Null if default separator
  int   sz;
  int   len;
} STRING_AGG_CONTEXT_T;

extern void pg_string_agg_step(sqlite3_context *context,
                             int              argc,
                             sqlite3_value  **argv) {
  STRING_AGG_CONTEXT_T *ctx;
  char                 *s;
  int                   slen;
  char                 *sep;
  int                   seplen;

  _ksu_null_if_null_param(argc, argv);
  if (sqlite3_value_type(argv[0]) == SQLITE_BLOB) {
    slen = sqlite3_value_bytes(argv[0]);
    seplen = sqlite3_value_bytes(argv[1]);
    s = (char *)sqlite3_value_text(argv[0]);
    sep = (char *)sqlite3_value_text(argv[1]);
  } else {
    s = (char *)sqlite3_value_text(argv[0]);
    sep = (char *)sqlite3_value_text(argv[1]);
    slen = strlen(s);
    seplen = strlen(sep);
  }
  ctx = (STRING_AGG_CONTEXT_T *)sqlite3_aggregate_context(context,
                              sizeof(STRING_AGG_CONTEXT_T));
  if (ctx) {
    if (ctx->sz == 0) {
      if ((ctx->aggr = (char *)sqlite3_malloc((1 + (slen+seplen)/CHUNK)
                                 *  CHUNK))
               == (char *)NULL) {
        sqlite3_result_error_nomem(context);
        return;
      }
      ctx->sz = (1 + (slen+seplen)/CHUNK) * CHUNK;
      memcpy(ctx->aggr, s, slen);
      ctx->len = slen;
    } else {
      if ((ctx->len + seplen + slen) > ctx->sz) {
        if ((ctx->aggr = (char *)sqlite3_realloc(ctx->aggr,
                             ctx->sz + (1 + (slen+seplen)/CHUNK)
                                 *  CHUNK))
               == (char *)NULL) {
          sqlite3_result_error_nomem(context);
          return;
        }
        ctx->sz += (1 + (slen+seplen)/CHUNK) * CHUNK;
      }
      memcpy(&(ctx->aggr[ctx->len]), sep, seplen);
      ctx->len += seplen;
      memcpy(&(ctx->aggr[ctx->len]), s, slen);
      ctx->len += slen;
    }
  }
}

extern void pg_string_agg_final(sqlite3_context *context) {
  STRING_AGG_CONTEXT_T *ctx;

  ctx = (STRING_AGG_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->sz == 0) {
      sqlite3_result_null(context);
    } else {
      // SQLite3 will free what we have allocated
      sqlite3_result_text(context, (char *)ctx->aggr, ctx->len, sqlite3_free);
      ctx->aggr = (char *)NULL;
      ctx->sz = 0;
      ctx->len = 0;
    }
  } else {
    sqlite3_result_null(context);
  }
}
