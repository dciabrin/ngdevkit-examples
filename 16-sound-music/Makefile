# Copyright (c) 2015-2025 Damien Ciabrini
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



all: cart bios

# These are various variables that you might want to customize
# based on your liking or your requirements.
# location of generated and compiled content
BUILDDIR=build
# all directories that contain source to be compiled
SRCDIRS=assets
# default build flags, can be overriden per target
CFLAGS=-I$(BUILDDIR) -std=c99 -fomit-frame-pointer -O2 -g
LDFLAGS=
Z80FLAGS=
Z80LDFLAGS=

# This is an autoconf-generated configuration for your environment
# (ngdevkit path, OS-specific configs...)
include config.mk

# This defines the layout of your game cartridge
# You can customize it to match your requirements
# Name of the rom file created
GAMEROM=16_sound_music
# Game title, long description
GAMETITLE=Music playback API
include rom.mk

# All the generic build targets (68k, Z80, assets, run)
include build.mk

# Some default targets for running your project via emulators
include emu.mk



# program ROM: your main program
# Add your dependencies below to compile your sources into an ELF binary
# that is used as the content of the program ROM (referenced by symbol PROM1)
# 
# Note: build rules (%.c -> %.o -> %.elf) are defined in Makefile.build
ELF=$(BUILDDIR)/rom.elf
$(PROM1): $(ELF)
$(ELF):	$(BUILDDIR)/main.o

# generate a list of music and soundfx for the m68k main program
$(BUILDDIR)/main.o: $(BUILDDIR)/snd_commands.h
$(BUILDDIR)/main.o: $(BUILDDIR)/snd_commands_info.h
$(BUILDDIR)/snd_commands_info.h: $(BUILDDIR)/user_commands.rel
	cat $(<:%.rel=%.lst) | awk '/cmd_jmptable::/ {table=1; next} table==1 && /cmd_jmptable_padding/ {exit} table==1 && / C3 / {print "\""$$8"\","} END {print "\"\""}' | tr '_' ' ' > $@



# fixed tiles ROM: all your 8x8 pixel tiles for the fixed layer
# Add your dependencies below to convert images to fixed tile binary data
# and pack it into the fixed tile ROM (referenced by symbol SROM1)
# 
# By default, this makefile creates a tileset ROM with small and tall
# latin characters for printing ASCII string, and tiles that are
# displayed during the attract mode.
# Note: build rules (%.gif -> %.fix) are defined in Makefile.build
$(SROM1): $(BUILDDIR)/assets/base-srom-text-shadow.fix



# sprite ROM: all your 16x16 pixel tiles for sprites
# Add your dependencies below to convert images to fixed sprite binary data
# and pack it into two separate sprite ROMs that hold odd and even bits of
# color information (referenced by symbols CROM1 and CROM2)
#
# By default, this makefile creates CROMs with tiles for displaying a ngdevkit
# logo during the attract mode.
# Note: build rules (%.gif -> %.c<1,2>) are defined in Makefile.build
$(CROM1): $(BUILDDIR)/assets/base-crom-logo.c1
$(CROM2): $(BUILDDIR)/assets/base-crom-logo.c2



# sound driver ROM: nullsound + your configured sound commands
# Add your dependencies below to compile your z80 sources into a HEX binary
# that is used as the sound driver ROM (referenced by symbol MROM1)
#
# By default, this makefile uses the empty sound driver provided by ngdevkit
# Note: build rules (%.s -> %.o -> %.ihx) are defined in Makefile.build
SOUND_DRIVER=$(BUILDDIR)/demo_driver.ihx
$(MROM1): $(SOUND_DRIVER)

# The Furnace module in the demo is converted into two Z80 asm files
$(SOUND_DRIVER): $(BUILDDIR)/assets/ngdevkit-eye-catcher.lib
$(SOUND_DRIVER): $(BUILDDIR)/user_commands.rel
$(SOUND_DRIVER): $(BUILDDIR)/nss/nss-starship-battle.rel
$(SOUND_DRIVER): $(BUILDDIR)/nss/instruments-starship-battle.rel



# sound FX ROM: all your ADPCM samples
# Add your dependencies below to convert your samples into suitable
# ADPCM data and pack it into the sound ROM (referenced by symbol VROM1)
#
# Music and SFX assets can be managed with <> rather to generate
# the dependencies automatically.
# By default, no sample is used, so the VROM is empty
# Note: build rules (%.wav-> %.adpcm<a,b>) are defined in Makefile.build
$(VROM1): assets/music-map.yaml
$(VROM1): assets/sample-map.yaml



# One-time asset preprocessing
# Various assets may have to be processed before they can be used to
# build your ROM, for example:
#   . the ngdevkit assets must be converted to sprite and text tiles
#   . converting your SFX assets from .mp3 into .wav so they can be
#     processed by ngdevkit tools
# To handle these pre-processing requirements, `make` automatically
# runs into all subdirectories under ./setup/, so you get a chance
# to preprocess what you need before anything is built.
#
# Other assets must be processed only once to generate source files
# that gets built into your ROM, for example:
#   . converting your musics from .fur to z80 code and data files
#   . generating make dependencies to pack gfx or sound assets into
#     the right ROM bank based on your input assets
# Those actions can be achieved by creating custom make targets
# that get invoked automatically when added to the variable below.
CUSTOM_GENERATE_TARGETS=generate-sfx


# TODO use a generic build rule from build.mk
generate-sfx: $(BUILDDIR)/samples.inc
$(BUILDDIR)/samples.inc: assets/music-map.yaml
$(BUILDDIR)/samples.inc: assets/sample-map.yaml
$(BUILDDIR)/samples.inc:
	$(VROMTOOL) -v --asm -s $(VROMSIZE) $^ -o $(VROM1) -m $@

# TODO use a generic build rule from build.mk
generate-sfx: $(BUILDDIR)/snd_commands.inc
$(BUILDDIR)/snd_commands.inc: assets/music-map.yaml
$(BUILDDIR)/snd_commands.inc: assets/sample-map.yaml
$(BUILDDIR)/snd_commands.inc:
	$(SOUNDTOOL) -v -z -s $^ -o $@

# TODO use a generic build rule from build.mk
generate-sfx: $(BUILDDIR)/snd_commands.h
$(BUILDDIR)/snd_commands.h: assets/music-map.yaml
$(BUILDDIR)/snd_commands.h: assets/sample-map.yaml
$(BUILDDIR)/snd_commands.h:
	$(SOUNDTOOL) -c -s $^ -o $@

# TODO use a makefile-generated rules
generate-sfx: $(BUILDDIR)/nss/nss-starship-battle.s
$(BUILDDIR)/nss/nss-starship-battle.s: assets/musics/starship-battle.fur | $(BUILDDIR)/nss
	$(NSSTOOL) -z $< -n $(call asm_label,$(notdir $(@))) -o $@

# TODO use a makefile-generated rules
generate-sfx: $(BUILDDIR)/nss/instruments-starship-battle.s
$(BUILDDIR)/nss/instruments-starship-battle.s: assets/musics/starship-battle.fur | $(BUILDDIR)/nss
	$(FURTOOL) $< --instruments -n $(call asm_label,$(notdir $(@))) -m $(BUILDDIR)/samples.inc -o $@

$(BUILDDIR)/nss:
	mkdir -p $@ && touch $@

# a default `clean` target removes .o .elf and .ihx from the builddir
# a default `distclean` target removes the build directory entirely
# you can customize clean up by adding dependencies to those targets
