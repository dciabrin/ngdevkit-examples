/*
 * Copyright (c) 2021 Damien Ciabrini
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
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <stdio.h>


// When a ROM starts, the BIOS typically tells it to
// run in an attract mode until player inserts coins
// (MVS) or starts the game (AES).
#define USER_MODE_BOOT 0x0
#define USER_MODE_DEMO 0x1
#define USER_MODE_GAME 0x2

// Hardware type as reported in the BIOS RAM
#define SYSTEM_AES 0x00
#define SYSTEM_MVS 0x80


// demo's prototypes
void demo_mode();
void title_mode();
void game_mode();
void game_over_screen();


// Temporary display buffer
char str[20];



// Display text in attract mode
void update_start_message() {
    static const char coin[]  = "INSERT COIN";
    static const char start[] = "PRESS START";
    static const char empty[] = "           ";
    static const char* msg;
    static u8 wait = 30;
    if (!--wait) {
        wait = 30;
        if (msg != empty) {
            msg = empty;
        } else if (bios_mvs_flag != SYSTEM_MVS) {
            // AES has no credits, just asks to press start
            msg = start;
        } else {
            // MVS asks for coins :P
            msg = coin;
        }
        ng_center_text(22, 0, msg);
    }
}

// Init the fix layer
void init_screen() {
  ng_cls();

  const u16 palette[]={0x8000, 0x0fff, 0x0555};
  for (u16 i=0; i<3; i++) {
    MMAP_PALBANK1[i]=palette[i];
  }
  // black backdrop color
  *((volatile u16*)0x401ffe)=0x0;
}



// This function is called by the BIOS when the start button of
// any player is pressed (AES) and when enough credits are available (MVS)
void player_start(void) {
    // Retrieve the player that pushed the start button
    // as reported by the BIOS in bios_start_flag
    // Note: if the player that pushed start was already
    // in-game, clear the bit to avoid consuming a credit
    if (bios_start_flag & 2) {
        if (!bios_player_mod2) {
            bios_player_mod2 = 1;
        } else {
            bios_start_flag &= ~2;
        }
    }
    if (bios_start_flag & 1) {
        if (!bios_player_mod1) {
            bios_player_mod1 = 1;
        } else {
            bios_start_flag &= ~1;
        }
    }

    // Tell the BIOS that the game has started
    bios_user_mode = USER_MODE_GAME;

    // When returning to the BIOS, credits will be consumed for
    // any bit still enabled in bios_start_flag (MVS)
}

// This function is called by the BIOS when a coin in inserted.
// It serves as a hook to play a sound with the sound driver.
void coin_sound(void) {
}



// ngdevkit's main() function
// --------------------------
// - A typical demo/attract mode consists in:
//   . the eye catcher animation (hardware logo)
//   . a game intro plus an arbitrary long demo
//   . a high score
//
// - Once player starts, the ROM enters a game mode
//
// - When the game is over:
//   . the ROM gives back control to the BIOS
//   . the BIOS restarts the ROM in attrach mode
//
int main(void) {
    init_screen();
    demo_mode();
    // when demo ends, only run game is start was pressed
    if (bios_user_mode == USER_MODE_GAME) game_mode();
    // returning to the BIOS to start demo again
    return 0;
}


// ngdevkit's optional main_mvs_title() function
// ---------------------------------------------
// - When a coin is inserted, MVS enters a special title
//   mode that ends demo and give the user 30 seconds to
//   start the game with one or two players.
//
// - Once the 30 seconds are over, the ROM enters game mode
//
// - When the game is over:
//   . the ROM gives back control to the BIOS
//   . the BIOS restarts the ROM in attrach mode (main())
//
int main_mvs_title(void) {
    init_screen();
    title_mode();
    game_mode();
    // returning to the BIOS to start demo again
    return 0;
}


void game_status(void) {
    ng_text(2,3,0,"[game status]");
    sprintf(str, "p1mod: %x", bios_player_mod1);
    ng_text(2,4,0,str);
    sprintf(str, "p2mod: %x", bios_player_mod2);
    ng_text(2,5,0,str);
    sprintf(str, "mode:  %x", bios_user_mode);
    ng_text(2,6,0,str);
    }

void demo_mode() {
    ng_center_text_tall(8, 0, "DEMO MODE");
    sprintf(str, "- %s -", (bios_mvs_flag==SYSTEM_MVS)?"MVS":"AES");
    ng_center_text(11,0,str);

    ng_center_text(16, 0,
                   (bios_mvs_flag==SYSTEM_MVS)?
                   "back to eye catcher":
                   "restart demo mode");
    ng_center_text(17, 0, "after 15s");

    u16 demo_end=60*15;
    while(demo_end-- && (bios_user_mode != USER_MODE_GAME)) {
        game_status();
        update_start_message();
        ng_wait_vblank();
    }
}

void title_mode() {
    ng_center_text_tall(8, 0, "TITLE  MODE");
    sprintf(str, "- %s -", (bios_mvs_flag==SYSTEM_MVS)?"MVS":"AES");
    ng_center_text(11,0,str);

    ng_center_text(22,0,"PRESS START");

    while(bios_user_mode != USER_MODE_GAME) {
        game_status();
        sprintf(str, "%02x", bios_compulsion_timer);
        ng_center_text(23,0,str);
        sprintf(str, "credit: %x ", *(u8*)0xd00034);
        ng_center_text(28,0,str);
        ng_wait_vblank();
    }
}

const char* get_status_string(u8 player_mod) {
    static const char coin[]  = "INSERT COIN";
    static const char start[] = "PRESS START";
    static const char empty[] = "           ";
    static u8 counter = 120;
    const char* msg;

    if (!--counter) {
        counter = 120;
    }

    if (player_mod != 0) {
        // running
        msg = empty;
    } else {
        if (counter < 60) {
            msg = empty;
        } else {
            // credits available?
            if (*(u8*)0xd00034 > 0) {
                msg = start;
            } else {
                msg = coin;
            }
        }
    }
    return msg;
}

void update_players_status() {
    const char* msg;
    msg = get_status_string(bios_player_mod1);
    ng_text(2, 29, 0, msg);
    msg = get_status_string(bios_player_mod2);
    ng_text(27, 29, 0, msg);
}

void game_mode() {
    ng_cls();
    // in-game backdrop color
    *((volatile u16*)0x401ffe)=0x488;

    ng_center_text_tall(14, 0, "GAME RUNNING!");
    ng_center_text(19, 0, "press A to game over 1UP");
    ng_center_text(20, 0, "press B to game over 2UP");

    while (bios_player_mod1 || bios_player_mod2) {
        game_status();
        sprintf(str, "credit: %x ", *(u8*)0xd00034);
        ng_center_text(28,0,str);

        // check input for ending player's game
        if (bios_p1current & 0x10) bios_player_mod1 = 0;
        if (bios_p2current & 0x10) bios_player_mod2 = 0;

        update_players_status();
        ng_wait_vblank();
    }

    // game over: update player's status and show a game over screen
    game_over_screen();
}


void game_over_screen() {
    ng_cls();
    *((volatile u16*)0x401ffe)=0xa00;
    ng_center_text_tall(14, 0, "GAME OVER");
    for(u8 wait=120;wait>0;wait--) {
        game_status();
        ng_wait_vblank();
    }
}
