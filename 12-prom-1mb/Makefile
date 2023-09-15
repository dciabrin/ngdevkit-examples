# Copyright (c) 2015-2022 Damien Ciabrini
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

# Override default to use a ROM that is known to have a 1MB P-ROM
# You must choose a known ROM name so that the emulator correctly
# loads the full 1MB P-ROM. ssideki2 is such an example.
GAMEROM=ssideki2
MROM=rom/061-m1.m1
SROM=rom/061-s1.s1
PROM=rom/061-p1.p1
VROM1=rom/061-v1.v1
VROM2=rom/061-v2.v2
VROM=$(VROM1) $(VROM2)
CROM1=rom/061-c1-16.c1
CROM2=rom/061-c2-16.c2
CROM3=rom/061-c3-16.c3
CROM4=rom/061-c4-16.c4
CROM=$(CROM1) $(CROM2) $(CROM3) $(CROM4)

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM) $(SROM) $(VROM) $(MROM) | rom

OBJS=main
ELF=rom.elf
FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

# Enable multiple .text segments explicitly
$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit`

%.o: %.c
	$(M68KGCC) `pkg-config --cflags ngdevkit` -std=c99 -fomit-frame-pointer -g -c $< -o $@


# sound driver ROM: ngdevkit's nullsound
MROMSIZE:=131072
$(MROM): | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(NGDKSHAREDIR)/nullsound_driver.ihx $@ --pad-to $(MROMSIZE)

# sample ROM: empty
VROMSIZE:=2097152
$(VROM): | rom
	dd if=/dev/zero bs=$(VROMSIZE) count=1 of=$@

# sprite ROM: empty
CROMSIZE:=2097152
$(CROM1) $(CROM2) $(CROM3) $(CROM4): | rom
	touch $@ && $(TRUNCATE) -s $(CROMSIZE) $@

# fixed tile ROM: fonts from common assets
SROMSIZE:=131072
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@

# program ROM
PROMSIZE:=1048576
$(PROM): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

clean:
	rm -f *.o *~ $(ELF) tmp.* rom/*.*

.PHONY: clean
