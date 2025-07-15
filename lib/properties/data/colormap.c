/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2009 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       colormap.c
 *  @ingroup    lib
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 24.11.09    Sebastian Waniorek
 *  @lastmodified 18.12.12    Michael Selzer
 *
 ********************************************************************/

#include "wrapper.h"
#include <menu.h>

#include "colormap.h"

enum ColorIndices{RED, GREEN, BLUE};

static unsigned char checkColor(long color, const char* colorstr) {
  if (color < 0 || color > 255) {
    myexit(EXIT_ERROR, "RGB value %ld of %s is out of range (must be >= 0 and < 256).", color, colorstr);
  }

  return (unsigned char) color;
}

/** @brief long vector to unsigned byte vector
  */
static void lv2ubv(unsigned char* dst, long* src) {
  dst[RED  ] = checkColor(src[RED  ], "red");
  dst[GREEN] = checkColor(src[GREEN], "green");
  dst[BLUE ] = checkColor(src[BLUE ], "blue");
}

typedef void (*ColorFunc_func)(ColorMap* colormap, unsigned char color[3], REAL value);


typedef struct color_s {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} color_t;

static void getColorByTable(const color_t table[], int tablecnt, float v, unsigned char color[3]) {
  int     interval;
  float   factor;

  factor = (tablecnt-1) * v;
  if (isnan(factor) || isinf(factor)) factor = 0.0;
  interval = floor(factor);
  factor -= interval;

  if (interval >= tablecnt - 1) {
    color[RED  ] = table[interval].r;
    color[GREEN] = table[interval].g;
    color[BLUE ] = table[interval].b;
  } else {
    color[RED  ] = table[interval].r + factor * (table[interval+1].r - table[interval].r);
    color[GREEN] = table[interval].g + factor * (table[interval+1].g - table[interval].g);
    color[BLUE ] = table[interval].b + factor * (table[interval+1].b - table[interval].b);
  }
}

static void getColorBYb(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =  0, .g=  0, .b=139 }, // dark-blue          #00008b =   0   0 139
    { .r =  0, .g=  0, .b=255 }, // blue               #0000ff =   0   0 255
    { .r =  0, .g=255, .b=255 }, // cyan               #00ffff =   0 255 255
    { .r =  0, .g=255, .b=  0 }, // green              #00ff00 =   0 255   0
    { .r =255, .g=255, .b=  0 }, // yellow             #ffff00 = 255 255   0
    { .r =255, .g=  0, .b=  0 }, // red                #ff0000 = 255   0   0
    { .r =139, .g=  0, .b=  0 }, // dark-red           #8b0000 = 139   0   0
    { .r =  0, .g=  0, .b=  0 }  // black              #000000 =   0   0   0
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief BRY - Blue Red Yellow
  */
static void getColorBRY(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =  0, .g=  0, .b=255 },
    { .r =127, .g=  0, .b=127 },
    { .r =255, .g=  0, .b=  0 },
    { .r =255, .g=127, .b=  0 },
    { .r =255, .g=255, .b= 85 }
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

static void getColorContrastlessBRY(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =  0, .g=  0, .b=127 },
    { .r = 85, .g=  0, .b= 64 },
    { .r =170, .g=  0, .b=  0 },
    { .r =255, .g=127, .b=  0 },
    { .r =255, .g=255, .b=  0 }
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief BRG - Blue Red Green
  */
static void getColorBRG(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =  0, .g=  0, .b=255 },
    { .r =255, .g=  0, .b=255 },
    { .r =255, .g=  0, .b=  0 },
    { .r =255, .g=255, .b=  0 },
    { .r =  0, .g=255, .b=  0 }
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief BYR - Blue Yellow Red
  */
static void getColorBYR(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =  0, .g=  0, .b=255 }, // blue
    { .r =255, .g=255, .b=  0 }, // yellow
    { .r =255, .g=  0, .b=  0 }  // red
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief OVB - Orange Violet Blue
  */
static void getColorOVB(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =255, .g=127, .b=  0 }, // orange
    { .r =127, .g=  0, .b=127 }, // violet
    { .r =  0, .g=127, .b=255 }  // blue
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief BGR - StarCCM+ like colorbar
  */
