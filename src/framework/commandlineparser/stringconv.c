/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2007 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       stringconv.c
 *  @ingroup    container
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 27.01.09    Michael Selzer
 *  @lastmodified 19.01.10    Sebastian Schulz
 *  @lastmodified 22.02.13    Vincenzo Brancato
 *  @lastmodified 31.03.15    Constantin Heisler
 *
 ********************************************************************/

#include "wrapper.h"
// #include <wrapper/regex.h>
#include "qsort.h"

#include "stringconv.h"

/** @brief Table for the masking of special characters in the infile
  */
const char *String_masktable[][2] = {
  {"@",  "@@"},
  {"ä",  "@ae"},
  {"ö",  "@oe"},
  {"ü",  "@ue"},
  {"ß",  "@sz"},
  {" ",  "@_"},
  {"\"", "@*"},
  {"\n", "@n"},
  {"/",  "@s"},
  {"\\", "@b"},
  {",",  "@c"},
  {"#",  "@S"},
  {"=",  "@e"},
  {"",   ""}
};

const char *DescriptionToHtml_masktable[][2] = {
  {"@infile",      "<h4>Infile</h4><hr/><pre><code>"},
  {"@endinfile",   "</code></pre><hr/>"},
  {"@listing",     "<h4>Listing</h4><hr/><pre><code>"},
  {"@endlisting",  "</code></pre><hr/>"},
  {"@hint",        "<h4><font color=\"#00C000\">Hint</font></h4><hr/>"},
  {"@endhint",     "<hr/>"},
  {"@tip",         "<h4><font color=\"#0000FF\">Tip</font></h4><hr/>"},
  {"@endtip",      "<hr/>"},
  {"@code",        "<h4><font color=\"#0000FF\">Code</font></h4><hr/>"},
  {"@endcode",     "<hr/>"},
  {"@warning",     "<h4><font color=\"#FF0000\">Warning</font></h4><hr/>"},
  {"@endwarning",  "<hr/>"},
  {"@example",     "<h4><font color=\"#FFCC00\">Example</font></h4><hr/>"},
  {"@endexample",  "<hr/>"},
  {"",   ""}
};

const char *XML_masktable[][2]  = {
  { "&",  "&amp;" },
  { "<",  "&lt;"  },
  { ">",  "&gt;"  },
  { "\"", "&quot;"},
  { "\'", "&apos;"},
  { "\t", "&#09;" },
  { "\n", "&#10;" },
  { "\r", "&#13;" },
  { "",   ""      }
};

const char *gnuplotmask[][2] = {
  {"\\Alpha",   "{/Symbol A}"},
  {"\\alpha",   "{/Symbol a}"},
  {"\\Beta",    "{/Symbol B}"},
  {"\\beta",    "{/Symbol b}"},
  {"\\Chi",     "{/Symbol C}"},
  {"\\chi",     "{/Symbol c}"},
  {"\\Delta",   "{/Symbol D}"},
  {"\\delta",   "{/Symbol d}"},
  {"\\Epsilon", "{/Symbol E}"},
  {"\\epsilon", "{/Symbol e}"},
  {"\\Eta",     "{/Symbol H}"},
  {"\\eta",     "{/Symbol h}"},
  {"\\Gamma",   "{/Symbol G}"},
  {"\\gamma",   "{/Symbol g}"},
  {"\\Iota",    "{/Symbol I}"},
  {"\\iota",    "{/Symbol i}"},
  {"\\Kappa",   "{/Symbol K}"},
  {"\\kappa",   "{/Symbol k}"},
  {"\\Lambda",  "{/Symbol L}"},
  {"\\lambda",  "{/Symbol l}"},
  {"\\Mu",      "{/Symbol M}"},
  {"\\mu",      "{/Symbol m}"},
  {"\\Nu",      "{/Symbol N}"},
  {"\\nu",      "{/Symbol n}"},
  {"\\Omicron", "{/Symbol O}"},
  {"\\omicron", "{/Symbol o}"},
  {"\\Omega",   "{/Symbol W}"},
  {"\\omega",   "{/Symbol w}"},
  {"\\Phi",     "{/Symbol F}"},
  {"\\phi",     "{/Symbol f}"},
  {"\\Psi",     "{/Symbol Y}"},
  {"\\psi",     "{/Symbol y}"},
  {"\\Pi",      "{/Symbol P}"},
  {"\\pi",      "{/Symbol p}"},
  {"\\Rho",     "{/Symbol R}"},
  {"\\rho",     "{/Symbol r}"},
  {"\\Sigma",   "{/Symbol S}"},
  {"\\sigma",   "{/Symbol s}"},
  {"\\Theta",   "{/Symbol Q}"},
  {"\\theta",   "{/Symbol q}"},
  {"\\Tau",     "{/Symbol T}"},
  {"\\tau",     "{/Symbol t}"},
  {"\\Upsilon", "{/Symbol U}"},
  {"\\upsilon", "{/Symbol u}"},
  {"\\Xi",      "{/Symbol X}"},
  {"\\xi",      "{/Symbol x}"},
  {"\\Zeta",    "{/Symbol Z}"},
  {"\\zeta",    "{/Symbol z}"},
  {"", ""}
};

