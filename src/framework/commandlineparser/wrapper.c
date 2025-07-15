/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2001 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       wrapper.c
 *  @ingroup    lib
 *  @ingroup    sim
 *  @brief      Helper functions which can not be inlined.
 *
 * Some functions i.e. with a variable number of parameters
 * are not inlined by the compiler.
 * This functions are compiled in this c-file.
 *
 ********************************************************************
 *
 *  @lastmodified  2013 Doxygen Header
 *
 ********************************************************************/

#include <time.h>
#include <sys/time.h>
#if !defined(_WIN64) && !defined(_WIN32)
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#else
#include <stdint.h>
#include <processthreadsapi.h>
#endif
#include <dirent.h>
#include <locale.h>

// #include <exception.h>
#include "wrapper.h"

#ifdef INSIDESOLVER
#include "../sim/pinfo.h"
#endif


/** array of strings identifing a side of the cube
  *
  * [0] = left   [1] = right
  * [2] = bottom [3] = top
  * [4] = back   [5] = front
  */
const char *side_str[6] = {
  "left",   "right",
  "bottom", "top",
  "back",   "front"
};

/** for use of printfm/printim
  */
char *direction_str[3] = {
  "x", "y", "z"
};

char  *progname;

/** memory size num_symbols
  * Should be save for the next years ;-)
  */
char   *memorysymbols[] = {"bytes","KB","MB","GB","TB","PB","EB","ZB","YB","XB","WB","VB","UB","TDB"};
size_t  num_symbols = (sizeof(memorysymbols)/sizeof(char*));


/** @brief Gets the biggest index possible for the memory symbol list.
  *
  */
size_t getBiggestMemorySymbolIndex(size_t bytes) {
  size_t i = 0;
  while (bytes / (1<<(i*10)) > 1 && i < num_symbols) {
    i++;
  }

  if (i == 0) return 0;

  // else show it a little more precise than calculated
  return i-1;
}

/** @brief Print out an line.
  *
  * e.g.:
  * Output format is " (II) <<date and time>> : <<string>>"
  *
  * If compiled with MPI support the format is
  * " (II) (Node <<rank>>): <<date and time>> : <<string>>"
  *
  * @param header      used for log files
  * @param colorheader used for console
  * @param str         string to be printed.
  *
  */
static inline void print_internal(const char* header, const char* colorheader, const char* str) {
#ifdef INSIDESOLVER  // only used inside the solver

  const char *strtime = sprinttime();
#ifdef MPI
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // maybe mpiinfo.world.rank is not available in early usage

  if (pinfo.logfp && pinfo.logfp != stderr) {
    fprintf(pinfo.logfp, " %s (Node %i) : %s : %s\n", header, rank, strtime, str);
    fflush(pinfo.logfp);
  } else {
    fprintf(stderr, " %s \033[01m(Node %i)\033[22m : %s : %s\033[m\n", colorheader,
                    rank, strtime, str);
  }

#else

  if (pinfo.logfp && pinfo.logfp != stderr) {
    fprintf(pinfo.logfp, " %s %s : %s\n", header, strtime, str);
    fflush(pinfo.logfp);
  } else {
    fprintf(stderr, " %s %s : %s\033[m\n", colorheader, strtime, str);
  }

#endif

#else  // outside of the solver
  (void)header;
  fprintf(stderr, " %s %s\033[m\n", colorheader, str);
#endif

}

/** @brief Print out an line.
  *
  * e.g.:
  * Output format is " (II) <<date and time>> : <<string>>"
  *
  * If compiled with MPI support the format is
  * " (II) (Node <<rank>>): <<date and time>> : <<string>>"
  *
  * @param header      used for log files
  * @param colorheader used for console
  * @param str         string to be printed.
  *
  */
