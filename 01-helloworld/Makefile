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

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) `pkg-config --libs ngdevkit` $^ -o $@

%.o: %.c
	$(M68KGCC) `pkg-config --cflags ngdevkit` -std=c99 -fomit-frame-pointer -g -c $< -o $@


# sound driver ROM: ngdevkit's nullsound
$(MROM): | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(NULLSOUNDDIR)/nullsound.ihx $@ --pad-to 131072

# sample ROM: empty
$(VROM): | rom
	dd if=/dev/zero bs=1024 count=512 of=$@

# sprite ROM C1 C2: unused, common assets
$(CROM1): $(ASSETS)/rom/c1.bin | rom
	cp $(ASSETS)/rom/c1.bin	$@ && $(TRUNCATE) -s 1048576 $@

$(CROM2): $(ASSETS)/rom/c2.bin | rom
	cp $(ASSETS)/rom/c2.bin	$@ && $(TRUNCATE) -s 1048576 $@

# fixed tile ROM: fonts from common assets
$(SROM): $(ASSETS)/rom/s1.bin | rom
	cp $(ASSETS)/rom/s1.bin	$@ && $(TRUNCATE) -s 131072 $@

# program ROM
$(PROM): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --reverse-bytes=2 $< $@

clean:
	rm -f *.png *.o *~ $(ELF) tmp.* rom/*.*

.PHONY: clean
