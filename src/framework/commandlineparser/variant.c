/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2024 IDM - Institute of Digital Materials Science @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       variant.c
 *  @ingroup    lib
 *  @brief      Interface for easy parsing of different types of commandline parameters.
 *
 ********************************************************************
 *
 *  @lastmodified 21.11.24 Michael Selzer
 *
 ********************************************************************/

#include "wrapper.h"
#include "stringconv.h"
// #include <plot/plot.h>
#include "structures.h"
#include "variant.h"

/** @brief Create a formated string out of a vector
  */
#define STRINGVECTOR(vector, size, formatstring, openbracket, closebracket) \
  do { \
    if (vector == NULL || size == 0) return Strdup(openbracket "" closebracket); \
    char *tostring = Strdup(openbracket); \
    long i=0; \
    for (; i < size-1; i++) { \
      String_appendFormatedString(&tostring, formatstring",", vector[i]); \
    } \
    String_appendFormatedString(&tostring, formatstring "" closebracket, vector[i]); \
    return tostring; \
  } while (0)

/** @brief Create a formated string out of a list
  */
#define STRINGLIST(list, count, formatstring) \
  do { \
    char *tostring = Strdup(""); \
    bool iscontinous = false; \
    if (count) { \
      long j; \
      for (j = 0; j<count-1; j++) { \
        if (list[j]+1 == list[j+1]) { \
          if (iscontinous == false) { \
            String_appendFormatedString(&tostring, formatstring "-", list[j]+1); \
            iscontinous = true; \
          } \
        } else { \
          String_appendFormatedString(&tostring, formatstring ",", list[j]+1); \
          if (iscontinous == true) iscontinous = false; \
        } \
      } \
      String_appendFormatedString(&tostring, formatstring, list[j]+1); \
    } \
    return tostring; \
  } while (0)

static char* long_toString(const void* value) {
  char *tostring = NULL;
  String_createFormatedString(&tostring, "%ld", *(long*)value);
  return tostring;
}

const Variant variant_long = (Variant) {
  .type = PARAM_LONG,
  .typestring = "long",
  .typeinfostring = "<i>",
  .toString = long_toString,
  .fromString = (fromString_fct)String_makeLong
};

static char* real_toString(const void* value) {
  char *tostring = NULL;
  String_createFormatedString(&tostring, "%"REALLENGTH"e", *(REAL*)value);
  return tostring;
}

const Variant variant_real = (Variant) {
  .type = PARAM_REAL,
  .typestring = "real",
  .typeinfostring = "<f>",
  .toString = real_toString,
  .fromString = (fromString_fct)String_makeFloat
};

static char* flag_toString(const void* value) {
  return (*(bool*)value) ? Strdup("true") : NULL;
}

static bool flag_fromString(UNUSED(const char* str), void* value) {
  *(bool*)value = !(*(bool*)value);
  return true;
}

const Variant variant_flag = (Variant) {
  .type = PARAM_FLAG,
  .typestring = "flag",
  .typeinfostring = NULL,
  .toString = flag_toString,
  .fromString = (fromString_fct)flag_fromString
};

static char* vector_long_toString(const void* value) {
  STRINGVECTOR((*(long (*)[3])value), 3, "%li", "[", "]");
}

static bool vector_long_fromString(const char* str, void* value) {
  return String_makeVectorLong(str, (*(long (*)[3])value), 3);
}

const Variant variant_vector_long = (Variant) {
  .type = PARAM_VECTOR_LONG,
  .typestring = "vector_long",
  .typeinfostring = "[<i>,<i>,<i>]",
  .toString = vector_long_toString,
  .fromString = vector_long_fromString
};

static char* vector_n_long_toString(const void* value) {
  vectorl_n_t *vector = ((vectorl_n_t*)value);
  STRINGVECTOR(vector->vector, vector->size, "%li", "[", "]");
}

// TODO create infostring?
  // if (argument.option == PARAM_OPTIONAL) {
    // printf("vector of size %ld ", size);
  // }
  // Free(str);


