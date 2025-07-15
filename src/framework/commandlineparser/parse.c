/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2005 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       parse.c
 *  @ingroup    datafiles
 *  @brief      Functions for parsing the parameters from the infile.
 *
 ********************************************************************
 *
 *  @lastmodified 28.11.07    Georg Eichhorn
 *  @lastmodified 27.12.07    Michael Selzer
 *  @lastmodified 08.05.14    Thomas Welte
 *
 ********************************************************************/

#include <search.h>
#include <dirent.h>

#include "wrapper.h"
// #include <wrapper/regex.h>
#include "stringconv.h"
// #include <evaluate/evaluate.h>
#include "validator.h"

// #include <datafiles/infile.h>
#include "parse.h"

/** @brief Compare the apperance a character at the actual infile line position.
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param ch                    character to be eaten
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if given char is found, else false
  */
bool compareChar(const char* line, long* pos, char ch, bool show_errors) {
  if (line[*pos] != ch) {
    if (show_errors) {
      if (line[*pos] == '\0') {
        myerror("end of line at position %ld where '%c' was expected.", *pos+1, ch);
      } else {
        myerror("found '%c' at position %ld where '%c' was expected.", line[*pos], *pos+1, ch);
      }
    }
    return false;
  }
  (*pos)++;
  return true;
}

/** @brief Compare the apperance of the end of line at actual infile line position.
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if end of line is found, else false
  */
bool getEndOfLine(const char* line, long* pos, bool show_errors) {
  if (line[*pos] != '\0') {
    if (show_errors) {
      myerror("Found '%c' at position %ld instead of end of line.", line[*pos], *pos+1);
    }
    return false;
  }

  return true;
}

/** @brief Read a bool from given line.
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param value                 reference to the bool which should be filled
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if parsed correctly, else false
  */
bool parseBoolean(const char* line, long* pos, bool* value, bool show_errors) {
  if (line[*pos] == '0') {
    *value = false;
    (*pos)++;
  } else if (line[*pos] == '1') {
    *value = true;
    (*pos)++;
  } else if (strcmp(&line[*pos], "on") == 0) {
    *value = true;
    (*pos) += 2;
  } else if (strcmp(&line[*pos], "off") == 0) {
    *value = false;
    (*pos) += 3;
  } else if (strcmp(&line[*pos], "true") == 0) {
    *value = true;
    (*pos) += 4;
  } else if (strcmp(&line[*pos], "false") == 0) {
    *value = false;
    (*pos) += 5;
  } else {
    if (show_errors) {
      myerror("Could not identify boolean value (must be 0 / 1 / off / on / false / true).");
    }
    return false;
  }
  return true;
}

/** @brief Get the rangeString out of the formatString
  *
  * @param format pointer to the actual position of the format string
  * @param rangestring  reference to the rangeString to fill
  *
  * @return pointer to the actual position of the format string
  *
  */
static const char* getRangeString(const char* format, char** rangestring) {
  if (*(format+1) != '{') {
    *rangestring = NULL;
    return format;
  }
  const char *formatstart = format+2;
  long bracketcount = 0;
  do {
    switch (*(++format)) {
      case '{':  bracketcount++; break;
      case '}':  bracketcount--; break;
      case '\0': myexit(ERROR_BUG, "Getting rangestring failed (Unexpected end of format string).");
    }
  } while (bracketcount > 0);
  size_t size = format - formatstart;
  if (size == 0) {
    myexit(ERROR_BUG, "Getting rangestring failed (Empty rangestring is not allowed).");
  }
  *rangestring = Strndup(formatstart, size);
  return format;
}

/** @brief Read a long int from given line. Long may start with '0x' to indicate a hexadecimal value.
  *
  * An integer term does consist of integer operands, the special operands Nx, Ny, Nz and
  * the following operants:
  *
  * +, -, *, / and ^
  *
  * e.g.  (Nz/2)-(Nz/4)-4
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param value                 reference to the integer which should be filled
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if parsed correctly, else false
  */
