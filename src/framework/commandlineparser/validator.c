/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2010 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       validator.c
 *  @ingroup    lib
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 02.07.10    Weiye Sun
 *  @lastmodified 15.04.11    Amit Purkait
 *  @lastmodified 08.05.14    Thomas Welte
 *
 ********************************************************************/

#include "wrapper.h"
#include "stringconv.h"
#include "parse.h"

#include "validator.h"


/** @brief Get the range type from the string for configuration.
  *
  * @param str   the type string for recognization of the range type
  *
  * @return 0, if the configure string can not be identified; else return type number
  */
static int getType(char* str) {
  while (*str == ' ') str++;
  if (strcmp(str, "INRANGE")    == 0) return INRANGE;
  if (strcmp(str, "NOTINRANGE") == 0) return NOTINRANGE;
  if (strcmp(str, "INLIST")     == 0) return INLIST;
  if (strcmp(str, "NOTINLIST")  == 0) return NOTINLIST;
  return 0;
}

/** @brief Skips none, one or more ' '
 */
static void skipSpace(const char* line, long* pos) {
  while (line[*pos] == ' ') (*pos)++;
}

/** @brief Scan the configuration String and configure each element in RangeType with approached ValueType.
  *
  * @param scanedrange     rangetype, that should be configured
  * @param config          string for configuration
  * @param parsefunction   the function to parse the range from string
  *
  * @return true if the range is valid, otherweise false
  */
static bool scanRange(RangeType* scanedrange, const char* config, stringParseFunc parsefunction) {
  char   firstchar = config[0];
  if (firstchar == '{') {
    int elementcount       = 0;
    int i;
    long pos = 1; //after '{'

    //Count elements
    elementcount++;
    while (config[pos] != '\0') {
      if (config[pos] == ',') elementcount++;
      pos++;
    }

    scanedrange->values = (void **)Calloc(elementcount, sizeof(char*));
    pos = 1;
    skipSpace(config, &pos);

    //Parse elements
    i = 0;
    while (true) {
      scanedrange->values[i] = Malloc(sizeof(char*));
      if (!parsefunction(config, &pos, scanedrange->values[i], SHOW_ERROR_MSG)) {
        mywarn("Error during convertering the %d-th element in List!  Please check data type in the string for configuration.", i+1);
        FreeM(scanedrange->values, i + 1);
        return false;
      }

      if (i >= elementcount - 1) break;

      skipSpace(config, &pos);
      if (!compareChar(config, &pos, ',', SHOW_ERROR_MSG)) {
        mywarn("List needs ',' between elements");
        FreeM(scanedrange->values, i + 1);
        return false;
      }
      i++;
    }

    //check closing }
    skipSpace(config, &pos);
    if (!compareChar(config, &pos, '}', SHOW_ERROR_MSG)) {
      mywarn("List needs a closing '}'");
      FreeM(scanedrange->values, elementcount);
      return false;
    }
    scanedrange->intervaltype = 4;
    scanedrange->elementcount = elementcount;
  } else if (firstchar == '(' || firstchar == '[') {
    long pos = 1;
    skipSpace(config, &pos);

    //left side
    if (config[pos] == '#') {
      scanedrange->left = Strdup("#");
      pos++;
    } else {
      scanedrange->left  = MallocIV(1);
      if (!parsefunction(config, &pos, scanedrange->left, SHOW_ERROR_MSG)) {
        Free(scanedrange->left);
        mywarn("Error during convertering left side of interval!  Please check data type in the string for configuration.");
        return false;
      }
    }

    skipSpace(config, &pos);
    if (!compareChar(config, &pos, ',', SHOW_ERROR_MSG)) {
      Free(scanedrange->left);
      mywarn("Interval needs ',' between the sides of the interval");
      return false;
    }
    skipSpace(config, &pos);

    //right side
    if (config[pos] == '#') {
      scanedrange->right = Strdup("#");
      pos++;
    } else {
      scanedrange->right  = MallocIV(1);
      if (!parsefunction(config, &pos, scanedrange->right, SHOW_ERROR_MSG)) {
        Free(scanedrange->left);
        Free(scanedrange->right);
        mywarn("Error during convertering right side of interval!  Please check data type in the string for configuration.");
        return false;
      }
    }

    if (firstchar == '[') {
      if      (config[pos] == ']') scanedrange->intervaltype = 0; // [,] case
      else if (config[pos] == ')') scanedrange->intervaltype = 1; // [,) case
      else {
        Free(scanedrange->left);
        Free(scanedrange->right);
        mywarn("Interval needs a closing ] or ).");
        return false;
      }
    } else if (firstchar == '(') {
      if      (config[pos] == ']') scanedrange->intervaltype = 2; // (,] case
      else if (config[pos] == ')') scanedrange->intervaltype = 3; // (,) case
      else {
        Free(scanedrange->left);
        Free(scanedrange->right);
        mywarn("Interval needs a closing ] or ).");
        return false;
      }
    }
    scanedrange->elementcount = 0;
  } else {
    mywarn("Range needs a opening {,[ or (.");
    return false;
  }
  return true;
}

