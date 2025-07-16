/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2007 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       stringconv.h
 *  @ingroup    container
 *  @brief      A collection of string manipulating functions
 *
 ********************************************************************
 *
 *  @lastmodified 26.10.07    Mathias Reichardt
 *  @lastmodified 19.01.10    Sebastian Schulz
 *  @lastmodified 22.02.13    Vincenzo Brancato
 *  @lastmodified 02.09.13    Constantin Heisler
 *
 ********************************************************************/

#ifndef SRC_FRAMEWORK_COMMANDLINEPARSER_STRINGCONV_H
#define SRC_FRAMEWORK_COMMANDLINEPARSER_STRINGCONV_H

#include "structures.h"

// switches for String_mask()
#define STRING_UNMASK 0
#define STRING_MASK   1

// switches for String_matchRegex()
#define REGEX_NOT_CASESENSITIVE 0
#define REGEX_CASESENSITIVE     1

extern const char *String_masktable[][2];
extern const char *DescriptionToHtml_masktable[][2];
extern const char *XML_masktable[][2];
extern const char *gnuplotmask[][2];

/** @brief Remove starting and ending white space characters.
  *
  * definition: _ means whitespace
  * e.g.
  * before trim:
  *  ____this_text_starts_and_endes_with_white_space____________
  * after trim:
  * this_text_starts_and_endes_with_white_space
  */
char*  String_trim(char* string);
void   String_removeSpaces(char* instr);
void   String_normalizeSpaces(char* string);

/** @brief Converts a given string to lower case.
  */
static inline void String_toLower(char* string) {
  for (; *string != '\0'; string++) {
    *string = tolower(*string);
  }
}

/** @brief Converts a given string to upper case.
  */
static inline void String_toUpper(char* string) {
  for (; *string != '\0'; string++) {
    *string = toupper(*string);
  }
}

size_t String_replace(const char* oldstring, char** newstring, const char* find, const char* replace);
void   String_mask(const char* oldstring, char **newstring, long mask, const char* stringmask[][2]);
void   String_makeTokenList (const char* list, char*** result, ulong* count, char separator); ///< separates a string into substrings
void   String_makeFrames(const char* list, long** frames, long* count);                     ///< reads a comma separated list and returns an array of numbers
                                                                                            ///< also intervals can be given. e.g. 4,6,8-10,16

void   String_makePositiveFrames(long** frames, long* count, long lastframe);               ///< get the right frames -1 will become the lastframe
void   String_makeSortFrames(long** frames, long* count);                                   ///< sort an array with numbers in it, e.g. given by String_makeFrames()
bool   String_makeBoundingBox2D(const char* bbox2Dstr, boundingbox2D_t* value);
bool   String_makeBoundingBox3D(const char* bbox3Dstr, boundingbox3D_t* value);
bool   String_makeFloat(const char* paramstring, REAL* value);                              ///< typecast with error msg string ->float, returns true if success
bool   String_getGermanFloat(const char* str, REAL* value);                                 ///< typecast with error msg string ->float (if dezimal separator is '.' or ','), returns true if success
bool   String_makeVectorFloat(const char* vectorstr, REAL* vector, size_t n);               ///< get a vector array from a string, accepts eg.. [3.3,5.1,6]
bool   String_makeLong(const char* paramstring, long* value);                               ///< typecast with error msg string ->long, returns true if success
bool   String_makeUlong(const char* paramstring, unsigned long* value);                     ///< typecast with error msg string ->unsigned long, returns true if success
bool   String_makeVectorLong(const char* coordstr, long* coord, size_t n);                  ///< gets a coordinates array from a string, accepts eg.. [3,5,6]
bool   String_makeBool(const char* paramstring, bool* value);                               ///< typecast with error msg string ->bool, returns true if success
void   String_makeDynamicLongList(const char* list, long** values, long* count);            ///< read a list of longs with arbitrary length, also accepting intervals
void   String_makeDynamicFloatList(const char* list, REAL** values, long* count);           ///< read a list of REALs with arbitrary length
bool   String_makeUsingColumns(const char* paramstring, long** columns, long* count);
bool   String_makeMatrixCoord(const char* matrixstr, long* matrixpoint, size_t n);          ///< parses a string containing a matrixcoordinate, accepts eg.. 2x33x4
bool   String_makePeriodicFlag(const char* periodicvalue, bool periodicflag[3]);

void   String_createVAString(char** buffer, const char* format, va_list* ap) ATTR_WARN_PRINTF(2, 0);
void   String_createFormatedString(char** buffer, const char* format, ...) ATTR_WARN_PRINTF(2, 3);
void   String_appendFormatedVAString(char** dest, const char* format, va_list* ap) ATTR_WARN_PRINTF(2, 0);
void   String_appendFormatedString(char** dest, const char* format, ...) ATTR_WARN_PRINTF(2, 3);
size_t String_appendChar(char** dest, char c);
size_t String_append(char** dest, const char* toappend);

/** @brief Generates filenames with a numbered postfix extension.
  *
  * @verbatim
  * char *value = String_generateFilename("abc.txt", 5);
  * @endverbatim
  *
  * This would produce: "abc_5.txt"
  *
  * @note The returned string has to be freed by the caller
  *
  * @param name The filename.
  * @param number The postfix number.
  *
  * @return The name with a numbered postfix extension.
  */
char* String_generateFilename(char* name, long number);

void   String_createBlocktext(char** buffer, size_t blocklength, const char* text);
void   String_writeBlocktext(FILE* fp, size_t blocklength, const char* text);

/** @brief Make a copy of the key in a line.
  *
  * @param entry  A line entry looks like key=value
  *
  * @return a copy of the resulting string
  */
char*  String_getKey(char* entry);

/** @brief Make a copy of the value in a line.
  *
  * @param entry  A line entry looks like key=value
  *
  * @return a copy of the resulting string
  */
char*  String_getValue(char* entry);

int    String_naturalCompare(const void* p1, const void* p2);
// bool   String_matchRegex(const char* string, const char* pattern, bool casesensitive);        ///< Check if a given string matches a pattern

/**
 * @brief Process a string using pandoc (default: from markdown to html)
 * @return true on success, false otherwise
 */
// bool   String_pandoc(const char* oldstring, char** newstring, const char* format_from, const char* format_to);

char** StringVec_copy(char** src, size_t count);                                              ///< Copy a vector of strings
int    StringVec_compare(const void* stringptr1, const void* stringptr2);                     ///< Compare two strings (e.g. for qsort and bsearch)
int    StringVec_naturalCompare(const void* stringptr1, const void* stringptr2);              ///< Compare two strings (e.g. for qsort and bsearch)

static inline void StringVec_createPrefixedVector(char*** prefixedvector, const char* prefix, ulong count) {
  (*prefixedvector) = (char**) Calloc(count, sizeof(char*));
  for (ulong i = 0; i < count; i++) {
    String_createFormatedString(&(*prefixedvector)[i], "%s%ld", prefix, i);
  }
}

static inline bool String_isEqual(const char* str1, const char* str2) {
  return (strcmp(str1, str2) == 0);
}

void String_findAllWordsInLine(char* line, char*** result, long* count);

#endif