bool parseInt(const char* line, long* pos, long* value, bool show_errors) {
  const char *token = &(line[*pos]);
  char       *endpt;

  errno = 0;  // clear error state

  *value = strtol(token, &endpt, 0);

  if (errno) {
    if (show_errors) {
      if (errno == ERANGE) {
        if (*value == HUGE_VAL) {
          myerror("Reading a REAL value failed (value is too large - positive)");
        } else if (*value == -HUGE_VAL) {
          myerror("Reading a REAL value failed (value is too large - negative)");
        } else {
          myerror("Reading a REAL value failed (value is too small - nearly zero)");
          *value = 0.0;
          *pos += (endpt - token);
          return true;
        }
      } else {
        myerror("Reading an interger value failed (%s)", strerror(errno));
      }
    }
    return false;
  }
  if (token == endpt) {    // no conversion was done
    if (show_errors) {
      myerror("Reading an integer value failed (%s)", ((*token)=='\0')?"end of line":"not a valid integer value");
    }
    return false;
  }

  *pos += (endpt - token);

  return true;
}

/** @brief Parse a term with REAL values.
  *
  * A REAL term does consist of floating point operands, the special operands Nx, Ny, Nz and
  * the following operants:
  *
  * +, -, *, / and ^
  *
  * e.g.  (Nz/2)-(Nz/4)-4
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param value                 reference to the float which should be filled
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if parsed correctly, else false
  */
bool parseREAL(const char* line, long* pos, REAL* value, bool show_errors) {
  const char *token = &(line[*pos]);
  char       *endpt;

  errno = 0;  // clear error state

  *value = (REAL)strtod(token, &endpt);

  if (errno) {
    if (show_errors) {
      if (errno == ERANGE) {
        if (*value == HUGE_VAL) {
          myerror("Reading a REAL value failed (value is too large - positive)");
        } else if (*value == -HUGE_VAL) {
          myerror("Reading a REAL value failed (value is too large - negative)");
        } else {
          myerror("Reading a REAL value failed (value is too small - nearly zero)");
          *value = 0.0;
          *pos += (endpt - token);
          return true;
        }
      } else {
        myerror("Reading a REAL value failed (%s)", strerror(errno));
      }
    }
    return false;
  }
  if (token == endpt) {    // no conversion was done
    if (show_errors) {
      myerror("Reading a REAL value failed (%s)", ((*token)=='\0')?"end of line":"not a valid REAL value");
    }
    return false;
  }

  *pos += (endpt - token);

  return true;
}

/** @brief Parse a single character [a-zA-Z0-9_-/+*.].
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param ch                    reference to the char which should be filled
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if a character was found, else print a warning and return false
  */
static bool parseChar(const char* line, long* pos, char* ch, bool show_errors) {
  char c = line[*pos];

  // _not_ [a-zA-Z0-9_.+-*/]
  if ( ! (isalnum(c)
       || (c == '_') || (c == '.')
       || (c == '+') || (c == '-')
       || (c == '*') || (c == '/') ) ) {
    if (show_errors) {
      myerror("'%c' is not a valid character (only following are allowed [a-zA-Z0-9_-/+*.]).", c);
    }
    return false;
  }

  *ch = c;
  (*pos)++;
  return true;
}

/** @brief Create the charset for limiting the valid characters of the parseString.
  *
  * Allocate a set of signs, that are legal for an infile parameter. Delimiter must be {...}
  * The signs can either be given in enumerated form {abcdefg}; {123567}
  * or as an interval {a-f} => {abcdef}; {4-9} => {456789}.
  *
  * @note To use intervals, the ASCII value of the left char must be smaller then the right char.
  *
  * @note Escape sequence is '\' which has to be escaped because the format string is parsed by the c-compiler before => look for '\\'.
  *
  */
