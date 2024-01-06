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

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM1) $(CROM2) $(SROM) $(VROM) $(MROM) | rom

OBJS=main
ELF=rom.elf
Z80_OBJS=user_commands
HEX=snd_driver.ihx
SAMPLES=woosh lefthook lightbulbbreaking
FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)


# MROM: Build a custom nullsound driver for the example
# it consists is the nullsound template, plus specific
# functions to trigger the configured actions (play samples)
#
$(HEX):	user_commands.rel
	$(Z80SDLD) -b DATA=0xf800 -i $@ $(NGDKZ80LIBDIR)/nullsound.lib $^

user_commands.rel: user_commands.s samples.inc
	$(Z80SDAS) -g -l -p -u -I$(NGDKZ80INCLUDEDIR)/nullsound -o $@ $<

MROMSIZE:=131072
$(MROM): $(HEX) | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(HEX) $@ --pad-to $(MROMSIZE)



# sample ROM
#
# trim and convert input sounds to expected format
samples/woosh.wav: sfx/woosh.mp3 | samples
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.01 0.1% reverse silence 1 0.05 0.5% reverse
samples/lefthook.wav: sfx/lefthook.mp3 | samples
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.1 1% reverse silence 1 0.2 1% reverse
samples/lightbulbbreaking.wav: sfx/lightbulbbreaking.mp3 | samples
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.1 1% reverse silence 1 0.1 1% reverse

samples/%.adpcma: samples/%.wav | samples
	$(ADPCMTOOL) -e $< -o $@ -v

# ADPCM sample ROM
VROMSIZE=524288
$(VROM): samples-map.yaml samples.inc | rom
	$(VROMTOOL) --roms -s $(VROMSIZE) $< -o $(VROM)

samples.inc: samples-map.yaml $(SAMPLES:%=samples/%.adpcma)
	$(VROMTOOL) --asm -s $(VROMSIZE) $< -o $(VROM) -m $@


# sprite ROM C1 C2: unused, common assets
CROMSIZE:=1048576
$(CROM1): $(ASSETS)/rom/c1.bin | rom
	cp $(ASSETS)/rom/c1.bin	$@ && $(TRUNCATE) -s $(CROMSIZE) $@

$(CROM2): $(ASSETS)/rom/c2.bin | rom
	cp $(ASSETS)/rom/c2.bin	$@ && $(TRUNCATE) -s $(CROMSIZE) $@


# fixed tile ROM: fonts from common assets
SROMSIZE:=131072
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@


# program ROM
$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit`

%.o: %.c
	$(M68KGCC) $(NGCFLAGS) -std=gnu99 -fomit-frame-pointer -g -c $< -o $@

PROMSIZE:=524288
$(PROM): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab


clean:
	rm -rf samples samples.inc *.adpcma *.o *~ $(ELF) *.def *.lst *.sym *.rel *.ihx *.rst tmp.* rom

samples:
	mkdir $@

.PHONY: clean
