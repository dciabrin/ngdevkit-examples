# Copyright (c) 2018 Damien Ciabrini
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

all: rom/s1-shadow.bin rom/s1.bin rom/c1.bin

include ../Makefile.config

smalltext.png:
	$(CONVERT) gfx/unscii8.png -crop 256x24+2+1 +repage -crop 8x8 +append +repage -background black -gravity east -extent 1024x8 -fuzz 30% -fill white -opaque white -colors 2 PNG8:smalltext.png

talltext.png:
	$(CONVERT) gfx/unscii16.png -crop 256x48+2+1 +repage -crop 8x16 +append +repage -background black -gravity east -extent 1024x16 -fuzz 30% -fill white -opaque white -colors 2 PNG8:talltext.png

smalltext-shadow.png: smalltext.png
	$(CONVERT) -size 1024x8 xc:black \( $< -transparent black -fuzz 30% -fill '#0000ff' -opaque white \) -geometry +1+1 -composite  \( $< -transparent black -fuzz 30% -fill '#ffffff' -opaque white -geometry +0+0 \) -composite -colors 16 PNG8:$@

talltext-shadow.png: talltext.png
	$(CONVERT) -size 1024x16 xc:black \( $< -transparent black -fuzz 30% -fill '#0000ff' -opaque white \) -geometry +1+1 -composite  \( $< -transparent black -fuzz 30% -fill '#ffffff' -opaque white -geometry +0+0 \) -composite -colors 16 PNG8:$@

srom.bmp: smalltext.png talltext.png
	./build-srom.py -s smalltext.png -t talltext.png -b talltext.png -o srom.bmp

srom-shadow.bmp: smalltext-shadow.png talltext-shadow.png talltext.png
	./build-srom.py -s smalltext-shadow.png -t talltext-shadow.png -b talltext.png -o srom-shadow.bmp

rom/s1.bin: srom.bmp | rom
	$(TILETOOL) --fix -c srom.bmp -o rom/s1.bin

rom/s1-shadow.bin: srom-shadow.bmp | rom
	$(TILETOOL) --fix -c srom-shadow.bmp -o rom/s1-shadow.bin

rom/c1.bin rom/c2.bin: | rom
	$(TILETOOL) --sprite -c gfx/logo.png -o tmp.c1 tmp.c2; \
	(dd bs=64 count=14 if=tmp.c1; dd bs=64 skip=15 if=tmp.c1; dd bs=64 count=197 if=/dev/zero) 2>/dev/null | cat >rom/c1.bin; \
	(dd bs=64 count=14 if=tmp.c2; dd bs=64 skip=15 if=tmp.c2; dd bs=64 count=197 if=/dev/zero) 2>/dev/null | cat >rom/c2.bin

clean:
	rm -f *.png tmp.* *.bmp *~
	rm -rf rom

rom:
	mkdir -p rom

.PHONY: all clean
