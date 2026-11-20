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
set(assets ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(yamls ${assets}/music-map.yaml ${assets}/sample-map.yaml)

# the sound FX for this demo is mp3-encoded and needs to get trimmed;
# the sample-map.yaml references it as assets/sfx/laser.wav
add_custom_command(OUTPUT ${gen}/assets/sfx/laser.wav
  COMMAND ${CMAKE_COMMAND} -E make_directory ${gen}/assets/sfx
  COMMAND ${NGDK_SOX} -V1 ${CMAKE_CURRENT_SOURCE_DIR}/setup/prepare-sfx/sfx/laser-gun-72558.mp3
          -c 1 -r 18500 ${gen}/assets/sfx/laser.wav
          silence 1 0.01 0.1% reverse silence 1 0.05 0.5% reverse trim 0 0.6
  DEPENDS setup/prepare-sfx/sfx/laser-gun-72558.mp3
  COMMENT "Preparing laser.wav" VERBATIM)

# the music-map.yaml references the Furnace module as assets/musics/...,
# stage it next to the generated sound FX
add_custom_command(OUTPUT ${gen}/assets/musics/starship-battle.fur
  COMMAND ${CMAKE_COMMAND} -E copy ${assets}/musics/starship-battle.fur
          ${gen}/assets/musics/starship-battle.fur
  DEPENDS ${assets}/musics/starship-battle.fur
  VERBATIM)
set(staged ${gen}/assets/sfx/laser.wav ${gen}/assets/musics/starship-battle.fur)

# sample ROM and the matching offsets for the sound driver
ngdevkit_vrom_from_yaml(VROM
  GAMEROM     16_sound_music
  YAML        ${yamls}
  SAMPLES_INC samples.inc
  STAGED      ${staged}
)

# generate the list of sound commands, both as z80 code for the sound
# driver (snd_commands.inc) and as a C header for the m68k program
add_custom_command(OUTPUT ${gen}/snd_commands.inc
  COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_SOUNDTOOL} -v -z -s ${yamls} -o ${gen}/snd_commands.inc
  DEPENDS ${yamls} ${staged}
  WORKING_DIRECTORY ${gen}
  COMMENT "Generating snd_commands.inc" VERBATIM)
add_custom_command(OUTPUT ${gen}/snd_commands.h
  COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_SOUNDTOOL} -c -s ${yamls} -o ${gen}/snd_commands.h
  DEPENDS ${yamls} ${staged}
  WORKING_DIRECTORY ${gen}
  COMMENT "Generating snd_commands.h" VERBATIM)

# the Furnace module is converted into two z80 asm files: the music
# stream and the instruments
add_custom_command(OUTPUT ${gen}/nss/nss-starship-battle.s
  COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_NSSTOOL} -z ${assets}/musics/starship-battle.fur
          -n nss_starship_battle -o ${gen}/nss/nss-starship-battle.s
  DEPENDS ${assets}/musics/starship-battle.fur
  COMMENT "Generating nss-starship-battle.s" VERBATIM)
add_custom_command(OUTPUT ${gen}/nss/instruments-starship-battle.s
  COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_FURTOOL} ${assets}/musics/starship-battle.fur
          --instruments -n instruments_starship_battle -m ${gen}/samples.inc
          -o ${gen}/nss/instruments-starship-battle.s
  DEPENDS ${assets}/musics/starship-battle.fur ${gen}/samples.inc
  COMMENT "Generating instruments-starship-battle.s" VERBATIM)

# sound driver: nullsound + eye catcher music + the example's sound
# commands and music data
ngdevkit_z80_rel(USER_COMMANDS_REL user_commands.s
  DEPENDS ${gen}/samples.inc ${gen}/snd_commands.inc
)
ngdevkit_z80_rel(NSS_REL ${gen}/nss/nss-starship-battle.s
  OUTPUT nss/nss-starship-battle.rel
)
ngdevkit_z80_rel(INSTRUMENTS_REL ${gen}/nss/instruments-starship-battle.s
  OUTPUT nss/instruments-starship-battle.rel
)
ngdevkit_sound_driver(SOUND_DRIVER
  NAME    demo_driver
  INPUTS  ${NGDK_EYE_CATCHER_LIB} ${USER_COMMANDS_REL} ${NSS_REL} ${INSTRUMENTS_REL}
  DEPENDS ${NGDK_SHARED_ASSETS_TARGET}
)

# generate the list of available sound commands for the m68k program,
# extracted from the sound driver's jump table listing
set(awk [=[awk '/cmd_jmptable::/ {table=1; next} table==1 && /cmd_jmptable_padding/ {exit} table==1 && / C3 / {print "\""$8"\","} END {print "\"\""}']=])
add_custom_command(OUTPUT ${gen}/snd_commands_info.h
  COMMAND ${NGDK_SH} -c "cat '${gen}/user_commands.lst' | ${awk} | tr '_' ' ' > '${gen}/snd_commands_info.h'"
  DEPENDS ${USER_COMMANDS_REL}
  COMMENT "Generating snd_commands_info.h" VERBATIM)

ngdevkit_elf(16-sound-music-rom
  SOURCES   main.c
  GENERATED ${gen}/snd_commands.h ${gen}/snd_commands_info.h
)

ngdevkit_cartridge(
  GAMEROM      16_sound_music
  TITLE        "Music playback API"
  ELF          16-sound-music-rom
  SOUND_DRIVER ${SOUND_DRIVER}
  VROM         ${VROM}
)
