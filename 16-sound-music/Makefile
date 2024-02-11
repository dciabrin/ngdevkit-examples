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


# music modules for this demo
MUSICS=music1
MUSIC1=starship-battle
music1-nss.s music1-instruments.s music1-map.yaml: music/$(MUSIC1).fur

# samples for this demo
ADPCMA_SAMPLES=laser
ADPCMB_SAMPLES=

# the sample map for this demo merges samples from
# sound effects, and samples from the Furnace module
SAMPLE_MAPS=$(MUSICS) sfx

OBJS=main
ELF=rom.elf
Z80_OBJS=user_commands $(MUSICS:%=%-instruments) $(MUSICS:%=%-nss)
HEX=snd_driver.ihx

FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)


# MROM: Build a custom sound driver for the example
# it consists is the nullsound library, plus specific
# functions to trigger the configured actions (play samples)
#
$(HEX): $(Z80_OBJS:%=%.rel) samples.inc
	$(Z80SDLD) -b DATA=0xf800 -m1 -i $@ $(NGDKZ80LIBDIR)/nullsound.lib $(Z80_OBJS:%=%.rel)

# instruments source files require sample map offsets
# note: make sound-related generated files explicit targets so
# make do not consider them as intermediate files and delete
# them at the end of the run
%-instruments.rel: %-instruments.s %-nss.s samples.inc

%-instruments.s:
	$(FURTOOL) $^ --instruments -n $(@:%-instruments.s=%_instruments) -m samples.inc -o $@

%-nss.s:
	$(NSSTOOL) -v $< -n $(@:%-nss.s=%_nss_stream) -o $@

$(MUSICS:%=%-map.yaml):
	$(FURTOOL) $< --samples -o $@

%.rel: %.s
	$(Z80SDAS) -g -l -p -u -I$(NGDKZ80INCLUDEDIR)/nullsound -o $@ $<

user_commands.s: $(MUSICS:%=%-nss.s)

MROMSIZE:=131072
$(MROM): $(HEX) | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(HEX) $@ --pad-to $(MROMSIZE)



# sample ROM
#
# trim and convert input sounds to expected format
sfx/laser.wav: sfx/laser-gun-72558.mp3
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.01 0.1% reverse silence 1 0.05 0.5% reverse trim 0 0.6

sfx/%.adpcma: sfx/%.wav
	$(ADPCMTOOL) -e $< -o $@ -v

VROMSIZE=2097152

# only target VROM1, vromtool generates both VROM1 and VROM2
# TODO check if we need to depend on samples.inc
$(VROM1): $(SAMPLE_MAPS:%=%-map.yaml) samples.inc | rom
	$(VROMTOOL) --roms -s $(VROMSIZE) $(SAMPLE_MAPS:%=%-map.yaml) -o $(VROMTEMPLATE) -n 2

samples.inc: $(SAMPLE_MAPS:%=%-map.yaml)
	$(VROMTOOL) --asm -s $(VROMSIZE) $^ -o $(VROMTEMPLATE) -m $@

# convert samples to ADPCM before vromtool consumes the sfx map
sfx-map.yaml: $(ADPCMA_SAMPLES:%=sfx/%.adpcma) $(ADPCMB_SAMPLES:%=sfx/%.adpcmb)

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
	rm -rf *.gif *.png *.pal sfx/*.wav sfx/*.adpcma *.o *~ $(ELF) *.lst *.sym music*-map.yaml music*-instruments.s music*-nss.s samples.inc *.rel *.ihx *.rst tmp.* rom *.def *.map

.PHONY: clean
