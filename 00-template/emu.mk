# Emulator targets
#
# regular case: pass the rompath to the emulator

SCALE_WIN?=3
SCALE_FULL?=5

MAME?=mame
GNGEO?=ngdevkit-gngeo

#
# GNGEO CONFIGURATIONS
#

ifeq ($(GNGEO_GLSL), yes)
SHADEROPTS=-b glsl
ifneq ($(ENABLE_MINGW),yes)
ifneq ($(SHADER_PATH),)
SHADEROPTS+= --shaderpath="$(SHADER_PATH)"
endif
endif
ifneq ($(SHADER),)
SHADEROPTS+= --shader="$(SHADER)"
endif
else
SHADEROPTS=
endif

ifeq ($(FULLSCREEN),)
GNGEO_RESOLUTION_FLAGS=--scale $(SCALE_WIN)
$(call export_path,gngeo)
else
GNGEO_RESOLUTION_FLAGS=--scale $(SCALE_FULL)
endif

$(call export_path,gngeo)
gngeo gngeo-aes:
	$(GNGEO) $(SHADEROPTS) $(EXTRAOPTS) $(GNGEO_RESOLUTION_FLAGS) --no-resize --system home -i $(ROM) -d $(ROM)/gngeo_data.zip $(GAMEROM)

$(call export_path,gngeo-mvs)
gngeo-mvs:
	$(GNGEO) $(SHADEROPTS) $(EXTRAOPTS) $(GNGEO_RESOLUTION_FLAGS) --no-resize --system arcade -i $(ROM) -d $(ROM)/gngeo_data.zip $(GAMEROM)

.PHONY: gngeo gngeo-aes gngeo-mvs


#
# MAME CONFIGURATIONS
#

# MAME_MEMCARD_FLAGS=-memc memcard.mc

ifeq ($(FULLSCREEN),)
MAME_RESOLUTION_FLAGS=-w -resolution $(shell echo $$(($(SCALE_WIN)*320))x$$(($(SCALE_WIN)*224)))
$(call export_path,gngeo)
else
MAME_RESOLUTION_FLAGS=-now -resolution $(shell echo $$(($(SCALE_FULL)*320))x$$(($(SCALE_FULL)*224)))
endif

mame-mvs:
	$(MAME) $(MAME_RESOLUTION_FLAGS) -noautosave -skip_gameinfo -hash $(ROM) -rp $(ROM) neogeo $(MAME_MEMCARD_FLAGS) -cart1 $(GAMEROM:%.zip=%)

mame mame-aes:
	$(MAME) $(MAME_RESOLUTION_FLAGS) -noautosave -skip_gameinfo -hash $(ROM) -rp $(ROM) aes $(MAME_MEMCARD_FLAGS) -cart $(GAMEROM:%.zip=%)


.PHONY: mame mame-aes mame-mvs