static bool vector_n_long_fromString(const char* str, void* value) {
  long size = ((vectorl_n_t*)value)->size;
  ((vectorl_n_t*)value)->vector = MallocIV(size);
  return String_makeVectorLong(str, ((vectorl_n_t*)value)->vector, size);
}

const Variant variant_vector_n_long = (Variant) {
  .type = PARAM_VECTOR_N_LONG,
  .typestring = "vector_n_long",
  .typeinfostring = "[<i>,<i>,...,<i>]",
  .toString = vector_n_long_toString,
  .fromString = vector_n_long_fromString
};

static char* list_long_toString(const void* value) {
  vectorl_n_t *vector = ((vectorl_n_t*)value);
  STRINGLIST(vector->vector, vector->size, "%li");
}

static bool list_long_fromString(const char* str, void* value) {
  vectorl_n_t *list = (vectorl_n_t*) (value);
  String_makeDynamicLongList(str, &(list->vector), &(list->size));
  return (list->vector != NULL);
}

const Variant variant_list_long = (Variant) {
  .type = PARAM_LIST_LONG,
  .typestring = "list_long",
  .typeinfostring = "<i>,<i>-<i>,<i>-<i>%<i>",
  .toString = list_long_toString,
  .fromString = list_long_fromString
};

static char* vector_real_toString(const void* value) {
  STRINGVECTOR((*(REAL (*)[3])value), 3, "%"REALLENGTH"f", "[", "]");
}

static bool vector_real_fromString(const char* str, void* value) {
  return String_makeVectorFloat(str, (*(REAL (*)[3])value), 3);
}

const Variant variant_vector_real = (Variant) {
  .type = PARAM_VECTOR_REAL,
  .typestring = "vector_real",
  .typeinfostring = "[<f>,<f>,<f>]",
  .toString = vector_real_toString,
  .fromString = vector_real_fromString
};

static char* vector_n_real_toString(const void* value) {
  vectorr_n_t *vector = ((vectorr_n_t*)value);
  STRINGVECTOR(vector->vector, vector->size, "%"REALLENGTH"e", "[", "]");
}

// TODO create infostring?
  // if (argument.option == PARAM_OPTIONAL) {
    // printf("vector of size %ld ", size);
  // }
  // Free(str);


static bool vector_n_real_fromString(const char* str, void* value) {
  long size = ((vectorr_n_t*)value)->size;
  ((vectorr_n_t*)value)->vector = MallocFV(size);
  return String_makeVectorFloat(str, ((vectorr_n_t*)value)->vector, size);
}

const Variant variant_vector_n_real = (Variant) {
  .type = PARAM_VECTOR_N_REAL,
  .typestring = "vector_n_real",
  .typeinfostring = "[<f>,<f>,...,<f>]",
  .toString = vector_n_real_toString,
  .fromString = vector_n_real_fromString
};

static char* list_real_toString(const void* value) {
  vectorr_n_t *vector = ((vectorr_n_t*)value);
  STRINGVECTOR(vector->vector, vector->size, "%"REALLENGTH"e", "", "");
}

static bool list_real_fromString(const char* str, void* value) {
  vectorr_n_t *list = (vectorr_n_t*) (value);
  String_makeDynamicFloatList(str, &(list->vector), &(list->size));
  return (list->vector != NULL);
}

const Variant variant_list_real = (Variant) {
  .type = PARAM_LIST_REAL,
  .typestring = "list_real",
  .typeinfostring = "<f>,<f>,...,<f>",
  .toString = list_real_toString,
  .fromString = list_real_fromString
};

static char* frame_toString(const void* value) {
  frames_t *framelist = ((frames_t *)value);
  if (framelist->count == 0) return NULL;
  STRINGLIST(framelist->frames, framelist->count, "%li");
}

static bool frames_fromString(const char* str, frames_t* frames) {
  String_makeFrames(str, &frames->frames, &frames->count);
  return (frames->frames != NULL);
}

const Variant variant_frame = (Variant) {
  .type = PARAM_FRAME,
  .typestring = "frame",
  .typeinfostring = "<i>,<i>-<i>,<i>-<i>%<i>",
  .toString = frame_toString,
  .fromString = (fromString_fct)frames_fromString
};


