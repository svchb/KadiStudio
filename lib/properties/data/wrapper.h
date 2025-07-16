/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2001 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       wrapper.h
 *  @ingroup    lib
 *  @brief      Prototypes for the inlined helper functions.
 *
 * The standard C-functions are wrapped for better error control.
 *
 ********************************************************************
 *
 *  @defgroup   wrapper wrapper functions
 *  @ingroup    lib
 *  @brief      This are the sourcefiles of miscellaneous helper functions.
 *
 ********************************************************************
 *
 *  @lastmodified 10.04.07    Sebastian Waniorek
 *  @lastmodified 08.11.08    Michael Selzer
 *
 ********************************************************************/

#ifndef LIB_PROPERTIES_DATA_WRAPPER_H
#define LIB_PROPERTIES_DATA_WRAPPER_H


#include <errno.h>

#ifdef __cplusplus
#include <cstdio>
#include <cstddef>
#include <cstdarg>
#include <cstdlib>
extern "C" {
#else
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#endif

#include <ctype.h>
#include <limits.h>

#include <sys/types.h>
/* fstat */
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

/* memset */
#include <string.h>

/* open */
#include <fcntl.h>

#include <float.h>
#include <math.h>

#ifdef OPENMP
#include <omp.h>
#endif

#ifndef ulong
typedef unsigned long ulong;
#endif
#ifndef uint
typedef unsigned int uint;
#endif

// data type size
#if 0
  // Running REAL as long double has several problems lurking. Use at your own risk.
  // See https://gitlab.iam-mms.kit.edu/imp/pacexd/-/issues/171 (issue 171 on gitlab) for more info.
  #define REAL long double
  #define REALLENGTH "L"
  #define PACE_MPI_REAL MPI_LONG_DOUBLE
  #define REALEPSILON LDBL_EPSILON
#elif 1
  #define REAL double
  #define REALLENGTH "l"
  #define PACE_MPI_REAL MPI_DOUBLE
  #define REALEPSILON DBL_EPSILON
#else
  // Running REAL as float has several problems lurking. Use at your own risk.
  // See https://gitlab.iam-mms.kit.edu/imp/pacexd/-/issues/171 (issue 171 on gitlab) for more info.
  #define REAL float
  #define REALLENGTH ""
  #define PACE_MPI_REAL MPI_FLOAT
  #define REALEPSILON FLT_EPSILON
#endif

#if !defined(__cplusplus) && !defined(__clang__)
#ifndef inline
#  define inline __inline__ __attribute__ ((always_inline))
#endif
#endif

/// maximum size of buffers for textlines
#define MAX_LINE_LENGTH              2048

/// do not print warning (gcc) when function parameter is not used
#define UNUSED(x) x __attribute__ ((__unused__))

#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)

// Translate a Macro to a string
#define STRING(s) INTERNAL_STRING(s)
#define INTERNAL_STRING(s) #s

// Must be three calls to expand the macros correctly
#define __INTERNAL_UNIQUE_2(a, b, c) a ## b ## c
#define __INTERNAL_UNIQUE_1(a, b, c) __INTERNAL_UNIQUE_2(a, b, c)
#define UNIQUE __INTERNAL_UNIQUE_1(__func__, __LINE__, __COUNTER__)

#define MIN(x,y) (((x)<(y)) ? (x) : (y))
#define MAX(x,y) (((x)>(y)) ? (x) : (y))

/** @brief This is a generic swap macro!
  *
  * Apart from the not 100% unique name for the
  * temporary variable this works fine.
  */
#define SWAP(a, b) \
do {\
__typeof__(a) \
__THIS_IS_A_TEMP_AND_UNIQUE_VARIABLE_FOR_A_SWAP_AND_THIS_WORKS__SO_DONT_TOUCH_THIS_IF_YOU_ARE_NOT_UNDERSTAND_THIS_M07H3RFUCK3RS##__FILE__##__func__##__LINE__ = a; \
a = b; \
b = __THIS_IS_A_TEMP_AND_UNIQUE_VARIABLE_FOR_A_SWAP_AND_THIS_WORKS__SO_DONT_TOUCH_THIS_IF_YOU_ARE_NOT_UNDERSTAND_THIS_M07H3RFUCK3RS##__FILE__##__func__##__LINE__;\
} while (0)

// #define SWAP(a, b)  do { a ^= b; b ^= a; a ^= b; } while ( 0 )


// helpers for coordinates
#ifdef __cplusplus
const int X = 0;
const int Y = 1;
const int Z = 2;
const int UNSPECIFIED = -1;
#else
#define X                               0
#define Y                               1
#define Z                               2
#define UNSPECIFIED                    -1
#endif

extern const char *side_str[6];
extern char       *direction_str[3];

extern char *progname;

/** memory size num_symbols
  * Should be save for the next years ;-)
  */
extern char   *memorysymbols[];
extern size_t  num_symbols;

size_t getBiggestMemorySymbolIndex(size_t bytes);

#if defined(__GNUC__) && !defined(__clang__)
#define ATTR_WARN_PRINTF(X,Y) __attribute__((format(gnu_printf,X,Y)))
#else
#define ATTR_WARN_PRINTF(X,Y) __attribute__((format(printf,X,Y)))
#endif

/* functions with variable parameters can not be inlined */
void fprintc(FILE* fp, long color1, long color2, const char* fmt, ...) ATTR_WARN_PRINTF(4, 5);
void myerror(const char* const fmt, ...) ATTR_WARN_PRINTF(1, 2);
void myexit(int exitcode, const char *fmt, ...) ATTR_WARN_PRINTF(2, 3) __attribute__ ((noreturn));
void myinfo(const char* const fmt, ...) ATTR_WARN_PRINTF(1, 2);
void mywarn(const char* const fmt, ...) ATTR_WARN_PRINTF(1, 2);
void mydebug_internal(const char *file, int line, const char *func, const char* const fmt, ...) ATTR_WARN_PRINTF(4, 5);

#define mydebug(...) mydebug_internal(__FILE__, __LINE__, __func__, __VA_ARGS__)

#define VERBOSE_QUIET   0
#define VERBOSE_NORMAL  1
#define VERBOSE_CONTROL 2
#define VERBOSE_DEBUG   3

extern long verbose;

void  myverbose(ulong msglevel, ulong verboselevel, const char* const fmt, ...) ATTR_WARN_PRINTF(3, 4);


void  RealPath(const char* const path, char** resolved_path);
int   Mkdir(const char* const pathname, mode_t mode);
int   Remove(const char* const dirname);
bool  checkFileWritable(const char* const filename);
void  AtExit(void (*function)(void));

void  getNumberOfProcessors(long* cpucount, long* cpucountmax);
long  getNumberOfInstances(char* prognames);

void  measureCPUTime(REAL* systemtime, REAL* usertime, REAL* totaltime);
void  printProcessUsageTime(void);
void  printResourceInformation(void);

const char* makehumanreadabletime(double eta);

/** @brief Convert time into a formatted string.
  *
  * if dhmsStyle is defined the format is the following
  *  "%llu d %llu h %llu m %llu s %llu ns" = 1s 245651654 ns
  * else the format is
  *  "%'llu" = 1.245.651.654
  *
  * @param buffer   char array that is big enough to store the result (char length is 69).
  * @param time     time, in ns that should converted into a sting.
  */
const char* makeHumanReadableTimell(char* buffer, size_t size, long long unsigned int time);
const char* sprinttime(void);

// #include "wrapper/errorcodes.h"
#define EXIT_OK 0
#define EXIT_ERROR 1
#define ERROR_PARAM 123
#define ERROR_FILEIO 234
#define ERROR_NOT_VALIDATED -996
#define ERROR_INCONSISTENCY -997
#define ERROR_NOT_IMPLEMENTED -998
#define ERROR_BUG -999

#include "mem.h"
// #include "wrapper/math.h"
// #include "wrapper/angleconvert.h"
// #include "wrapper/vector.h"
// #include "wrapper/rotation.h"
// #include "wrapper/fileio.h"
// #ifdef MPI
// #include "wrapper/mpiwrapper.h"
// #endif

// #include <globals.h>
// #include "wrapper/stdio.h"

// #include "exception.h"

#ifdef INSIDESOLVER
#define PACE3DMAIN(x) \
do { \
  progname=x; \
  setupSignals(EXCEPTION_ALL&~(EXCEPTION_INTERRUPT|EXCEPTION_CHILD)); \
  I18N \
} while (0)
#else
#ifdef MPI
#define PACE3DMAIN(x) \
do { \
  progname=x; \
  setupSignals(EXCEPTION_ALL&~(EXCEPTION_INTERRUPT|EXCEPTION_CHILD)); \
  setupMPI(&argc, &argv); \
  I18N \
  AtExit(deinitMPI); \
  AtExit(printProcessUsageTime); \
} while (0)
#else
#define PACE3DMAIN(x) \
do { \
  progname=x; \
  setupSignals(EXCEPTION_ALL&~(EXCEPTION_INTERRUPT|EXCEPTION_CHILD)); \
  I18N \
  AtExit(printProcessUsageTime); \
} while (0)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
