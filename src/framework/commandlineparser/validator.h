/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2010 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       validator.h
 *  @ingroup    lib
 *  @brief      Validate the input value with Validator structure.
 *
 * Condition structure in Validator structure save the user defined validating
 * Information. To config the Validator with condition we can use:
 *     VALIDATOR_LONG(validator,rangeString,configFunction)
 * to config a Validator for long-ValueType.
 * We can use :
 *     VALIDATOR_REAL(validator,rangeString,configFunction)
 * to config a Validator for real-ValueType.
 *
 * @param rangeString     String to config range
 * @param configFunction  function pointer points user defined function
 *
 *
 * Format of the rangeString to config range: "[situation]:[range]"
 * - situation
 *   - INRANGE       value in interval
 *   - NOTINRANGE    value not in interval
 *   - INLIST        value in list
 *   - NOTINLIST     value not in list
 * - range
 *   - [x,y]         Interval [x,y]
 *   - [x,y)         Interval [x,y)
 *   - (x,y]         Interval (x,y]
 *   - (x,y)         Interval (x,y)
 *   - {x1,..., xn}  List of value x1, ..., xn
 *   - #             unlimited value
 *
 * The default Validators for long- and real-ValueType are also available.
 * The default Validator have the same action of Validator with range of (-#,#).
 * To get the default Validator for long- or real-datetype
 * @code
 *  Validator *validator = newLongValidator(void);
 *  Validator *validator = newRealValidator(void);
 * @endcode
 *
 * e.g.: example for Validator with a sample interval/list for Long and Real value
 * @code
 * Validator_validate(VALIDATOR_LONG(&validator, "INLIST:{1,2,3,4}", NULL), input, &bla_l, SHOW_ERROR_MSG)
 * @endcode
 *  table of results with different input:
 *                  input        bal_l        return value
 *                    2            2               1
 *                    5            5               0
 * @code
 * Validator_validate(VALIDATOR_REAL(&validator, "INRANGE:[3,10)", NULL), input, &bla_r, SHOW_ERROR_MSG)
 * @endcode
 *  table of results with different input:
 *                  input        bal_r        return value
 *                   2.0          2.0              0
 *                   5.0          5.0              1
 *
 *
 * e.g.: example for Validator with a user defined fuction: mod() to test if the
 *       input value is even
 * @code
 * long mod(void * input) {
 *   return !(*(long *)input % 2);
 * }
 * Validator_validate(VALIDATOR_LONG(&validator, NULL, mod), input ,&bla_l, SHOW_ERROR_MSG);
 * @endcode
 *  table of results with different input:
 *                  input        bal_l        return value
 *                    4            4               1
 *                    5            5               0
 *
 * e.g.: example for Validator with a user defined fuction, that also uses the range
 *       structure in Validator: foo() to test if the result of input value mod 17
 *       is in list equal to 2,5,8 or 10
 * @code
 * long foo(void *input) {
 *   long test = (*(long *)input % 17);
 *   return Validator_validateWithRange(&validator, &test);
 * }
 * Validator_validate(VALIDATOR_LONG(&validator, "INLIST:{2,5,8,10}", foo), input, &bla_l, SHOW_ERROR_MSG)
 * @endcode
 *  table of results with different input:
 *                  input        bal_l        return value
 *                   19           19              1
 *                   20           20              0
 *
 *
 * e.g.: example for Validator that have more than one condition with all posibility
 *       usage above. We use foo() and mod() here to test if the input value satisties
 *       the following conditions:
 *       (1) input value should be bigger than 100
 *       (2) it is a even number
 *       (3) the result of it mod 17 is equal to 2,5,8 or 10
 *       To make it, first config the validator with the first condition, and than add
 *       other conditions in it
 * @code
 * VALIDATOR_LONG(&validator, "INRANGE:(100,#)", NULL);
 * Validator_addCondition(&validator, NULL, mod));
 * Validator_addCondition(&validator, "INLIST:{2,5,8,10}", foo);
 * Validator_validate(&validator, input, &bla_l, SHOW_ERROR_MSG);
 * @endcode
 *  table of results with different input:
 *                  input        bal_l        return value
 *                   22           22              0
 *                   175          175             0
 *                   172          172             1
 *
 * For safly release the memory after using Validator:
 * @code
 * Validator_deinit(&validator);
 * @endcode
 *  all malloced memories of the elements in Validator are released.
 *
 * @todo
 * long Validator_validate(Validator* validator, void* input, void* value); // validates typeless
 *
 * @todo Validator with any longer Parameters for "conditionfunction"
 * long Validator_validate(Validator* validator, char* input, void* value, long parameterNumber, ...);
 *
 ********************************************************************
 *
 *  @lastmodified 02.07.10    Weiye Sun
 *  @lastmodified 15.04.11    Amit Purkait
 *  @lastmodified 08.05.14    Thomas Welte
 *  @lastmodified 31.10.14    Markus Maier
 *
 ********************************************************************/

