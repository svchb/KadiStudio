/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2001 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       parse.h
 *  @ingroup    datafiles
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 26.10.04    Michael Selzer
 *  @lastmodified 15.04.05    Daniel Schabunow
 *  @lastmodified 08.05.14    Thomas Welte
 *
 ********************************************************************/

#ifndef SRC_FRAMEWORK_COMMANDLINEPARSER_PARSE_H
#define SRC_FRAMEWORK_COMMANDLINEPARSER_PARSE_H

#ifndef HIDE_ERROR_MSG
#  define HIDE_ERROR_MSG false
#  define SHOW_ERROR_MSG true
#endif

typedef bool (*parseFunc)(const char* line, long *pos, void* cell, bool show_errors);

bool compareChar(const char* line, long* pos, char ch, bool show_errors);
bool getEndOfLine(const char* line, long* pos, bool show_errors);
bool parseBoolean(const char* line, long* pos, bool* value, bool show_errors);
// bool parseScalarInt(const char* line, long* pos, long* value, bool show_errors);
bool parseInt(const char* line, long* pos, long* value, bool show_errors);
// bool parseScalarFloat(const char* line, long* pos, REAL* value, bool show_errors);
bool parseREAL(const char* line, long* pos, REAL* value, bool show_errors);
bool parseString(const char* line, long* pos, char** value, bool show_errors);

#endif
