/********************************************************************//**
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2009 IMP - Institute of Materials and Processes @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       colormap.h
 *  @ingroup    lib
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 24.11.09    Sebastian Waniorek
 *
 ********************************************************************/

#ifndef LIB_COLORMAP_H
#define LIB_COLORMAP_H

#ifdef __cplusplus
extern "C" {
#endif

enum PaceColorMode                            { BRY,   BRG,   ContrastlessBRY,   OVB,   BYR,   BYb,   BGR,   Jet,   BWR,   Seismic,   Viridis,   Inferno,   Magma,   Plasma,   GREY,   CONST,   COLORFILE, NUM_PACECOLORS};
static const char* const PaceColorStrings[] = {"BRY", "BRG", "ContrastlessBRY", "OVB", "BYR", "BYb", "BGR", "Jet", "BWR", "Seismic", "Viridis", "Inferno", "Magma", "Plasma", "grey", "const", "colorfile"};

typedef struct colormap_s {
  enum PaceColorMode  mode;
  unsigned char       constcolor[3];
  unsigned char      *colormap;
  long                nummapentries;
  bool                interpolate;
  bool                invert;
  bool                negate;
} ColorMap;

static inline ColorMap* newColorMap(void) {
  return (ColorMap*)Calloc(1, sizeof(ColorMap));
}

static inline void freeColorMap(ColorMap* colormap) {
  Free(colormap);
}

ColorMap* ColorMap_init(ColorMap* colormap, const char* colorfilename);
void ColorMap_deinit(ColorMap* colormap);

static inline long ColorMap_resolveMode(const char* mode) {
  long i;
  for (i = 0; i < NUM_PACECOLORS; i++) {
    if (strcmp(mode, PaceColorStrings[i]) == 0) return i;
  }
  return -1;
}

/** @brief Allows individual settings of colorvalues for each object but points to the colormap of src.
  */
ColorMap* ColorMap_flatClone(ColorMap* dst, ColorMap* src);

static inline const char* ColorMap_getModeStr(ColorMap* colormap) {
  if (colormap->mode >= NUM_PACECOLORS) return "INVALID MODE";
  return PaceColorStrings[colormap->mode];
}

void ColorMap_setFile(ColorMap* colormap, const char* colorfilename);

static inline bool ColorMap_hasColorFile(ColorMap* colormap) { return (colormap->colormap != NULL); }

static inline void ColorMap_switchColorMode(ColorMap* colormap) {
  do {
    colormap->mode = (enum PaceColorMode)((colormap->mode+1)%NUM_PACECOLORS);
  } while (colormap->mode == COLORFILE && !ColorMap_hasColorFile(colormap));
}

static inline void ColorMap_setColorMode(ColorMap* colormap, int colormode) {
  colormap->mode = (enum PaceColorMode)colormode;

  if (colormap->mode == COLORFILE && !ColorMap_hasColorFile(colormap)) {
    ColorMap_switchColorMode(colormap);
  }
}

unsigned char* ColorMap_getColorNormalized(ColorMap* colormap, unsigned char color[3], REAL value);

static inline unsigned char* ColorMap_getColor(ColorMap* colormap, unsigned char color[3], REAL value, REAL min, REAL max) {
  REAL scaledval;

  if (value == -FLT_MAX) {
    // value represents barrier
    scaledval = -FLT_MAX;
  } else if (colormap->mode == COLORFILE) {
    // Omit scaling but crop to tableentries
    scaledval = MAX(0.0, MIN(value, (REAL)(colormap->nummapentries-1)));
  } else if (max == min) {
    // Assume zero
    scaledval = 0.0;
  } else {
    // scale value if not barrier
    scaledval = (value-min) / (max-min);
  }

  return ColorMap_getColorNormalized(colormap, color, scaledval);
}

#ifdef __cplusplus
}
#endif

#endif