static void getColorBGR(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =  0, .g=   0, .b= 255 },
    { .r =  0, .g=  92, .b= 255 },
    { .r =  0, .g= 130, .b= 255 },
    { .r =  0, .g= 159, .b= 255 },
    { .r =  0, .g= 183, .b= 255 },
    { .r =  0, .g= 205, .b= 255 },
    { .r =  0, .g= 224, .b= 255 },
    { .r =  0, .g= 242, .b= 255 },
    { .r =  0, .g= 255, .b= 251 },
    { .r =  0, .g= 255, .b= 234 },
    { .r =  0, .g= 255, .b= 215 },
    { .r =  0, .g= 255, .b= 194 },
    { .r =  0, .g= 255, .b= 171 },
    { .r =  0, .g= 255, .b= 145 },
    { .r =  0, .g= 255, .b= 112 },
    { .r =  0, .g= 255, .b=  65 },
    { .r = 65, .g= 255, .b=   0 },
    { .r =112, .g= 255, .b=   0 },
    { .r =145, .g= 255, .b=   0 },
    { .r =171, .g= 255, .b=   0 },
    { .r =194, .g= 255, .b=   0 },
    { .r =215, .g= 255, .b=   0 },
    { .r =234, .g= 255, .b=   0 },
    { .r =251, .g= 255, .b=   0 },
    { .r =255, .g= 242, .b=   0 },
    { .r =255, .g= 224, .b=   0 },
    { .r =255, .g= 205, .b=   0 },
    { .r =255, .g= 183, .b=   0 },
    { .r =255, .g= 159, .b=   0 },
    { .r =255, .g= 130, .b=   0 },
    { .r =255, .g=  92, .b=   0 },
    { .r =255, .g=   0, .b=   0 }
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief Jet
  */
static void getColorJet(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =    0, .g=    0, .b=  128 },
    { .r =    0, .g=    0, .b=  255 },
    { .r =    0, .g=  128, .b=  255 },
    { .r =    0, .g=  255, .b=  255 },
    { .r =  128, .g=  255, .b=  128 },
    { .r =  255, .g=  255, .b=    0 },
    { .r =  255, .g=  128, .b=    0 },
    { .r =  255, .g=    0, .b=    0 },
    { .r =  128, .g=    0, .b=    0 },
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief BWR Blue White Red
  */
static void getColorBWR(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =    0, .g=    0, .b=  255 },
    { .r =  255, .g=  255, .b=  255 },
    { .r =  255, .g=    0, .b=    0 },
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief Seismic
  */
static void getColorSeismic(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =    0, .g=    0, .b=   76 },
    { .r =    0, .g=    0, .b=  255 },
    { .r =  255, .g=  255, .b=  255 },
    { .r =  255, .g=    0, .b=    0 },
    { .r =  127, .g=    0, .b=    0 },
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief Viridis
  */
static void getColorViridis(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r = 253, .g = 231, .b =  37},
    { .r = 234, .g = 229, .b =  26},
    { .r = 210, .g = 226, .b =  27},
    { .r = 186, .g = 222, .b =  40},
    { .r = 162, .g = 218, .b =  55},
    { .r = 139, .g = 214, .b =  70},
    { .r = 119, .g = 209, .b =  83},
    { .r =  99, .g = 203, .b =  95},
    { .r =  80, .g = 196, .b = 106},
    { .r =  63, .g = 188, .b = 115},
    { .r =  49, .g = 181, .b = 123},
    { .r =  38, .g = 173, .b = 129},
    { .r =  33, .g = 165, .b = 133},
    { .r =  30, .g = 157, .b = 137},
    { .r =  31, .g = 148, .b = 140},
    { .r =  34, .g = 140, .b = 141},
    { .r =  37, .g = 131, .b = 142},
    { .r =  41, .g = 123, .b = 142},
    { .r =  44, .g = 115, .b = 142},
    { .r =  47, .g = 107, .b = 142},
    { .r =  51, .g =  98, .b = 141},
    { .r =  56, .g =  89, .b = 140},
    { .r =  60, .g =  79, .b = 138},
    { .r =  64, .g =  69, .b = 136},
    { .r =  68, .g =  59, .b = 132},
    { .r =  70, .g =  48, .b = 126},
    { .r =  72, .g =  37, .b = 118},
    { .r =  72, .g =  26, .b = 108},
    { .r =  71, .g =  13, .b =  96},
    { .r =  68, .g =   1, .b =  84},
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief Inferno
  */
