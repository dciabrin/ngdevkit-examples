;;;
;;; Copyright (c) 2020 Damien Ciabrini
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

;;; user-specific commands to instantiate the nullsound driver
;;; ----------------------------------------------------------
        .include "helpers.inc"
        .area   CODE

cmd_jmptable::
        ;; common/reserved sound commands
        jp      snd_command_unused
        jp      snd_command_01_prepare_for_rom_switch
        jp      snd_command_unused
        jp      snd_command_03_reset_driver
        jp      snd_command_04_play_sample_woosh
        jp      snd_command_05_play_sample_hook
        jp      snd_command_06_play_sample_break
        init_unused_cmd_jmptable


;;; This ROM contains three sound commands that play
;;; ADPCM-a samples. The sound commands use nullsound
;;; to configure the YM2610 and start playback.
;;;

snd_command_04_play_sample_woosh:
        ld      ix, #adpcm_a_woosh
        call    snd_adpcm_a_play
        ret

snd_command_05_play_sample_hook:
        ld      ix, #adpcm_a_hook
        call    snd_adpcm_a_play
        ret

snd_command_06_play_sample_break:
        ld      ix, #adpcm_a_break
        call    snd_adpcm_a_play_exclusive
        ret



;;; ADPCM-A sample data
;;;
        ;; offset of the ADPCM samples in VROM
        .include "samples.inc"

adpcm_a_woosh:
        .db     WOOSH_START_LSB         ;
        .db     WOOSH_START_MSB         ; start>>8 in VROM
        .db     WOOSH_STOP_LSB          ;
        .db     WOOSH_STOP_MSB          ; stop>>8  in VROM
        .db     0                       ; channel 1
        .db     0xdf                    ; l/r output + volume
        .db     1                       ; channel 1 (bit)
adpcm_a_hook:
        .db     HOOK_START_LSB          ;
        .db     HOOK_START_MSB          ; start>>8 in VROM
        .db     HOOK_STOP_LSB           ;
        .db     HOOK_STOP_MSB           ; stop>>8  in VROM
        .db     1                       ; channel 2
        .db     0xdf                    ; l/r output + volume
        .db     2                       ; channel 2 (bit)
adpcm_a_break:
        .db     BREAK_START_LSB         ;
        .db     BREAK_START_MSB         ; start>>8 in VROM
        .db     BREAK_STOP_LSB          ;
        .db     BREAK_STOP_MSB          ; stop>>8  in VROM
        .db     2                       ; channel 3
        .db     0xdf                    ; l/r output + volume
        .db     4                       ; channel 3 (bit)
