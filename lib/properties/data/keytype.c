/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2013 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       keytype.c
 *  @ingroup    datafiles
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 10.06.13    Maik Sigwarth
 *
 ********************************************************************/

#include <wrapper.h>
#include <datafiles/parse.h>
#include <validator.h>
#include <evaluate/evaluate.h>
#include <container/stringconv.h>
#include "keytype.h"


typedef struct KeytypeLiteral_s {
  char  symbol;
  long  keytypenumber;
} KeytypeLiteral;

/// index of first composite type
#define KEYTYPE_MAX_PRIMITIVE_INDEX 9

KeytypeLiteral Keytype_typetable[] = {
  {'u', KEYTYPE_UNKNOWN,          },         // %u is not defined, just a dummy
  {'i', KEYTYPE_INT,              },
  {'f', KEYTYPE_FLOAT,            },
  {'c', KEYTYPE_CHAR,             },
  {'s', KEYTYPE_STRING,           },
  {'b', KEYTYPE_BOOL,             },
  {'V', KEYTYPE_VALIDATOR,        },
  {'r', KEYTYPE_OPTION,           },
  {'o', KEYTYPE_OR,               },
  {'n', KEYTYPE_NAMESPACEMODULE,  },
  {'v', KEYTYPE_VECTOR,           },
  {'m', KEYTYPE_MATRIX,           },
  {'d', KEYTYPE_MATRIX3D,         },
  {'y', KEYTYPE_STATIC            },
  {'F', KEYTYPE_FILE              },
  {'x', -1,                       }
};

char* Keytype_typestrings_ascii[] = {
  "unknown",
  "int",
  "float",
  "char",
  "string",
  "bool",
  "Validator",
  "option",
  "or",
  "bool", // namespace module
  "vector",
  "matrix",
  "3dMatrix",
  "",     // static
  "File"
};

char* Keytype_typestrings_latex[] = {
  "unknown",
  "\\mathds{N}",
  "\\mathds{R}",
  "char",
  "string",
  "bool",
  "Validator",
  "",
  "or",
  "bool", // namespace module
  "",
  "",
  "",
  ""
};

char* Keytype_typestrings_infile[] = {
  "unknown",
  "%i",
  "%f",
  "%c",
  "%s",
  "%b",
  "%V",
  "%r",
  "%o",
  "%n",
  "%v",
  "%m",
  "%d",
  "%F"
};


/** @brief Returns the codenumber for given char
 *
 * @param type char with type symbol
 *
 * @return number symbolic for given char
 */
static long getTypenumber(char type) {
  for (int i = 0; Keytype_typetable[i].symbol; i++) {
    if (type == Keytype_typetable[i].symbol) {
      return Keytype_typetable[i].keytypenumber;
    }
  }
  return KEYTYPE_UNKNOWN;
}

/** @brief Returns a copy of the '[' ']' enclosed string in a raw type string
 *
 *  Caller must free memory.
 *
 *  @param keytype keytype object
 *  @param dimension  number of which dimension requested
 *
 *  @return string of dimension
 */
char* Keytype_getDimensionFromVariable(Keytype* keytype, long dimension) {
  const char *dimensionstart = keytype->startofdimension[dimension];
  if (!dimensionstart) myexit(ERROR_PARAM, "Could not get %ld-dimension start from type: '%ld'", dimension, keytype->type);
  // get end
  char *dimensionend = strchr(dimensionstart, ']');
  if (!dimensionend) myexit(ERROR_PARAM, "Could not get %ld-dimension end from type: '%ld' (dimensionstart='%s')", dimension, keytype->type, dimensionstart);
  // copy dimension
  return Strndup(dimensionstart, dimensionend-dimensionstart);
}

/** @brief Returns the dimension of the Keytype
 *
 *  @param keytype keytype object
 *  @param dimension  number of which dimension requested
 *
 *  @return value of dimension
 */
