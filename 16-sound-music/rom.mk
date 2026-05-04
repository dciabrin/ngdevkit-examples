# -*- makefile -*-

BUILDDIR?=build
ROM?=$(BUILDDIR)/rom

# ROM name
GAMEROM?=ngdevkit

# program ROM
PROMSIZE=1048576
PROM1=$(ROM)/$(GAMEROM)-p1.p1
# For 2MB PROM, define PROM2 and its targets in your makefile
# PROM2=$(ROM)/$(GAMEROM)-p2.p2
# use variable PROM2SIZE to build PROM2 as a banked ROM, i.e
# the concatenation of all the banked ELF files built in your
# project. By default, PROM2 only contains the higher half of
# the 2MB code segment build from your ELF file.
# PROM2SIZE=1048576

# sprite ROM
CROMSIZE=2097152
CROM1=$(ROM)/$(GAMEROM)-c1.c1
CROM2=$(ROM)/$(GAMEROM)-c2.c2
# use variable CROMxSIZE to customize the size of a particular ROM
# by default, CROMSIZE value is used for every CROM
# CROM2SIZE=1048576
# CROM3=$(ROM)/$(GAMEROM).c3
# CROM4=$(ROM)/$(GAMEROM).c4

# fixed tile ROM
SROMSIZE=131072
SROM1=$(ROM)/$(GAMEROM)-s1.s1

# sound driver ROM
MROMSIZE=131072
MROM1=$(ROM)/$(GAMEROM)-m1.m1

# sound FX ROM
VROMSIZE=524288
VROM1=$(ROM)/$(GAMEROM)-v1.v1
# use variable VROMTEMPLATE below to build your music and SFX
# assets with vromtool instead of defining dependencies manually
# in makefile. See build.mk for more details
VROMTEMPLATE=$(ROM)/$(GAMEROM)-vX.vX
