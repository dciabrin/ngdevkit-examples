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
        .module user_commands
        .area   CODE (REL)

;;; commands dependencies
;;;
        .include "ym2610.def"
        .include "nullsound.def"
        .include "user_commands.def"
        .include "sfx_adpcma.s"

        ;; [cmd 04][module 0] play ADPCM-A sample [0000..02ff] on channel 1
        snd_command_request 04, 0, sfx_adpcm_a_play, 0
snd_command_04_action_config:
        .dw     0x0                     ; sample start addr >> 8
        .dw     0x2                     ; sample stop addr >> 8
        .db     1                       ; channel 3
        .db     0xdf                    ; l/r output + volume

        ;; [cmd 05][module 0] play ADPCM-A sample [0300..0dff] on channel 1
        snd_command_request 05, 0, sfx_adpcm_a_play, 0
snd_command_05_action_config:
        .dw     0x3                     ; sample start addr >> 8
        .dw     0xd                     ; sample stop addr >> 8
        .db     1                       ; channel 3
        .db     0xdf                    ; l/r output + volume

        ;; [cmd 06][module 1] play ADPCM-A sample [0e00..29ff] on channel 2, when not in use
        snd_command_request 06, 1, sfx_adpcm_a_play, 1
snd_command_06_action_config:
        .dw     0xe                     ; sample start addr >> 8
        .dw     0x29                    ; sample stop addr >> 8
        .db     2                       ; channel 2
        .db     0xdf                    ; l/r output + volume
