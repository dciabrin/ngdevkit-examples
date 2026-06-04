# -*- makefile -*-
# TODO tool to generate those fields
# TODO move the build name outside of this makefile

BUILDDIR?=build
ROM?=$(BUILDDIR)/rom

# ROM name
GAMEROM=puzzledp

# program ROM
PROMSIZE=524288
PROM1=$(ROM)/202-p1.p1
# For 2MB PROM, define PROM2 and its targets in your makefile
# PROM2=
# use variable PROM2SIZE to build PROM2 as a banked ROM, i.e
# the concatenation of all the banked ELF files built in your
# project. By default, PROM2 only contains the higher half of
# the 2MB code segment build from your ELF file.
# PROM2SIZE=4194304

# sprite ROM
CROMSIZE=1048576
CROM1=$(ROM)/202-c1.c1
# use variable CROMxSIZE to customize the size of a particular ROM
# by default, CROMSIZE value is used for every CROM
# CROM2SIZE=1048576
CROM2=$(ROM)/202-c2.c2

# fixed tile ROM
SROMSIZE=131072
SROM1=$(ROM)/202-s1.s1

# sound driver ROM
MROMSIZE=131072
MROM1=$(ROM)/202-m1.m1

# sound FX ROM
VROMSIZE=524288
VROM1=$(ROM)/202-v1.v1
# use variable VROMTEMPLATE below to build your music and SFX
# assets with vromtool instead of defining dependencies manually
# in makefile. Se build.mk for more details
# VROMTEMPLATE=$(ROM)/202-vX.vX
