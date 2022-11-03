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

# Override default to use a ROM that is known to have two P-ROMs
# This requires special linking and objcopy commands below
GAMEROM=samsho
MROM=rom/045-m1.m1
SROM=rom/045-s1.s1
PROM1=rom/045-p1.p1
PROM2=rom/045-pg2.sp2
PROM=$(PROM1) $(PROM2)
VROM1=rom/045-v1.v1
VROM2=rom/045-v2.v2
VROM=$(VROM1) $(VROM2)
CROM1=rom/045-c1.c1
CROM2=rom/045-c2.c2
CROM3=rom/045-c3.c3
CROM4=rom/045-c4.c4
CROM5=rom/045-c51.c5
CROM6=rom/045-c61.c6
CROM=$(CROM1) $(CROM2) $(CROM3) $(CROM4) $(CROM5) $(CROM6)

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM) $(SROM) $(VROM) $(MROM) | rom

OBJS=overflow main
ELF=rom.elf
FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

# Enable multiple .text segments explicitly
$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit` -Wl,--enable-non-contiguous-regions

%.o: %.c
	$(M68KGCC) `pkg-config --cflags ngdevkit` -std=c99 -fomit-frame-pointer -g -c $< -o $@


# sound driver ROM: ngdevkit's nullsound
MROMSIZE:=131072
$(MROM): | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(NULLSOUNDDIR)/nullsound.ihx $@ --pad-to $(MROMSIZE)

# sample ROM: empty
VROMSIZE:=2097152
$(VROM): | rom
	dd if=/dev/zero bs=$(VROMSIZE) count=1 of=$@

# sprite ROM: empty
CROMSIZE1234:=2097152
$(CROM1) $(CROM2) $(CROM3) $(CROM4): | rom
	touch $@ && $(TRUNCATE) -s $(CROMSIZE1234) $@

CROMSIZE56:=1048576
$(CROM5) $(CROM6): | rom
	touch $@ && $(TRUNCATE) -s $(CROMSIZE56) $@

# fixed tile ROM: fonts from common assets
SROMSIZE:=131072
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@

# program ROM
PROMSIZE:=1048576
# We generate two different ROM files out of a single ELF
# The first ROM file only contains what was linked to segment .text
$(PROM1): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .text2 -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

# The first ROM file only contains what was linked to segment .text2
$(PROM2): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -j .text2 --gap-fill 0xff --pad-to $$((0x200000+$(PROMSIZE))) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

clean:
	rm -f *.png *.o *~ $(ELF) tmp.* rom/*.*

.PHONY: clean
