/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2007 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       parameter.h
 *  @ingroup    lib
 *  @brief      Interface for easy access of commandline parameters.
 *
 ********************************************************************
 *
 *  @lastmodified 20.07.07    Borys Trakhter
 *  @lastmodified 26.10.07    Mathias Reichardt
 *  @lastmodified 15.05.14    Thomas Welte
 *
 ********************************************************************/

#ifndef SRC_FRAMEWORK_COMMANDLINEPARSER_PARAMETER_H
#define SRC_FRAMEWORK_COMMANDLINEPARSER_PARAMETER_H


typedef enum variant_type_e {
  PARAM_LONG,
  PARAM_VECTOR_LONG,
  PARAM_VECTOR_N_LONG,
  PARAM_LIST_LONG,
  PARAM_REAL,
  PARAM_VECTOR_N_REAL,
  PARAM_VECTOR_REAL,
  PARAM_LIST_REAL,
  PARAM_FRAME,
  // PARAM_CHAR,
  PARAM_STRING,
  PARAM_FILEIN,
  PARAM_FILEOUT,
  PARAM_FLAG,
  // PARAM_BOOL,
  PARAM_BOUNDINGBOX_2D,
  PARAM_BOUNDINGBOX_3D,
  PARAM_TOKENLIST,
  PARAM_PLOT,
  PARAM_PERIODICFLAG
} variant_type;

#define PARAM_REQUIRED      1024
#define PARAM_OPTIONAL      2048

#define ARGUMENT(arg)  (sizeof(arg)/sizeof(arg[0]))


typedef struct argument_s {
  const char *longname;
  const char  name;
  const long  option;
  const char *andparams;
  const char *interval;
  const char *description;
  const long  flag;
  const void *parameter;
} argument_t;

typedef struct toolparam_s {
  const char        *description;
  const char        *example;
  const argument_t  *arguments;
  const long         istested;
} toolparam_t;


bool  getParams(int argc, char* argv[], toolparam_t tool, size_t count);
void  printUsage(const char* progname, toolparam_t tool, size_t count);

#endif