static long parseSizeParameter(long* value, const char* format, va_list* arg_list) {
  char *next_format_pos;

  if (strncmp(format, "[%i]", 4) == 0) {
    *value = (va_arg(*arg_list, long));
    return 4;
  } else {
    if (*format != '[') {
      myexit(ERROR_BUG, "missing opening brace '[' in format string");
    }
    errno = 0;
    *value = strtol(format+1, &next_format_pos, 10);

    if (errno) {
      myexit(ERROR_BUG, "error: reading an integer for a parameter value failed (%s).", strerror(errno));
    }
    if (format == next_format_pos) {    // no conversion was done
      myexit(ERROR_BUG, "error: reading an integer for a parameter value failed (%s).", ((*format)=='\0')?"end of line":"not a valid integer value");
    }

    if (*next_format_pos != ']') {
      myexit(ERROR_BUG, "missing closing brace ']' in format string");
    }
    return next_format_pos+1-format;
  }
}

static size_t getSliceParameters(const char* format, va_list* arg_list, long** slicevector, long* slicevectorsize) {
  size_t  length;

  // check for opening brace, indicating a slice-vector
  if (*format != '{') {
    *slicevector = NULL;
    return 0;
  }
  // TODO the input of vectors should also be possible like this: (2,3,...) and not only like that: %v
  if (strncmp(format+1, "%v", 2) != 0) {
    myexit(ERROR_BUG, "only long-arrays can define a sliceVector");
  }

  *slicevector = va_arg(*arg_list, long*);

  length = 3;
  length += parseSizeParameter(slicevectorsize, format+length, arg_list);
  if (*(format+length) != 'i') {
    myexit(ERROR_BUG, "only long-arrays can define a sliceVector");
  }

  length++;
  if (*(format + length) != '}') {
    myexit(ERROR_BUG, "missing closing brace '}' in format string");
  }
  return length + 1;
}

/** @brief Remove starting and ending white space characters.
  *
  * @param string input string with
  *
  * definition: _ means whitespace
  * e.g.
  * before trim:
  * @verbatim
  *  ____this_text_starts_and_endes_with_white_space____________
  * @endverbatim
  * after trim:
  * @verbatim
  * this_text_starts_and_endes_with_white_space
  * @endverbatim
  * @note Memory of the string will not be shrunk.
  *
  * @return pointer to the input string
  */
char* String_trim(char* string) {
  long   i;
  size_t slen = strlen(string)+1;

  for (i = 0; isspace(string[i]); i++);
  if (i > 0 && string[i] != '\0') {
    slen -= i;
    memmove(string, &string[i], slen);
  }
  for (i = slen-2; i >= 0; i--) {
    if (!isspace(string[i])) {
      string[i+1] = '\0';
      return string;
    }
  }
  // is empty string
  string[0] = '\0';

  return string;
}

/** @brief Get rid of the spaces in the string.
  *
  * The space is removed by shifting the string to the left and is filled with null-termination from the right.
  * @note String will not be shrunk.
  */
void String_removeSpaces(char* instr) {
  // first scan through string if at least one whitespace
  char *scaner = instr;
  while (*scaner != '\0' && !isspace(*scaner)) scaner++;

  // only if whitespaces are left remove them
  char *cursor = scaner;
  while (*scaner++ != '\0') { // we start removing it
    if (!isspace(*scaner)) *cursor++ = *scaner;
  }
}

/** @brief Collapses all consecutive spaces in a string into a single space.
  *
  * @note String will not be shrunk.
  */
void String_normalizeSpaces(char* string) {
  // first scan through string for two consecutive whitespaces
  char *scaner = string;
  while (*scaner != '\0' && !(isspace(*scaner) && isspace(*(scaner+1)))) scaner++;

  // only if we found two consecutive whitespaces remove them
  char *cursor = scaner;
  while (*scaner++ != '\0') { // we start removing it
    while (isspace(*scaner) && isspace(*(scaner+1))) scaner++;
    *cursor++ = *scaner;
  }
}

/** @brief Copy a string to new string and replace 'find' by 'replace'.
  *
  * @param oldstring   input string
  * @param newstring   reference to string where each appearance of 'find' is replaced by 'replace', could also be the referrence to the 'oldstring'
  * @param find        string which should be replaced
  * @param replace     string which replaces 'find'
  */
size_t String_replace(const char* oldstring, char** newstring, const char* find, const char* replace) {
  size_t    size = MAX_LINE_LENGTH;
  long      buffersizeleft = size-1; // reserving the '\0'-termination
  size_t    strlenfind    = strlen(find);
  size_t    strlenreplace = strlen(replace);

  if (newstring == NULL) {
    myexit(ERROR_BUG, "Result pointer for new string is NULL.");
  }
  if (strlenfind == 0) {
    myexit(ERROR_BUG, "'find'-string can not be empty.");
  }

  // Allocate memory for the resulting new String
  char *bufferstring = MallocS(size);
  char *cur_str_pos  = bufferstring;

  while (*oldstring != '\0') {
    while ((long)(buffersizeleft-strlenreplace) <= 0) {
      // newstring could not be completely filled
      buffersizeleft += size;
      size *= 2;
      size_t newpos = cur_str_pos-bufferstring;
      bufferstring = (char*)Realloc(bufferstring, size);
      cur_str_pos = bufferstring+newpos;
    }
    if (strncmp(oldstring, find, strlenfind) != 0) {
      // 'find' does not appear at the current string position => copy current char to newstring
      *cur_str_pos = *oldstring;
      cur_str_pos++;
      buffersizeleft--;
      oldstring++;
    } else {
      // 'find' appears at the current string position => copy 'replace' to newstring
      strcpy(cur_str_pos, replace);
      cur_str_pos    += strlenreplace;
      buffersizeleft -= strlenreplace;
      oldstring      += strlenfind;
    }
  }
  *cur_str_pos = '\0';

  size -= buffersizeleft;
  if (*newstring) Free(*newstring);
  *newstring = Realloc(bufferstring, size);
  return size;
}

