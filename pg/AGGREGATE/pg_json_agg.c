#include <stdio.h>
#include <strings.h>
#include <ksu_common.h>

#define CHUNK         100

typedef struct {
  char *aggr;  // Null if default separator
  int   sz;
  int   len;
} STRING_AGG_CONTEXT_T;

extern void pg_json_agg_step(sqlite3_context *context,
                             int              argc,
                             sqlite3_value  **argv) {
  STRING_AGG_CONTEXT_T *ctx;
  char                 *s;
  int                   slen;
  int                   typ;

  typ = sqlite3_value_type(argv[0]);
  if (typ == SQLITE_NULL) {
    return;
  }
  if (typ == SQLITE_BLOB) {
    ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "json_agg");
    return;
  }
  s = (char *)sqlite3_value_text(argv[0]);
  slen = strlen(s) + 3;
  ctx = (STRING_AGG_CONTEXT_T *)sqlite3_aggregate_context(context,
                              sizeof(STRING_AGG_CONTEXT_T));
  if (ctx) {
    if (ctx->sz == 0) {
      if ((ctx->aggr = (char *)sqlite3_malloc((1 + (slen+1)/CHUNK)
                                 *  CHUNK))
               == (char *)NULL) {
        sqlite3_result_error_nomem(context);
        return;
      }
      ctx->sz = (1 + (slen+1)/CHUNK) * CHUNK;
      if ((typ == SQLITE_INTEGER) || (typ == SQLITE_FLOAT)) {
        strcpy(ctx->aggr, "[");
        strcat(ctx->aggr, s);
      } else {
        strcpy(ctx->aggr, "[\"");
        strcat(ctx->aggr, s);
        strcat(ctx->aggr, "\"");
      }
      ctx->len = strlen(ctx->aggr);
    } else {
      if ((ctx->len + 1 + slen) > ctx->sz) {
        if ((ctx->aggr = (char *)sqlite3_realloc(ctx->aggr,
                             ctx->sz + (1 + (slen+1)/CHUNK)
                                 *  CHUNK))
               == (char *)NULL) {
          sqlite3_result_error_nomem(context);
          return;
        }
        ctx->sz += (1 + (slen+1)/CHUNK) * CHUNK;
      }
      if ((typ == SQLITE_INTEGER) || (typ == SQLITE_FLOAT)) {
        ctx->aggr[ctx->len] = ',';
        (ctx->len)++;
      } else {
        memcpy(&(ctx->aggr[ctx->len]), ",\"", 2);
        ctx->len += 2;
      }
      memcpy(&(ctx->aggr[ctx->len]), s, strlen(s));
      ctx->len += strlen(s);
      if ((typ != SQLITE_INTEGER) && (typ != SQLITE_FLOAT)) {
        ctx->aggr[ctx->len] = '"';
        (ctx->len)++;
      }
      ctx->aggr[ctx->len] = '\0';
    }
  }
}

extern void pg_json_agg_final(sqlite3_context *context) {
  STRING_AGG_CONTEXT_T *ctx;

  ctx = (STRING_AGG_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->sz == 0) {
      sqlite3_result_null(context);
    } else {
      // SQLite3 will free what we have allocated
      strcat(ctx->aggr, "]");
      sqlite3_result_text(context, ctx->aggr, 1 + ctx->len, sqlite3_free);
      ctx->aggr = (char *)NULL;
      ctx->sz = 0;
      ctx->len = 0;
    }
  } else {
    sqlite3_result_null(context);
  }
}