static inline void print_both_internal(const char* header, const char* colorheader, const char* str) {
#ifdef INSIDESOLVER  // only used inside the solver

  const char *strtime = sprinttime();
#ifdef MPI
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // maybe mpiinfo.world.rank is not available in early usage

  if (pinfo.logfp && pinfo.logfp != stderr) {
    fprintf(pinfo.logfp, " %s (Node %i) : %s : %s\n", header, rank, strtime, str);
    fflush(pinfo.logfp);
  }
  fprintf(stderr, " %s \033[01m(Node %i)\033[22m : %s : %s\033[m\n", colorheader,
                  rank, strtime, str);

#else

  if (pinfo.logfp && pinfo.logfp != stderr) {
    fprintf(pinfo.logfp, " %s %s : %s\n", header, strtime, str);
    fflush(pinfo.logfp);
  }
  fprintf(stderr, " %s %s : %s\033[m\n", colorheader, strtime, str);

#endif

#else  // outside of the solver
  (void)header;
  fprintf(stderr, " %s %s\033[m\n", colorheader, str);
#endif

}

/** @brief Function to generate a string by using a format like with printf
  */
__attribute__((__format__ (__printf__, 1, 0)))
static char* stringfmt(const char* const fmt, va_list* ap) {

  int   size = MAX_LINE_LENGTH;             /* default size for output                  */
  char *str  = (char*)malloc(size);         /* get all memory needed                    */

  while (str != NULL) {
    va_list apcopy;
    va_copy(apcopy, *ap);                   /* get a copy of argument list              */
    int n = vsnprintf(str, size, fmt, apcopy); /* and convert it into the format string*/
    va_end(apcopy);
#ifdef DEBUG
    if (n < 0) {
      myexit(ERROR_BUG, "error on vsnprintf, fmt string=%s", fmt);
    }
#endif
    if (n < size) {                         /* if the output hasn't been truncated then */
      return str;
    }
    size = n + 1;
    str  = (char*)realloc(str, size);       /* or get more memory and try again         */
  }

  /* if we reach this point we got not enough memory */
  print_both_internal("(EE)", "\033[1;31m(EE)", "internal error: not enough memory");
  return NULL;
}

/** @brief Print a formated string like with printf but using terminal colors
  */
void fprintc(FILE* fp, long color1, long color2, const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  char *str = stringfmt(fmt, &ap);
  va_end(ap);

  if (str) {
    fprintf(fp, "\033[%ld;%ldm%s\033[0m\n", color1, color2, str);
    free(str); /* use free not Free as its our own pointer */
  }
}


/** @brief Print out an error message by a format string.
  *
  * @note Only use this function in a path which leads to a myexit
  *
  * @sa myexit
  */
void myerror(const char* const fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  char *str = stringfmt(fmt, &ap);
  va_end(ap);

  if (str) {
    print_both_internal("(EE)", "\033[1;31m(EE)", str);
    free(str); /* use free not Free as its our own pointer */
  }
}

/** @brief Print out an error message and exit with exit code and take a format string.
  */