/** @brief examine if the configed Rangetype has error.
  *
  * @param range                 the range type to be examined
  * @param config                the configure string, used to output the information
  * @param comparefunction       the function to compare value with range
  *
  * @return true if the Rangetype is correct configured, otherweise false
  */
static bool examineRange(RangeType* range, const char* config, int (*comparefunction)(const void*, const void*)) {
  // the range is configured as interval
  if (INTERVALCASE(range->rangetype)) {
    // {,}
    if (range->intervaltype == 4) {
      mywarn("Interval condition can not process list of values '%s'", config);
      return false;
    }
    // other situations
    bool left_isinfinite  = (*(char *)range->left  == '#') ? true : false;
    bool right_isinfinite = (*(char *)range->right == '#') ? true : false;
    switch (range->intervaltype) {
      case 0: // [,]
        if (!(left_isinfinite || right_isinfinite) && comparefunction(range->left, range->right) >  0) {
          mywarn("Interval error with values '%s'.\n  For Interval case \"[,]\" must satisfy: left <= right", config);
          return false;
        }
        break;
      case 1: // [,)
        if (!(left_isinfinite || right_isinfinite) && comparefunction(range->left, range->right) >= 0) {
          mywarn("Interval error with values '%s'\n  For Interval case \"[,)\" must satisfy: left < right", config);
          return false;
        }
        break;
      case 2: // (,]
        if (!(left_isinfinite || right_isinfinite) && comparefunction(range->left, range->right) >= 0) {
          mywarn("Interval error with values '%s'\n  For Interval case \"(,]\" must satisfy: left < right", config);
          return false;
        }
        break;
      case 3: // (,)
        if (!(left_isinfinite || right_isinfinite) && comparefunction(range->left, range->right) >= 0) {
          mywarn("Interval error with values '%s'\n  For Interval case \"(,)\" must satisfy: left < right", config);
          return false;
        }
    }
  }
  // the range is configured as list
  if (LISTCASE(range->rangetype) && range->intervaltype != 4) {
    mywarn("List condition can not process interval '%s'", config);
    return false;
  }
  // examination passed, report no error
  return true;
}

/** @brief Configure the range type of a condition with configuration string and approached ValueType.
  *
  * @param range                 rangetype, that should be configured
  * @param config                string for configuration
  * @param comparefunction       the function to compare value with range
  * @param parsefunction         the function to parse the range from string
  *
  * @return the range type itself
  */
static RangeType* configRangeType(RangeType* range, const char* config, int (*comparefunction)(const void*, const void*), stringParseFunc parsefunction) {
  char  typestr[strlen(config)+1];
  char *vaargument;

  // get type string and separate it
  strcpy(typestr, config);
  vaargument = strchr(typestr, ':');
  if (!vaargument) {
    //myexit(EXIT_ERROR, "Error! Could not find separator ':' in Configuration string '%s'", config);
//     mywarn("Warning: Syntax is deprecated, please use INRANGE:[x,y] to define a range.");
    vaargument = typestr;
    range->rangetype = (config[0] == '{') ? getType("INLIST") : getType("INRANGE"); // default RANGE Type : INRANGE
  } else {
    *vaargument = '\0';
    char *checkSpace = vaargument-1;
    while (*checkSpace == ' ') {
      *checkSpace = '\0';
      checkSpace--;
    }

    // points to the first non space char after the separator
    do {
      vaargument++;
    } while (*vaargument == ' ');

    // get the type of the range
    range->rangetype = getType(typestr);
    if (!range->rangetype) {
      myexit(EXIT_ERROR, "Error! Could not get type of validation for '%s'", typestr);
    }
  }

  // configure each element in range
  if (!scanRange(range, vaargument, parsefunction)) {
    myexit(EXIT_ERROR, "Interval or List is invalid.");
  }

  // check the correctness of range
  if (!examineRange(range, vaargument, comparefunction)) {
    myexit(EXIT_ERROR, "Configuration of Interval or List is invalid. Please check configuration string.");
  }
  return range;
}

