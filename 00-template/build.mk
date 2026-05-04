# -*- makefile -*-
# Copyright (c) 2018-2025 Damien Ciabrini
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



define err
	$(error $(1) unset, do not call this makefile directly)
endef
BUILDDIR?=$(call err,BUILDDIR)
ROM?=$(call err,ROM)


# CART_TARGETS
# In order to build a cartridge, one must build various targets:
#  - build the archive that holds the cartridge's contents (e.g. zip)
#  - build a hash file that the emulator can understand to boot the cartridge
#
# Different emulators expect different input format, so build the supported
# format below.

# Zip cartridge file
CART_TARGETS+=$(CART_ZIP)
CART_ZIP?=$(ROM)/$(GAMEROM).zip

# MAME hash file
CART_TARGETS+=$(HASH_MAME)
HASH_MAME?=$(ROM)/neogeo.xml
$(HASH_MAME): $(CART_ZIP)
	$(ROMTOOL) -b hash -f mame -p $(PROM1) $(PROM2) -c $(CROM1) $(CROM2) $(CROM3) $(CROM4) $(CROM5) $(CROM6) $(CROM7) $(CROM8) -v $(VROM1) $(VROM2) $(VROM3) $(VROM4) -s $(SROM1) -m $(MROM1) -n $(GAMEROM) -l "$(GAMETITLE)" -o $@

# GnGeo hash file plus emulator's skin data (this is a single file in GnGeo)
CART_TARGETS+=$(HASH_GNGEO)
HASH_GNGEO?=$(ROM)/gngeo_data.zip
$(HASH_GNGEO): $(CART_ZIP)
	$(ROMTOOL) -b hash -f gngeo -p $(PROM1) $(PROM2) -c $(CROM1) $(CROM2) $(CROM3) $(CROM4) $(CROM5) $(CROM6) $(CROM7) $(CROM8) -v $(VROM1) $(VROM2) $(VROM3) $(VROM4) -s $(SROM1) -m $(MROM1) -n $(GAMEROM) -l "$(GAMETITLE)" -x gngeo.data=$(GNGEO_DATA) -o $@


CART?=$(CART_ZIP)
# A cartrige is made of one or several chips, as defined in rom.mk
# add all possible dependencies below (undefined chips are just ignored)
$(CART): $(PROM1) $(PROM2)
$(CART): $(MROM1)
$(CART): $(CROM1) $(CROM2) $(CROM3) $(CROM4) $(CROM5) $(CROM6) $(CROM7) $(CROM8)
$(CART): $(SROM1)
$(CART): $(VROM1) $(VROM2) $(VROM3) $(VROM4)

# for convenience, we ensure than the initial `make` always builds the
# generated files _before_ the cartridge target, even with parallel builds
# (NOTE: do not use .WAIT yet as it's only available starting GNU Make 4.4)
cart:
	@ if [ ! -f $(BUILDDIR)/.generated ]; then $(MAKE) $(BUILDDIR)/.generated; fi && \
	$(MAKE) --no-print-directory $(CART_TARGETS)

$(CART_ZIP):
	$(ROMTOOL) -b cartridge -f zip -p $(PROM1) $(PROM2) -c $(CROM1) $(CROM2) $(CROM3) $(CROM4) $(CROM5) $(CROM6) $(CROM7) $(CROM8) -v $(VROM1) $(VROM2) $(VROM3) $(VROM4) -s $(SROM1) -m $(MROM1) -n $(GAMEROM) -x "zip.comment=$(echo -e '===\nNGDEVKIT example ROM\nhttps://github.com/dciabrin/ngdevkit\n===')" -o $@

$(BUILDDIR) $(ROM):
	mkdir -p $@ && touch $@

.PHONY: cart


# -----------------------------------
# Build rules for pre-processing assets before build
#
# This target triggers the generation of assets that must be present prior
# to building the project (including custom generate targets)
$(BUILDDIR)/.generated: | $(SRCDIRS:%=$(BUILDDIR)/%)
	find setup -mindepth 1 -maxdepth 1 -type d -print | xargs -r -n1 $(MAKE) -C
	echo $(CUSTOM_GENERATE_TARGETS) | xargs -r $(MAKE)
	touch $@

$(SRCDIRS:%=$(BUILDDIR)/%): | $(BUILDDIR)
	find $(@:$(BUILDDIR)/%=%) -type d -exec mkdir -p $(BUILDDIR)/{} \; -exec touch $(BUILDDIR)/{} \;

# This target can be use to regenerate all assets manually
generate:
	$(MAKE) -B $(BUILDDIR)/.generated

.PHONY: generate $(CUSTOM_GENERATE_TARGETS)


# -----------------------------------
# Build rules for program ROM