long Keytype_getDimension(Keytype* keytype, long dimension) {
  char *dimensionstring = Keytype_getDimensionFromVariable(keytype, dimension);
  long  dimensionsize;
  String_makeLong(dimensionstring, &dimensionsize);
  Free(dimensionstring);
  return dimensionsize;
}

/** @brief Returns a copy of the '[' ']' enclosed string in a raw type string of current iterator position
 *
 *  Caller must free memory.
 *
 *  @param iter iterator with string containing requested dimension
 *  @param dimension  number of which dimension requested
 *
 *  @return string of dimension
 */
static char* getDimensionFromIter(KeytypeIter* iter, long dimension) {
  return Keytype_getDimensionFromVariable(&iter->keytype, dimension);
}

static char* getStaticString(Keytype* keytype) {
  const char *start = keytype->startofdimension[0];

  char *end = strchr(start, '%');
  if (end == NULL) {
    return Strdup(start);
  }

  size_t length = end-start;
  return Strndup(start, length);
}

/** @brief reads the type string and fills information in a given Keytype struct.
 *
 *  @param keytype    struct to change
 *  @param typestring string containing type
 *
 * @return the keytype object
 */
Keytype* Keytype_readType(Keytype* keytype, const char* typestring) {

  // empty keytype
  if (typestring == NULL || *typestring == '\0') {
    *keytype = (Keytype) {
      .type       = -1,
      .dimension  = 0,
      .isoptional = 0,
      .keyend     = NULL,
      .startofdimension = { NULL, NULL, NULL }
    };
    return keytype;
  }

  // static part
  if (*typestring != '%') {
    *keytype = (Keytype) {
      .type       = KEYTYPE_STATIC,
      .dimension  = 0,
      .isoptional = 0,
      .keyend     = strchr(typestring, '%'),
      .startofdimension = { typestring, NULL, NULL }
    };
    return keytype;
  }

  // proccess non static type
  typestring++;
  const char *variablestart = typestring;

  // isoptional (TODO) does not work!!
  keytype->isoptional = (strncmp("r{", variablestart, 2) == 0);
  if (keytype->isoptional) {
    mydebug("isoptional");
//     variablestart = variablestart + 2;
  }

  // type
  char type     = *variablestart;
  keytype->type = getTypenumber(type);

  int numberofdimensions = 0;
  if (keytype->type & KEYTYPE_VECTOR) {
    numberofdimensions = 1;
  } else if (keytype->type & KEYTYPE_MATRIX) {
    numberofdimensions = 2;
  } else if (keytype->type & KEYTYPE_MATRIX3D) {
    numberofdimensions = 3;
  }

  // dimension
  char *dimensionjumper = strchr(variablestart, '[');
  for (int dim=0; dim<numberofdimensions; dim++) {
    dimensionjumper = strchr(dimensionjumper, '[');
    if (!dimensionjumper) myexit(ERROR_PARAM, "Invalid type string. Could not find '[' for dimension %d", dim);
    keytype->startofdimension[dim] = dimensionjumper+1;
    dimensionjumper = strchr(dimensionjumper, ']');
    if (!dimensionjumper) myexit(ERROR_PARAM, "Invalid type string. Could not find ']' for dimension %d", dim);
  }
  for (long dim=numberofdimensions; dim<KEYTYPEMAXDIMENSION; dim++) {
    keytype->startofdimension[dim] = NULL;
  }
  keytype->dimension = numberofdimensions;

  // is this a composite type
  if (keytype->dimension > 0) {
    char *primitive = dimensionjumper+1;
    keytype->type = keytype->type | getTypenumber(*primitive);
    keytype->keyend = (*(primitive+1) == 'a') ? primitive+1 : primitive;
  } else {
    keytype->keyend = variablestart;
  }
  keytype->keyend++;

  return keytype;
}

/** returns a readable version of Keytype.
 */
