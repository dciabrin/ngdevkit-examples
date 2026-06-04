;;;
;;; Copyright (c) 2026 Damien Ciabrini
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

;;; Barebone nullsound driver for building Neo Geo cartriges
;;; It only provides the sound commands triggered by the BIOS
;;; ------------------------------------------------------

        .include "helpers.inc"

        .area   CODE


;;; The M-ROM must provide a jump table to all sound commands
;;; The jump table includes command 01 and 03 which are
;;; implemented by nullsound.
;;; If relies on a new
;;;

cmd_jmptable::
        ;; common/reserved sound commands
        jp      snd_command_unused
        jp      snd_command_01_prepare_for_rom_switch
        jp      music_ngdevkit_eye_catcher
        jp      snd_command_03_reset_driver
        ;; complete unused sound commands
        init_unused_cmd_jmptable
