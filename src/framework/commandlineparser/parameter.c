/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2007 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       parameter.c
 *  @ingroup    lib
 *  @brief      Interface for easy access of commandline parameters.
 *
 ********************************************************************
 *
 *  @lastmodified 19.07.07    Borys Trakhter
 *  @lastmodified 27.01.09    Michael Selzer
 *  @lastmodified 15.05.14    Thomas Welte
 *
 ********************************************************************/

#include <getopt.h>

// #include "./version.h"
#include "wrapper.h"
#include "validator.h"
#include "structures.h"
#include "stringconv.h"
#include "parse.h"
#include "variant.h"

#include "parameter.h"


#define BLOCKLENGTH 90

/** @brief Tokenize description or example text if it is not too large.
  */
static void printHelpTextwithNewline(const char* text) {
  int oldlinepos = 0;
  int newlinepos = 0;

  while (text[oldlinepos] != '\0') {
    while (text[newlinepos] != '\0' && text[newlinepos] != '\n') newlinepos++;
    if (text[newlinepos] == '\n') newlinepos++;
    if (newlinepos-oldlinepos < BLOCKLENGTH) {
      fprintf(stdout, "                %.*s", newlinepos-oldlinepos, &text[oldlinepos]);
    } else {
      // printing the text if token with newline is too large.
      size_t linelength = 0;
      int oldtokenpos = oldlinepos;
      int newtokenpos = oldlinepos;

      fprintf(stdout, "                ");
      while (newtokenpos < newlinepos) {
        while (newtokenpos < newlinepos && text[newtokenpos] != ' ') newtokenpos++;

        linelength += newtokenpos - oldtokenpos;
        if (linelength >= BLOCKLENGTH) {
          if (oldtokenpos != oldlinepos) oldtokenpos++;
          fprintf(stdout, "\n                ");
          linelength = newtokenpos-oldtokenpos;
        }
        fprintf(stdout, "%.*s", newtokenpos-oldtokenpos, &text[oldtokenpos]);
        oldtokenpos = newtokenpos++;
      }
    }
    oldlinepos = newlinepos++;
  }
  fputc('\n', stdout);
  fputc('\n', stdout);
}

static const char verbosemsg[] = "enable the output (stderr) of some (helpful) log "
                                 "messages with log-level <i>, a higher level <i> will "
                                 "create more messages.";

static void printXMLUsage(const char* progname, const toolparam_t* tool, size_t count, bool* bitfield, char** defaultstrings) {
  char *escapedstr = NULL;

#define PACE3D_VERSION "0.1.0"
#define PACE3D_RELEASE_DATE "20250603"

  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  String_mask(tool->description, &escapedstr, STRING_MASK, XML_masktable);
  printf("<program name=\"%s\" version=\""PACE3D_VERSION" (Release date: "PACE3D_RELEASE_DATE")\" istested=\"%ld\" description=\"%s\" ", progname, tool->istested, escapedstr);
  Free(escapedstr);

  String_mask(tool->example, &escapedstr, STRING_MASK, XML_masktable);
  printf("example=\"%s\" >\n", escapedstr);
  Free(escapedstr);

  for (size_t i = 0, unnamed = 0; i < count; i++) {
    const argument_t *arg = &tool->arguments[i];

    const Variant *variant = getVariant(arg->flag);

    printf("\t<param ");

    if (arg->longname != NULL) {
      printf("name=\"%s\" ", arg->longname);
    } else {
      printf("name=\"arg%zu\" ", unnamed++);
    }

    if (arg->name != ' ') {
      printf("char=\"%c\" ", arg->name);
    }

    if (arg->description) {
      String_mask(arg->description, &escapedstr, STRING_MASK, XML_masktable);
      printf("description=\"%s\" ", escapedstr);
      Free(escapedstr);
    }

    if (arg->option == PARAM_REQUIRED) {
      printf("required=\"true\" ");
    }

    printf("type=\"%s\" ", variant->typestring);

    if (arg->andparams && arg->andparams[0] != '\0') {
      printf("relations=\"%s\" ", arg->andparams);
    }

    if (arg->interval && arg->interval[0] != '\0') {
      printf("interval=\"%s\" ", arg->interval);
    }

    if (arg->option == PARAM_OPTIONAL) {
      char *defaultstring = defaultstrings[i];
      if (defaultstring != NULL) {
        String_mask(defaultstring, &defaultstring, STRING_MASK, XML_masktable);
        printf("default=\"%s\" ", defaultstring);
        Free(defaultstring);
      }
    }

    if (bitfield[i]) {
      char *valuestring = variant->toString(arg->parameter);
      if (valuestring != NULL) {
        String_mask(valuestring, &valuestring, STRING_MASK, XML_masktable);
        printf("value=\"%s\" ", valuestring);
        Free(valuestring);
      }
      printf("enabled=\"true\" ");
    }

    printf("/>\n");
  }

  String_mask(verbosemsg, &escapedstr, STRING_MASK, XML_masktable);
  printf("\t<param name=\"verbose\" char=\"v\" description=\"%s\" type=\"%s\" default=\"%li\"",
          escapedstr,
          getVariant(PARAM_LONG)->typestring,
          1l);
  if (verbose != VERBOSE_NORMAL) {
    printf(" value=\"%ld\"", verbose);
  }
  printf("/>\n");
  Free(escapedstr);
  printf("\t<param name=\"help\" char=\"h\" description=\"print help\" type=\"%s\" />\n", getVariant(PARAM_FLAG)->typestring);

  printf("</program>\n");
}