/** @brief Validate value with the range of a condition.
  *
  * @param validator  pointer to the Validator
  * @param value      destination to save the input value after validating
  *
  * @return true/false if the value is accepted/refused
  */
static bool Condition_validateWithRange(Condition* condition, void* value) {
  bool       isinrange = true;
  RangeType *range     = condition->range;

  if (range == NULL) {
    myexit(ERROR_BUG, "Range is not initialized, please configure it");
  }
  if (condition->comparefunction == NULL) {
    myexit(ERROR_BUG, "Comparefunction is not set, please set a functionpointer in condition to compare value with range.");
  }

  bool left_isinfinite  = false;
  bool right_isinfinite = false;
  if (range->intervaltype < 4) {
    left_isinfinite  = (*(char *)range->left  == '#') ? true : false;
    right_isinfinite = (*(char *)range->right == '#') ? true : false;
  }
  switch (range->intervaltype) {
    case 0: // [,]
      if ((!left_isinfinite && condition->comparefunction(value,range->left) < 0) || (!right_isinfinite && condition->comparefunction(value,range->right) > 0)) {
        isinrange = false;
      }
      break;
    case 1: // [,)
      if ((!left_isinfinite && condition->comparefunction(value,range->left) < 0) || (!right_isinfinite && condition->comparefunction(value,range->right) >= 0)) {
        isinrange = false;
      }
      break;
    case 2: // (,]
      if ((!left_isinfinite && condition->comparefunction(value,range->left) <= 0) || (!right_isinfinite && condition->comparefunction(value,range->right) > 0)) {
        isinrange = false;
      }
      break;
    case 3: // (,)
      if ((!left_isinfinite && condition->comparefunction(value,range->left) <= 0) || (!right_isinfinite && condition->comparefunction(value,range->right) >=0)) {
        isinrange = false;
      }
      break;
    case 4: // {.}
      isinrange = false;
      for (int i=0; i<range->elementcount; i++) {
        if (!condition->comparefunction(value,range->values[i])) {
          isinrange = true;
          break;
        }
      }
  }
  if (!isinrange) {
    mywarn("Value is out of range.");
  }
  return (range->rangetype>0) ? isinrange : !isinrange;
}

/** @brief Initialize validator for String use.
  *
  * @param validator            the Validator to be initialized
  * @param stringparsefunction  the function to parse the string values
  * @param conditions           the condition to compare the value with range
  *
  * @return pointer the Validator object that has been initialized
  */
Validator* Validator_init(Validator* validator, stringParseFunc stringparsefunction, Condition* conditions) {
  validator->converter        = stringparsefunction;
  validator->conditions       = conditions;

  return validator;
}

/** @brief Free the elements of Validator.
  *
  * @param validator       pointer to the Validator object that will be freed
  *
  */
void Validator_deinit(Validator* validator) {
  if (validator->conditions) {
    Condition_deinit(validator->conditions);
    freeCondition(validator->conditions);
  }
}

/** @brief Insert a condition for validator.
  *
  * @param validator            the Validator where the condition shoult be inserted
  * @param addcondition         user self defined function to operate validating process
  *
  * @return pointer the Condition object that has been added
  */
Condition* Validator_addCondition(Validator* validator, Condition* addcondition) {
  if (addcondition == NULL) {
    myexit(ERROR_BUG, "You must add a valid condition (NULL pointer is not allowed)!");
  }
  if (validator->conditions == NULL) {
    validator->conditions = addcondition;
  } else {
    Condition_add(validator->conditions, addcondition);
  }
  return addcondition;
}

