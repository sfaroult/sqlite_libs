#define ARRAY_SIZE    8

typedef struct {
                char  code; 
                short val;
               } VAL_T;

static VAL_T G_vals[ARRAY_SIZE] = {{'M', 1000},
                                   {'D', 500},
                                   {'C', 100},
                                   {'L', 50},
                                   {'X', 10},
                                   {'V', 5},
                                   {'I', 1},
                                   {'*', 0}};

static short get_val(char code) {
    short i = 0;

    G_vals[ARRAY_SIZE - 1].code = code;
    while (G_vals[i].code != toupper(code)) {
       i++;
    }
    if (i == ARRAY_SIZE - 1) {
       return 0;
    } else {
       return G_vals[i].val;
    } 
}

static int compute_roman(char *str) {
    char *p;
    int   val = 0;
    int   next_val;
    int   remainder;

    if (str && *str) {
       p = str;
       p++;
       if (*p == '\0') {
          val = get_val(*str);
       } else {
          remainder = compute_roman(p);
          val = get_val(*str);
          next_val = get_val(*p);
          if (val < next_val) {
             val = remainder - val;
          } else {
             val = remainder + val;
          }
       }
    }
    return val;
}