/** @brief Mask a string by a given table.
  *
  * @note for masking of whole strings the order in this table is important, first matched rule is the one which is applied
  *
  * @param oldstring   input string
  * @param newstring   reference to string where each appearance of 'find' is replaced by 'replace', could also be the reference to the 'oldstring'
  * @param mask 1, each appearance of a string part in the first column will be replaced by the corresponding string in the right column,
  *             0 will unmask in the opposite manner
  * @param stringmask  table with character sequences to be masked, terminated by zero length string
  */
void String_mask(const char* oldstring, char** newstring, long mask, const char* stringmask[][2]) {
  size_t  i, maskindex;
  size_t  maskstrlen, unmaskstrlen;

  size_t  writepos;
  size_t  resultsize = MAX_LINE_LENGTH;
  char   *result = MallocS(resultsize);

  // create fast lookup table for first char of all replacement rules
  char    fastlookup[256]; // one for every ascii char
  memset(fastlookup, 0x00, 256*sizeof(char));
  for (i = 0; stringmask[i][!mask][0] != '\0'; i++) {
    fastlookup[(int)(stringmask[i][!mask][0])] = '1'; // remember first char of all strings in masktable
  }

  // walk through the string replace check real table if fastlookup char is found
  writepos = 0;
  i        = 0;
  while (oldstring[i] != '\0') {
    // candidate for replacement
    if (fastlookup[(int)oldstring[i]] == '1') {
      // is candidate of a real hit
      bool hit = false;
      for (maskindex = 0; stringmask[maskindex][!mask][0] != '\0'; maskindex++) {
        maskstrlen = strlen(stringmask[maskindex][!mask]);
        if (strncmp(&(oldstring[i]), stringmask[maskindex][!mask], maskstrlen) == 0) {
          hit = true;
          break;
        }
      }

      if (hit) {
        // move future read pointer over replaced string
        i += maskstrlen;

        // write masktable string
        // make room
        unmaskstrlen = strlen(stringmask[maskindex][mask]);
        if (writepos + unmaskstrlen >= resultsize) {
          resultsize = unmaskstrlen + resultsize*2;
          result = Realloc(result, resultsize);
        }

        // copy
        strcpy(&(result[writepos]), stringmask[maskindex][mask]);
        writepos += unmaskstrlen;

        continue;
      }
    }

    // not interesting just copy char
    if (writepos >= resultsize) {
      resultsize *= 2;
      result = Realloc(result, resultsize);
    }
    result[writepos] = oldstring[i];
    writepos++;

    i++;
  }

  // trim and add null termination
  result = Realloc(result, writepos+1);
  result[writepos] = '\0';

  // store result
  if (oldstring == *newstring) Free(*newstring);
  *newstring = result;
}

/** @brief Separate a string separated by a char into single tokens.
  *
  * @param list      input string
  * @param result    result string array
  * @param count     number of tokens found
  * @param separator separating character
  */
void String_makeTokenList(const char* list, char*** result, ulong* count, char separator) {
  long  i;

  // filter invalid argument lists
  if (list == NULL || strlen(list) == 0) {
    myexit(ERROR_BUG, "No argument list to tokenize");
  }
  if (list[0] == separator) {
    myexit(ERROR_PARAM, "A '%c'-separated list must not start with a '%c'.", separator, separator);
  }
  if (list[strlen(list) - 1] == separator) {
    myexit(ERROR_PARAM, "A '%c'-separated list must not end with a '%c'", separator, separator);
  }
  if (result == NULL) {
    myexit(ERROR_BUG, "Result pointer for list is NULL.");
  }
  if (count == NULL) {
    myexit(ERROR_BUG, "Count pointer for list is NULL.");
  }

  *count = 1;
  // count separators, for i separators there are i+1 arguments in a valid list
  for (i = 0; list[i] != '\0'; i++) {
    if (list[i] == separator) {
      if (list[i + 1] == separator) {
        myexit(ERROR_PARAM, "A separated list must not contain two consecutive '%c'.", separator);
      }
      (*count)++;
    }
  }

  // copy the original argument list because parsing works destructive
  char *tokenizelist = Strdup(list);

  // Separate the token-separated values into the result array
  char  separatorstr[2] = { separator, '\0' };
  char *token = strtok(tokenizelist, separatorstr);

  if (token == NULL) {
    mywarn("empty string or not tokenizable (%s)", tokenizelist);
    *count = 0;
    *result = NULL;
  } else {

    // Allocate memory for the resulting array of strings
    *result = (char**) Malloc( (*count) * sizeof(char*) );

    i = 0;
    (*result)[i++] = Strdup(token);

    while ((token = strtok(NULL, separatorstr)) != NULL) {
      (*result)[i++] = Strdup(token);
    }
  }

  Free(tokenizelist);
}

