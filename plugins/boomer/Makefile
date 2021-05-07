#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = boomer

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	BoomerPlugin.cpp \
	MidiLooper.cpp \
	Song.cpp
	
FILES_UI  = \
	BoomerUI.cpp \
	widgets/src/ListView.cpp \
	fonts.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk
include ../../sfizz/dpf.mk

# Extra flags
BUILD_C_FLAGS += $(SFIZZ_C_FLAGS)
BUILD_CXX_FLAGS += $(SFIZZ_CXX_FLAGS)
LINK_FLAGS += $(SFIZZ_LINK_FLAGS)
LINK_FLAGS += $(shell $(PKG_CONFIG) --libs fmt)
LINK_FLAGS += ../../midifile/lib/libmidifile.a
BASE_FLAGS += -I../../midifile/include -I../../fmidi \
	-I../../sfizz/external/filesystem/include/ghc \
	-I./widgets \
	-I./Ring-Buffer

# --------------------------------------------------------------
# Enable lv2, vst and jack app
ifeq ($(HAVE_JACK),true)
ifeq ($(HAVE_OPENGL),true)
TARGETS += jack
endif
endif

ifeq ($(HAVE_OPENGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif

TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------