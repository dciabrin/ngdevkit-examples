/*
 * Copyright (c) 2026 Damien Ciabrini
 * This file is part of ngdevkit
 *
 * ngdevkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * ngdevkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ngdevkit/neogeo.h>
#include <ngdevkit/bios-ram.h>
#include <ngdevkit/memory-card.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <stdio.h>
#include <stdbool.h>

#define A_PRESSED 0x10
#define B_PRESSED 0x20
#define C_PRESSED 0x40
#define D_PRESSED 0x80


u8 status_timer;
void status_display(char* str) {
    char tmp[30];
    tmp[29] = 0;
    snprintf(tmp, 28, "OK: %s", str);
    ng_center_text(20, 2, "                        ");
    ng_center_text(20, 2, tmp);
    status_timer = 90;
}

void status_update(void) {
    if ((status_timer > 0) && (--status_timer == 0)) {
        ng_center_text(20, 0, "                             ");
    }
}

void status_format_card_error(u8 card_answer) {
    static char* errs[] = {
        "no card inserted!",
        "card is not formatted",
        "data not found",
        "card corrupted",
        "card is full",
        "card cannot be written"
    };
    if (card_answer == 0) {
        status_display("no error");
    } else {
        char tmp[30];
        u8 err_idx = card_answer & 0xf;
        sprintf(tmp, "ERROR: %s", errs[err_idx]);
        ng_center_text(20, 1, tmp);
        status_timer = 90;
    }
}

void tst_format(void) {
    ng_memory_card_unlock();
    bios_card_command = MC_CMD_FORMAT;
    bios_card();
    if (bios_card_answer == 0) {
        status_display("card formatted");
    } else {
        status_format_card_error(bios_card_answer);
    }
}

void tst_save_name(void) {
    char name[]="ngdevkit";
    ng_memory_card_unlock();
    bios_card_start = (u32)name;
    bios_card_command = MC_CMD_SAVE_USER_NAME;
    bios_card();
    if (bios_card_answer == 0) {
        status_display("card name set");
    } else {
        status_format_card_error(bios_card_answer);
    }
}

void tst_load_name(void) {
    char name[17];
    name[0]=0;
    name[16]=0;
    bios_card_start = (u32)name;
    bios_card_command = MC_CMD_LOAD_USER_NAME;
    bios_card();
    if (bios_card_answer == 0) {
        char tmp[30];
        sprintf(tmp, "card name: %s", name);
        status_display(tmp);
    } else {
        status_format_card_error(bios_card_answer);
    }
}


void check_card_status() {
    if (ng_memory_card_inserted()) {
        ng_center_text(26, 0, "CARD DETECTED");
    } else {
        static u8 tempo;
        if (!tempo) {
            static char* msgs[] = {" INSERT CARD ", "            "};
            static u8 msg_id = 0;
            ng_center_text(26, 0, msgs[msg_id]);
            msg_id = ++msg_id & 1;
        }
        tempo = ++tempo & 31;
    }
}

void clear_demo_screen(void) {
    ng_cls();
    ng_center_text_tall(5, 0, "MEMORY CARD TEST");
    ng_center_text(8, 0, "D: cycle views");
}

void view_card_state(void) {
    clear_demo_screen();
    ng_text(9, 14, 0, "A: format memcard");
    ng_text(9, 15, 0, "B: save name to memcard");
    ng_text(9, 16, 0, "C: load name from memcard");

    for(;;) {
        ng_wait_vblank();
        check_card_status();
        if (bios_p1change & A_PRESSED) {
            tst_format();
        } else if (bios_p1change & B_PRESSED) {
            tst_save_name();
        } else if (bios_p1change & C_PRESSED) {
            tst_load_name();
        } else if (bios_p1change & D_PRESSED) {
            ng_wait_vblank();
            return;
        }
        status_update();
    }
}

void tst_save_game_data() {
    ng_memory_card_unlock();
    static char data[]="RAW DATA!";
    bios_card_start = (u32)data;
    bios_card_size = (u8)sizeof(data);
    bios_card_command = MC_CMD_SAVE_DATA;
    bios_card_fcb = 0x1338;
    bios_card_sub = 1;
    bios_card();
    if (bios_card_answer == 0) {
        status_display("saved!");
    } else {
        status_format_card_error(bios_card_answer);
    }
}

void tst_load_game_data() {
    ng_memory_card_unlock();
    char data[64];
    bios_card_start = (u32)data;
    bios_card_size = 64;
    bios_card_command = MC_CMD_LOAD_DATA;
    bios_card_fcb = 0x1338;
    bios_card_sub = 1;
    bios_card();
    if (bios_card_answer == 0) {
        char tmp[30];
        tmp[29] = 0;
        snprintf(tmp, 28, "game data: %s", data);
        status_display(tmp);
    } else {
        status_format_card_error(bios_card_answer);
    }
}

void view_card_game_saves(void) {
    clear_demo_screen();
    ng_text(7, 14, 0, "A: save game data (slot 1/16)");
    ng_text(7, 15, 0, "B: load game data (slot 1/16)");

    for(;;) {
        ng_wait_vblank();
        check_card_status();
        if (bios_p1change & A_PRESSED) {
            tst_save_game_data();
        } else if (bios_p1change & B_PRESSED) {
            tst_load_game_data();
        } else if (bios_p1change & D_PRESSED) {
            return;
        }
        status_update();
    }
}

int main(void) {
    bios_fix_clear();

    const u16 clut[][16]= {
        {0x8000, 0x0fff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        {0x8000, 0x0fff, 0x0a40, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        {0x8000, 0x0fff, 0x004a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        {0x8000, 0x0888, 0x0444, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        {0x8000, 0x0f00, 0x0800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
    };
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
    *((volatile u16*)0x401ffe)=0x0333;

    for (;;) {
    // Loop between views each time button D is pressed
    view_card_state();
    view_card_game_saves();
    }

    return 0;
}