void String_makeFrames(const char* list, long** frames, long* count) {
  //1. create long list from string
  String_makeDynamicLongList(list, frames, count);
  //2. modify frames to start at 0 (convert Fortran-style -> C-style numbering)
  for (long i = 0; i < *count; i++) {
    (*frames)[i]--;
    if ((*frames)[i] < 0) myexit(ERROR_PARAM, "All frame values need to be positive.");
  }
}

void String_makeDynamicLongList(const char* list, long** values, long* count) {
  char **result=NULL;
  ulong  tokencount;
  long   index = 0;
  long   value;
  long   var1, var2, step = 0;
  long   arguments;

  String_makeTokenList(list, &result, &tokencount, ',');

  *count = 0;

  for (ulong token = 0; token < tokencount; token++) {
    // Get Format 1-2%3
    arguments = sscanf(result[token], "%ld-%ld%%%ld", &var1, &var2, &step);
    if (arguments <= 0) { // no match
      myexit(ERROR_PARAM, "No value regonized.");
    } else if (arguments == 3) { // 1-2%3
    } else if (arguments == 2) { // 1-2
      step = (var1 > var2) ? -1 : 1;
    } else if (arguments == 1) { // 1
      step = 1;
      var2 = var1;
    }

    if (step * (var1 - var2) > 0) {
      myexit(ERROR_PARAM, "Step size for inverted intervals must be negativ, and positive for normal intervals.");
    }

    // Add long to list
    *count += labs((long)((labs(var2-var1)+1) / step)) + 1;
    (*values) = (long *)Realloc((*values), (*count) * sizeof(long));
    for (value = var1; step * value <= step * var2; value+=step) {
      (*values)[index] = value;
      index++;
    }
  }
  *count = index;

  FreeM(result, tokencount);
}

void String_makePositiveFrames(long** frames, long* count, long lastframe) {
  long *frame = *frames;

  for (long i = 0; i < *count; i++) {
    if (frame[i] < 0) {
      frame[i] += lastframe;
    }
  }
}

void String_makeSortFrames(long** frames, long* count) {
  // long *frame = *frames;
  // long  i, j;
  //
  // if (*count > 1) {
  //   qsort(frame, *count, sizeof(long), compare_long_ascending);
  //   for (i = 0, j = 1; j < *count; j++) {
  //     if (frame[i] != frame[j]) {
  //       i++;
  //       if (i < j) {
  //         frame[i] = frame[j];
  //       }
  //     }
  //   }
  //   if (i+1 < (*count)) {
  //     (*count) = i+1;
  //     (*frames) = (long*)Realloc((*frames), (*count) * sizeof(long));
  //   }
  // }
}

/** @brief Analyze the entry, identification two 2D-coordinates and save the result in boundingbox2D_t
  *
  * @param bbox2Dstr     to analyse string
  * @param value         boundingbox2D_t, where the identificated values are saved
  *
  * @return true if the analysis was successful, false otherwise
  */
bool String_makeBoundingBox2D(const char* bbox2Dstr, boundingbox2D_t* value) {
  char *endptr;

  // filter invalid argument
  if (bbox2Dstr == NULL) {
    myexit(ERROR_BUG, "No bounding box as argument given.");
  }

  if (*bbox2Dstr++ != '[') {
    return false;
  }

  value->x1 = strtol(bbox2Dstr, &endptr, 10);
  if (bbox2Dstr == endptr) {
    return false;
  }
  bbox2Dstr = endptr;

  if (*bbox2Dstr++ != ',') {
    return false;
  }

  value->y1 = strtol(bbox2Dstr, &endptr, 10);
  if (bbox2Dstr == endptr) {
    return false;
  }
  bbox2Dstr = endptr;

  if (*bbox2Dstr++ != ']' || *bbox2Dstr++ != ',' || *bbox2Dstr++ != '[') {
    return false;
  }

  value->x2 = strtol(bbox2Dstr, &endptr, 10);
  if (bbox2Dstr == endptr) {
    return false;
  }
  bbox2Dstr = endptr;

  if (*bbox2Dstr++ != ',') {
    return false;
  }

  value->y2 = strtol(bbox2Dstr, &endptr, 10);
  if (bbox2Dstr == endptr) {
    return false;
  }
  bbox2Dstr = endptr;

  if (*bbox2Dstr++ != ']' || *bbox2Dstr != '\0') {
    return false;
  }

  if ((value->y1 > value->y2) || (value->x1 > value->x2)) {
    myexit(ERROR_PARAM, "You have specified wrong 2D bounding box coordinates.\n" \
           "      It is incorrect that the first coordinate is larger than the second coordinate.\n" \
           "      Your input was [%ld,%ld],[%ld,%ld]", value->x1, value->y1, value->x2, value->y2);
  }

  return true;
}

/** @brief Analyze the entry, identification two 3D-coordinates and save the result in boundingbox3D_t
  *
  * @param bbox3Dstr     to analyse string
  * @param value         boundingbox3D_t, where the identificated values are saved
  *
  *  @return true if the analysis was successful, false otherwise
  */