char* Keytype_getPrettyPrint(Keytype* keytype) {
  char *pretty = NULL;

  if (keytype->type == KEYTYPE_UNKNOWN) {
    return Strdup("unknown");
  }

  if (keytype->type & KEYTYPE_STATIC) {
    return getStaticString(keytype);
  }

  // add type to pretty string
  for (long i=1; Keytype_typetable[i].keytypenumber != -1; i++) {
    if (Keytype_typetable[i].keytypenumber & (keytype->type)) {
      if (Keytype_typestrings_ascii[i]) {
        if (pretty == NULL) {
          String_append(&pretty, Keytype_typestrings_ascii[i]);
        } else {
          String_appendFormatedString(&pretty, " %s", Keytype_typestrings_ascii[i]);
        }
      }
    }
  }

  // if existing add dimensions to pretty string
  if (Keytype_isComposite(keytype)) {
    for (long i = 0; i < keytype->dimension; i++) {
      char *dimension = Keytype_getDimensionFromVariable(keytype, i);
      if (strlen(dimension) == 0) { // size is not specified
        String_appendFormatedString(&pretty, "[%c]", 'm' + (int)i);
      } else {
        String_appendFormatedString(&pretty, "[%s]", dimension);
      }
      Free(dimension);
    }
  }

  return pretty;
}

char* Keytype_getPrettyPrintLatexMath(Keytype* keytype) {
  char *pretty = NULL;

  if (keytype->type == KEYTYPE_UNKNOWN) {
    return Strdup("$ unknown type $");
  }

  if (keytype->type & KEYTYPE_STATIC) {
    char *staticstring = getStaticString(keytype);
    if (staticstring[0] == '{') { // only here because of missing range detection
      pretty = Strdup("");
    } else {
      String_createFormatedString(&pretty, "$ %s $", staticstring);
    }
    Free(staticstring);
    return pretty;
  }

  String_append(&pretty, "$");

  // add type to pretty string
  for (long i=1; i <= KEYTYPE_MAX_PRIMITIVE_INDEX; i++) {
    if (Keytype_typetable[i].keytypenumber & (keytype->type)) {
      String_appendFormatedString(&pretty, " %s", Keytype_typestrings_latex[i]);
    }
  }

  // if existing add dimensions to pretty string
  if (Keytype_isComposite(keytype)) {
    String_append(&pretty, "^{");
    for (long i=0; i < keytype->dimension; i++) {
      char *dimension = Keytype_getDimensionFromVariable(keytype, i);
      // size is not specified nothing or
      // dimension size contains % we have no dependency print m instead
      if (strlen(dimension) == 0 ||
          strchr(dimension, '%') != NULL) {
        Free(dimension);
        char dimensionname[2] = {'m' + i, '\0'};
        dimension = Strdup(dimensionname);
      }
      String_append(&pretty, dimension);
      Free(dimension);
      if (i+1 < keytype->dimension) {
        String_append(&pretty, " \\times ");
      }
    }
    String_append(&pretty, "}");
  }
  String_append(&pretty, " $");
  return pretty;
}