/** @brief Validate input line with Validator.
  *
  * @param validator      pointer to the Validator
  * @param input          the value to be validated
  * @param value          destination to save the input value after validating
  * @param show_errors    If errors should be printed to console
  *
  * @return true/false if the value is accepted/refused
  */
bool Validator_validate(Validator* validator, const char* input, void* value, bool show_errors) {
  long pos = 0;

  if ((Validator_validateSubstring(validator, input, &pos, value, show_errors) == false) ||
      (getEndOfLine(input, &pos, show_errors) == false)) {
    if (show_errors) {
      myerror("Error was at '%s'\n      %*s^", input, (int)pos + 14, ""); //strlen("Error was at '") == 14
    }
    return false;
  }

  return true;
}

/** @brief Validate input value with Validator.
  *
  * @param validator             validator to use for validate
  * @param line                  pointer to line
  * @param pos                   start position for reading
  * @param value                 destination to save the input value after validating
  * @param show_errors           If errors should be printed to console
  *
  * @return true/false if the value is accepted/refused
  */
bool Validator_validateSubstring(Validator* validator, const char* line, long* pos, void* value, bool show_errors) {
  long startpos = *pos;

  if (validator->converter(line, pos, value, show_errors) == false) {
    if (show_errors) {
      myerror("Error during converting string to requested value type.");
    }
    return false;
  }

  if (!Condition_validate(validator->conditions, value)) {
    if (show_errors) {
      myerror("Value does not match the condition (Validation failed).");
    }
    *pos = startpos; // position of value for later error message
    return false;
  }

  return true;
}

/** @brief Validate a vector.
  *
  * e.g. a vector 2 looks like (value[0],value[1])
  *
  * @param validator             validator to use for validate
  * @param line                  pointer to line
  * @param pos                   start position for reading
  * @param vector                pointer to memory where the vector will be stored
  * @param n                     number of vector components
  * @param struct_size           size of the struct of components in the vector
  * @param show_errors           If errors should be printed to console
  */
bool Validator_validateVector(Validator* validator, const char* line, long* pos, void* vector, size_t n, size_t struct_size, bool show_errors) {
  size_t i;

  // a vector must always start with '('
  if (!compareChar(line, pos, '(', show_errors)) return false;

  if (n != 0) {
    i = 0;
    while (true) {
      if ( Validator_validateSubstring(validator, line, pos, (void*) ((char*) vector + i * struct_size), show_errors) == false ) {
        if (show_errors) {
          myerror("could not parse vector component %zd.", i);
        }
        return false;
      }
      i++;
      if ( i == n ) break;
      if (!compareChar(line, pos, ',', show_errors)) return false;
    }
  }
  // and end with ')'
  if (!compareChar(line, pos, ')', show_errors)) return false;

  return true;
}

/** @brief Parse a N x M matrix.
  *
  * e.g. a matrix 2x2 looks like [(value[0][0],value[0][1]),(value[1][0],value[1][1])]
  *
  * @param validator             validator to use for validate
  * @param line                  pointer to line
  * @param pos                   start position for reading
  * @param matrix                pointer to memory where the matrix will be stored
  * @param n                     number of matrix rows
  * @param m                     number of matrix cols
  * @param struct_size           size of the struct of components in the vector
  * @param show_errors           If errors should be printed to console
  */
bool Validator_validateMatrix(Validator* validator, const char* line, long* pos, void **matrix, size_t n, size_t m, size_t struct_size, bool show_errors) {
  size_t k;
  // a matrix must always start with '['
  if (!compareChar(line, pos, '[', show_errors)) return false;

  if (n != 0) {
    // go through the (n x m) matrixIndex and read n row vectors with m elements
    k = 0;
    while (true) {
      // read one vector with m elements
      if ( Validator_validateVector(validator, line, pos, matrix[k], m, struct_size, show_errors) == false ) {
        if (show_errors) {
          myerror("parsing vector component failed for matrix (row=%zd).", k);
        }
        return false;
      }
      k++;
      if ( k == n ) break;
      if (!compareChar(line, pos, ',', show_errors)) return false;
    }
  }
  // and end with ']'
  if (!compareChar(line, pos, ']', show_errors)) return false;

  return true;
}

/** @brief Initialize Condition.
  *
  * @param condition            the condition to be initialized
  * @param rangestring          the string with the range definition
  * @param stringparsefunction  the function to parse the string values
  * @param comparefunction      the function to compare the value with range
  * @param conditionfunction    the condition function which the Value must pass
  *
  * @return pointer the Condition object that has been initialized
  */