bool String_makeBoundingBox3D(const char* bbox3Dstr, boundingbox3D_t* value) {
  char *endptr;

  // filter invalid arguments
  if (bbox3Dstr == NULL) {
    myexit(ERROR_BUG, "No bounding box as argument given.");
  }

  if (*bbox3Dstr++ != '[') {
    return false;
  }

  value->x1 = strtol(bbox3Dstr, &endptr, 10);
  if (bbox3Dstr == endptr) {
    return false;
  }
  bbox3Dstr = endptr;

  if (*bbox3Dstr++ != ',') {
    return false;
  }

  value->y1 = strtol(bbox3Dstr, &endptr, 10);
  if (bbox3Dstr == endptr) {
    return false;
  }
  bbox3Dstr = endptr;

  if (*bbox3Dstr++ != ',') {
    return false;
  }

  value->z1 = strtol(bbox3Dstr, &endptr, 10);
  if (bbox3Dstr == endptr) {
    return false;
  }
  bbox3Dstr = endptr;

  if (*bbox3Dstr++ != ']' || *bbox3Dstr++ != ',' || *bbox3Dstr++ != '[') {
    return false;
  }

  value->x2 = strtol(bbox3Dstr, &endptr, 10);
  if (bbox3Dstr == endptr) {
    return false;
  }
  bbox3Dstr = endptr;

  if (*bbox3Dstr++ != ',') {
    return false;
  }

  value->y2 = strtol(bbox3Dstr, &endptr, 10);
  if (bbox3Dstr == endptr) {
    return false;
  }
  bbox3Dstr = endptr;

  if (*bbox3Dstr++ != ',') {
    return false;
  }
  value->z2 = strtol(bbox3Dstr, &endptr, 10);
  if (bbox3Dstr == endptr) {
    return false;
  }
  bbox3Dstr = endptr;

  if (*bbox3Dstr++ != ']' || *bbox3Dstr != '\0') {
    return false;
  }

  return true;
}

bool String_makeFloat(const char* paramstring, REAL* value) {
  char  *endptr = NULL;

  // reset errno to see conversion result
  errno = 0;

  *value = (REAL)strtod(paramstring, &endptr);

  if (errno != 0 || &paramstring[strlen(paramstring)] != endptr) {
    // error
    if (errno == 0) errno = EINVAL;
    return false;
  }

  // cast ok
  return true;
}

void String_makeDynamicFloatList(const char* list, REAL** values, long* count) {
  char **result=NULL;
  ulong  tokencount;

  String_makeTokenList(list, &result, &tokencount, ',');

  *count = tokencount;
  *values = MallocFV(tokencount);
  for (ulong token = 0; token < tokencount; token++) {
    if (!String_makeFloat(result[token], &((*values)[token]))) {
      myexit(ERROR_PARAM, "Failed to convert string to float. In token %lu of \"%s\" : \"%s\" is not a floating point number.", token + 1, list, result[token]);
    }
  }
  FreeM(result, tokencount);
}

bool String_getGermanFloat(const char* str, REAL* value) {
  char *str2 = Strdup(str);

  // replace ',' if exists
  char *pos = strrchr(str2, ',');
  if (pos) {
    *pos = '.';
  }

  long rc = String_makeFloat(str2, value);
  Free(str2);

  return rc;
}

/** @brief Parses a string containing the coordinates into n float values.
  *
  * @param vectorstr string array containing the vector (like this: "[X,Y,...]")
  * @param vector    float array containing n parsed coordinates
  * @param n         number of vector elements (which should (and must) be parsed) within the string
  *
  * @return true if the analysis was successful, false otherwise
  */
bool String_makeVectorFloat(const char* vectorstr, REAL* vector, size_t n) {
  char   *endptr = NULL;

  if (*vectorstr++ != '[') {
    return false;
  }

  vector[0] = (REAL)strtod(vectorstr, &endptr);
  if (vectorstr == endptr) {
    return false;
  }
  vectorstr = endptr;

  for (size_t i = 1; i < n; i++) {

    if (*vectorstr != ',') {
      return false;
    }
    vectorstr++;

    vector[i] = (REAL)strtod(vectorstr, &endptr);

    if (vectorstr == endptr) {
      return false;
    }
    vectorstr = endptr;
  }

  if (*vectorstr != ']' || (*(vectorstr+1)) != '\0') {
    return false;
  }

  return true;
}

bool String_makeLong(const char* paramstring, long* value) {
  char  *endptr = NULL;

  errno = 0;

  // convert to long, base = 10, or hex starting with 0x
  *value = strtol(paramstring, &endptr, 0);

  if (errno != 0 || &paramstring[strlen(paramstring)] != endptr) {
    // error
    if (errno == 0) errno = EINVAL;
    return false;
  }

  // cast ok
  return true;
}

bool String_makeUlong(const char* paramstring, unsigned long* value) {
  char  *endptr = NULL;

  errno = 0;

  // convert to unsigned long, base = 10, or hex starting with 0x
  *value = strtoul(paramstring, &endptr, 0);

  if (errno != 0 || &paramstring[strlen(paramstring)] != endptr) {
    // error
    if (errno == 0) errno = EINVAL;
    return false;
  }

  // cast ok
  return true;
}

/** @brief Parses a string containing a vector into n integer values.
  *
  * @param vectorstr  string array containing the vector elements like this: "[x0,x1,...]"
  * @param vector     integer array containing n parsed coordinates
  * @param n          number of vector elements (which should (and must) be parsed) within the string
  *
  * @return true if the analysis was successful, false otherwise
  */