char* Keytype_getPrettyPrintInfile(Keytype* keytype) {
  char *pretty = NULL;

  if (keytype->type == KEYTYPE_UNKNOWN) {
    return Strdup("");
  }

  if (keytype->type & KEYTYPE_STATIC) {
    return getStaticString(keytype);
  }

  if (Keytype_isComposite(keytype)) {
    for (long i=KEYTYPE_MAX_PRIMITIVE_INDEX+1; Keytype_typetable[i].keytypenumber != -1; i++) {
      if (Keytype_typetable[i].keytypenumber & (keytype->type)) {
        String_append(&pretty, Keytype_typestrings_infile[i]);
      }
    }

    for (long i = 0; i < keytype->dimension; i++) {
      String_append(&pretty, "[");

      char *dimension = Keytype_getDimensionFromVariable(keytype, i);
      if (strlen(dimension) != 0 && strcmp(dimension, "%i") != 0) { // size is not specified nothing
        String_append(&pretty, dimension);
      }
      Free(dimension);

      String_append(&pretty, "]");
    }

    for (long i=1; i <= KEYTYPE_MAX_PRIMITIVE_INDEX; i++) {
      if (Keytype_typetable[i].keytypenumber & (keytype->type)) {
        String_appendFormatedString(&pretty, "%c", Keytype_typetable[i].symbol);
      }
    }
  } else {
    for (long i=1; Keytype_typetable[i].keytypenumber != -1 && i <= KEYTYPE_MAX_PRIMITIVE_INDEX; i++) {
      if (Keytype_typetable[i].keytypenumber & (keytype->type)) {
        String_append(&pretty, Keytype_typestrings_infile[i]);
      }
    }
  }
  return pretty;
}

/** returns a readable version of entired type.
 */
char* Keytype_prettyPrint(const char* typestring, long style) {
  if (typestring == NULL) myexit(ERROR_BUG, "Keytype_prettyPrint: called with NULL.");

  char *prettytype = NULL;
  KeytypeIter printiter;
  KeytypeIter_getFirst(&printiter, typestring);

  char *pretty;
  do {
    if (style == TEXTSTYLE_ASCII) {
      pretty = Keytype_getPrettyPrint(&(printiter.keytype));
    } else if (style == TEXTSTYLE_LATEX) {
      pretty = Keytype_getPrettyPrintLatexMath(&(printiter.keytype));
    } else if (style == TEXTSTYLE_INFILE) {
      pretty = Keytype_getPrettyPrintInfile(&(printiter.keytype));
    } else {
      myexit(ERROR_BUG, "Unknown style option given.");
    }

    String_append(&prettytype, pretty);
    Free(pretty);

    if (!KeytypeIter_hasNext(&printiter)) {
      break;
    }

    KeytypeIter_getNext(&printiter);
  } while (true);

  return prettytype;
}

size_t KeytypeIter_getSizeOfDimensionFromVariable(KeytypeIter* iter, long dimension) {
  char *size = getDimensionFromIter(iter, dimension);
  parseNode_t  termnode;
  termnode.type = -1; // The 'node->type' may not -2, otherwise do not care.
  long pos = 0;

  if (Term_toTree("", &pos, &termnode, Evaluate_getGlobal() != NULL ? &Evaluate_getGlobal()->globalsymbols : NULL, NULL, SHOW_ERROR_MSG) == false) {
    myexit(ERROR_BUG, "Error in determining dimension");
  }
  size_t evaluate = (size_t)Term_getTreeValue(&termnode);
  Free(size);
  return evaluate;
}

char Keytype_getPrimitiveChar(Keytype* keytype) {
  for (int i=0; i <= KEYTYPE_MAX_PRIMITIVE_INDEX; i++) {
    if (Keytype_typetable[i].keytypenumber & keytype->type) {
      return Keytype_typetable[i].symbol;
    }
  }

  // unknown
  myexit(ERROR_BUG, "Unknown primitive type.");
}

char* Keytype_getPrimitiveCharType(Keytype* keytype) {
  char typestring[3] = { '%', Keytype_getPrimitiveChar(keytype), '\0' };
  return Strdup(typestring);
}

void** Keytype_allocateMatrix(Keytype* keytype, int n, int m) {
  long type = keytype->type;
  if (!(type & KEYTYPE_MATRIX)) {
    mywarn("Type is not a Matrix ");
    return NULL;
  }

  void **matrix;
  if (type & KEYTYPE_INT) {
    matrix = (void**) MallocIM(n, m);
  } else if (type & KEYTYPE_BOOL) {
    matrix = (void**) MallocBM(n, m);
  } else if (type & KEYTYPE_FLOAT) {
    matrix = (void**) MallocFM(n, m);
  } else if (type & KEYTYPE_STRING || type & KEYTYPE_VALIDATOR) {
    matrix = (void**) MallocM(n, m, char*);
  } else if (type & KEYTYPE_CHAR) {
    matrix = (void**) MallocSM(n, m);
  } else {
    matrix = NULL;
    mywarn("Matrix has incompatible primitive ");
  }

  return matrix;
}

