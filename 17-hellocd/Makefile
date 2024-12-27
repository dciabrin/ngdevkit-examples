# Copyright (c) 2015-2024 Damien Ciabrini
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

all: chd nullbios

include ../Makefile.config

# TODO autodetect these in autoconf
MKISOFS=mkisofs
CHDMAN=chdman

# ROM names and common targets
NAME=hello
ISO_VOLUME=Hello CD
include ../Makefile.common

ISO_CONTENTS=$(PRG) $(FIX) $(Z80) $(ABS) $(BIB) $(CPY)
$(ISO): $(ISO_CONTENTS)

OBJS=main
ELF=rom.elf
FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin

$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit` --specs ngdevkit-cd

%.o: %.c
	$(M68KGCC) `pkg-config --cflags ngdevkit` -std=c99 -fomit-frame-pointer -g -c $< -o $@


# sound driver ROM: ngdevkit's nullsound
$(Z80):
	$(Z80SDOBJCOPY) -I ihex -O binary $(NGDKSHAREDIR)/nullsound_driver.ihx $@

# fixed tile ROM: fonts from common assets
$(FIX): $(FIX_ASSETS)
	cat $(FIX_ASSETS) > $@

# program ROM
$(PRG): $(ELF)
	$(M68KOBJCOPY) -O binary -S -R .comment $< $@

clean:
	rm -f *.o *~ $(ELF) $(ISO) $(CHD) $(ISO_CONTENTS) tmp.*

.PHONY: clean
