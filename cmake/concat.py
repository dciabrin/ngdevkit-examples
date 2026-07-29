#!/usr/bin/env python3
# Copyright (c) 2026 Florian Loitsch
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

"""Concatenate input files into an output file, optionally resized.

Portable replacement for `cat inputs > output && truncate -s SIZE output`
used by the Makefile-based build (macOS ships no `truncate`). With no
inputs and a size, produces a zero-filled file of that size.
"""

import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--size", type=int, default=None,
                        help="truncate or zero-extend the output to SIZE bytes")
    parser.add_argument("output")
    parser.add_argument("inputs", nargs="*")
    args = parser.parse_args()

    with open(args.output, "wb") as out:
        for name in args.inputs:
            with open(name, "rb") as f:
                while True:
                    chunk = f.read(1 << 20)
                    if not chunk:
                        break
                    out.write(chunk)
        if args.size is not None:
            out.truncate(args.size)


if __name__ == "__main__":
    main()