static void printParameter(const char shortname, const char* longname, const char* hint) {
  if (isgraph(shortname)) {
    printf(" -%c", shortname);
    if (hint) fputs(hint, stdout);
  }
  if (longname != NULL) {
    printf(" --%s", longname);
    if (hint) {putchar('=');fputs(hint, stdout);}
  } else if (hint && !isgraph(shortname)) {
    printf(" %s", hint);
  }
}

static void printArgumentHelptext(const argument_t* argument, char* defaultstring) {
  const char  shortname = argument->name;
  const char *longname  = argument->longname;

  const Variant *variant = getVariant(argument->flag);
  printParameter(shortname, longname, variant->typeinfostring);

  if (argument->option == PARAM_OPTIONAL) {
    if (defaultstring != NULL) {
      printf(" (default=%s) ", defaultstring);
      Free(defaultstring);
    }
  } else if (argument->option == PARAM_REQUIRED) {
    printf(" (required)");
  } // else argument->flag == PARAM_FLAG
  printf("\n");

  if (argument->description == NULL) {
    mydebug("!!!Help text is not given!!!");
  } else {
    printHelpTextwithNewline(argument->description);
  }
}

static void printSectionText(const char* sectiontitle, const char* sectiontext) {
  printf("%s\n", sectiontitle);
  if (sectiontext == NULL) {
    mydebug("No text for section %s is not given.", sectiontitle);
  } else {
    String_writeBlocktext(stdout, 100, sectiontext);
  }
}

static void printVerboseHelptext(bool extendedhelp) {
  printf(" -v<i> --verbose=<i>\n");
  printHelpTextwithNewline(verbosemsg);
  printf(" -h --help\n                print this help (--helpall prints an extended help)\n\n");
  if (extendedhelp) {
    printf("    --xmlhelp\n                print this help as XML\n\n");
  }
}

static void printParametersUsage(const char* progname, toolparam_t tool, size_t count) {

  printf("\n\nUSAGE: %s ", progname);

#define SHORTUSAGE
#ifdef SHORTUSAGE
  bool first = false;
  for (size_t i = 0; i < count; i++) {
    const char  shortname = tool.arguments[i].name;
    const int   flag      = tool.arguments[i].flag;
    if (shortname != ' ' && flag == PARAM_FLAG) {
      if (first == false) {
        printf("[-");
        first = true;
      }
      printf("%c", shortname);
    }
  }
  if (first) {
    printf("]");
  }

  // USAGE: --longopt und type Optionen Ausgabe + optional OPT Ausgabe
  for (size_t i = 0; i < count; i++) {
    const argument_t *arg = &tool.arguments[i];
    const char  shortname = arg->name;
    const char *longname  = arg->longname;
    const int   flag      = arg->flag;
    if (flag != PARAM_FLAG && !((shortname == ' ' && (longname == NULL || strcmp(longname, "") == 0)))) {
      printf(" ");
      if (arg->option == PARAM_OPTIONAL) {
        printf("[");
      }
      if (shortname != '\0') {
        printf("-%c", shortname);
      }
      if (longname != NULL && (strcmp(longname, "") != 0)) {
        if (shortname != '\0') {
          printf("|");
        }
        printf("--%s", longname);
      }
      printf(" \e[4m%s\e[0m", getVariant(arg->flag)->typestring);
      if (arg->option == PARAM_OPTIONAL) {
        printf("]");
      }
    }
  }
  for (size_t i = 0; i < count; i++) {
    const argument_t *arg = &tool.arguments[i];
    const char  shortname = arg->name;
    const char *longname  = arg->longname;
    if (shortname == ' ' && (longname == NULL || strcmp(longname, "") == 0)) {
      if (arg->option == PARAM_REQUIRED) {
        printf(" %s", getVariant(arg->flag)->typestring);
      } else {
        printf(" [%s]", getVariant(arg->flag)->typestring);
      }
    }
  }
#else
  (void)tool;
  (void)count;
  printf("[OPTIONS]");
#endif

  printf("\n\n\n");
}

