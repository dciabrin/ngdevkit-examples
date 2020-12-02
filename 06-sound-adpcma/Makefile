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
Z80_NULLSOUND_OBJS=nullsound
Z80_USER_OBJS=user_commands
HEX=driver.ihx
SAMPLES=woosh lefthook lightbulbbreaking

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)


# MROM: Build a custom nullsound driver for the example
# it consists is the nullsound template, plus specific
# functions to trigger the configured actions (play samples)
#
$(HEX):	nullsound.rel user_commands.rel
	$(Z80SDLD) -i $@ -u $^

user_commands.rel: user_commands.s
	$(Z80SDAS) -g -l -p -u -I$(NULLSOUNDSRCDIR) -o $@ $<

nullsound.rel: $(NULLSOUNDSRCDIR)/nullsound.s
	$(Z80SDAS) -g -l -p -u -I$(NULLSOUNDSRCDIR) -o $@ $<

$(MROM): $(HEX) | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(HEX) $@ --pad-to 131072



# sample ROM
#
# trim and convert input sounds to expected format
samples/woosh.wav: sfx/woosh.mp3 | samples
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.01 0.1% reverse silence 1 0.07 1% reverse
samples/lefthook.wav: sfx/lefthook.mp3 | samples
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.1 1% reverse silence 1 0.2 1% reverse
samples/lightbulbbreaking.wav: sfx/lightbulbbreaking.mp3 | samples
	$(SOX) -V1 $< -c 1 -r 18500 $@ silence 1 0.1 1% reverse silence 1 0.1 1% reverse

samples/%.adpcma: samples/%.wav | samples
	$(ADPCMTOOL) -e $< -o $@ -v

$(VROM): $(SAMPLES:%=samples/%.adpcma) | rom
	cat $(SAMPLES:%=samples/%.adpcma) > $@ && $(TRUNCATE) -s 524288 $@


# sprite ROM C1 C2: unused, common assets
$(CROM1): $(ASSETS)/rom/c1.bin | rom
	cp $(ASSETS)/rom/c1.bin	$@ && $(TRUNCATE) -s 1048576 $@

$(CROM2): $(ASSETS)/rom/c2.bin | rom
	cp $(ASSETS)/rom/c2.bin	$@ && $(TRUNCATE) -s 1048576 $@


# fixed tile ROM: fonts from common assets
$(SROM): $(ASSETS)/rom/s1.bin | rom
	cp $(ASSETS)/rom/s1.bin	$@ && $(TRUNCATE) -s 131072 $@


# program ROM
$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) `pkg-config --libs ngdevkit` $^ -o $@

%.o: %.c
	$(M68KGCC) $(NGCFLAGS) -std=gnu99 -fomit-frame-pointer -g -c $< -o $@

$(PROM): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment $< $@ && dd if=$@ of=$@ conv=notrunc,swab


clean:
	rm -rf samples *.gif *.png *.pal *.adpcma *.o *~ $(ELF) *.lst *.sym *.rel *.ihx *.rst tmp.* rom

samples:
	mkdir $@

.PHONY: clean
