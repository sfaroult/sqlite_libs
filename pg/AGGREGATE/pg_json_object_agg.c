#include <stdio.h>
#include <strings.h>
#include <ksu_common.h>

#define CHUNK         100

typedef struct {
  char *aggr;  // Null if default separator
  int   sz;
  int   len;
} STRING_AGG_CONTEXT_T;

extern void pg_json_object_agg_step(sqlite3_context *context,
                                    int              argc,
                                    sqlite3_value  **argv) {
  STRING_AGG_CONTEXT_T *ctx;
  char                 *k;
  char                 *v;
  int                   len;
  int                   typ;
  int                   i;

  for (i = 0; i < argc; i++) {
    typ = sqlite3_value_type(argv[i]);
    if (typ == SQLITE_NULL) {
      return;
    }
    if (typ == SQLITE_BLOB) {
      ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "json_agg");
      return;
    }
  }
  k = (char *)sqlite3_value_text(argv[0]);
  v = (char *)sqlite3_value_text(argv[1]);
  len = strlen(k) + strlen(v) + 7;
  ctx = (STRING_AGG_CONTEXT_T *)sqlite3_aggregate_context(context,
                              sizeof(STRING_AGG_CONTEXT_T));
  if (ctx) {
    if (ctx->sz == 0) {
      if ((ctx->aggr = (char *)sqlite3_malloc((1+(len+1)/CHUNK)*CHUNK))
               == (char *)NULL) {
        sqlite3_result_error_nomem(context);
        return;
      }
      ctx->sz = (1 + (len+1)/CHUNK) * CHUNK;
      strcpy(ctx->aggr, "{\"");
      strcat(ctx->aggr, k);
      strcat(ctx->aggr, "\":");
      if ((typ == SQLITE_INTEGER) || (typ == SQLITE_FLOAT)) {
        strcat(ctx->aggr, v);
      } else {
        strcat(ctx->aggr, "\"");
        strcat(ctx->aggr, v);
        strcat(ctx->aggr, "\"");
      }
      ctx->len = strlen(ctx->aggr);
    } else {
      if ((ctx->len + 1 + len) > ctx->sz) {
        if ((ctx->aggr = (char *)sqlite3_realloc(ctx->aggr,
                             ctx->sz + (1 + (len+1)/CHUNK) * CHUNK))
               == (char *)NULL) {
          sqlite3_result_error_nomem(context);
          return;
        }
        ctx->sz += (1 + (len+1)/CHUNK) * CHUNK;
      }
      memcpy(&(ctx->aggr[ctx->len]), ",\"", 2);
      ctx->len += 2;
      len = strlen(k);
      memcpy(&(ctx->aggr[ctx->len]), k, len);
      ctx->len += len;
      memcpy(&(ctx->aggr[ctx->len]), "\":", 2);
      ctx->len += 2;
      if ((typ != SQLITE_INTEGER) && (typ != SQLITE_FLOAT)) {
        ctx->aggr[ctx->len] = '"';
        (ctx->len)++;
      }
      len = strlen(v);
      memcpy(&(ctx->aggr[ctx->len]), v, len);
      ctx->len += len;
      if ((typ != SQLITE_INTEGER) && (typ != SQLITE_FLOAT)) {
        ctx->aggr[ctx->len] = '"';
        (ctx->len)++;
      }
      ctx->aggr[ctx->len] = '\0';
    }
  }
}

extern void pg_json_object_agg_final(sqlite3_context *context) {
  STRING_AGG_CONTEXT_T *ctx;

  ctx = (STRING_AGG_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->sz == 0) {
      sqlite3_result_null(context);
    } else {
      // SQLite3 will free what we have allocated
      strcat(ctx->aggr, "}");
      sqlite3_result_text(context, ctx->aggr, 1 + ctx->len, sqlite3_free);
      ctx->aggr = (char *)NULL;
      ctx->sz = 0;
      ctx->len = 0;
    }
  } else {
    sqlite3_result_null(context);
  }
}