static const char* getCharSet(const char* format, char* charset) {
  unsigned char next;
  unsigned char last = '\0';

  if (*format != '{') {
    myexit(ERROR_BUG, "Wrong format string for charset after %%s expected '{'.");
  }
  format++;
  if (*format == '}') {
    myexit(ERROR_BUG, "Empty charset is not allowed.");
  }
  memset(charset, 0x00, 256);
  for (; *format != '\0'; format++) {
    if (*format == '{') {
      myexit(ERROR_BUG, "Wrong inner format string do not use cascaded '{'.");
    } else if (*format == '}') {
      return format;
    } else if (*format == '-') {
      if (last == '\0') myexit(ERROR_BUG, "Interval missing lower bound.");
      format++;
      if (*format == '\\') format++;
      next = *format;     // read the next format character

      if ( next < last ) myexit(ERROR_BUG, "ASCII value of the upper bound of the interval %c-%c must be larger than the lower bound.", last, next);
      while (last < next) {
        last++;
        charset[last] = last;
      }
      last = '\0';
    } else {
      if (*format == '\\') format++;
      // just another character
      last = *format;
      charset[last] = last;
    }
  }
  myexit(ERROR_BUG, "unexpected end of the format string");
}

/** @brief Parse a string consisting of a charset.
  *
  * @note the memory for the string is always allocated, so make sure the pointer
  *       does not reference any allocated memory and free the memory by your self.
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param value                 reference to an unallocated string pointer
  * @param charset               a charset generated with getCharSet
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if a string was found, else print a warning and return false
  */
static bool parseCharSet(const char* line, long* pos, char** value, const char* charset, bool show_errors) {
  char  c = line[*pos];
  ulong i, j = 0;

  if (c == '\0' || charset[(long)c] == '\0') {
    if (show_errors) {
      char charsetstring[257]; // charset + '\0' at most
      for (i=0; i<256; i++) {
        if (charset[i] != '\0') {
          charsetstring[j] = charset[i];
          j++;
        }
      }
      charsetstring[j] = '\0';
      myerror("got an empty expression (no valid character [%s] found).", charsetstring);
    }
    return false;
  }

  // check if the actual char is part of the charset
  for (i=0; c != '\0' && charset[(long)c] != '\0'; i++, (*pos)++, c = line[*pos]);

  (*value) = Strndup(&line[(*pos)-i], i);

  return true;
}

/** @brief Parse a string and replace "Tags" with a pattern, that is readable by parseRegEx
  *
  * @param format pointer to the actual position of the format string
  * @param regex  reference to the string containing the resulting regex
  *
  * @return pointer to the actual position of the format string
  *
  */
// static const char* getRegEx(const char* format, char** regex) {
//   const char *regexstart;
//   long  bracketcount = 1;
//
//   if (*format != '[') {
//     myexit(ERROR_BUG, "Getting regex failed (Wrong format string for regex after %%s expected '[').");
//   }
//   regexstart = format+1;
//   do {
//     format++;
//     switch (*format) {
//       case '[':  bracketcount++; break;
//       case ']':  bracketcount--; break;
//       case '\0': myexit(ERROR_BUG, "Getting regex failed (Unexpected end of format string).");
//     }
//   } while (bracketcount > 0);
//
//   size_t size = format - regexstart;
//   if (size == 0) {
//     myexit(ERROR_BUG, "Getting regex failed (Empty regex is not allowed).");
//   }
//   *regex = Strndup(regexstart, size);
//
//   // const char *RegEx_stringmask[][2] = {
//   //   {"[:term]",   "-a-zA-Z0-9\\+\\*/\\^,\\(\\)_\\."}, // - must be the first character
//   //   {"[:alnum]",  "a-zA-Z0-9"},
//   //   {"[:cmp]",    "\\(=\\|==\\|<\\|>\\|<=\\|>=\\|!=\\|<>\\)"},
//   //   {"",          ""}
//   // };
//
//   // String_mask(*regex, regex, 1, RegEx_stringmask);
//
//   return format;
// }

