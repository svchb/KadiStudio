/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2013 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       keytype.h
 *  @ingroup    datafiles
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 10.06.13    Maik Sigwarth
 *
 ********************************************************************/

#ifndef LIB_DATAFILES_KEYTYPE_H
#define LIB_DATAFILES_KEYTYPE_H

// escape sequences for the infile parameters (e.g. used by getValue())
#define KEYTYPESTRING_UNKNOWN           "%u"
#define KEYTYPESTRING_INT               "%i"
#define KEYTYPESTRING_FLOAT             "%f"
#define KEYTYPESTRING_CHAR              "%c"
#define KEYTYPESTRING_STRING            "%s"
#define KEYTYPESTRING_BOOL              "%b"
#define KEYTYPESTRING_VALIDATOR         "%V"
#define KEYTYPESTRING_OPTION            "%r"
#define KEYTYPESTRING_OR                "%o"
#define KEYTYPESTRING_NAMESPACEMODULE   "%n"

#define KEYTYPESTRING_VECTOR            "%v"
#define KEYTYPESTRING_MATRIX            "%m"
#define KEYTYPESTRING_MATRIX3D          "%d"

#define KEYTYPESTRING_FILE              "%F"

// key types used by Keytype struct and access methods
// may be combined bitwise
#define KEYTYPE_UNKNOWN           0
#define KEYTYPE_INT               (1<< 1)
#define KEYTYPE_FLOAT             (1<< 2)
#define KEYTYPE_CHAR              (1<< 3)
#define KEYTYPE_STRING            (1<< 4)
#define KEYTYPE_BOOL              (1<< 5)
#define KEYTYPE_VALIDATOR         (1<< 6)
#define KEYTYPE_OPTION            (1<< 7)
#define KEYTYPE_OR                (1<< 8)
#define KEYTYPE_NAMESPACEMODULE   (1<< 9)

#define KEYTYPE_VECTOR            (1<<10)
#define KEYTYPE_MATRIX            (1<<11)
#define KEYTYPE_MATRIX3D          (1<<12)

// static Keytype
#define KEYTYPE_STATIC            (1<<13)

#define KEYTYPE_FILE              (1<<14)

// composite types
#define KEYTYPE_COMPOSITEMASK     (KEYTYPE_VECTOR|KEYTYPE_MATRIX|KEYTYPE_MATRIX3D)

// maximal amount of dimensions currently supported by Keytype (Scalar(0), Vector(1), Matrix(2), Matrix3D(3))
#define KEYTYPEMAXDIMENSION  3

// styles for the pretty print of types
#define TEXTSTYLE_ASCII      0
#define TEXTSTYLE_LATEX      1
#define TEXTSTYLE_INFILE     2

typedef struct Keytype_s {
  long        type;                                  ///< type of the value, int, float bool, string...
  long        dimension;                             ///< 0: single value, 1 vector, 2 matrix, 3 3d-matrix
  bool        isoptional;                            ///< %r
  const char *keyend;                                ///< pointer to end of Type
  const char *startofdimension[KEYTYPEMAXDIMENSION]; ///< if type is static first element holds pointer to start position else pointing to first char inside '[]'
} Keytype;

typedef struct KeytypeIter_s {
  Keytype keytype;
} KeytypeIter;

/** @brief reads the type string and fills information in a given Keytype struct.
  *
  * @param keytype    struct to change
  * @param typestring string containing type
  */
Keytype* Keytype_readType(Keytype* keytype, const char* typestring);

char*    Keytype_getDimensionFromVariable(Keytype* keytype, long dimension);             ///< returns content of @param dimension
long     Keytype_getDimension(Keytype* keytype, long dimension);                         ///< Returns the dimension of the Keytype


/** @brief Get first element in the type string.
  *
  * @param iter       Iterator to be used in subsequent calls to @sa KeytypeIter_getNext and @sa KeytypeIter_hasNext
  * @param typestring string containing type
  *
  * @return First KeyType element of @sa typestring
  */
static inline Keytype* KeytypeIter_getFirst(KeytypeIter* iter, const char* typestring) {
  if (typestring == NULL) myexit(ERROR_BUG, "Given Keytypestring is NULL.");

  return Keytype_readType(&(iter->keytype), typestring);
}

/** @brief Return iterator to the to next type.
  *
  * @param iter iterator at the actual type
  *
  * @return position to the next type
  */
static inline Keytype* KeytypeIter_getNext(KeytypeIter* iter) {
  return Keytype_readType(&(iter->keytype), iter->keytype.keyend);
}

/** @brief Check if @param iter has a following type
  */
static inline bool KeytypeIter_hasNext(KeytypeIter* iter) {
  return (iter->keytype.keyend != NULL) && (*iter->keytype.keyend != '\0');
}

/** Check if @param iter has a following none static type.
  */
static inline bool KeytypeIter_hasNextNoneStaticType(KeytypeIter* iter) {
  return (strchr(iter->keytype.keyend+1, '%') == NULL);
}

/** @brief Returns whether type is a composite or not
 *
 * @param keytype  actual key type
 *
 * @returns true if it is composite, false else
 */
static inline bool Keytype_isComposite(Keytype* keytype) {
  return keytype->type & KEYTYPE_COMPOSITEMASK;
}

/** Check if @param keytype is a optional
  *
  * @param keytype  actual key type
  *
  * @returns true if it is optional, false else
  */
static inline bool Keytype_isVariableOptional(Keytype* keytype) {
  return keytype->isoptional;
}

// returns the pretty print type
char*    Keytype_prettyPrint(const char* typestring, long style);                        ///< returns whole type in user readable string
char*    Keytype_getPrettyPrint(Keytype* keytype);                                       ///< returns a user readable version of type
char*    Keytype_getPrettyPrintLatexMath(Keytype* keytype);                              ///< returns a user readable version of type in latex math style
char*    Keytype_getPrettyPrintInfile(Keytype* keytype);                                 ///< returns a user readable version of type in infile style
char     Keytype_getPrimitiveChar(Keytype* keytype);                                     ///< returns the character of the primitive type from keytype
char*    Keytype_getPrimitiveCharType(Keytype* keytype);                                 ///< returns the type of primitive in string form
char*    Keytype_getTypeName(Keytype* keytype);                                          ///< returns name of type


void**   Keytype_allocateMatrix(Keytype* keytype, int n, int m);                         ///< returns allocated void Matrix with size depending on type
char*    Keytype_getRegex(Keytype* keytype);                                             ///< returns regular expression to validate value
char*    Keytype_getDefault(Keytype* keytype);                                           ///< returns validate Value vor keytype
struct Validator_s* Keytype_getValidator(Keytype* keytype);                              ///< returns Validator
size_t   Keytype_getSizeOfValue(Keytype* keytype);                                       ///< returns Size of Value

size_t   KeytypeIter_getSizeOfDimensionFromVariable(KeytypeIter* iter, long dimension);  ///< returns value of dimension given in @param dimension of type given in @param iter

#endif
