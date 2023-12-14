# Example programs for ngdevkit, the open source SDK for Neo-Geo


ngdevkit-examples is a collection of Neo-Geo programs that can be
built with [ngdevkit](ngdevkit), the open source software development
kit for the Neo-Geo hardware.

The examples in this repository go from a simple hello world, to
various aspects of the Neo-Geo hardware, such as palette handling,
tiles and sprites, vertical blank and interrupt handling...

With ngdevkit-examples you will learn what it takes to run a C or
assembler program on the Neo-Geo hardware, how to compile your
program with ngdevkit, and how to use the various tools and API
provided by ngdevkit.

For additional retro points, ngdevkit-examples provides a
GnGeo configuration that enables GLSL pixel shader rendering and
comes with a simple and fast CRT scanline shader :P


## How to compile the examples

### Installing packages

Prior to using ngdevkit-examples, you must install
[ngdevkit](ngdevkit) and a couple of additional packages on your
system. If you're running on Ubuntu or Debian, you can install debian
packages of ngdevkit and GnGeo with:

    add-apt-repository -y ppa:dciabrin/ngdevkit
    apt-get update
    apt-get install -y ngdevkit ngdevkit-gngeo
    # the remaining packages are only required for the examples
    apt-get install -y pkg-config autoconf automake zip imagemagick sox libsox-fmt-mp3

If you are running a Fedora distribution, pre-built packages are
available in COPR, and can be installed with the following commands:

    dnf copr enable dciabrin/ngdevkit
    dnf install ngdevkit ngdevkit-gngeo
    # the remaining packages are only requred for the examples
    dnf install install pkg-config autoconf zip ImageMagick sox

If you're running on macOS, you can install pre-built [brew][brew]
packages, available in the ngdevkit tap:

    # If you haven't done it yet, make sure XCode is installed first
    sudo xcode-select --install
    brew tap dciabrin/ngdevkit
    brew install ngdevkit ngdevkit-gngeo
    # make sure you use brew's python3 in your shell
    export PATH=/usr/local/opt/python3/bin:$PATH
    pip3 install pygame
    # the remaining packages are only required for the examples
    brew install pkg-config autoconf automake zip imagemagick sox

If you're running on Windows 10, you can use native pre-built packages
for the [MSYS2][msys2] environment, available for the ucrt64 subsystem.
To use them, start a MSYS2 shell for ucrt64, and configure the packages
as follows:

    MSYSTEM=UCRT64 /usr/bin/bash -l
    echo -e "\n[ngdevkit]\nSigLevel = Optional TrustAll\nServer = https://dciabrin.net/msys2-ngdevkit/\$arch" >> /etc/pacman.conf
    # install pacboy with `pacman -S pactoys` if necessary
    pacboy -Sy
    pacboy -S ngdevkit:u ngdevkit-gngeo:u
    # the remaining packages are only required for the examples
    pacboy -S autoconf automake make zip imagemagick:u sox:u

If you can't install pre-built binary packages, you can clone the
[ngdevkit](ngdevkit) git repository and follow the build instructions:

    git clone https://github.com/dciabrin/ngdevkit
    # build the devkit according to your system
    # once done, add ngdevkit in your path with:
    eval $(make shellinit)


### Compiling examples

Once everything is installed and ngdevkit is available in your
path, you can prepare the compilation with:

    # import the GLSL CRT shader for GnGeo
    git submodule update --init --recursive

And build all the examples with the following commands if you are running
Linux:

    autoreconf -iv
    ./configure
    make

For macOS, make sure you use brew's python3 and gmake:

    export PATH=$HOMEBREW_PREFIX/opt/python3/bin:$PATH
    autoreconf -iv
    ./configure
    gmake

For Windows, make sure you shell uses the ucrt64 subsystem. Older versions
of this repo assumed you used mingw64, which is now deprecated. Also,
python3 and pygame are now autoconfigured, so the examples can be
build with the following commands:

    # make sure you are using the UCRT64 subsystem
    MSYSTEM=UCRT64 /usr/bin/bash
    autoreconf -iv
    ./configure
    make


## Running the compiled examples

Once you have built the examples, go into a subdirectory to
test the compiled example and run GnGeo from the makefile:

    cd 01-helloworld
    make gngeo
    # or run "make gngeo-fullscreen" for a more immersive test

GnGeo will automatically enable the GLSL pixel shader if OpenGL
is available on your system and detected during the configure
step. For more info, look at the [common makefile](Makefile.common).

Note: If you're running a recent macOS, [System Integrity Protection][sip]
may prevent you from running GnGeo from make, so you may need to run
it from your terminal:

    eval $(gmake -n gngeo)


## Source-level debugging

ngdevkit provides a modified version of GnGeo which supports remote
debugging via GDB. You need to start GnGeo in debug mode and run
GDB to attach to it:

    cd 01-helloworld
    # start GnGeo in debug mode
    eval $(make -n gngeo) -D &
    m68k-neogeo-elf-gdb rom.elf

GDB uses the debug symbols from the ELF file rather than the ROM file
directly, so it knows all C source files, symbols and locations. This
enables source-level debugging of your C program in addition to usual
m68k ASM debugger.

Once GDB is started, connect to the emulator to start the the
debugging session. For example:

    # GnGeo listens on the standard GDB port 2159
    (gdb) target remote :2159
    Remote debugging using :2159
    0x00c04300 in ?? ()
    (gdb) b main.c:52
    Breakpoint 1 at 0x57a: file main.c, line 52.
    (gdb) c


## License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program. If not, see
<http://www.gnu.org/licenses/>.


[ngdevkit]: https://github.com/dciabrin/ngdevkit
[brew]: https://brew.sh
[sip]: https://support.apple.com/en-us/HT204899
[msys2]: https://www.msys2.org
[pywin]: https://www.python.org/downloads/windows
