BASE=../../..
include $(BASE)/scripts/globals.mk

INCLUDEPATH += -I. -I.. -I$(BASE)/lib -I../cpputils/include

LIBNAME = properties

properties_SOURCE = \
          data/valuetypeinterfacehint.cpp \
          data/valuetypeinterface/abstractvaluetypeinterface.cpp \
          \
          data/ambassador.cpp \
          data/propertiesmodel.cpp \
          data/property.cpp \
          data/propertyfactory.cpp \
          \
          ui/factory/propertywidgetfactory.cpp \
          ui/factory/propertypanel.cpp \
          ui/factory/vtiwidget.cpp \
          \
          data/properties/linkedproperty.cpp \
          \
          data/properties/controls/boundingboxcontrolproperty.cpp \
          data/properties/controls/columnproperty.cpp \
          data/properties/controls/colormapproperty.cpp \
          data/properties/controls/colorcontrolproperty.cpp \
          data/properties/controls/cropselectionproperty.cpp \
          data/properties/controls/formattedtextproperty.cpp \
          data/properties/controls/webviewproperty.cpp \
          data/properties/controls/periodictableselectionproperty.cpp

properties_SOURCE += 	$(BASE)/lib/colormap.c

#
#
#

include $(BASE)/scripts/lib.mk

#
#
#

.PHONY: PIC
define ASCII_PIC
            "///oo§"
         '/oo////§/"/!"
        /o//////oo'!#ooo/!'
      'oo///////§' '§o////o/'
      oo///////o/'  oo/////oo"
     "§/////////oo/ /o///////§"                                           '!|//|!'
     /o//////////o/!§o///////oo'                                          "o§o//ooo/!'
     /o/////////oo'oo/////////§!   '"|///|"                           '!/!  "oo/////oo/'
     !o//////////o|'|o////////§/ '///o/''"!o/'                      !ooo#!    !#//////oo'
      o///////////oo!o////////#!|o"o|/#o    !o"     '!||||!'      'oo//§/    'oo////////§"
      "§////////////oo///////oo"§' o###|     !o   !////' '!//"   'oo//§|    !oo/////////oo'
       "o///////////////////o§'!o   ""'      '#''o||/|#§     //  oo//oo/|"''/ooo/////////§|
        '/o///////////////o§/' '§!           // // /###o      o/!§/////o§/'   |§o////////o/
          '/ooo////////oo§o§"   "o/'       '/o' o/  !|!       oo/o///o§/'  "/oo//////////§o
             "/oo§ooo§§oo/o§'     !/o//|////"   |o'          '§!|o/////oo|'!#////////////#|
                 /§oo/ooo§/'       |§//o#'       |o!       '|o" '§////////oo§o//////////oo'
                  |oo#o//oo        oo//oo        |§ooo//////!    !§////////////////////o§"
                     /o///§/       oo//oo       |o///o/''         !§o/////////////////o§"
                     '§o///#!      oo//oo      '§///o§'           |oo§o/////////////o§/'
                      !#///o§"     /§//o#!"!!!!o§///#/            §///o§§ooooooooo§o|'
                       /§///oo    '/#§ooooo/////oooo§o/!'         "ooo///oooo§#o|"'
                       '§o///§/|/ooo///////////////////ooo/"      "ooooooooooo/'
                        |#o/o§oo///////////////////////////oo/' '/o/////oo'
                        '§§o//////////////////////////////////oo§o////oo|
                      '/o///////////////////////////////////////oooo§o|'
                     !§o//////////////////////////////////////////o§!
                   '/§/////////////////////////////////////////////o§!
                !//§§///////////////////////////////////////////////o§!
             '/oooo§§////////////            ////////////////////////o#§oooo///"
            "oo§////§o/////////////////////////////////////////////§ooooooooooooo/"
           |§§#//ooo///ooo////////////////////////////////////////o§///////////oo§#o'
          !#o§ooo!      "/oo§ooooo////////////////////////////////o#§oooo§§o§§ooooo!'
          /"!#o|'             //ooo§§§§ooooooooooooooooooooooooo§§oo/||!!"""""!|/o§#/
            //'                            oooooooooooooooo////|!!"              ''"!'

endef
export ASCII_PIC
PIC:
	@echo "$$ASCII_PIC"