static void printUsageInternal(const char* progname, toolparam_t tool, char** defaultstrings, size_t count, bool extendedhelp) {
  printf("\n");
  printSectionText("DESCRIPTION:", tool.description);

  printf("Release Version "PACE3D_VERSION" ("PACE3D_RELEASE_DATE")\n");

  printParametersUsage(progname, tool, count);

  printf("OPTIONS:\n\n");
  for (size_t i = 0; i < count; i++) {
    printArgumentHelptext(&tool.arguments[i], defaultstrings ? defaultstrings[i] : NULL);
  }

  printVerboseHelptext(extendedhelp);

  printSectionText("EXAMPLE:", tool.example);
  printf("\n");
}

static char **defaultstrings;

void printUsage(const char* progname, toolparam_t tool, size_t count) {
  printUsageInternal(progname, tool, defaultstrings, count, false);
}

#ifdef DEBUG
static void printIntervalWarning(char name) {
  mydebug("Parameter '-%c' is not initialized by user.", name);
}
#else
static void printIntervalWarning(UNUSED(char name)) {
}
#endif

static bool validateParams(toolparam_t tool, size_t argumentcount, bool* givenargs) {
  ulong size;

  long  parameterlong;
  long *parameterlongvector;
  REAL  parameterreal;
  REAL *parameterrealvector;

  Condition condition;

  for (size_t i = 0; i < argumentcount; i++) {
    const argument_t *argument = &tool.arguments[i];

    if (argument->interval != NULL) {

      switch (argument->flag) {
        case PARAM_LONG:
          Condition_init(&condition, argument->interval, (stringParseFunc)parseInt, compare_long_ascending, NULL);
          parameterlong = *(long*) (argument->parameter);

          if (!Condition_validate(&condition, &parameterlong)) {
            if (givenargs[i]) {
              myexit(ERROR_PARAM, "Parameter '-%c' with '%ld' is out of range. Interval = %s", argument->name, parameterlong, argument->interval);
            } else {
              printIntervalWarning(argument->name);
            }
          }
          break;

        case PARAM_VECTOR_LONG:
        case PARAM_VECTOR_N_LONG:
          Condition_init(&condition, argument->interval, (stringParseFunc)parseInt, compare_long_ascending, NULL);
          if (argument->flag == PARAM_VECTOR_N_LONG) {
            size = ((vectorl_n_t*)argument->parameter)->size;
            parameterlongvector = ((vectorl_n_t*) argument->parameter)->vector;
          } else {
            size = 3;
            parameterlongvector = ((long*) argument->parameter);
          }

          for (ulong j = 0; j < size; j++) {
            if (!Condition_validate(&condition, &parameterlongvector[j])) {
              if (givenargs[i]) {
                myexit(ERROR_PARAM, "Parameter '-%c'[%ld] with '%ld' is out of range. Interval = %s", argument->name, j, parameterlongvector[j], argument->interval);
              } else {
                printIntervalWarning(argument->name);
              }
            }
          }
          break;

        case PARAM_REAL:
          Condition_init(&condition, argument->interval, (stringParseFunc)parseREAL, compare_REAL_ascending, NULL);
          parameterreal = *(REAL*) (argument->parameter);

          if (!Condition_validate(&condition, &parameterreal)) {
            if (givenargs[i]) {
              myexit(ERROR_PARAM, "Parameter '-%c' with '%"REALLENGTH"f' is out of range. Interval = %s", argument->name, parameterreal, argument->interval);
            } else {
              printIntervalWarning(argument->name);
            }
          }
          break;

        case PARAM_VECTOR_REAL:
        case PARAM_VECTOR_N_REAL:
          Condition_init(&condition, argument->interval, (stringParseFunc)parseREAL, compare_REAL_ascending, NULL);
          if (argument->flag == PARAM_VECTOR_N_REAL) {
            size = ((vectorr_n_t*)argument->parameter)->size;
            parameterrealvector = ((vectorr_n_t*) argument->parameter)->vector;
          } else {
            size = 3;
            parameterrealvector = ((REAL*) argument->parameter);
          }

          for (ulong j = 0; j < size; j++) {
            if (!Condition_validate(&condition, &parameterrealvector[j])) {
              if (givenargs[i]) {
                myexit(ERROR_PARAM, "Parameter '-%c'[%ld] with '%"REALLENGTH"f' is out of range. Interval = %s", argument->name, j, parameterrealvector[j], argument->interval);
              } else {
                printIntervalWarning(argument->name);
              }
            }
          }
          break;

        default:
          myexit(ERROR_BUG, "Unknown type %ld for parameter '-%c'/'--%s' to validate.", argument->flag, argument->name, argument->longname);
      }
      Condition_deinit(&condition);
    }
  }

  return true;
}

