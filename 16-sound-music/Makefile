# Copyright (c) 2015-2020 Damien Ciabrini
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

# Override default to 2-VROM game
GAMEROM=fatfury2
MROM=rom/047-m1.m1
SROM=rom/047-s1.s1
PROM1=rom/047-p1.p1
PROM2=rom/047-p2.p2
PROM=$(PROM1) $(PROM2)
VROM1=rom/047-v1.v1
VROM2=rom/047-v2.v2
VROM=$(VROM1) $(VROM2)
VROMTEMPLATE=$(subst 1,X,$(VROM1))
CROM1=rom/047-c1.c1
CROM2=rom/047-c2.c2
CROM3=rom/047-c3.c3
CROM4=rom/047-c4.c4
CROM=$(CROM1) $(CROM2) $(CROM3) $(CROM4)

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM) $(SROM) $(VROM) $(MROM) | rom

# TODO remove hardcodes here
FURTOOL=./furtool.py
NSSTOOL=./nsstool.py

OBJS=main
ELF=rom.elf
FURMODS=starship_battle
Z80_OBJS=user_commands $(FURMODS:%=%_instruments)
ADPCMA_SAMPLES=laser
SAMPLE_MAPS=starship_battle sfx
HEX=snd_driver.ihx

FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)


# MROM: Build a custom sound driver for the example
# it consists is the nullsound library, plus specific
# functions to trigger the configured actions (play samples)
#
# All furnace mods for this demo ROM
$(HEX):	$(Z80_OBJS:%=%.rel)
	$(Z80SDLD) -b DATA=0xf800 -m1 -i $@ $(NGDKZ80LIBDIR)/nullsound.lib $^

%.rel: %.s
	$(Z80SDAS) -g -l -p -u -I$(NGDKZ80INCLUDEDIR)/nullsound -o $@ $<

%_instruments.s: music/%.fur samples.inc
	$(FURTOOL) $< --instruments -n $(<:music/%.fur=%)_instruments -m samples.inc -o $@

user_commands.s: $(FURMODS:%=%_nss.s)

%_nss.s: music/%.fur
	$(NSSTOOL) -v $< -o $@

MROMSIZE:=131072
$(MROM): $(HEX) | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(HEX) $@ --pad-to $(MROMSIZE)



# sample ROM
#
# trim and convert input sounds to expected format
samples/laser.wav: sfx/laser-gun-72558.mp3 | samples
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.01 0.1% reverse silence 1 0.05 0.5% reverse trim 0 0.6

samples/%.adpcma: samples/%.wav | samples
	$(ADPCMTOOL) -e $< -o $@ -v

VROMSIZE=2097152

# only target VROM1, vromtool generates both VROM1 and VROM2
# TODO check if we need to depend on samples.inc
$(VROM1): $(SAMPLE_MAPS:%=%_map.yaml) samples.inc | rom
	$(VROMTOOL) --roms -s $(VROMSIZE) $(SAMPLE_MAPS:%=%_map.yaml) -o $(VROMTEMPLATE) -n 2

samples.inc: $(SAMPLE_MAPS:%=%_map.yaml)
	$(VROMTOOL) --asm -s $(VROMSIZE) $^ -o $(VROMTEMPLATE) -m $@

%_map.yaml: music/%.fur
	$(FURTOOL) $< --samples -o $@

sfx_map.yaml: $(ADPCMA_SAMPLES:%=samples/%.adpcma)

# sprite ROM C1 C2: unused, common assets
CROMSIZE:=2097152
$(CROM1): $(ASSETS)/rom/c1.bin | rom
	cp $(ASSETS)/rom/c1.bin	$@ && $(TRUNCATE) -s $(CROMSIZE) $@

$(CROM2): $(ASSETS)/rom/c2.bin | rom
	cp $(ASSETS)/rom/c2.bin	$@ && $(TRUNCATE) -s $(CROMSIZE) $@
$(CROM3) $(CROM4): | rom
	touch $@ && $(TRUNCATE) -s $(CROMSIZE) $@


# fixed tile ROM: fonts from common assets
SROMSIZE:=131072
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@


# program ROM
$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit`

%.o: %.c
	$(M68KGCC) $(NGCFLAGS) -std=gnu99 -fomit-frame-pointer -g -c $< -o $@

PROMSIZE:=1048576
$(PROM1): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab
$(PROM2): | rom
	dd if=/dev/zero bs=$(PROMSIZE) count=1 of=$@


clean:
	rm -rf samples *.gif *.png *.pal *.adpcma *.o *~ $(ELF) *.lst *.sym $(FURMODS:%=%_map.yaml) samples.inc *.rel *.ihx *.rst tmp.* *_instruments.s *_nss.s rom *.def *.map

samples:
	mkdir $@

.PHONY: clean
