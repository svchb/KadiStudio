/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2024 IDM - Institute of Digital Materials Science @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       variant.h
 *  @ingroup    lib
 *  @brief      Interface for easy parsing of different types of commandline parameters.
 *
 ********************************************************************
 *
 *  @lastmodified 21.11.24 Michael Selzer
 *
 ********************************************************************/

#ifndef LIB_VARIANT_H
#define LIB_VARIANT_H

#include "parameter.h"

typedef char* (*toString_fct)(const void*);
typedef bool (*fromString_fct)(const char*, void*);

typedef struct variant_s {
  variant_type    type;
  const char     *typestring;
  const char     *typeinfostring;
  toString_fct    toString;
  fromString_fct  fromString;
} Variant;

extern const Variant variant_flag;
extern const Variant variant_long;
extern const Variant variant_vector_long;
extern const Variant variant_vector_n_long;
extern const Variant variant_list_long;
extern const Variant variant_real;
extern const Variant variant_vector_real;
extern const Variant variant_vector_n_real;
extern const Variant variant_list_real;
extern const Variant variant_frame;
extern const Variant variant_string;
extern const Variant variant_boundingbox2d;
extern const Variant variant_boundingbox3d;
extern const Variant variant_tokenlist;
extern const Variant variant_filein;
extern const Variant variant_fileout;
extern const Variant variant_periodicflag;

static inline const Variant* getVariant(int type) {
  switch (type) {
    case PARAM_FLAG:           return &variant_flag;
    case PARAM_LONG:           return &variant_long;
    case PARAM_VECTOR_LONG:    return &variant_vector_long;
    case PARAM_VECTOR_N_LONG:  return &variant_vector_n_long;
    case PARAM_LIST_LONG:      return &variant_list_long;
    case PARAM_REAL:           return &variant_real;
    case PARAM_VECTOR_REAL:    return &variant_vector_real;
    case PARAM_VECTOR_N_REAL:  return &variant_vector_n_real;
    case PARAM_LIST_REAL:      return &variant_list_real;
    case PARAM_FRAME:          return &variant_frame;
    case PARAM_BOUNDINGBOX_2D: return &variant_boundingbox2d;
    case PARAM_BOUNDINGBOX_3D: return &variant_boundingbox3d;
    case PARAM_TOKENLIST:      return &variant_tokenlist;
    case PARAM_STRING:         return &variant_string;
    case PARAM_FILEIN:         return &variant_filein;
    case PARAM_FILEOUT:        return &variant_fileout;
    case PARAM_PERIODICFLAG:   return &variant_periodicflag;
    default:
      myexit(ERROR_BUG, "Unknown type %i", type);
  }
}

#endif