static char* boundingbox_2d_toString(const void* value) {
  char *tostring = NULL;
  boundingbox2D_t *bbox = (boundingbox2D_t *)value;
  String_createFormatedString(&tostring, "[%li,%li],[%li,%li]",
                              bbox->x1, bbox->y1,
                              bbox->x2, bbox->y2);
  return tostring;
}

const Variant variant_boundingbox2d = (Variant) {
  .type = PARAM_BOUNDINGBOX_2D,
  .typestring = "bbox2D",
  .typeinfostring = "[<i>,<i>],[<i>,<i>]",
  .toString = boundingbox_2d_toString,
  .fromString = (fromString_fct)String_makeBoundingBox2D
};

static char* boundingbox_3d_toString(const void* value) {
  char *tostring = NULL;
  boundingbox3D_t *bbox = (boundingbox3D_t *)value;
  String_createFormatedString(&tostring, "[%li,%li,%li],[%li,%li,%li]",
                              bbox->x1, bbox->y1, bbox->z1,
                              bbox->x2, bbox->y2, bbox->z2);
  return tostring;
}

const Variant variant_boundingbox3d = (Variant) {
  .type = PARAM_BOUNDINGBOX_3D,
  .typestring = "bbox3D",
  .typeinfostring = "[<i>,<i>,<i>],[<i>,<i>,<i>]",
  .toString = boundingbox_3d_toString,
  .fromString = (fromString_fct)String_makeBoundingBox3D
};

static char* tokenlist_toString(const tokenlist_t* tokenlist) {
  if (tokenlist->result == NULL) return NULL;
  char *tostring = Strdup("");
  long i = 0;
  for (; i<tokenlist->count-1; i++) {
    String_appendFormatedString(&tostring, "%s%c", tokenlist->result[i], tokenlist->separator);
  }
  String_appendFormatedString(&tostring, "%s", tokenlist->result[i]);
  return tostring;
}

static bool tokenlist_fromString(const char* value, tokenlist_t* tokenlist) {
  String_makeTokenList(value, &tokenlist->result, &tokenlist->count, tokenlist->separator);
  return (tokenlist->result != NULL);
}

const Variant variant_tokenlist = (Variant) {
  .type = PARAM_TOKENLIST,
  .typestring = "tokenlist",
  .typeinfostring = "<s>%c<s>%c...%c<s>", // TODO
  .toString = (toString_fct)tokenlist_toString,
  .fromString = (fromString_fct)tokenlist_fromString
};

static char* string_toString(const char** value) {
  if (*value == NULL) return NULL;
  return Strdup(*value);
}

static bool string_fromString(const char* str, char** value) {
  if (*value) Free(*value);
  *value = Strdup(str);
  return true;
}

const Variant variant_string = (Variant) {
  .type = PARAM_STRING,
  .typestring = "string",
  .typeinfostring = "<s>",
  .toString = (toString_fct)string_toString,
  .fromString = (fromString_fct)string_fromString
};

const Variant variant_filein = (Variant) {
  .type = PARAM_FILEIN,
  .typestring = "filein",
  .typeinfostring = "<inputfile>",
  .toString = (toString_fct)string_toString,
  .fromString = (fromString_fct)string_fromString
};

const Variant variant_fileout = (Variant) {
  .type = PARAM_FILEOUT,
  .typestring = "fileout",
  .typeinfostring = "<outputfile>",
  .toString = (toString_fct)string_toString,
  .fromString = (fromString_fct)string_fromString
};

static char* periodicflag_toString(const bool* value) {
  if (! (value[X] || value[Y] || value[Z])) return NULL;
  char *tostring = NULL;
  String_createFormatedString(&tostring, "%s%s%s", value[X] ? "x" : "", value[Y] ? "y" : "", value[Z] ? "z" : "");
  return tostring;
}

const Variant variant_periodicflag = (Variant) {
  .type = PARAM_PERIODICFLAG,
  .typestring = "periodic",
  .typeinfostring = "<x|y|z|xy|xz|yz|xyz>",
  .toString = (toString_fct)periodicflag_toString,
  .fromString = (fromString_fct)String_makePeriodicFlag
};