static void getParameterValue(const char* parameterprefix, const char* parametername, char* optarg, void* parametervalue, long parameterflag) {
  const Variant *variant = getVariant(parameterflag);
  if (variant->fromString(optarg, parametervalue) == false) {
    myexit(ERROR_PARAM, "Could not get %s from parameter '%s%s'.", variant->typestring, parameterprefix, parametername);
  }
}

bool getParams(int argc, char *argv[], toolparam_t tool, size_t count) {

  opterr                   = 0;  // disable getopt error output
  size_t i, j;
  long   requiredargs      = 0;
  char   args[count*2 + 4 + 1];
  char   requiredarglist[count+1];

#ifdef MPI
  int mpirank;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);

  if (mpirank == 0) {
#endif
    if (tool.istested == 0) {
      mywarn("PLEASE BE CAREFUL, THIS TOOL IS UNDER CONSTRUCTION AND NOT TESTED YET!");
    } else if (tool.istested < 0) {
      mywarn("PLEASE BE CAREFUL, THIS TOOL DOES NOT WORK CORRECTLY!!!");
    } else {
      if (tool.istested < 40) {
        fprintc(stderr, 1, 31, "The test process for this tool is completed by %ld%%", tool.istested);
      } else if (tool.istested < 90) {
        fprintc(stderr, 1, 33, "The test process for this tool is completed by %ld%%", tool.istested);
      } else if (tool.istested < 100) {
        fprintc(stderr, 1, 32, "The test process for this tool is completed by %ld%%", tool.istested);
      }
    }
#ifdef MPI
  }
#endif

#ifdef DEBUG
  bool wasrequiredpositional = true;

  // some validations
  for (long i = 0; i < count; i++) {
    const argument_t *argument = &tool.arguments[i];
    if (argument->name == ' ') {
      if (argument->option == PARAM_REQUIRED) {
        if (!wasrequiredpositional) {
          myexit(ERROR_BUG, "Required positional parameters follows optional one.");
        }
      } else {
        wasrequiredpositional = false;
      }
      if (argument->flag == PARAM_FLAG) {
        myexit(ERROR_BUG, "PARAM_FLAG must have an argument name.");
      }
    }
  }
#endif

  defaultstrings = Calloc(count, sizeof(char*));

  for (size_t i = 0; i < count; i++) {
    const argument_t *argument = &tool.arguments[i];

    const Variant *variant = getVariant(argument->flag);

    defaultstrings[i] = variant->toString(argument->parameter);
  }

  // First test for help message. If found do no further check of parameters.
  for (long n = 0; n < argc; n++) {
    if (strcmp(argv[n], "-h") == 0 || strcmp(argv[n], "--help") == 0) {
      printUsage(argv[0], tool, count);
#ifdef MPI
      MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS);
#endif
      exit(EXIT_OK);
    } else if (strcmp(argv[n], "--helpall") == 0) {
      // flag for extended help text
      printUsageInternal(argv[0], tool, defaultstrings, count, true);
#ifdef MPI
      MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS);