Condition* Condition_init(Condition* condition, const char* rangestring, stringParseFunc stringparsefunction, int (*comparefunction)(const void*, const void*), bool (*conditionfunction)(Condition* condition, void* value)) {
  // check parameters, if a condition could create with them
  if (conditionfunction != NULL) {
    // set conditionfunction
    condition->conditionfunction = conditionfunction;
    condition->comparefunction   = NULL;
    condition->range             = NULL;
  } else if (rangestring != NULL && stringparsefunction != NULL && comparefunction != NULL) {
    // set range
    condition->conditionfunction = Condition_validateWithRange;
    condition->comparefunction   = comparefunction;
    condition->range = configRangeType((RangeType*) Malloc(sizeof(RangeType)), rangestring, condition->comparefunction, stringparsefunction);
  } else {
    myexit(ERROR_BUG, "No parseString, stringparsefunction or comparefunction neither a conditionfunction found, condition couldn't be added!");
  }
  condition->next = NULL;
  return condition;
}

/** @brief Add a Conditionfunction to Conditions.
  *
  * @param condition            the condition to be cleaned
  */
void Condition_deinit(Condition* condition) {
  if (condition == NULL) return;
  if (condition->next != NULL) {
    Condition_deinit(condition->next);
    freeCondition(condition->next);
  }

  if (condition->range != NULL) {
    if (condition->range->intervaltype != 4) {
      Free(condition->range->left);
      Free(condition->range->right);
    } else {
      FreeM(condition->range->values, condition->range->elementcount);
    }
    Free(condition->range);
  }
}

/** @brief Add a new condition function to the list of conditions to match.
  *
  * @param conditions           the conditions where to add a new Conditionfunction
  * @param newcondition         the condition to be added
  *
  * @return pointer the Condition object that has been added
  */
Condition* Condition_add(Condition* conditions, Condition* newcondition) {
  // searching last condition in list
  Condition **current = &(conditions);
  while (*current != NULL) {
    // go to next condition
    current = &((*current)->next);
  }

  // appending new condition
  *current = newcondition;

  return *current;
}

/** @brief Validate input value with Validator.
  *
  * @param conditions           pointer to the condition object
  * @param value                destination to save the input value after validating
  *
  * @return true/false if the value is accepted/refused
  */
bool Condition_validate(Condition* conditions, void* value) {
  Condition *current = conditions;

  while (current != NULL) {
    // use the user self defined function to validate
    if (current->conditionfunction(current, value) == false) return false; // TODO neue conditionfunction mit payload?
    current = current->next;
  }

  return true;
}

/** @brief Validate a vector.
  *
  * e.g. a vector 2 looks like (value[0],value[1])
  *
  * @param conditions           pointer to the condition object
  * @param vector               pointer to memory where the vector will be stored
  * @param n                    number of vector components
  * @param struct_size          size of the struct of components in the vector
  */
bool Condition_validateVector(Condition* conditions, void* vector, size_t n, size_t struct_size) {
  for (size_t i = 0; i < n; i++) {
    if ( Condition_validate(conditions, (void*) ((char*) vector + i * struct_size)) == false ) {
      myerror("vector component %zd is not valid.", i);
      return false;
    }
  }

  return true;
}

/** @brief Check a N x M matrix.
  *
  * e.g. a matrix 2x2 looks like [(value[0][0],value[0][1]),(value[1][0],value[1][1])]
  *
  * @param conditions           pointer to the condition object
  * @param matrix               pointer to memory where the matrix will be stored
  * @param n                    number of matrix rows
  * @param m                    number of matrix cols
  * @param struct_size          size of the struct of components in the vector
  */
bool Condition_validateMatrix(Condition* conditions, void** matrix, size_t n, size_t m, size_t struct_size) {
  // go through the (n x m) matrixIndex and read n row vectors with m elements
  for (size_t k = 0; k < n; k++) {
    // check one vector with m elements
    if ( Condition_validateVector(conditions, matrix[k], m, struct_size) == false ) {
      myerror("validation of vector component failed for matrix (row=%zd).", k);
      return false;
    }
  }

  return true;
}