void myexit(int exitcode, const char* const fmt, ...) {
  fflush(stdout);
  fputc('\n', stderr);
#ifndef DEBUG
  if (exitcode == ERROR_BUG) {
    myerror("*** A bug was triggered (exitcode %d) by PID %d ***", exitcode, getpid());
#else
  if (exitcode != ERROR_SIGNAL) {
    myerror("*** Received error with exitcode %d by PID %d ***", exitcode, getpid());
#endif
#if !defined(_WIN32) && !defined(_WIN64)
    // ucontext_t ucontext;
    // getcontext(&ucontext);
    // printBacktrace(&ucontext, 2);
#endif
  }

  if (exitcode == ERROR_NOT_IMPLEMENTED) {
    myerror("A functionality is used, which is not (yet) implemented!");
  } else if (exitcode == ERROR_INCONSISTENCY) {
    myerror("An inconsistent setup is used (e.g. two features can not be combined).");
  } else if (exitcode == ERROR_NOT_VALIDATED) {
    myerror("A functionality is used, which is not validated so far!");
  }

  va_list ap;

  va_start(ap, fmt);
  char *str = stringfmt(fmt, &ap);
  va_end(ap);

  if (str) {
    print_both_internal("(EE)", "\033[1;31m(EE)", str);
    fputc('\n', stderr);
#ifdef INSIDESOLVER  // only used inside the solver
    mymessage(MESSAGE_FEW, "SOLVER ABORT (exit code %d): %s", exitcode, str); // send a message to the user
#endif
    free(str); /* use free not Free as its our own pointer */
  }

#ifdef INSIDESOLVER  // only used inside the solver
  printProcessUsageTime();               /* get system and process time parameters   */
  /* close logfile before exiting to ensure all messages will be written to file     */
  if ((pinfo.logfp != NULL) && (pinfo.logfp != stdout)) {
    if (fclose(pinfo.logfp) != 0) {      /* do not use FClose because it uses myexit */
      print_both_internal("(EE)", "\033[1;31m(EE)", "could not close log-file");
    }
  }
#endif
  fflush(stderr);
#ifdef MPI
  MPI_Abort(MPI_COMM_WORLD, exitcode);
#endif
  exit(exitcode);                        /* bye bye and fade away with exit code     */
}

/** @brief Print out a formated info message. (MPI: only server and/or the first worker)
  *
  * @sa print_internal
  */
void myinfo(const char* const fmt, ...) {
  va_list ap;

#if defined(INSIDESOLVER) && defined(MPI) && !defined(DEBUG)  // only used inside the solver
  if (!(isMasterRank(mpiinfo.world.rank) || (mpiinfo.workers.rank == 0))) return; // output only by the server and/or the first worker
#endif

  va_start(ap, fmt);
  char *str = stringfmt(fmt, &ap);
  va_end(ap);

  if (str) {
    print_internal("(II)", "\033[1;32m(II)\033[30;0m", str);
    free(str); /* use free not Free as its our own pointer */
  }
}

/** @brief Print out a formated warning.  (MPI: only server and/or the first worker)
  *
  * @sa print_both_internal
  */
void mywarn(const char* const fmt, ...) {
  va_list ap;

#if defined(INSIDESOLVER) && defined(MPI) && !defined(DEBUG)  // only used inside the solver
  if (!(isMasterRank(mpiinfo.world.rank) || (mpiinfo.workers.rank == 0))) return; // output only by the server and/or the first worker
#endif

  va_start(ap, fmt);
  char *str = stringfmt(fmt, &ap);
  va_end(ap);

  if (str) {
    print_both_internal("(WW)", "\033[1;35m(WW)", str);
    free(str); /* use free not Free as its our own pointer */
  }
}

/** @brief Print out a formated message for debugging.
  *
  */
void mydebug_internal(const char* file, int line, const char* func, const char* const fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  char *str = stringfmt(fmt, &ap);
  va_end(ap);

  if (str) {
#ifdef INSIDESOLVER  // only used inside the solver

#ifdef MPI

    if (pinfo.logfp && pinfo.logfp != stderr) {
      fprintf(pinfo.logfp, " (DD) (Node %i) : %s : (%20s:%-4d)%s() %s\n",
              mpiinfo.world.rank, sprinttime(),
              file, line, func, str);
      fflush(pinfo.logfp);
    }
    fprintf(stderr, "\033[44;1m (DD) (Node %i) : %s : (%20s:%-4d)%s() %s\033[0m\n",
                    mpiinfo.world.rank, sprinttime(),
                    file, line, func, str);

#else

    if (pinfo.logfp && pinfo.logfp != stderr) {
      fprintf(pinfo.logfp, " (DD) %s : (%20s:%-4d)%s() %s\n",
                           sprinttime(), file, line, func, str);
      fflush(pinfo.logfp);
    }
    fprintf(stderr, "\033[44;1m (DD) %s : (%20s:%-4d)%s() %s \033[30;0m\n",
                    sprinttime(), file, line, func, str);

#endif

#else  // outside of the solver
    fprintf(stderr, "\033[44;1m (DD) (%20s:%-4d)%s() %s\033[30;0m\n",
                    file, line, func, str);
#endif
    free(str); /* use free not Free as its our own pointer */
  }
}

long verbose = VERBOSE_NORMAL;

/** @brief Print out a formated string dependent on the current verbose level.
  *
  * @sa printverbose
  *
  * @param msglevel
  *     The minimum level when the current message is printed ( > 0 ).
  * @param verboselevel
  *     The verboselevel of the current program. level 0 means no output.
  * @param fmt
  *     Format string like in printf()
  *
  *  A message will be printed to stderr if msglevel <= verboselevel.
  *  Additional user levels can be defined in this manner.
  *
  *  level                 description @n
  *  - 0 (VERBOSE_QUIET)   no messages are printed
  *  - 1 (VERBOSE_NORMAL)  print informative messages
  *  - 2 (VERBOSE_CONTROL) print control messages of process flow
  *  - 3 (VERBOSE_DEBUG)   print additional debug messages
  */
void myverbose(ulong msglevel, ulong verboselevel, const char* const fmt, ...) {
  if (msglevel > verboselevel) return;

  va_list ap;

  va_start(ap, fmt);
  char *str = stringfmt(fmt, &ap);
  va_end(ap);

  if (str) {
    print_both_internal("(VV)", "\033[1;01m(VV)", str);
    free(str); /* use free not Free as its our own pointer */
  }
}

void FScanf(FILE* fp, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  if (vfscanf(fp, format, ap) < 1 && errno != 0) {
    myexit(ERROR_FILEIO, "Could not get excepted option (%s).", strerror(errno));
  }
  va_end(ap);
}

FILE* FOpenf(const char* mode, const char* filenameformat, ...) {
  // char *str  = MallocS(PATH_MAX);               /* get all memory needed                    */
  //
  // va_list  ap;
  // va_start(ap, filenameformat);
  // size_t n = vsnprintf(str, PATH_MAX, filenameformat, ap); // and convert it into the format string
  // va_end(ap);
  //
  // if (n >= PATH_MAX) {
  //   myexit(ERROR_FILEIO, "FOpenF(): filename is to long");
  // }
  //
  // FILE *fp = FOpen(str, mode);
  // Free(str);
  // return fp;
  return NULL;
}

/** @brief Convert the given path to normalized form and return it by reference.
  */
void RealPath(const char* path, char** resolved_path) {
  size_t size;
  char   resolvedpath[PATH_MAX];

  resolvedpath[0] = '\0';              /* mark string as empty */
  if (realpath(path,resolvedpath) == NULL) {
    myexit(ERROR_FILEIO, "error fetching path info for file %s (%s).", path, strerror(errno));
  }
  size = strlen(resolvedpath);
  // allocate memory big enough to add a tailing / and copy the received path
  (*resolved_path) = memcpy(MallocS(size+2), resolvedpath, size);
  (*resolved_path)[size  ] = '/';
  (*resolved_path)[size+1] = '\0';
}

/** @brief Create recursively a directory if it does not already exist.
 */
int Mkdir(const char* const pathname, mode_t mode) {
  // int         rc;
  // struct stat dirstat;
  //
  // if ((rc = mkdir(pathname, mode)) != 0) {
  //   if (errno == EEXIST) {
  //     memset(&dirstat, 0x00, sizeof(dirstat));
  //     Stat(pathname, &dirstat);
  //     if (S_ISDIR(dirstat.st_mode)) {
  //       errno = 0;
  //       return 0;
  //     }
  //   } else if (errno == ENOENT) {
  //     char *copypathname = Strdup(pathname);
  //     char *subpathname  = dirname(copypathname);
  //
  //     if (strcmp(subpathname, ".") == 0) {
  //       Free(copypathname);
  //       errno = 0;
  //       return 0;
  //     }
  //     rc = Mkdir(subpathname, mode);
  //     Free(copypathname);
  //     if (rc != 0) {
  //       return rc;
  //     }
  //     rc = Mkdir(pathname, mode);
  //   }
  //   return rc;
  // }

  return 0;
}

/** @brief Remove recursively a directory and its contents.
  */
int Remove(const char* const dirname) {
  // struct stat statbuf;
  // char filename[PATH_MAX];
  // DIR           *dir;
  // struct dirent *entry;
  //
  // if ((dir = opendir(dirname)) == NULL) {
  //   myexit(ERROR_FILEIO, "Could not delete directory '%s' (%s).", dirname, strerror(errno));
  // }
  // while ((entry = readdir(dir)) != NULL) {
  //   snprintf(filename, sizeof(filename), "%s/%s", dirname, entry->d_name);
  //   /* get file informations */
  //   Stat(filename, &statbuf);
  //   if (S_ISDIR(statbuf.st_mode)) {
  //     if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
  //       Remove(filename);
  //     }
  //   } else {
  //     Unlink(filename);
  //   }
  // }
  // closedir(dir);
  // int ret = rmdir(dirname);
  // if (ret) {
  //   // some file systems, like NFS, keep stupid immortal dotfiles in directories, which cannot be removed until the owning process dies
  //   // but the owning process is this very process and killing it isn't an option
  //   // hence we just tell the user nicely that there are dangling almost-empty directories and hope he cleans after effing NFS
  //   mywarn("%s. Failed to remove '%s'. Dear user, please remove it on your own.", strerror(errno), dirname);
  // }
  // return ret;
  return 0;
}

/** @brief Check if file is writable.
  *
  * @return true if file is writable or is createable, else false
  */
bool checkFileWritable(const char* const filename) {
  if ( (access(filename, F_OK )) == -1 ) {
    // File does not exist, so we will check if we
    // could create a new one in this directory

    char *path = Strdup(filename);

    long rc = checkFileWritable(dirname(path));

    Free(path);

    return rc;
  }

  // file does exist
  // is it writable
  return ( access(filename, W_OK) != -1 );
}

void AtExit(void (*function)(void)) {
  if (atexit(function) != 0) {
    myexit(EXIT_ERROR, "error calling atexit() function. (%s).", strerror(errno));
  }
}

/** @brief Get the number of CPUs avialable and online.
  */
void getNumberOfProcessors(long* cpucount, long* cpucountmax) {

#ifdef _SC_NPROCESSORS_ONLN
  *cpucount = sysconf(_SC_NPROCESSORS_ONLN);
  if (*cpucount < 1) {
    mywarn("Could not determine number of CPUs online (%s).", strerror (errno));
  }
  *cpucountmax = sysconf(_SC_NPROCESSORS_CONF);
  if (*cpucountmax < 1) {
    mywarn("Could not determine number of CPUs configured (%s).", strerror (errno));
  }
//   mydebug("%ld of %ld processors online", cpucount, cpucountmax);
#else
  *cpucount = LONG_MAX;
  *cpucountmax = LONG_MAX;
  mywarn("Could not determine number of CPUs");
#endif
}

/** @brief Get the number of instances of specified programs running.
  */
long getNumberOfInstances(char* prognames) {
  char execute[MAX_LINE_LENGTH];
  long count;

  snprintf(execute, sizeof(execute), "ps --no-headers -C %s | wc -l", prognames);
  FILE *fp = popen(execute, "r");
  if (fp == NULL || fscanf(fp, "%ld", &count) != 1) {
    mywarn("getNumberOfInstances(): Failed to get number of instances of '%s'.", prognames);
    if (fp) pclose(fp);
    return -1;
  }

  pclose(fp);

  return count;
}

/** @brief Calculate the time between two calls.
  */
void measureCPUTime(REAL* systemtime, REAL* usertime, REAL* totaltime) {
  REAL          user;
  REAL          sys;

#if !defined(_WIN64) && !defined(_WIN32)
  struct rusage selfusage;
  struct rusage childusage;

  /* what time did parent take */
  if (getrusage(RUSAGE_SELF,&selfusage) < 0) {
    print_both_internal("(EE)", "\033[1;31m(EE)\033[30;0m", "internal error: getrusage error.");
    return;
  }
  /* what time did children take */
  if (getrusage(RUSAGE_CHILDREN,&childusage) < 0) {
    print_both_internal("(EE)", "\033[1;31m(EE)\033[30;0m", "internal error: getrusage error.");
    return;
  }
  /* sum up my cpu time taken and the time the children took */
  user  = (double) selfusage.ru_utime.tv_sec +  selfusage.ru_utime.tv_usec/1000000.0;
  user += (double)childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec/1000000.0;
  /* sum up the time the system took handeling parent and his children */
  sys   = (double) selfusage.ru_stime.tv_sec +  selfusage.ru_stime.tv_usec/1000000.0;
  sys  += (double)childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec/1000000.0;
#else
  HANDLE hProcess = GetCurrentProcess();
  FILETIME ftCreation, ftExit, ftUser, ftKernel;
  GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser);

  int64_t itmp = *(int64_t *)(&ftUser);
  user = (REAL)itmp / 10000000U;

  itmp = *(int64_t *)(&ftKernel);
  sys  = (REAL)itmp / 10000000U;
#endif
  /* subtract old time from new time to get the time take since the last call */
  *systemtime = sys-(*systemtime);
  *usertime   = user-(*usertime);
  *totaltime  = (*systemtime)+(*usertime);
}