#endif
      exit(EXIT_OK);
    }
  }

  // used for checking required arguments and already given arguments
  bool bitfield[count];

  // assemble the opt string
  args[0] = ':';
  args[1] = 'v';
  args[2] = ':';
  args[3] = 'V';
  for (j = 4, i = 0; i < count; i++) {
    const argument_t *argument = &tool.arguments[i];
    if (argument->name != ' ') {
      args[j++] = argument->name;
      if (argument->flag != PARAM_FLAG) {
        args[j++] = ':';
      }
    }
    if (argument->option == PARAM_REQUIRED) {
      requiredarglist[requiredargs] = argument->name;
      requiredargs++;
    }
  }
  args[j]                       = '\0';
  requiredarglist[requiredargs] = '\0';
  myverbose(VERBOSE_DEBUG, verbose, "argument string: %s required arguments: %s", args, requiredarglist);

  // Assemble long options
  struct option *long_options = Malloc((count+4)*sizeof(struct option));
  long_options[0] = (struct option){"verbose", required_argument, NULL, 'v'};
  long_options[1] = (struct option){"version", no_argument,       NULL, 'V'};
  long_options[2] = (struct option){"xmlhelp", no_argument,       NULL,  0 };

  for (j = 3, i = 0; i < count; i++) {
    const argument_t *argument = &tool.arguments[i];
    if (argument->longname) {
      long_options[j++] = (struct option) {
        .name    = argument->longname,
        .has_arg = (argument->flag == PARAM_FLAG) ? no_argument : required_argument,
        .flag    = NULL,
        .val     = argument->name
      };
    }
  }
  long_options[j] = (struct option){NULL,      0,                 NULL,  0 };

  myverbose(VERBOSE_DEBUG, verbose, "parsing %d parameter%s...", argc-1, (argc-1>1)?"s":"");

  while (true) {
    int option_index = -1;
#if 0  // if only short is are available
    int opt = getopt(argc, argv, args);
#else
    int opt = getopt_long(argc, argv, args, long_options, &option_index);
#endif
    if (opt == -1) break; // got last argument

    bool islongopt = (option_index != -1);

    const char shortname[] = {opt, '\0'};
    const char *parametername = islongopt ? long_options[option_index].name : shortname;
    const char *parameterprefix = islongopt ? "--" : "-";

    for (i = 0; i < count; i++) {
      if (opt == tool.arguments[i].name) {
        if (bitfield[i]) {
          myexit(ERROR_PARAM, "Argument '%s%s' is already given.", parameterprefix, parametername);
        }
        // Save flag for arguments by user.
        bitfield[i] = true;

        getParameterValue(parameterprefix, parametername, optarg, (void*)tool.arguments[i].parameter, tool.arguments[i].flag);
        break;
      } // IF
    } // FOR

    switch (opt) {
      case 'v':
        // extern verbose is by default set to VERBOSE_NORMAL
        getParameterValue(parameterprefix, parametername, optarg, &verbose, PARAM_LONG);
        break;
      case 'V':
        printf("Release Version %s - Date %s\n", PACE3D_VERSION, PACE3D_RELEASE_DATE);
        exit(EXIT_OK);
        break;
      case ':':
        printUsage(argv[0], tool, count);
        if (islongopt) {
          myexit(ERROR_PARAM, "Option '%s' requires an argument.", argv[optind - 1]);
        } else {
          myexit(ERROR_PARAM, "Option '-%c' requires an argument.", optopt);
        }
      case '?':
        printUsage(argv[0], tool, count);
        if (optopt == '\0') {
          myexit(ERROR_PARAM, "Unknown parameter '%s' is given.", argv[optind - 1]);
        } else {
          myexit(ERROR_PARAM, "Unknown parameter '-%c' is given.", optopt);
        }
    }

  } // WHILE
  Free(long_options);

  // Get all required positional arguments (NOT Options)
  for (long i = 0; i < count; i++) {
    const argument_t *argument = &tool.arguments[i];
    if (argument->name == ' ') {
      if (optind >= argc) {
        break; // if argument is required a message is printed later
      }
      getParameterValue("", "<positional parameter>", argv[optind], (void*)argument->parameter, argument->flag);
      optind++;
      bitfield[i] = true;
    }
  }
  if (optind < argc) {
    printUsage(argv[0], tool, count);
    myexit(ERROR_PARAM, "Too many positional parameters are given.");
  }

  for (long n = 0; n < argc; n++) {
    if (strcmp(argv[n], "--xmlhelp") == 0) {
      printXMLUsage(argv[0], &tool, count, bitfield, defaultstrings);
      exit(EXIT_OK);
    }
  }

  bool errorfound = false;

  // generate relation matrices for 'and' and 'xor'
  // might be not set symmetric therfore test all combinations
  bool **andmatrix = MallocBM(count, count);
  bool **xormatrix = MallocBM(count, count);

  for (long i = 0; i < count; i++) {
    const argument_t *argument = &tool.arguments[i];
    for (long j = 0; j < strlen(argument->andparams); j++) {
      bool xorflag = false;
      if (argument->andparams[j] == '!') {
        xorflag = true;
        j++;
      }
      bool found = false;
      for (long k = 0; k < count; k++) {
        if (tool.arguments[k].name == argument->andparams[j]) {
          if (i != k) {
            if (xormatrix[i][k] || andmatrix[i][k]) mydebug("Parameter '%c' is already set at pos %ld", argument->andparams[j], j);
            if (xorflag) {
              xormatrix[i][k] = true;
            } else {
              andmatrix[i][k] = true;
            }
          } else {
            mydebug("Parameter '%c' is in its own andparams at pos %ld", argument->name, j);
          }
          found = true;
          // search to the end for debugging purpose
          // break;
        }
      }
      if (!found) {
        mydebug("Parameter '%s%c' in andparams is not found", (xorflag) ? "!" : "", argument->andparams[j]);
      }
    }
  }
  // printf("and matrix\n");
  // for (i = 0; i < count; i++) { for (j = 0; j < count; j++) { printf("%s", (andmatrix[i][j])?"1":"0"); } printf("\n"); }
  // printf("xor matrix\n");
  // for (i = 0; i < count; i++) { for (j = 0; j < count; j++) { printf("%s", (xormatrix[i][j])?"1":"0"); } printf("\n"); }
  // check for symmetry in matrices
  for (long i = 0; i < count; i++) {
    for (long j = i+1; j < count; j++) {
#ifdef DEBUG
      if (andmatrix[i][j] != andmatrix[j][i]) {
        mydebug("andparams 'and' list not symmetric (might be ok)");
      }
      if (xormatrix[i][j] != xormatrix[j][i]) {
        mydebug("andparams 'xor' list not symmetric (is not ok)");
      }
#endif
      if (andmatrix[i][j] && xormatrix[i][j]) {
        myexit(ERROR_BUG, "For parameter '%c' the parameter '%c' is set for 'and' and 'xor' in andparams", tool.arguments[j].name, tool.arguments[i].name);
      }
    }
  }
  // validate paramter combinations
  for (long i = 0; i < count; i++) {
    if (bitfield[i]) {
      for (long j = 0; j < count; j++) {
        // test for AND Parameters (and do not print twice)
        if (andmatrix[i][j] && !bitfield[j] && ((j > i) || !andmatrix[j][i])) {
          if (!errorfound) printUsage(argv[0], tool, count);
          errorfound = true;
          myerror("Parameter '-%c' requires parameter '-%c'", tool.arguments[i].name, tool.arguments[j].name);
        }
        // test for XOR Parameters (and do not print twice)
        if (xormatrix[i][j] && bitfield[j] && ((j > i) || !xormatrix[j][i])) {
          if (!errorfound) printUsage(argv[0], tool, count);
          errorfound = true;
          myerror("Parameter '-%c' is not allowed with '-%c'", tool.arguments[i].name, tool.arguments[j].name);
        }
      }
    }
  }

  FreeM(andmatrix, count);
  FreeM(xormatrix, count);

  // check if enough or too many parameters are given
  long positional = 0;
  for (i = 0; i < count; i++) {
    if (tool.arguments[i].name == ' ')  positional++;
    if (!bitfield[i] && tool.arguments[i].option == PARAM_REQUIRED) {
      if (!errorfound) printUsage(argv[0], tool, count);
      errorfound = true;

      const Variant *variant = getVariant(tool.arguments[i].flag);
      if (tool.arguments[i].name == ' ') {
        myerror("Positional parameter (%ld) with type %s is missing", positional, variant->typeinfostring);
      } else {
        myerror("Parameter '-%c' with type %s is missing.", tool.arguments[i].name, variant->typeinfostring);
      }

    }
  }

  if (errorfound) {
    myexit(ERROR_PARAM, "Parameter error");
  }

  validateParams(tool, count, bitfield);

  // printUsage uses this vector
  for (i = 0; i < count; i++) {
    if (defaultstrings[i]) Free(defaultstrings[i]);
  }
  Free(defaultstrings);

  return true;
}