bool String_makeVectorLong(const char* vectorstr, long* vector, size_t n) {
  char   *endptr = NULL;

  if (*vectorstr++ != '[') {
    return false;
  }

  vector[0] = strtol(vectorstr, &endptr, 10);

  if (vectorstr == endptr) {
    return false;
  }
  vectorstr = endptr;

  for (size_t i = 1; i < n; i++) {

    if (*vectorstr != ',') {
      return false;
    }
    vectorstr++;

    vector[i] = strtol(vectorstr, &endptr, 10);

    if (vectorstr == endptr) {
      return false;
    }
    vectorstr = endptr;
  }

  if (*vectorstr != ']' || (*(vectorstr+1)) != '\0') {
    return false;
  }

  return true;
}

bool String_makeBool(const char* paramstring, bool* value) {
  size_t pos = 0;
  if (*paramstring == '0') {
    *value = false;
    pos++;
  } else if (*paramstring == '1') {
    *value = true;
    pos++;
  } else if (strcmp(paramstring, "on") == 0) {
    *value = true;
    pos += 2;
  } else if (strcmp(paramstring, "off") == 0) {
    *value = false;
    pos += 3;
  } else if (strcmp(paramstring, "true") == 0) {
    *value = true;
    pos += 4;
  } else if (strcmp(paramstring, "false") == 0) {
    *value = false;
    pos += 5;
  }
  if (pos == 0 || paramstring[pos] != '\0') {
    mywarn("Could not identify boolean value (must be 0 / 1 / off / on / false / true).");
    return false;
  }
  return true;
}

bool String_makeUsingColumns(const char* paramstring, long** columns, long* count) {
  char  *endptr;
  const char  *tmplist;

  // filter invalid argument lists
  if (paramstring == NULL) {
    myexit(ERROR_BUG, "No arguments to tokenize");
  }

  tmplist = paramstring;
  *count = 0;

  while (*tmplist != '\0') {
    if (*tmplist == '[') (*count) += 2;
    tmplist++;
  }

  // Allocate memory for the columns
  *columns = MallocIV(*count);

  long i = 0;

  while (*paramstring != '\0') {
    if (*paramstring++ != '[') {                         // '[' in [x:y]
      return false;
    }

    (*columns)[i++] = strtol(paramstring, &endptr, 10);  // 'x' in [x:y]
    if (paramstring == endptr) {
      return false;
    }
    paramstring = endptr;

    if (*paramstring++ != ':') {                         //  ':' in [x:y]
      return false;
    }

    (*columns)[i++] = strtol(paramstring, &endptr, 10);  // 'y' in [x:y]
    if (paramstring == endptr) {
      return false;
    }
    paramstring = endptr;

    if (*paramstring++ != ']') {                         // ']' in [x:y]
      return false;
    }
  }

  // cast ok
  return true;
}

/** @brief Parses a string containing a matrixcoordinate.
 *
 * @param matrixstr    string array containing the matrixcoordinate (like this: "2x2")
 * @param matrixpoint  integer array containing parsed coordinate
 * @param n            matrix dimension (which should (and must) be parsed) within the string
 *
 * @return true if the analysis was successful, false otherwise
 */
bool String_makeMatrixCoord(const char* matrixstr, long* matrixpoint, size_t n) {
  char   *endptr = NULL;

  matrixpoint[0] = strtol(matrixstr, &endptr, 10);

  if (matrixstr == endptr) {
    return false;
  }

  for (size_t i = 1; i < n; i++) {

    for (matrixstr = endptr; *matrixstr != '\0'; matrixstr++) {
      if (!isblank(*matrixstr)) break;
    }
    if (*matrixstr != 'x') {
      return false;
    }
    matrixstr++;

    matrixpoint[i] = strtol(matrixstr, &endptr, 10);

    if (matrixstr == endptr) {
      return false;
    }
  }

  for (matrixstr = endptr; *matrixstr != '\0'; matrixstr++) {
    if (!isblank(*matrixstr)) break;
  }
  if (*matrixstr != '\0') {
    return false;
  }

  return true;
}

/** @brief Convert input string to a vector with flags for periodic directions.
  *
  * @return true if the analysis was successful, false otherwise
  */
bool String_makePeriodicFlag(const char* periodicvalue, bool periodicflag[3]) {
  long pos = 0;

  while (*periodicvalue) {
    if (pos == 3) return false;
    pos++;
    switch (*periodicvalue) {
      case 'x':
      case 'X':
        if (periodicflag[X]) return false;
        periodicflag[X] = true;
        break;
      case 'y':
      case 'Y':
        if (periodicflag[Y]) return false;
        periodicflag[Y] = true;
        break;
      case 'z':
      case 'Z':
        if (periodicflag[Z]) return false;
        periodicflag[Z] = true;
        break;
      default:
        return false;
    }
    periodicvalue++;
  }
  return true;
}

/** @brief Create a formated string buffer from a va_list.
  *
  * @code
  * myfunc(const char* format, ...) {
  *   va_list ap;                            // get argument list
  *   char   *infotext = NULL;
  *
  *   va_start(ap, format);
  *   String_createVAString(&infotext, format, &ap);
  *   va_end(ap);
  *   printf("%s", infotext);
  *   Free(infotext);
  * }
  * @endcode
  */
