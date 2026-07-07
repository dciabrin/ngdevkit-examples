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

set(gen ${CMAKE_CURRENT_BINARY_DIR}/gen)
set(sfx ${CMAKE_CURRENT_SOURCE_DIR}/setup/prepare-sfx/sfx)

# The original sound FX for this demo are mp3-encoded and need to get
# cropped to a specific time window, hence these prepare steps. The
# samples-map.yaml references the prepared files as assets/<name>.wav.
add_custom_command(OUTPUT ${gen}/assets/woosh.wav
  COMMAND ${NGDK_SOX} -V1 ${sfx}/woosh.mp3 -c 1 -r 18500 ${gen}/assets/woosh.wav
          silence 1 0.01 0.1% reverse silence 1 0.05 0.5% reverse
  DEPENDS ${sfx}/woosh.mp3
  COMMENT "Preparing woosh.wav" VERBATIM)
add_custom_command(OUTPUT ${gen}/assets/lefthook.wav
  COMMAND ${NGDK_SOX} -V1 ${sfx}/lefthook.mp3 -c 1 -r 18500 ${gen}/assets/lefthook.wav
          silence 1 0.1 1% reverse silence 1 0.2 1% reverse
  DEPENDS ${sfx}/lefthook.mp3
  COMMENT "Preparing lefthook.wav" VERBATIM)
add_custom_command(OUTPUT ${gen}/assets/lightbulbbreaking.wav
  COMMAND ${NGDK_SOX} -V1 ${sfx}/lightbulbbreaking.mp3 -c 1 -r 18500
          ${gen}/assets/lightbulbbreaking.wav
          silence 1 0.1 1% reverse silence 1 0.1 1% reverse
  DEPENDS ${sfx}/lightbulbbreaking.mp3
  COMMENT "Preparing lightbulbbreaking.wav" VERBATIM)

# sample ROM and the matching offsets for the sound driver
ngdevkit_vrom_from_yaml(VROM
  GAMEROM     06_sound_adpcma
  YAML        ${CMAKE_CURRENT_SOURCE_DIR}/assets/samples-map.yaml
  SAMPLES_INC assets/samples.inc
  STAGED      ${gen}/assets/woosh.wav ${gen}/assets/lefthook.wav
              ${gen}/assets/lightbulbbreaking.wav
)

# sound driver: nullsound + the example's sound commands
ngdevkit_z80_rel(USER_COMMANDS_REL user_commands.s
  DEPENDS ${gen}/assets/samples.inc
)
ngdevkit_sound_driver(SOUND_DRIVER
  NAME    demo_driver
  INPUTS  ${NGDK_EYE_CATCHER_LIB} ${USER_COMMANDS_REL}
  DEPENDS ${NGDK_SHARED_ASSETS_TARGET}
)

ngdevkit_elf(06-sound-adpcma-rom
  SOURCES main.c
)

ngdevkit_cartridge(
  GAMEROM      06_sound_adpcma
  TITLE        "ADPCM-A sound FX"
  ELF          06-sound-adpcma-rom
  SOUND_DRIVER ${SOUND_DRIVER}
  VROM         ${VROM}
)