/** @brief Print information about cpu time taken from system and user space.
  */
void printProcessUsageTime(void) {
  REAL systemtime = 0.0;
  REAL usertime   = 0.0;
  REAL totaltime  = 0.0;

  measureCPUTime(&systemtime, &usertime, &totaltime);

  /* print out the time */
#ifdef MPI
#ifdef INSIDESOLVER
  if (mpiinfo.world.rank < 2 || verbose >= VERBOSE_CONTROL)
#else
  if (mpiinfo.world.rank < 1 || verbose >= VERBOSE_CONTROL)
#endif
#endif
  if (verbose >= VERBOSE_NORMAL) {
    myinfo("system time = %"REALLENGTH"gs, user time = %"REALLENGTH"gs, total time = %"REALLENGTH"gs", systemtime, usertime, totaltime);
  }
}

/** @brief Print resource usage informations.
  *
  */
void printResourceInformation(void) {
#if !defined(_WIN64) && !defined(_WIN32)
  /* get usage informations */
  struct rusage usage;
  if (getrusage(RUSAGE_SELF, &usage) < 0) {
    print_both_internal("(EE)", "\033[1;31m(EE)\033[30;0m", "internal error: getrusage error.");
    return;
  }

  /* print informations */
  myinfo("maximum resident set size    = %li", usage.ru_maxrss);
  myinfo("integral shared memory size  = %li", usage.ru_ixrss);
  myinfo("integral unshared data size  = %li", usage.ru_idrss);
  myinfo("integral unshared stack size = %li", usage.ru_isrss);
  myinfo("page reclaims                = %li", usage.ru_minflt);
  myinfo("page faults                  = %li", usage.ru_majflt);
  myinfo("swaps                        = %li", usage.ru_nswap);
  myinfo("block input operations       = %li", usage.ru_inblock);
  myinfo("block output operations      = %li", usage.ru_oublock);
  myinfo("messages sent                = %li", usage.ru_msgsnd);
  myinfo("messages received            = %li", usage.ru_msgrcv);
  myinfo("signals received             = %li", usage.ru_nsignals);
#else
  mywarn("printResourceInformation: Not supported on this platform.");
#endif
}