char* Keytype_getRegex(Keytype* keytype) {
  long rownumber;
  long columnnumber;
  long dimension = 0;

  char *regex = NULL;
  char *primitive = "";
  String_createValue(&regex, "^");

  if (keytype->type & KEYTYPE_INT) {
    primitive = "[+|-]?\\d+";

  } else if (keytype->type & KEYTYPE_FLOAT) {
    primitive = "[+|-]?\\d+(\\.\\d+)?(e[+|-]?((30[0-8]|[1-2]\\d\\d)|(\\d\\d?)))?";

  } else if (keytype->type & KEYTYPE_BOOL || keytype->type & KEYTYPE_NAMESPACEMODULE) {
    primitive = "[01]";

  } else if (keytype->type & KEYTYPE_STRING || keytype->type & KEYTYPE_STATIC || keytype->type & KEYTYPE_OPTION) {
    primitive = ".*";

  } else if (keytype->type & KEYTYPE_CHAR) {
    primitive = "\\c?";
  } else {
    mywarn("Could not create Regex for this Type :%ld", keytype->type);
  }

  if (keytype->type & KEYTYPE_MATRIX) {
    String_append(&regex, "\\[(");
    String_makeLong(Keytype_getDimensionFromVariable(keytype, 0), &rownumber);
    // if not found print one
    if (rownumber == 0) rownumber++;
    dimension++;
  }

  if (keytype->type & (KEYTYPE_COMPOSITEMASK)) {
    String_makeLong(Keytype_getDimensionFromVariable(keytype, dimension), &columnnumber);
    // if not found print one
    if (columnnumber == 0) columnnumber++;
    String_appendFormatedString(&regex, "\\((%s(,%s){%li})?\\)", primitive, primitive, columnnumber-1);
    if (keytype->type & KEYTYPE_MATRIX) String_appendFormatedString(&regex, "(,\\((%s(,%s){%li})?\\)){%li})?\\]", primitive, primitive, columnnumber-1, rownumber-1);
  } else {
    String_append(&regex, primitive);
  }

  String_append(&regex, "$");

  return regex;
}

static char* makeVectorString(Keytype* keytype, long size) {
  char *primitive;

  if (keytype->type & KEYTYPE_BOOL) {           // bool
    primitive = "0";
  } else if (keytype->type & KEYTYPE_INT) {     // int
    primitive = "0";
  } else if (keytype->type & KEYTYPE_FLOAT) {   // float
    primitive = "0.0";
  } else if (keytype->type & KEYTYPE_STRING) {  // string
    primitive = "none";
  } else {
    // type not identified
    mywarn("unknown primitive type %ld", keytype->type);
    primitive = " ";
  }
  char *vectorstring = Strdup("(");
  for (long i = 0; i < size - 1; i++) {
    String_appendFormatedString(&vectorstring, "%s,", primitive);
  }
  String_appendFormatedString(&vectorstring, "%s)", primitive);

  return vectorstring;
}

static char* makeMatrixString(Keytype* keytype, long m, long n) {
  char *matrixstring = Strdup("[");
  char *vectorstring = makeVectorString(keytype, n);
  for (long i = 0; i < m - 1; i++) {
    String_appendFormatedString(&matrixstring, "%s,", vectorstring);
  }
  String_appendFormatedString(&matrixstring, "%s]", vectorstring);
  Free(vectorstring);
  return matrixstring;
}