#ifndef SRC_FRAMEWORK_COMMANDLINEPARSER_VALIDATOR_H
#define SRC_FRAMEWORK_COMMANDLINEPARSER_VALIDATOR_H

#include "qsort.h"

#define VALIDATOR_LONG(validator, conditions)               Validator_init(validator, (stringParseFunc)parseInt, conditions)
#define VALIDATOR_REAL(validator, conditions)               Validator_init(validator, (stringParseFunc)parseREAL, conditions)

#define INRANGE      1
#define NOTINRANGE  -1
#define INLIST       2
#define NOTINLIST   -2

#define INTERVALCASE(x) ( abs(x) == 1 )
#define LISTCASE(x)     ( abs(x) == 2 )

#ifndef HIDE_ERROR_MSG
#  define HIDE_ERROR_MSG false
#  define SHOW_ERROR_MSG true
#endif

typedef bool (*stringParseFunc)(const char* line, long* pos, void* value, bool show_errors);
typedef bool (*stringParseWithPayloadFunc)(const char* line, long* pos, void* value, void* payload, bool show_errors);

typedef struct RangeType_s {
  void  *left;
  void  *right;
  void **values;
  int    rangetype;
  int    intervaltype;
  long   elementcount;
} RangeType;

typedef struct Condition_s {
  RangeType           *range;
  bool               (*conditionfunction)(struct Condition_s*, void*);
  int                (*comparefunction)  (const void*, const void*);
  struct Condition_s  *next;
} Condition;

typedef struct Validator_s {
  stringParseFunc converter;
  void *payload;
  Condition  *conditions;
} Validator;


/** @brief Allocate mem for Validator.
  *
  */
static inline Validator* newValidator(void) {
  return (Validator*) Calloc(1, sizeof(Validator));
}

/** @brief Free mem for Validator.
  *
  */
static inline void freeValidator(Validator* validator) {
  Free(validator);
}

Validator* Validator_init(Validator* validator, stringParseFunc stringparsefunction, Condition* conditions);
void       Validator_deinit(Validator* validator);

Condition* Validator_addCondition(Validator* validator, Condition* addcondition);

bool       Validator_validate(Validator* validator, const char* input, void* value, bool show_errors);
bool       Validator_validateSubstring(Validator* validator, const char* line, long* pos, void* value, bool show_errors);
bool       Validator_validateVector(Validator* validator, const char* line, long* pos, void* vector, size_t n, size_t struct_size, bool show_errors);
bool       Validator_validateMatrix(Validator* validator, const char* line, long* pos, void** matrix, size_t n, size_t m, size_t struct_size, bool show_errors);

#define CONDITION_LONG(rangestring)         Condition_init(newCondition(), rangestring, (stringParseFunc)parseInt, compare_long_ascending, NULL)
#define CONDITION_REAL(rangestring)         Condition_init(newCondition(), rangestring, (stringParseFunc)parseREAL, compare_REAL_ascending, NULL)
#define CONDITION_FUNCTION(conditionfunc)   Condition_init(newCondition(), NULL, NULL, NULL, conditionfunc)

static inline Condition* newCondition(void) {
  return (Condition*) Calloc(1, sizeof(Condition));
}

static inline void freeCondition(Condition* condition) {
  Free(condition);
}

Condition* Condition_init(Condition* condition, const char* rangestring, stringParseFunc stringparsefunction, int (*comparefunction)(const void*, const void*), bool (*conditionfunction)(Condition* condition, void* value));
void       Condition_deinit(Condition* condition);

/** @brief Add a new condition function to the list of conditions to be matched.
  *
  * @param conditions           the conditions where to add a new Conditionfunction
  * @param newcondition         the condition to be added
  *
  * @return pointer the Condition object that has been added
  */
Condition* Condition_add(Condition* conditions, Condition* newcondition);

bool       Condition_validate(Condition* conditions, void* value);
bool       Condition_validateVector(Condition* conditions, void* vector, size_t n, size_t elementsize);
bool       Condition_validateMatrix(Condition* conditions, void** matrix, size_t n, size_t m, size_t elementsize);

#endif
