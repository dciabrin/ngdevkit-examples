# Copyright (c) 2015-2019 Damien Ciabrini
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

all: cart nullbios

include ../Makefile.config

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM1) $(CROM2) $(SROM) $(VROM) $(MROM) | rom

OBJS=main
ELF=rom.elf
FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

sprites/back.gif: gfx/country-platform-back.png | sprites
	$(CONVERT) $^ -crop 384x144+0+16 +repage $@

sprites/back.c1 sprites/back.c2: sprites/back.gif
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/back.pal: sprites/back.gif
	$(PALTOOL) $< -o $@

sprites/forest.gif: gfx/country-platform-forest.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 480x144+0+80 +repage -background black -flatten $@

sprites/forest.c1 sprites/forest.c2: sprites/forest.gif
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/forest.pal: sprites/forest.gif
	$(PALTOOL) $< -o $@

sprites/front.gif: gfx/country-platform-tiles-example.png | sprites
	$(CONVERT) $^ $^ +append -crop 384x144+0+80 +repage -background black -flatten $@

sprites/front.c1 sprites/front.c2: sprites/front.gif
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/front.pal: sprites/front.gif
	$(PALTOOL) $< -o $@

$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit`

%.o: %.c
	$(M68KGCC) $(NGCFLAGS) -std=gnu99 -fomit-frame-pointer -g -c $< -o $@

main.c: sprites/back.pal sprites/forest.pal sprites/front.pal

# sound driver ROM: ngdevkit's nullsound
MROMSIZE:=131072
$(MROM): | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(NGDKSHAREDIR)/nullsound_driver.ihx $@ --pad-to $(MROMSIZE)

# sample ROM: empty
$(VROM): | rom
	dd if=/dev/zero bs=1024 count=512 of=$@

# sprite ROM C1 C2: parallax layers
CROMSIZE:=1048576
$(CROM1): $(ASSETS)/rom/c1.bin sprites/back.c1 sprites/forest.c1 sprites/front.c1 | rom
	cat $(ASSETS)/rom/c1.bin $(filter %.c1,$^) > $@ && $(TRUNCATE) -s $(CROMSIZE) $@

$(CROM2): $(ASSETS)/rom/c2.bin sprites/back.c2 sprites/forest.c2 sprites/front.c2 | rom
	cat $(ASSETS)/rom/c2.bin $(filter %.c2,$^) > $@ && $(TRUNCATE) -s $(CROMSIZE) $@

# fixed tile ROM: fonts from common assets
SROMSIZE:=131072
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@

# program ROM
PROMSIZE:=524288
$(PROM): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

clean:
	rm -rf *.gif *.pal *.o *~ $(ELF) tmp.* rom sprites

sprites:
	mkdir $@

.PHONY: clean