char* Keytype_getDefault(Keytype* keytype) {
  long defaultsize[KEYTYPEMAXDIMENSION];
  long size[KEYTYPEMAXDIMENSION];

  for (int i=0; i<keytype->dimension; i++) {
    defaultsize[i] = 1;
    if (String_makeLong(Keytype_getDimensionFromVariable(keytype, i), &size[i]) ) {       //TODO ersetzen durch KeytypeIter_getSizeOfDimensionFromVariable(&iter, 0);
      defaultsize[i] = size[i];                                                                   // reduzieren von Keytype_getDimensionFromVariable auf static
    }
  }

  if (keytype->type & KEYTYPE_VECTOR) {
    return makeVectorString(keytype, defaultsize[0]);

  } else if (keytype->type & KEYTYPE_MATRIX) {
    return makeMatrixString(keytype, defaultsize[0], defaultsize[1]);

  } else if (keytype->type & KEYTYPE_MATRIX3D) {
    mywarn("3D Matrix found default funct needed");
    return Strdup("no default supported yet");

  } else if (keytype->type & KEYTYPE_BOOL || keytype->type & KEYTYPE_NAMESPACEMODULE) {      // bool
    return Strdup("0");

  } else if (keytype->type & KEYTYPE_INT) {       // int
    return Strdup("0");

  } else if (keytype->type & KEYTYPE_OPTION) {    // option
    return Strdup("0");

  } else if (keytype->type & KEYTYPE_FLOAT) {     // float
    return Strdup("0.0");

  } else if (keytype->type & KEYTYPE_STRING) {    // string
    return Strdup("none");

  } else if (keytype->type & KEYTYPE_FILE) {      // file
    return Strdup("none");

  } else if (keytype->type & KEYTYPE_VALIDATOR) { // validator
    return Strdup("validator");

  } else if (keytype->type & KEYTYPE_STATIC) {
    return Keytype_getPrettyPrintInfile(keytype);
  }

  myexit(ERROR_BUG, "Could not identify type %ld.", keytype->type);
}

Validator *Keytype_getValidator(Keytype* keytype) {
  Validator *validator = NULL;

  if (keytype->type & KEYTYPE_COMPOSITEMASK) {
    return NULL;
  }

  if (keytype->type & KEYTYPE_INT) {
    validator = VALIDATOR_LONG(newValidator(), NULL);
  } else if (keytype->type & KEYTYPE_FLOAT) {
    validator = VALIDATOR_REAL(newValidator(), NULL);
  }

  return validator;
}

size_t   Keytype_getSizeOfValue(Keytype* keytype) {
  // in case of composite
  int multiplier = 1;

  if (keytype->type & KEYTYPE_COMPOSITEMASK) {
    if (keytype->type & KEYTYPE_VECTOR) {
      multiplier = Keytype_getDimension(keytype, 0);
    } else if (keytype->type & KEYTYPE_MATRIX) {
      multiplier = Keytype_getDimension(keytype, 0) * Keytype_getDimension(keytype, 1);
    } else if (keytype->type & KEYTYPE_MATRIX3D) {
      mywarn("3D Matrix found support needed");
    }
  }

  if (keytype->type & KEYTYPE_INT) {
    return (sizeof(long) * multiplier);
  } else if (keytype->type & KEYTYPE_FLOAT) {
    return (sizeof(REAL) * multiplier);
  } else {
    return (sizeof(long) * multiplier);// no other Types supported yet
  }

  return 0;
}

char* Keytype_getTypeName(Keytype* keytype) {
  long  type = keytype->type;
  char *name = NULL;

  if (type & (KEYTYPE_COMPOSITEMASK) ) {
    type = type & (KEYTYPE_COMPOSITEMASK);
  }

  for (int i=1; Keytype_typetable[i].keytypenumber != -1; i++) {
    if (Keytype_typetable[i].keytypenumber & (type)) {
      String_append(&name, Keytype_typestrings_ascii[i]);
    }
  }

  return name;
}