void String_createVAString(char** buffer, const char* format, va_list* ap) {
  va_list  apcopy;
  char    *str = *buffer;

  size_t   size;
  size_t   n = MAX_LINE_LENGTH;                      // default size for output

  do {
    size = n;                                        // if the output has been truncated then
    str  = (char*)Realloc(str, size*sizeof(char));   // get more memory and try again
    va_copy(apcopy, *ap);                            // this is a stack, if loop is used the stack would be empty, work with a copy
    n = vsnprintf(str, size, format, apcopy);        // and convert it into the format string
    va_end(apcopy);
  } while (++n > size);                              // check if size of buffer was big enough incl. termination

  if (size != n) {
    size = n;
    str  = (char*)Realloc(str, size*sizeof(char));   // reduce allocation to needed size
  }

  *buffer = str;                                     // return buffer by reference
}

/** @brief Create a formated string buffer from a format string.
  *
  * @see String_createVAString
  */
void String_createFormatedString(char** buffer, const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  String_createVAString(buffer, format, &ap);
  va_end(ap);
}

void String_appendFormatedVAString(char** dest, const char* format, va_list* ap) {
  char *toappend = NULL;

  String_createVAString(&toappend, format, ap);
  String_append(dest, toappend);
  Free(toappend);
}

void String_appendFormatedString(char** dest, const char* format, ...) {
  va_list ap;

  va_start(ap, format);
  String_appendFormatedVAString(dest, format, &ap);
  va_end(ap);
}

/** @brief Appends a char to first string.
  *
  * Given string is manipulated. Give at least a '\0' terminated string.
  *
  * @param dest has to be a pointer to a \\0 terminated string or a pointer to a NULL-pointer.
  * @param c The character to append.
  */
size_t String_appendChar(char** dest, char c) {
#ifdef DEBUG
  if (dest == NULL) {
    myexit(ERROR_BUG, "dest is NULL but has to be at least a pointer to a \\0 terminated string.");
  }
#endif
  size_t destlen = (*dest == NULL) ? 0 : strlen(*dest);
  *dest = Realloc(*dest, destlen + 2);
  (*dest)[destlen] = c;
  (*dest)[destlen+1] = '\0';

  return destlen + 1;
}

/** @brief Appends second string to first string.
  *
  * Given string is manipulated. Give at least a '\0' terminated string.
  *
  * @param dest has to be a pointer to a \\0 terminated string or a pointer to a NULL-pointer.
  * @param toappend may be NULL, in this case dest is not modified
  */
size_t String_append(char** dest, const char* toappend) {
#ifdef DEBUG
  if (dest == NULL) {
    myexit(ERROR_BUG, "dest is NULL but has to be at least a pointer to a \\0 terminated string.");
  }
#endif
  if (toappend == NULL) {
    return strlen(*dest);
  }

  size_t destlen = (*dest == NULL) ? 0 : strlen(*dest);
  size_t toappendlen = strlen(toappend);

  *dest = Realloc(*dest, destlen + toappendlen + 1);
  strcpy(*dest + destlen, toappend);

  return destlen + toappendlen;
}

char* String_generateFilename(char* name, long number) {
  char *string = NULL;
  char *i = strrchr(name, '.');

  if (i != NULL) {
    *i = '\0';
    String_createFormatedString(&string, "%s_%ld.%s", name, number, i + 1);
  } else {
    String_createFormatedString(&string, "%s_%ld", name, number);
  }

  return string;
}

void String_createBlocktext(char** buffer, size_t blocklength, const char* text) {
  char *bufferstore = *buffer;

  *buffer = Strdup("");
  size_t  bufferlength = 1;

  // tokenize description or example text if it is not too large.
  size_t oldlinepos = 0;
  size_t newlinepos = 0;

  while (text[oldlinepos] != '\0') {
    while (text[newlinepos] != '\0' && text[newlinepos] != '\n') newlinepos++;
    if (oldlinepos+blocklength > newlinepos) {
      bufferlength += newlinepos - oldlinepos;
      *buffer = Realloc(*buffer, bufferlength);
      strncat(*buffer, &text[oldlinepos], newlinepos-oldlinepos);
    } else {
      // printing the text if token with newline is too large.
      size_t linelength = 0;
      size_t oldtokenpos = oldlinepos;
      size_t newtokenpos = oldlinepos;

      while (newtokenpos < newlinepos) {
        while (newtokenpos < newlinepos && text[newtokenpos] != ' ') newtokenpos++;
        linelength += newtokenpos - oldtokenpos;
        if (linelength < blocklength) {
          bufferlength += newtokenpos - oldtokenpos;
          *buffer = Realloc(*buffer, bufferlength);
        } else {
          if (oldtokenpos != oldlinepos) oldtokenpos++;
          bufferlength += newtokenpos - oldtokenpos+1;
          *buffer = Realloc(*buffer, bufferlength);
          strcat(*buffer, "\n");
          linelength = newtokenpos-oldtokenpos;
        }
        strncat(*buffer, &text[oldtokenpos], newtokenpos-oldtokenpos);
        oldtokenpos = newtokenpos++;
      }
    }
    oldlinepos = newlinepos++;
  }

  if (bufferstore) Free(bufferstore);
}

