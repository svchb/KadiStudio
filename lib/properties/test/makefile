BASE=../../..
include $(BASE)/scripts/globals.mk

CFLAGS      += -fPIC
INCLUDEPATH += -I. -I.. -I$(BASE)/lib

PROGRAMS = sizeinfo propertiestest

sizeinfo_SOURCE       = sizeinfo.cpp
sizeinfo_LIBS         = $(BASE)/lib/properties/libproperties.a
propertiestest_SOURCE = propertiestest.cpp
propertiestest_LIBS   = $(BASE)/lib/properties/libproperties.a

#
#
#

include $(BASE)/scripts/progtargets.mk

#
#
#

define ASCII_PIC

   ___     _______
  |'v'|   |  '^'  |
          |       |
          |       |


endef
export ASCII_PIC
PIC:
	@echo "$$ASCII_PIC"
