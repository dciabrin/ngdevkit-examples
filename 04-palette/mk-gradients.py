#!/usr/bin/env python3
#
# Copyright (c) 2019 Damien Ciabrini
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
#


def rgb18_to_packed16(col):
    r, g, b = col
    # heuristic: set the dark bit only when the
    # LSB of all components is null. Just follow
    # the same rule when you design your palette
    # for your sprites and tiles, for them to
    # match when converted for the Neo Geo.
    darkbit = 1 if ((r & 1) + (g & 1) + (b & 1)) == 0 else 0
    r, g, b = r >> 1, g >> 1, b >> 1
    lsb = ((r & 1) << 2) | ((g & 1) << 1) | (b & 1)
    r, g, b = r >> 1, g >> 1, b >> 1
    packed = darkbit << 15 | lsb << 12 | r << 8 | g << 4 | b
    return packed


def pal_chunks(l):
    for i in range(0, len(l), 15):
        yield l[i:i+15]


def main():
    n = list(range(64))
    # 1 color every 16 is transparent, so a full gradient
    # is encoded with 4 full palettes + 4 colors
    r = [rgb18_to_packed16([x, 0, 0]) for x in n]
    g = [rgb18_to_packed16([0, x, 0]) for x in n]
    b = [rgb18_to_packed16([0, 0, x]) for x in n]
    w = [rgb18_to_packed16([x, x, x]) for x in n]
    for chunk in pal_chunks(r[:-4]+g[:-4]+b[:-4]+w[:-4]):
        hexa = ["0x%04x" % c for c in chunk]
        print("{0x0000, %s}," % (", ".join(hexa)))


if __name__ == '__main__':
    main()