static void getColorInferno(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r = 252, .g =  255, .b =  164},
    { .r = 243, .g =  245, .b =  134},
    { .r = 242, .g =  230, .b =   97},
    { .r = 246, .g =  213, .b =   67},
    { .r = 250, .g =  196, .b =   42},
    { .r = 252, .g =  178, .b =   22},
    { .r = 252, .g =  163, .b =    9},
    { .r = 250, .g =  146, .b =    7},
    { .r = 247, .g =  130, .b =   18},
    { .r = 241, .g =  115, .b =   29},
    { .r = 235, .g =  100, .b =   41},
    { .r = 226, .g =   87, .b =   52},
    { .r = 217, .g =   77, .b =   61},
    { .r = 206, .g =   67, .b =   71},
    { .r = 193, .g =   58, .b =   80},
    { .r = 180, .g =   51, .b =   89},
    { .r = 166, .g =   45, .b =   96},
    { .r = 152, .g =   39, .b =  102},
    { .r = 140, .g =   35, .b =  105},
    { .r = 125, .g =   30, .b =  109},
    { .r = 111, .g =   25, .b =  110},
    { .r =  97, .g =   19, .b =  110},
    { .r =  82, .g =   14, .b =  109},
    { .r =  68, .g =   10, .b =  104},
    { .r =  54, .g =    9, .b =   97},
    { .r =  38, .g =   12, .b =   81},
    { .r =  24, .g =   12, .b =   60},
    { .r =  12, .g =    8, .b =   38},
    { .r =   4, .g =    3, .b =   18},
    { .r =   0, .g =    0, .b =    4},
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief Magma
  */
static void getColorMagma(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r = 252, .g =  253, .b =  191},
    { .r = 252, .g =  238, .b =  176},
    { .r = 253, .g =  222, .b =  160},
    { .r = 254, .g =  205, .b =  144},
    { .r = 254, .g =  189, .b =  130},
    { .r = 254, .g =  172, .b =  118},
    { .r = 254, .g =  157, .b =  108},
    { .r = 252, .g =  140, .b =   99},
    { .r = 249, .g =  123, .b =   93},
    { .r = 245, .g =  107, .b =   92},
    { .r = 238, .g =   91, .b =   94},
    { .r = 228, .g =   79, .b =  100},
    { .r = 217, .g =   70, .b =  107},
    { .r = 204, .g =   63, .b =  113},
    { .r = 189, .g =   57, .b =  119},
    { .r = 174, .g =   52, .b =  123},
    { .r = 160, .g =   47, .b =  127},
    { .r = 145, .g =   43, .b =  129},
    { .r = 132, .g =   38, .b =  129},
    { .r = 118, .g =   33, .b =  129},
    { .r = 104, .g =   28, .b =  129},
    { .r =  90, .g =   22, .b =  126},
    { .r =  76, .g =   17, .b =  122},
    { .r =  61, .g =   15, .b =  113},
    { .r =  47, .g =   17, .b =   99},
    { .r =  33, .g =   17, .b =   78},
    { .r =  21, .g =   14, .b =   56},
    { .r =  11, .g =    9, .b =   36},
    { .r =   3, .g =    3, .b =   18},
    { .r =   0, .g =    0, .b =    4},
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

static void getColorPlasma(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r = 240, .g =  249, .b =   33},
    { .r = 245, .g =  235, .b =   39},
    { .r = 249, .g =  220, .b =   36},
    { .r = 252, .g =  205, .b =   37},
    { .r = 254, .g =  190, .b =   42},
    { .r = 253, .g =  177, .b =   48},
    { .r = 252, .g =  165, .b =   55},
    { .r = 249, .g =  152, .b =   62},
    { .r = 245, .g =  140, .b =   70},
    { .r = 240, .g =  128, .b =   78},
    { .r = 235, .g =  117, .b =   86},
    { .r = 229, .g =  106, .b =   93},
    { .r = 222, .g =   97, .b =  100},
    { .r = 215, .g =   86, .b =  108},
    { .r = 207, .g =   76, .b =  116},
    { .r = 199, .g =   66, .b =  124},
    { .r = 190, .g =   56, .b =  133},
    { .r = 180, .g =   46, .b =  141},
    { .r = 171, .g =   36, .b =  148},
    { .r = 160, .g =   26, .b =  156},
    { .r = 148, .g =   16, .b =  162},
    { .r = 135, .g =    7, .b =  166},
    { .r = 122, .g =    2, .b =  168},
    { .r = 108, .g =    0, .b =  168},
    { .r =  96, .g =    1, .b =  166},
    { .r =  81, .g =    2, .b =  163},
    { .r =  67, .g =    3, .b =  158},
    { .r =  51, .g =    5, .b =  151},
    { .r =  34, .g =    6, .b =  144},
    { .r =  13, .g =    8, .b =  135},
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief Grey
  */
