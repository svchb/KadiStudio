/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2014 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       qsort.h
 *  @ingroup    wrapper
 *  @brief      Various definitions of comparison functions for the standard
 *              qsort(void* base, size_t num, size_t size, int (*compar)(const void*,const void*))
 *              function.
 *
 ********************************************************************
 *
 *  @lastmodified 31.01.2014 Constantin Heisler
 *
 ********************************************************************/

#ifndef LIB_WRAPPER_QSORT_H
#define LIB_WRAPPER_QSORT_H

/** @brief Declares a comparison function for
 *         qsort(void* base, size_t num, size_t size, int (*compar)(const void*,const void*))
 *         with a type suffix, which sorts the values in <b>ascending order</b>.
 *
 * @param type The type of the comparison function to generate.
 */
#define DECLARE_COMPARE_FUNCTION_ASCENDING(type) \
static inline int compare_##type##_ascending(const void* a, const void* b) { \
  if (*(type*)a < *(type*)b) return -1; \
  if (*(type*)a > *(type*)b) return  1; \
  return 0; \
}

/** @fn int compare_long_ascending(const void* a, const void* b)
 *
 * @brief Compares two long values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_ASCENDING(long)

/** @fn int compare_double_ascending(const void* a, const void* b)
 *
 * @brief Compares two double values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_ASCENDING(double)

/** @fn int compare_float_ascending(const void* a, const void* b)
 *
 * @brief Compares two float values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_ASCENDING(float)

/** @fn int compare_REAL_ascending(const void* a, const void* b)
 *
 * @brief Compares two REAL values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_ASCENDING(REAL)

/** @fn int compare_int_ascending(const void* a, const void* b)
 *
 * @brief Compares two int values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_ASCENDING(int)

/** @fn int compare_char_ascending(const void* a, const void* b)
 *
 * @brief Compares two char values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_ASCENDING(char)

/** @fn int compare_short_ascending(const void* a, const void* b)
 *
 * @brief Compares two short values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_ASCENDING(short)

/** @fn int compare_pointer_ascending(const void* a, const void* b)
 *
 * @brief Compares two short values in ascending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
static inline int compare_pointer_ascending(const void* a, const void* b) {
  if (a < b) return -1;
  if (a > b) return  1;
  return 0;
}

/** @brief Declares a comparison function for
 *         qsort(void* base, size_t num, size_t size, int (*compar)(const void*,const void*))
 *         with a type suffix, which sorts the values in <b>descending order</b>.
 *
 * @param type The type of the comparison function to generate.
 */
#define DECLARE_COMPARE_FUNCTION_DESCENDING(type) \
static inline int compare_##type##_descending(const void* a, const void* b) { \
  if (*(type*)a > *(type*)b) return -1; \
  if (*(type*)a < *(type*)b) return  1; \
  return 0; \
}

/** @fn int compare_long_descending(const void* a, const void* b)
 *
 * @brief Compares two long values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_DESCENDING(long)

/** @fn int compare_double_descending(const void* a, const void* b)
 *
 * @brief Compares two double values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_DESCENDING(double)

/** @fn int compare_float_descending(const void* a, const void* b)
 *
 * @brief Compares two float values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_DESCENDING(float)

/** @fn int compare_REAL_descending(const void* a, const void* b)
 *
 * @brief Compares two REAL values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_DESCENDING(REAL)

/** @fn int compare_int_descending(const void* a, const void* b)
 *
 * @brief Compares two int values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_DESCENDING(int)

/** @fn int compare_char_descending(const void* a, const void* b)
 *
 * @brief Compares two char values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_DESCENDING(char)

/** @fn int compare_short_descending(const void* a, const void* b)
 *
 * @brief Compares two short values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
DECLARE_COMPARE_FUNCTION_DESCENDING(short)

/** @fn int compare_pointer_descending(const void* a, const void* b)
 *
 * @brief Compares two pointer values in descending order.
 *
 * @param a The first value to compare
 * @param b The second value to compare
 *
 * @return -1 if the value a is less than value b, 0 if both are equal and 1 otherwise.
 */
static inline int compare_pointer_descending(const void* a, const void* b) {
  if (a > b) return -1;
  if (a < b) return  1;
  return 0;
}

#endif
