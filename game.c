/** @file   game.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Battleships game*/


#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include "button.h"
#include "led.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "timer.h"
#include "ir_uart.h"
#include "map.h"
#include "flipship.h"
#include "move.h"
#include "position.h"
#include "initmove.h"
#include "missile.h"
#include "gamestate.h"
#include "placeships.h"
#include "mapfunctions.h"
#include "send.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define NUM_COLS 5
#define NUM_ROWS 7

static game_state_t game_state = PLACE_SHIPS;

uint8_t current_col = 0;
uint8_t hits = 0;

position_t position;

bool bothDone = false;
bool placingShips = true;
bool recieved = false;
bool launch = true;

static uint8_t missile = 0x01;

uint8_t col_upper_lim;
uint8_t row_upper_lim;

int8_t playerOne = -1;

//opponent will wait for a bomb and return whether it is a hit or not
void receiving_bombs(void) {
    displayMap(placedShips[current_col], current_col);
    current_col++;
    if (current_col > 4)
    {
        current_col = 0;
    }
}

void check_bomb(char position) {
    uint8_t column = position & 0x0F;
    uint8_t row = (position >> 4) & 0x0F;
    uint8_t mask = (0x01 << row);
    if((placedShips[column] & mask) != 0) {
        send('h'); //hit
    } else {
        send('m'); //miss
    }

}


void finishGame(void) {
    if (hits == 9) {
        tinygl_text ("YOU WON!");

    } else {
        tinygl_text ("YOU LOST");
    }
    game_state = GAME_FINISHED;
    send('x');
}

void setStartScreen(void) {
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (15);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text("BATTLESHIPS PUSH TO START");
}

void waitToStart(void) {
    navswitch_update ();
    if(navswitch_push_event_p(NAVSWITCH_PUSH)) {
        game_state = PLACE_SHIPS;
    }
}

int main (void)
{
    timer_init ();
    reset (&position);
    system_init ();
    navswitch_init ();
    pacer_init (1000);
    initLedMat ();
    button_init();
    ir_uart_init ();
    tinygl_init (1000);
    setStartScreen();

    while (1) {
        pacer_wait();

        switch (game_state) {
            case PLACE_SHIPS:
                if (ir_uart_read_ready_p() && !recieved) {
                    char chr = ir_uart_getc();
                    if (chr == 'a') {  // Ensure byte is valid and matches 'a'
                        playerOne = 0;
                        recieved = true;
                    }
                }
                place_ships(&position, &playerOne, &game_state, &bothDone);
                tinygl_text("WAITING FOR OPPONENT");
                break;
            case SEND_MAP:
                tinygl_update();

                if (ir_uart_read_ready_p()) {
                    char chr = ir_uart_getc();
                    if (chr == 'b') {
                        bothDone = true;
                    }
                }
                if (bothDone) {
                    if (playerOne == 0) {
                    game_state = THEIR_TURN;
                } else if (playerOne == 1) {
                    game_state = YOUR_TURN;
                }
                }
                break;

            case YOUR_TURN:
                if (launch) {
                    place_ship_on_map (missile, missileMap, &position);
                    launch = !launch;
                }

                if (ir_uart_read_ready_p()) {
                    char chr = ir_uart_getc();
                    if (chr == 'm') { //Player missed
                        game_state = THEIR_TURN;
                        send ('n'); //Tell opponent to switch turn
                    } else if (chr == 'h') { //Player hit
                        hits++;
                        if (hits == 9) {
                            finishGame();
                        }
                        place_ship_on_map (missile, missileMap, &position);
                    } 
                }
                moveMissile(&position, missile);
                break;

            case THEIR_TURN:
                if (ir_uart_read_ready_p()) {
                    char chr = ir_uart_getc();
                    if (chr == 'n') { //Told to switch turn
                        game_state = YOUR_TURN;
                    } else if (chr == 'x') { //Told to end game
                        finishGame ();
                    } else if (chr <= 100) { //Received a missile
                        check_bomb(chr);
                    }
                }
                receiving_bombs();
                break;
            case GAME_FINISHED:
                tinygl_update();
                waitToStart();
                break;
            case START_SCREEN:
                tinygl_update();
                waitToStart();
                break;
        }
    }
}