static void getColorGrey(UNUSED(ColorMap* colormap), unsigned char color[3], REAL value) {
  static const color_t table[] = {
    { .r =  0, .g=  0, .b=  0 },  // black
    { .r =255, .g=255, .b=255 }   // white
  };

  getColorByTable(table, sizeof(table)/sizeof(color_t), value, color);
}

/** @brief Const
  */
static void calcConst(ColorMap* colormap, unsigned char color[3], UNUSED(REAL value)) {
  memcpy(color, colormap->constcolor, 3*sizeof(unsigned char));
}

static void calcMap(ColorMap* colormap, unsigned char color[3], REAL value) {
  long cindex = (long)round(value);
  if (cindex >= colormap->nummapentries) {  // undefined color in colormap
    memset(color, 0x00, 3*sizeof(unsigned char));
    return;
  }

  memcpy(color, colormap->colormap+cindex*3, 3*sizeof(unsigned char));
}

static ColorFunc_func colorfunctions[] = {
  getColorBRY,
  getColorBRG,
  getColorContrastlessBRY,
  getColorOVB,
  getColorBYR,
  getColorBYb,
  getColorBGR,
  getColorJet,
  getColorBWR,
  getColorSeismic,
  getColorViridis,
  getColorInferno,
  getColorMagma,
  getColorPlasma,
  getColorGrey,
  calcConst,
  calcMap
};

ColorMap* ColorMap_init(ColorMap* colormap, const char* colorfilename) {
  *colormap = (ColorMap) {
    .mode        = BRY,
    .constcolor = { 255, 255, 255 },
    .interpolate = true,
    .invert      = false,
    .negate      = false,
  };

  ColorMap_setFile(colormap, colorfilename);

  return colormap;
}

void ColorMap_deinit(ColorMap* colormap) {
  if (colormap->colormap != NULL) Free(colormap->colormap);
}

ColorMap* ColorMap_flatClone(ColorMap* dst, ColorMap* src) {
  return Memcpy(dst, src, sizeof(ColorMap));
}

void ColorMap_setFile(ColorMap* colormap, const char* colorfilename) {

  // if (colormap->colormap) Free(colormap->colormap);
  // if (colorfilename == NULL || strcmp(colorfilename, "") == 0) return;
  //
  // Infile *colordata = newInfile();
  //
  // // init colorfile reader
  // Infile_init(colordata, colorfilename);
  //
  // // read number of indices
  // Infile_getValue(colordata, "phasecount=%i", &colormap->nummapentries);
  //
  // // alloc colormap
  // colormap->colormap = Malloc(colormap->nummapentries*3*sizeof(unsigned char));
  //
  // // Read color infomation from file into array
  // for (long i = 0; i < colormap->nummapentries; i++) {
  //   long color[3];
  //   // Read from infile to tmp. variables
  //   Menu_printProgressBar("reading colormap entries", i+1, colormap->nummapentries);
  //   Infile_getValue(colordata, "phase(%i)=(%i,%i,%i)", i, color+RED, color+GREEN, color+BLUE);
  //   // Copy and convert to color array
  //   lv2ubv(colormap->colormap+3*i, color);
  // }
  //
  // // deinit colorfile reader
  // Infile_deinit(colordata);
}

unsigned char* ColorMap_getColorNormalized(ColorMap* colormap, unsigned char color[3], REAL value) {
  if (value == -FLT_MAX) {
    color[RED] = color[GREEN] = color[BLUE] = 180;
    return color;
  }

  if (colormap->mode != COLORFILE) {
    if (colormap->invert) {
      value = 1.0 - value;
    }
    if (colormap->interpolate) {
      if (value < 0.0) {
        value = 0.0;
      } else if (value > 1.0) {
        value = 1.0;
      }
    } else {
      if (       value < 0.125) {
        value = 0.0;
      } else if (value < 0.375) {
        value = 0.25;
      } else if (value < 0.625) {
        value = 0.5;
      } else if (value < 0.875) {
        value = 0.75;
      } else {
        value = 1.0;
      }
    }
  }

  colorfunctions[colormap->mode](colormap, color, value);
  if (colormap->negate) {
    for (long i = 0; i < 3; i++) color[i] = 255-color[i];
  }
  return color;
}