/** @brief Make human readable time from double in the format "DDd hh:mm:ss".
  */
const char* makehumanreadabletime(double eta) {
  static char rtime[20];
  long day;
  long hour;
  long min;
  long sec;

  if (eta < 0.0) {
    snprintf(rtime, sizeof(rtime), "   -d --:--:--");
  } else {

#if 0
    long _eta = (long)floor(eta);
    sec = _eta%60;
    _eta /= 60;
    min = _eta%60;
    _eta /= 60;
    hour = _eta%24;
    day = _eta/24;
#else
    ldiv_t dseconds = ldiv((long)floor(eta), 60);
    sec = dseconds.rem;
    ldiv_t dminutes = ldiv(dseconds.quot, 60);
    min = dminutes.rem;
    ldiv_t dhour = ldiv(dminutes.quot, 24);
    hour = dhour.rem;
    day = dhour.quot;
#endif
    snprintf(rtime, sizeof(rtime), "%4ldd %02ld:%02ld:%02ld"/*.%.3ld"*/, day, hour, min, sec); // , (long)((eta-floor(eta))*1000));
  }
  return rtime;
}

const char* makeHumanReadableTimell(char* buffer, size_t size, long long unsigned int time) {
#ifdef dhmsStyle
  unsigned long long nanoseconds  = time % 1000000000;
//  unsigned long long microseconds = (time  / 1000UL) % 1000;
//  unsigned long long milliseconds = (time  / (1000UL * 1000UL)) % 1000;
  unsigned long long seconds      = ( time / (1000UL * 1000UL * 1000UL)) % 60 ;
  unsigned long long minutes      = ((time / (1000UL * 1000UL * 1000UL * 60UL)) % 60);
  unsigned long long hours        = ((time / (1000UL * 1000UL * 1000UL * 60UL * 60UL)) % 24);
  unsigned long long days         = ( time / (1000UL * 1000UL * 1000UL * 60UL * 60UL * 24UL));

  if (days > 0) {
    snprintf(buffer, size, "%llu d %llu h %llu m %llu s %llu ns", days, hours, minutes, seconds, nanoseconds);
  } else if (hours > 0) {
    snprintf(buffer, size, "%llu h %llu m %llu s %llu ns", hours, minutes, seconds, nanoseconds);
  } else if (minutes > 0) {
    snprintf(buffer, size, "%llu m %llu s %llu ns", minutes, seconds, nanoseconds);
  } else if (seconds > 0) {
    snprintf(buffer, size, "%llu s %llu ns", seconds, nanoseconds);
  } else {
    snprintf(buffer, size, "%llu ns", nanoseconds);
  }
  return buffer;
#else
#if 0
  /* Get the name of the current locale.  */
  char *old_locale = setlocale(LC_ALL, NULL);
  /* Copy the name so it won't be clobbered by setlocale. */
  char *saved_locale = Strdup(old_locale);

  setlocale(LC_ALL, "de_DE");
  snprintf(buffer, size, "%'llu", time);

  /* Restore the original locale. */
  setlocale(LC_ALL, saved_locale);
  Free(saved_locale);
  return buffer;
#else
#ifdef DEBUG
  if (size < 28) myexit(ERROR_BUG, "Internal error: buffersize must be at least 27 bytes.");
#else
  (void)size;
#endif
  long i = 27;
  buffer[i+1] = '\0';
  do {
    buffer[i--] = '0' + time%10;
    if (!(i%4) && time>9) buffer[i--]='.';
  } while (time /= 10);
  return &buffer[++i];
#endif
#endif
}

/** @brief Return a formated time string with the local time in a human readable manner.
  */
const char* sprinttime(void) {
  time_t       t;
  static char  datestr[27];

  time(&t);
  strftime(datestr, sizeof(datestr), "%a, %d. %b %Y %H:%M:%S", localtime(&t));
  return datestr;
}
