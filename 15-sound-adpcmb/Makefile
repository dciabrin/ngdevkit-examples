# Copyright (c) 2015-2023 Damien Ciabrini
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

# Override default to a ROM cartridge layout that supports
# both ADPCM-A and ADPCM-B in the same VROM chip, as all
# the recent carts can demultiplex accesses from the YM2610
# to a common ROM chip.
#
GAMEROM=lastbld2
MROMSIZE:=131072
MROM=rom/243-m1.m1
SROMSIZE:=262144
SROM=rom/243-s1.s1
PROMSIZE:=1048576
PROM2SIZE:=4194304
PROM1=rom/243-pg1.p1
PROM2=rom/243-pg2.sp2
PROM=$(PROM1) $(PROM2)
VROMSIZE=4194304
VROM1=rom/243-v1.v1
VROM2=rom/243-v2.v2
VROM3=rom/243-v3.v3
VROM4=rom/243-v4.v4
VROM=$(VROM1) $(VROM2) $(VROM3) $(VROM4)
CROMSIZE:=524288
CROM1=rom/243-c1.c1
CROM2=rom/243-c2.c2
CROM3=rom/243-c3.c3
CROM4=rom/243-c4.c4
CROM5=rom/243-c5.c5
CROM6=rom/243-c6.c6
CROM=$(CROM1) $(CROM2) $(CROM3) $(CROM4) $(CROM5) $(CROM6)

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM) $(SROM) $(VROM) $(MROM) | rom

OBJS=main
ELF=rom.elf
Z80_OBJS=user_commands
HEX=snd_driver.ihx
SAMPLES=beat dance
FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)


# MROM: Build a custom sound driver for the example
# it consists is the nullsound library, plus specific
# functions to trigger the configured actions (play samples)
#
$(HEX):	user_commands.rel
	$(Z80SDLD) -b DATA=0xf800 -i $@ $(NGDKZ80LIBDIR)/nullsound.lib $^

user_commands.rel: user_commands.s
	$(Z80SDAS) -g -l -p -u -I$(NGDKZ80INCLUDEDIR)/nullsound -o $@ $<

MROMSIZE:=131072
$(MROM): $(HEX) | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(HEX) $@ --pad-to $(MROMSIZE)




# sample ROM
#
# trim and convert input sounds to expected format
samples/beat.wav: sfx/hip-hop-beat-n2-49252.mp3 | samples
	$(SOX) -V1 $< -c 1 $@ reverse trim 0 2.290 reverse pad 0 0.41 speed 0.996555118
samples/dance.wav: sfx/insect-dance-loop-93865.mp3 | samples
	$(SOX) -V1 $< -c 1 $@ reverse trim 0 3.37 reverse speed 0.999802215

samples/%.adpcmb: samples/%.wav | samples
	$(ADPCMTOOL) -b -e $< -o $@ -v

$(VROM1): | rom
	touch $@ && $(TRUNCATE) -s $(VROMSIZE) $@
$(VROM2): $(SAMPLES:%=samples/%.adpcmb) | rom
	cat $(SAMPLES:%=samples/%.adpcmb) > $@ && $(TRUNCATE) -s $(VROMSIZE) $@
$(VROM3) $(VROM4): | rom
	touch $@ && $(TRUNCATE) -s $(VROMSIZE) $@

# sprite ROM C1 C2: unused, common assets
$(CROM1): $(ASSETS)/rom/c1.bin | rom
	cp $(ASSETS)/rom/c1.bin	$@ && $(TRUNCATE) -s $(CROMSIZE) $@

$(CROM2): $(ASSETS)/rom/c2.bin | rom
	cp $(ASSETS)/rom/c2.bin	$@ && $(TRUNCATE) -s $(CROMSIZE) $@
$(CROM3) $(CROM4) $(CROM5) $(CROM6): | rom
	touch $@ && $(TRUNCATE) -s $(CROMSIZE) $@


# fixed tile ROM: fonts from common assets
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@
	touch rom/vs-bios.rom && truncate -s $(SROMSIZE) rom/vs-bios.rom


# program ROM
$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit`

%.o: %.c
	$(M68KGCC) $(NGCFLAGS) -std=gnu99 -fomit-frame-pointer -g -c $< -o $@

$(PROM1): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab
$(PROM2): | rom
	dd if=/dev/zero bs=$(PROM2SIZE) count=1 of=$@


clean:
	rm -rf samples *.gif *.png *.pal *.adpcmb *.o *~ $(ELF) *.lst *.sym *.rel *.ihx *.rst tmp.* *.def *.map rom

samples:
	mkdir $@

.PHONY: clean
