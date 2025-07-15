/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2009 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       structures.h
 *  @ingroup    lib
 *  @brief
 *
 ********************************************************************
 *
 *  @lastmodified 09.03.09    Swetlana Kuklina
 *
 ********************************************************************/

#ifndef LIB_STRUCTURES_H
#define LIB_STRUCTURES_H

typedef struct tokenlist_s {
  char  **result;
  ulong   count;
  char    separator;
} tokenlist_t;

typedef struct vectorl_n_s {
  long  *vector;
  long   size;
} vectorl_n_t;

typedef struct vectorr_n_s {
  REAL  *vector;
  long   size;
} vectorr_n_t;

typedef struct frames_s {
  long  *frames;
  long   count;
} frames_t;

typedef struct boundingbox2D_s {
  long  x1;
  long  y1;
  long  x2;
  long  y2;
} boundingbox2D_t;

typedef struct boundingbox3D_s {
  long  x1;
  long  y1;
  long  z1;
  long  x2;
  long  y2;
  long  z2;
} boundingbox3D_t;

/** @brief Check boundingbox 3D [Left,Bottom,Back],[Right,Top,Front].
  *
  * @return true if: 0 <= Left <= Right <= RightMax and 0 <= Bottom <= Top <= TopMax and 0 <= Back <= Front <= FrontMax.
  */
bool validateBoundingBox3D(const long bottom[3], const long top[3], const boundingbox3D_t* bbox3D);

/** @brief Check boundingbox 2D
  *
  * @return true if bounding box is inside the domain
  */
bool validateBoundingBox2D(const long bottom[3], const long top[3], const boundingbox2D_t* bbox2D, long planeid);

typedef struct plot_s {
  char  *terminal;                ///< for terminal
  char  *filename;                ///< name of graphic-file
  FILE  *fp;                      ///< file pointer for plotter output
  char  *dat_filename;            ///< name of .dat-file
  char  *title;                   ///< title for graphic
  char  *titlefont;               ///< font of the title for graphic
  long   title_fontsize;          ///< size of the title font (in pt)
  long  *columns;                 ///< for 'using'
  long   count;                   ///< counter for columns
  char  *linecolor;               ///< for line color (rgb-name)
  char  *linetype;                ///< for line design
  REAL   linewidth;               ///< width of line
  bool   multiplot;               ///< for multiplot (mehrere plot's in einem Bild)
  char  *x_label;                 ///< X-axis-label
  bool   xtic;                    ///< ob xtic (Achsenmarkierung) gewuenscht ist    -----  muss am Anfang auf true gesetzt
  REAL   x_logscale;              ///< Skalierungsfaktor fuer die log. Darstellung der x-Achse
  char  *y_label;                 ///< Y-axis-label
  bool   ytic;                    ///< ob ytic (Achsenmarkierung) gewuenscht ist    -----  muss am Anfang auf true gesetzt
  REAL   y_logscale;              ///< Skalierungsfaktor fuer die log. Darstellung der y-Achse
  char  *xy_labelfont;            ///< font of the xy_label for graphic
  long   xy_label_fontsize;       ///< size of the xy_label font (in pt)
  bool   key;                     ///< ob die Legende gewuenscht ist                -----  muss am Anfang auf true gesetzt
  char  *keylabel;                ///< label of key
  REAL   key_position[2];         ///< position of key                              -----  muss bei der Initialisierung auf 0 gesetzt werden
  bool   animation;               ///< for animation (ob die animation gewuenscht ist)
  REAL   delay;                   ///< delay for animation
  long   size[2];                 ///< size of graphic in pixel
  REAL   scalfactor;              ///< scaling factor for size of graphic
  boundingbox2D_t bbox;           ///< for bounding of graphic
  char  *configfilename;          ///< name of configfile, if rerun with same settings is requested
  int    floatstyle;              ///< spezieller Stil wie die float-Zahlen in der Tabelle zu darstellen sind; 1- e, 2- E, 3- f, 4- g
  unsigned int  accuracy;         ///< die Genauigkeit, mit der die float-Zahlen zu darstellen sind; 0 -not specified
  bool   stackplot;               ///< indicates that all files (over t) should be plottet in one plot
  char **stackplotfilenames;      ///< names of dat files for multiplot
  long   stackplotfilecount;      ///< number of multiplotfiles
  char **stackplotcolumnnames;    ///< in case of a stackplot remember the columnames
  char  *previousterminal;        ///< to restore after stackplot file generation init previous terminal
} plot_t;

#endif