$(BUILDDIR)/%.o: %.cc
	$(M68KGXX) $(NGCFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.c
	$(M68KGCC) $(NGCFLAGS) $(CFLAGS) -c $< -o $@

%.elf:
	$(M68KGCC) -o $@ $^ $(NGLDFLAGS) $(LDFLAGS)

$(PROM1): | $(ROM)
	$(M68KOBJCOPY) -O binary -S -R .text2 --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab status=none

ifdef PROM2SIZE
$(PROM2)_bank%: | $(ROM)
	$(M68KOBJCOPY) -O binary -j .text2 --gap-fill 0xff --pad-to $$((0x200000+$(PROMSIZE))) $< $@ && dd if=$@ of=$@ conv=notrunc,swab status=none

$(PROM2): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(PROM2SIZE) $@
else
$(PROM2): | $(ROM)
	$(M68KOBJCOPY) -O binary -j .text2 --gap-fill 0xff --pad-to $$((0x200000+$(PROMSIZE))) $< $@ && dd if=$@ of=$@ conv=notrunc,swab status=none
endif


# -----------------------------------
# Build rules for sound driver ROM

$(BUILDDIR)/nss/%.rel: $(BUILDDIR)/nss/%.s
	$(Z80SDAS) -g -l -p -u -I$(NGZ80INCLUDEDIR)/nullsound -I$(BUILDDIR) -o $@ $<

$(BUILDDIR)/%.rel: %.s
	$(Z80SDAS) -g -l -p -u -I$(NGZ80INCLUDEDIR)/nullsound -I$(BUILDDIR) -o $@ $<

%.ihx:
	$(Z80SDLD) -b DATA=0xf800 -i $@ $(NGZ80LIBDIR)/nullsound.lib $^

$(MROM1): | $(ROM)
	$(Z80SDOBJCOPY) -I ihex -O binary $< $@ --pad-to $(MROMSIZE)


# -----------------------------------
# Build rules for fixed tiles ROM

$(BUILDDIR)/%.fix: %.gif
	$(TILETOOL) --fix -c $< -o $@

$(SROM1): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(SROMSIZE) $@


# -----------------------------------
# Build rules for sprite tiles ROM

$(BUILDDIR)/%.pal: %.gif
	$(PALTOOL) $< -o $@

$(BUILDDIR)/%.c1 $(BUILDDIR)/%.c2: %.gif
	$(TILETOOL) --sprite -c $< -o $(@:%.c2=%.c1) $(@:%.c1=%.c2)

$(CROM1): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROMSIZE) $@
$(CROM2): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROMSIZE) $@
$(CROM3): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROMSIZE) $@
$(CROM4): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROMSIZE) $@
$(CROM5): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROM5SIZE) $@
$(CROM6): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROM6SIZE) $@
$(CROM7): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROM7SIZE) $@
$(CROM8): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(CROM8SIZE) $@


# -----------------------------------
# Build rules for ADPCM samples ROM

ifdef VROMTEMPLATE
VROMS?=$(shell echo $(VROM1) $(VROM2) $(VROM3) $(VROM4) | wc -w)
$(VROM1) $(VROM2) $(VROM3) $(VROM4): | $(ROM)
	$(VROMTOOL) --roms -s $(VROMSIZE) $^ -o $(VROMTEMPLATE) -n $(VROMS)
else
$(VROM1): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(VROMSIZE) $@
$(VROM2): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(VROMSIZE) $@
$(VROM3): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(VROMSIZE) $@
$(VROM4): | $(ROM)
	echo $^ | xargs -r cat > $@ && $(TRUNCATE) -s $(VROMSIZE) $@
endif

# -----------------------------------
# Build rules for setting up a BIOS ROM for the generated game ROM
# note: by default, use nullbios to avoid any external dependency
AES_BIOS?=$(NGSHAREDIR)/aes.zip
$(ROM)/$(notdir $(AES_BIOS)): $(AES_BIOS) | $(ROM)
	cp $< $@

MVS_BIOS?=$(NGSHAREDIR)/neogeo.zip
$(ROM)/$(notdir $(MVS_BIOS)): $(MVS_BIOS) | $(ROM)
	cp $< $@

bios: $(ROM)/$(notdir $(AES_BIOS)) $(ROM)/$(notdir $(MVS_BIOS))

.PHONY: bios


# -----------------------------------
# Build rules for housekeeping

clean:
	find . \( -name '*~' -or -name '*.o' -or -name '*.rel' -or -name '*.elf' -or -name '*.ihx' \) -delete

distclean:
	rm -rf build
	$(MAKE) clean
	find setup -mindepth 1 -maxdepth 1 -type d -print | xargs -I{} -r -n1 $(MAKE) -C {} distclean


.PHONY: clean distclean





# nullsound

define asm_label
$(shell echo $(1) | sed -e 's/\.[^.]*//' -e 's/[^a-zA-Z0-9_]/_/g' -e 's/^\([0-9]\)/_\1/g' | tr A-Z a-z)
endef



# --- OS-specific targets ---

# macOS doesn't ship truncate
ifeq ($(shell uname -s), Darwin)
TRUNCATE=$(PYTHON) -c 'import sys;open(sys.argv[3],"a").truncate(int(sys.argv[2]))'
else
TRUNCATE=truncate
endif

# ASSETS=../assets

# for macOS, may interfere with System Integrity Protection
define export_path
$(eval
ifeq ($(shell uname -s), Darwin)
$(1): export DYLD_LIBRARY_PATH=$(NGLIBDIR):$(DYLD_LIBRARY_PATH)
else
$(1): export LD_LIBRARY_PATH=$(NGLIBDIR):$(LD_LIBRARY_PATH)
endif
)
endef


