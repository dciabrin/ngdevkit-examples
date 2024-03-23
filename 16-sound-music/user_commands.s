;;;
;;; Copyright (c) 2020-2024 Damien Ciabrini
;;; This file is part of ngdevkit-examples
;;;
;;; ngdevkit is free software: you can redistribute it and/or modify
;;; it under the terms of the GNU Lesser General Public License as
;;; published by the Free Software Foundation, either version 3 of the
;;; License, or (at your option) any later version.
;;;
;;; ngdevkit is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU Lesser General Public License for more details.
;;;
;;; You should have received a copy of the GNU Lesser General Public License
;;; along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.
;;;

;;; user-specific commands to link this ROM's sound driver
;;; ------------------------------------------------------
        .include "helpers.inc"
        .include "samples.inc"

        .area   CODE


;;; The ROM must provide a jump table to all sound commands
;;; The jump table includes command 01 and 03 which are
;;; implemented by nullsound
;;; The other commands are specific to this ROM
;;;

cmd_jmptable::
        ;; common/reserved sound commands
        jp      snd_command_unused
        jp      snd_command_01_prepare_for_rom_switch
        jp      snd_command_unused
        jp      snd_command_03_reset_driver
        ;; the command below can be anything, e.g. playing music...
        jp      play_music1
        jp      play_music2
        ;; ... or stopping music playback...
        jp      stop_music
        ;; ... or playing sample for sound FX...
        jp      sfx_laser
        init_unused_cmd_jmptable




stop_music::
        call    snd_stream_stop
        ret

play_music1::
        ld      bc, #music1_instruments
        ld      de, #music1_nss_stream
        call    snd_stream_play
        ret

;;; TODO: add another example stream for SSG
play_music2::
        ld      bc, #music2_instruments
        ld      de, #music2_nss_stream
        call    snd_stream_play
        ret

sfx_laser::
        ld      ix, #sfx_laser_data
        call    snd_adpcm_a_play
        ret


sfx_laser_data:
        .db     LASER_START_LSB, LASER_START_MSB ; start>>8 in VROM
        .db     LASER_STOP_LSB, LASER_STOP_MSB   ; stop>>8  in VROM
        .db     4                       ; channel 5
        .db     0xda                    ; l/r output + volume
        .db     16                      ; channel 3 (bit)

