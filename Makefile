# Copyright (c) 2019 Damien Ciabrini
# This file is part of ngdevkit
#
# ngdevkit is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# ngdevkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.

EXAMPLES = \
01-helloworld \
02-sprite \
03-sprite-animation \
04-palette \
05-scrolling

include Makefile.config

all:
	for i in $(EXAMPLES); do $(MAKE) -C $$i cart nullbios; done

clean:
	for i in $(EXAMPLES); do $(MAKE) -C $$i clean; done
	$(MAKE) -C assets clean

distclean:
	-$(MAKE) clean
	find . -name '*~' -delete
	rm -rf config.log config.status configure aclocal.m4 Makefile.config autom4te.cache


include Makefile.common

# ngdevkit-gngeo config targets
all: build-gngeo-config

define GNGEO_DEFAULT_INPUT_SETTINGS =
# default blitter
blitter $(if $(findstring yes,$(GNGEO_GLSL)),glsl,soft)
$(if $(and $(findstring yes,$(GNGEO_GLSL)),$(if $(findstring yes,$(ENABLE_MINGW)),,nomingw)),shaderpath $(GLSL_SHADER_PATH),)
$(if $(findstring yes,$(GNGEO_GLSL)),shader $(SHADER),)

# default scale factor
scale 3

# default key mapping
p1control A=K97,B=K115,C=K113,D=K119,START=K49,COIN=K51,UP=K82,DOWN=K81,LEFT=K80,RIGHT=K79,A=J0B0,B=J0B1,C=J0B2,D=J0B3,START=J0B9,COIN=J0B8,UP=J0a3,DOWN=J0a3,LEFT=J0A0,RIGHT=J0A0
endef

ifneq ($(ENABLE_MINGW),yes)
GNGEO_CFG=$(HOME)/.gngeo/ngdevkit-gngeorc
else
GNGEO_CFG=$(GNGEO_INSTALL_PATH)/conf/ngdevkit-gngeorc
endif

build-gngeo-config: $(GNGEO_CFG)

$(GNGEO_CFG): export INPUT_SETTINGS:=$(GNGEO_DEFAULT_INPUT_SETTINGS)
$(GNGEO_CFG):
	@ echo generating a default input config for gngeo; \
	mkdir -p $(dir $(GNGEO_CFG)) && \
	echo "$$INPUT_SETTINGS" > $(GNGEO_CFG)



# ngdevkit-gngeo shaders when running under mingw
ifeq ($(ENABLE_MINGW),yes)
ifneq ($(SHADER),)
ifneq ($(SHADER),noop.glslp)
ifneq ($(GLSL_SHADER_PATH),)
all: copy-gngeo-shaders
copy-gngeo-shaders: $(GNGEO_INSTALL_PATH)/shaders/$(SHADER)
endif
endif
endif
endif



.PHONY: all clean distclean
