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

SPRITES_IMGS=sprites/idle.gif sprites/walk.gif

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

# fix up the transparent color on all GIFs
sprites/%.gif: gfx/spr_m_traveler_%_anim.gif | sprites
	cp $< $@ ; \
	printf "\xff\x00\xff" | dd of=$@ bs=1 seek=13 count=3 conv=notrunc

# prepare the sprites for 64x64, and use same palette for all sprites
sprites/all.png: $(SPRITES_IMGS)
	$(CONVERT) -coalesce $^ -append +repage xc:'#ff00ff[64x1024!]' -compose dstover -composite PNG8:$@

sprites/all.c1 sprites/all.c2: sprites/all.png | sprites
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprite.pal: sprites/all.png | sprites
	$(PALTOOL) $< -o $@

$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) `pkg-config --libs ngdevkit` $^ -o $@

%.o: %.c
	$(M68KGCC) $(NGCFLAGS) -std=gnu99 -fomit-frame-pointer -g -c $< -o $@

main.c: sprite.pal

# sound driver ROM: ngdevkit's nullsound
$(MROM): | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(NULLSOUNDDIR)/nullsound.ihx $@ --pad-to 131072

# sample ROM: empty
$(VROM): | rom
	dd if=/dev/zero bs=1024 count=512 of=$@

# sprite ROM C1 C2: animations from player sprite
$(CROM1): $(ASSETS)/rom/c1.bin | rom sprites/all.c1
	cat $(ASSETS)/rom/c1.bin sprites/all.c1 > $@ && $(TRUNCATE) -s 1048576 $@

$(CROM2): $(ASSETS)/rom/c2.bin | rom sprites/all.c2
	cat $(ASSETS)/rom/c2.bin sprites/all.c2 > $@ && $(TRUNCATE) -s 1048576 $@

# fixed tile ROM: fonts from common assets
$(SROM): $(ASSETS)/rom/s1.bin | rom
	cp $(ASSETS)/rom/s1.bin	$@ && $(TRUNCATE) -s 131072 $@

# program ROM
$(PROM): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --reverse-bytes=2 $< $@

clean:
	rm -rf *.gif *.png *.pal *.o *~ $(ELF) tmp.* rom sprites

sprites:
	mkdir $@

.PHONY: clean