/** @brief Parse a string that matches a regular expression.
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param value                 reference to an unallocated string pointer
  * @param regex                 reference to a regular expression that is to be matched
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if a the string matches the given pattern, else return false and print a warning
  */
// static bool parseRegEx(const char* line, long* pos, char** value, const char* regex, bool show_errors) {
//   regex_t    re;
//   regmatch_t rm;
//   int        errorcode;
//   char       errorbuf[MAX_LINE_LENGTH];
//
//   errorcode = regcomp(&re, regex, REG_EXTENDED);
//   if (errorcode != 0) {
//     regerror(errorcode, &re, errorbuf, sizeof(errorbuf));
//
//     myexit(ERROR_BUG, "parseRegEx(): the given pattern '%s' could not be compiled (%s).", regex, errorbuf);
//   }
//
//   errorcode = regexec(&re, &(line[*pos]), (size_t) 1, &rm, 0);
//
//   if (errorcode != 0 || (size_t)rm.rm_so != 0) {
//     if (show_errors) {
//       regerror(errorcode, &re, errorbuf, sizeof(errorbuf));
//       myerror("Could not match the pattern '%s' (%s).", regex, errorbuf);
//     }
//     regfree(&re);
//     return false;
//   }
//
//   size_t size = (size_t)rm.rm_eo - (size_t)rm.rm_so;
//
//   *value = Strndup(&(line[*pos]), size);
//   *pos += size;
//
//   regfree(&re);
//
//   return true;
// }

/** @brief Parse a string consist of following characters [a-zA-Z_]{1}[a-zA-Z0-9_.+*\/-]*.
  *
  * @note the memory for the string is always allocated, so make sure the pointer
  *       does not reference any allocated memory and free the memory by your self.
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param value                 reference to an unallocated string pointer
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if a string was found, else print a warning and return false
  */
bool parseString(const char* line, long* pos, char** value, bool show_errors) {
  char  c = line[*pos];
  ulong i = 0;

  if ( !(isalpha(c) || (c == '_')) ) {  // string must start with a letter
    if (show_errors) {
      mywarn("error: converting '%c' to a string failed (string must start with a letter).", c);
    }
    return false;
  }
  do {
    (*pos)++;
    c = line[*pos];
    i++;
  } while (c != '\0' && (isalnum(c)    // is letter or number
          || (c == '_') || (c == '.') || (c == ':')
          || (c == '+') || (c == '-')
          || (c == '*') || (c == '/')));

  (*value) = Strndup(&line[(*pos)-i], i);

  return true;
}

typedef struct {
  const char **format;
  char       **result;
} stringwrapper;

/** @brief Parse a strings with expression, charset or normal.
  *
  * @param line                  pointer to the current data line
  * @param pos                   reference to the current position in the data line
  * @param sw                    reference to a struct containing format and result pointer
  * @param show_errors           flag if error-message will be displayed as a warning.
  *
  * @return true if a string was found, else print a warning and return false
  */
static bool parseStringWrapper(const char* line, long* pos, stringwrapper* sw, bool show_errors) {
  char  *regex = NULL;
  char   charset[256];         // maximum size => one for each possible char
  const char **format = sw->format;

  switch (*(*format + 1)) {
    case '{':
      (*format)++; // jump over s
      (*format) = getCharSet(*format, charset); // jump over the format String characters that belong to %s and fill the charset
      if (parseCharSet(line, pos, sw->result, charset, show_errors) == false) { // get a string, as long as the characters in infile are in charset
        return false;
      }
      break;
    // case '[':
    //   (*format)++; // jump over s
    //   (*format) = getRegEx(*format, &regex);
    //   if (parseRegEx(line, pos, sw->result, regex, show_errors) == false) {
    //     myerror("Value must match regular expression %s, too.", regex);
    //     Free(regex);
    //     return false;
    //   }
    //   Free(regex);
    //   break;
    default:
      if (parseString(line, pos, sw->result, show_errors) == false) {
        return false;
      }
  }

  return true;
}
