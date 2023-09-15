# Copyright (c) 2020-2022 Damien Ciabrini
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
# and the second P-ROM being used for bankswitching.
GAMEROM=pulstar
MROM=rom/089-m1.m1
SROM=rom/089-s1.s1
PROM1=rom/089-p1.p1
PROM2=rom/089-p2.sp2
PROM=$(PROM1) $(PROM2)
VROM1=rom/089-v1.v1
VROM2=rom/089-v2.v2
VROM=$(VROM1) $(VROM2)
CROM1=rom/089-c1.c1
CROM2=rom/089-c2.c2
CROM3=rom/089-c3.c3
CROM4=rom/089-c4.c4
CROM5=rom/089-c5.c5
CROM6=rom/089-c6.c6
CROM7=rom/089-c7.c7
CROM8=rom/089-c8.c8
CROM=$(CROM1) $(CROM2) $(CROM3) $(CROM4) $(CROM5) $(CROM6) $(CROM7) $(CROM8)

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM) $(SROM) $(VROM) $(MROM) | rom

OBJS_COMMON=\
  main

# All files whose path or name contains '__bank' get automatically
# linked into the P-ROM2 address space.
OBJS_0=\
  $(OBJS_COMMON) \
  impl__bank0

OBJS_1=\
  $(OBJS_COMMON) \
  impl__bank1

ELF_0=rom0.elf
ELF_1=rom1.elf

FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

# Enable multiple .text segments explicitly
$(ELF_0): $(OBJS_0:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit` -Wl,--enable-non-contiguous-regions

$(ELF_1): $(OBJS_1:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit` -Wl,--enable-non-contiguous-regions

%.o: %.c
	$(M68KGCC) `pkg-config --cflags ngdevkit` -I. -std=c99 -fomit-frame-pointer -g -c $< -o $@

# sound driver ROM: ngdevkit's nullsound
MROMSIZE:=131072
$(MROM): | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(NGDKSHAREDIR)/nullsound_driver.ihx $@ --pad-to $(MROMSIZE)

# sample ROM: empty
VROMSIZE:=4194304
$(VROM1) $(VROM2): | rom
	dd if=/dev/zero bs=$(VROMSIZE) count=1 of=$@

# sprite ROM C1 C2: unused, common assets
CROMSIZE16:=4194304
CROMSIZE78:=2097152
$(CROM1): $(ASSETS)/rom/c1.bin | rom
	cp $(ASSETS)/rom/c1.bin	$@ && $(TRUNCATE) -s $(CROMSIZE16) $@

$(CROM2): $(ASSETS)/rom/c2.bin | rom
	cp $(ASSETS)/rom/c2.bin	$@ && $(TRUNCATE) -s $(CROMSIZE16) $@

$(CROM3) $(CROM4) $(CROM5) $(CROM6): | rom
	touch $@ && $(TRUNCATE) -s $(CROMSIZE16) $@

$(CROM7) $(CROM8): | rom
	touch $@ && $(TRUNCATE) -s $(CROMSIZE78) $@

# fixed tile ROM: fonts from common assets
SROMSIZE:=131072
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@

# program ROMs
PROMSIZE:=1048576
# We generate two different ROM files out two ELF programs which
# share a common part.
# The first ROM file only contains what was linked to segment .text
# this is common in both ELF files
$(PROM1): $(ELF_0) | rom
	$(M68KOBJCOPY) -O binary -S -R .text2 -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

# The second ROM is the concatenation of all .text2 segments
# from all ELF files. It is a series of 1MB chunks, that are
# bankswitched at runtime.
$(PROM2:%.sp2=%)_bank0: $(ELF_0) | rom
	$(M68KOBJCOPY) -O binary -j .text2 --gap-fill 0xff --pad-to $$((0x200000+$(PROMSIZE))) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

$(PROM2:%.sp2=%)_bank1: $(ELF_1) | rom
	$(M68KOBJCOPY) -O binary -j .text2 --gap-fill 0xff --pad-to $$((0x200000+$(PROMSIZE))) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

$(PROM2): $(PROM2:%.sp2=%)_bank0 $(PROM2:%.sp2=%)_bank1 | rom
	cat $^ > $@

clean:
	rm -f *.o *~ $(ELF_0) $(ELF_1) tmp.* rom/*.*

.PHONY: clean
