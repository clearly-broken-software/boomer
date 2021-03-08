#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

all: dgl libs plugins gen
plugins: sfizz-lib

include sfizz/dpf.mk

# --------------------------------------------------------------
dgl:
ifeq ($(HAVE_CAIRO_OR_OPENGL),true)
	$(MAKE) -C dpf/dgl
endif

libs:
	$(MAKE) library -C midifile

plugins: dgl
	$(MAKE) all -C plugins/boomer

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean: sfizz-clean
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C dpf
	$(MAKE) clean -C plugins/boomer
	$(MAKE) clean -C midifile
	rm -rf bin build

# --------------------------------------------------------------

.PHONY: plugins