void String_writeBlocktext(FILE* fp, size_t blocklength, const char* text) {
  // tokenize description or example text if it is not too large.
  int oldlinepos = 0;
  int newlinepos = 0;

  while (text[oldlinepos] != '\0') {
    while (text[newlinepos] != '\0' && text[newlinepos] != '\n') newlinepos++;
    if ((size_t)newlinepos-oldlinepos < blocklength) {
      fprintf(fp, "%.*s", newlinepos-oldlinepos, &text[oldlinepos]);
    } else {
      // printing the text if token with newline is too large.
      size_t linelength = 0;
      int oldtokenpos = oldlinepos;
      int newtokenpos = oldlinepos;

      while (newtokenpos < newlinepos) {
        while (newtokenpos < newlinepos && text[newtokenpos] != ' ') newtokenpos++;

        linelength += newtokenpos - oldtokenpos;
        if (linelength >= blocklength) {
          if (oldtokenpos != oldlinepos) oldtokenpos++;
          fputc('\n', fp);
          linelength = newtokenpos-oldtokenpos;
        }
        fprintf(fp, "%.*s", newtokenpos-oldtokenpos, &text[oldtokenpos]);
        oldtokenpos = newtokenpos++;
      }
    }
    oldlinepos = newlinepos++;
  }
  fputc('\n', fp);
}

char* String_getKey(char* entry) {
  char *pos = strchr(entry, '=');
  if (pos == NULL) {
    mywarn("String_getKey(): There is no \"=\"-sign in this line! '%s'", entry);
    return NULL;
  }

  size_t length = pos - entry;
  if (length == 0) {
    mywarn("String_getKey(): There are no characters to the left of \"=\"-sign in this line! '%s'", entry);
    return NULL;
  }

  return strncpy(MallocS(length+1), entry, length);
}

char* String_getValue(char* entry) {
  char *pos = strchr(entry, '=');
  if (pos == NULL) {
    mywarn("String_getValue(): There is no \"=\"-sign in this line! '%s'", entry);
    return NULL;
  }

  if (*(++pos) == '\0') {
    mywarn("String_getValue(): There are no characters to the right of \"=\"-sign in this line! '%s'", entry);
    return NULL;
  }

  return Strdup(pos);
}

/** @brief Like strcmp but compare sequences of digits numerically.
  *
  * Here is a (tested) comparison function that does the job.
  * It understands only unsigned integers, not signed integers or floating point.
  *
  * http://stackoverflow.com/questions/1343840/natural-sort-in-c-array-of-strings-containing-numbers-and-letters
  */
int String_naturalCompare(const void* p1, const void* p2) {
  const char *s1 = p1;
  const char *s2 = p2;

  while (true) {
    int c1 = *s1;
    int c2 = *s2;
    if (c2 == '\0') {
      return (c1 != '\0');
    } else if (c1 == '\0') {
      return -1;
    } else if (isdigit(c1) && isdigit(c2)) {
      char *lim1, *lim2;
      unsigned long n1 = strtoul(s1, &lim1, 10);
      unsigned long n2 = strtoul(s2, &lim2, 10);
      if (n1 != n2) {
        return n1 - n2;
      }
      s1 = lim1;
      s2 = lim2;
    } else {
      if (c1 != c2) {
        return c1 - c2;
      }
      s1++;
      s2++;
    }
  }
}

/** @brief Copy a vector of strings.
  */
char** StringVec_copy(char** src, size_t count) {
  char **dst = (char**) Malloc(count*sizeof(char*));

  for (size_t n=0; n<count; n++) {
    dst[n] = Strdup(src[n]);
  }

  return dst;
}

/** @brief Compare two strings (e.g. for qsort and bsearch).
  */
int StringVec_compare(const void* stringptr1, const void* stringptr2) {
  char *a = *(char * const *)stringptr1;
  char *b = *(char * const *)stringptr2;

  return strcmp(a, b);
}

/** @brief Compare two strings (e.g. for qsort and bsearch) with String_naturalCompare
  */
int StringVec_naturalCompare(const void* stringptr1, const void* stringptr2) {
  char *a = *(char * const *)stringptr1;
  char *b = *(char * const *)stringptr2;

  return String_naturalCompare(a, b);
}

/** @brief Findet in der Zeile alle Woerter/Elemente und legt sie in einem Array ab.
 *
 * @param line            eine Zeile aus der Datei/Pipe; als Trennzeichen dient hier 'white space'
 * @param result          alle gefundenen Woerter/Elementen werden hier abgelegt
 * @param count           Anzahl der Elementen in der Zeile
 *
 */
void String_findAllWordsInLine(char* line, char*** result, long* count) {
  char wordlist[MAX_LINE_LENGTH] = "";

  // Woerter zaehlen
  long n = 0;
  for (long i = 0; line[i] != '\0'; i++) {
    if (!isblank(line[i])) {
      if ((line[i+1] == ' ' && line[i+2] == ' ') || line[i+1] == '\t' || line[i+1] == '\0') {
        n++;
      }
    }
  }

  *count = n;
  if (n == 0) {
    *result = NULL;
    return;
  }

  *result = (char**) Malloc(n * sizeof(char*));
  n = 0;
  size_t j = 0;
  for (size_t i = 0; line[i] != '\0'; i++) {
    if (!isblank(line[i])) {
      wordlist[j++] = line[i];
      if ((line[i+1] == ' ' && line[i+2] == ' ') || line[i+1] == '\t' || line[i+1] == '\0') {
        wordlist[j] = '\0';
        (*result)[n] = Strdup(wordlist);
        j = 0;
        n++;
      }
    } else if (i > 1 && i < strlen(line) && line[i] == ' ' && line[i+1] != ' ' && line[i-1] != ' ') {
      wordlist[j++] = line[i];
    }
  }
}
