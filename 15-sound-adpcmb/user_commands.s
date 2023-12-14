;;;
;;; Copyright (c) 2020-2023 Damien Ciabrini
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
        .include "ym2610.inc"
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
        ;; sound commands for this ROM
        jp      play_beat_22
        jp      play_beat_16_5
        jp      play_beat_33
        jp      play_beat_44
        jp      play_dance_22
        jp      play_dance_16_5
        jp      play_dance_33
        jp      play_dance_44
        jp      loop_beat_22
        jp      loop_beat_16_5
        jp      loop_beat_33
        jp      loop_beat_44
        jp      loop_dance_22
        jp      loop_dance_16_5
        jp      loop_dance_33
        jp      loop_dance_44
        init_unused_cmd_jmptable


;;; This ROM contains sound commands that play an ADPCM-B
;;; sample at various playback speed and with/without looping.
;;;



;;; ADPCM-B sample data
;;;
;;; Note on sample speed computation, e.g. with adpcm_b_beat_44
;;;  . This config plays the ADPCM_B at 44100Hz
;;;  . YM2610 playback speed is expressed in Delta-N
;;;
;;; The relation between frequency in Hz and Delta-N is as follows:
;;;   frequency = 8M / 12 / 12 / (65535 / Delta-N)
;;;   frequency =     55555    * (Delta-N / 65535)
;;;   frequency = (55555/65535)* Delta-N
;;;   Delta-N   = frequency * (65536/55555)
;;;
;;; In our case, this gives:
;;;   Delta-N = 44100 * 65536 / 55555 = 52022 (0xcb36)
;;;

adpcm_b_beat_44:
        .dw     0x4000                  ; sample start addr >> 8
        .dw     0x407e                  ; sample stop addr >> 8
        .dw     0xcb6b                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume
adpcm_b_beat_33:
        .dw     0x4000                  ; sample start addr >> 8
        .dw     0x407e                  ; sample stop addr >> 8
        .dw     0x80e8                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume
adpcm_b_beat_22:
        .dw     0x4000                  ; sample start addr >> 8
        .dw     0x407e                  ; sample stop addr >> 8
        .dw     0x65b5                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume
adpcm_b_beat_16_5:
        .dw     0x4000                  ; sample start addr >> 8
        .dw     0x407e                  ; sample stop addr >> 8
        .dw     0x4c08                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume
adpcm_b_dance_44:
        .dw     0x407f                  ; sample start addr >> 8
        .dw     0x411c                  ; sample stop addr >> 8
        .dw     0xcb6b                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume
adpcm_b_dance_33:
        .dw     0x407f                  ; sample start addr >> 8
        .dw     0x411c                  ; sample stop addr >> 8
        .dw     0x80e8                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume
adpcm_b_dance_22:
        .dw     0x407f                  ; sample start addr >> 8
        .dw     0x411c                  ; sample stop addr >> 8
        .dw     0x65b5                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume
adpcm_b_dance_16_5:
        .dw     0x407f                  ; sample start addr >> 8
        .dw     0x411c                  ; sample stop addr >> 8
        .dw     0x4c08                  ; delta-n
        .db     0xc0                    ; l/r output
        .db     0xff                    ; volume



;;; The sound command functions for this ROM
;;; These functions are called by nullsound during the main event-loop
;;;
play_beat_44:
        ld      ix, #adpcm_b_beat_44
        call    snd_adpcm_b_play
        ret

play_beat_33:
        ld      ix, #adpcm_b_beat_33
        call    snd_adpcm_b_play
        ret

play_beat_22:
        ld      ix, #adpcm_b_beat_22
        call    snd_adpcm_b_play
        ret

play_beat_16_5:
        ld      ix, #adpcm_b_beat_16_5
        call    snd_adpcm_b_play
        ret

loop_beat_44:
        ld      ix, #adpcm_b_beat_44
        call    snd_adpcm_b_play_loop
        ret

loop_beat_33:
        ld      ix, #adpcm_b_beat_33
        call    snd_adpcm_b_play_loop
        ret

loop_beat_22:
        ld      ix, #adpcm_b_beat_22
        call    snd_adpcm_b_play_loop
        ret

loop_beat_16_5:
        ld      ix, #adpcm_b_beat_16_5
        call    snd_adpcm_b_play_loop
        ret

play_dance_44:
        ld      ix, #adpcm_b_dance_44
        call    snd_adpcm_b_play
        ret

play_dance_33:
        ld      ix, #adpcm_b_dance_33
        call    snd_adpcm_b_play
        ret

play_dance_22:
        ld      ix, #adpcm_b_dance_22
        call    snd_adpcm_b_play
        ret

play_dance_16_5:
        ld      ix, #adpcm_b_dance_16_5
        call    snd_adpcm_b_play
        ret

loop_dance_44:
        ld      ix, #adpcm_b_dance_44
        call    snd_adpcm_b_play_loop
        ret

loop_dance_33:
        ld      ix, #adpcm_b_dance_33
        call    snd_adpcm_b_play_loop
        ret

loop_dance_22:
        ld      ix, #adpcm_b_dance_22
        call    snd_adpcm_b_play_loop
        ret

loop_dance_16_5:
        ld      ix, #adpcm_b_dance_16_5
        call    snd_adpcm_b_play_loop
        ret
