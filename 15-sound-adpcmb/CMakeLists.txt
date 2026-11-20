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

# the original music loops for this demo are mp3-encoded and need to get
# trimmed and resampled, hence these prepare steps
add_custom_command(OUTPUT ${gen}/assets/beat.wav
  COMMAND ${NGDK_SOX} -V1 ${sfx}/hip-hop-beat-n2-49252.mp3 -c 1 ${gen}/assets/beat.wav
          reverse trim 0 2.290 reverse pad 0 0.41 speed 0.996555118
  DEPENDS ${sfx}/hip-hop-beat-n2-49252.mp3
  COMMENT "Preparing beat.wav" VERBATIM)
add_custom_command(OUTPUT ${gen}/assets/dance.wav
  COMMAND ${NGDK_SOX} -V1 ${sfx}/insect-dance-loop-93865.mp3 -c 1 ${gen}/assets/dance.wav
          reverse trim 0 3.37 reverse speed 0.999802215
  DEPENDS ${sfx}/insect-dance-loop-93865.mp3
  COMMENT "Preparing dance.wav" VERBATIM)

# sample ROM and the matching offsets for the sound driver
ngdevkit_vrom_from_yaml(VROM
  GAMEROM     15_adpcm_b
  YAML        ${CMAKE_CURRENT_SOURCE_DIR}/assets/samples-map.yaml
  SAMPLES_INC assets/samples.inc
  STAGED      ${gen}/assets/beat.wav ${gen}/assets/dance.wav
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

ngdevkit_elf(15-sound-adpcmb-rom
  SOURCES main.c
)

ngdevkit_cartridge(
  GAMEROM      15_adpcm_b
  TITLE        "ADPCM-B sound FX API"
  ELF          15-sound-adpcmb-rom
  SOUND_DRIVER ${SOUND_DRIVER}
  VROM         ${VROM}
)
