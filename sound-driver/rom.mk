# -*- makefile -*-
# TODO tool to generate those fields 
# TODO move the build name outside of this makefile

BUILDDIR?=build
ROM?=$(BUILDDIR)/rom

# ROM name
GAMEROM=shadowgangs

# program ROM
PROMSIZE=1048576
PROM1=$(ROM)/shadowgangs-p1.p1
# For 2MB PROM, define PROM2 and its targets in your makefile
PROM2=$(ROM)/shadowgangs-pg2.sp2
# use variable PROM2SIZE to build PROM2 as a banked ROM, i.e
# the concatenation of all the banked ELF files built in your
# project. By default, PROM2 only contains the higher half of
# the 2MB code segment build from your ELF file.
PROM2SIZE=2097152

# sprite ROM
CROMSIZE=16777216
CROM1=$(ROM)/shadowgangs-c1.c1
CROM2=$(ROM)/shadowgangs-c2.c2
# use variable CROMxSIZE to customize the size of a particular ROM
# by default, CROMSIZE value is used for every CROM
# CROM2SIZE=1048576

# fixed tile ROM
SROMSIZE=131072
SROM1=$(ROM)/shadowgangs-s1.s1

# sound driver ROM
MROMSIZE=524288
MROM1=$(ROM)/shadowgangs-m1.m1

# sound FX ROM
# VROMSIZE=2097152
VROMSIZE=16777216
VROM1=$(ROM)/shadowgangs-v1.v1
# VROM2=$(ROM)/shadowgangs-v2.v2
# use variable VROMTEMPLATE below to build your music and SFX
# assets with vromtool instead of defining dependencies manually
# in makefile. Se build.mk for more details
VROMTEMPLATE=$(ROM)/shadowgangs-vX.vX
