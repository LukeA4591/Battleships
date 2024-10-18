#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include "button.h"
#include "led.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
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

uint8_t current_column = 0;
uint8_t hits = 0;

position_t position;

bool bothDone = false;
bool placingShips = true;
bool recieved = false;
bool launch = true;
bool switch_on = false;

static uint8_t missile = 0x01;

uint8_t col_upper_lim;
uint8_t row_upper_lim;

int8_t turn = -1;

void place_ship(uint8_t ship);

//opponent will wait for a bomb and return whether it is a hit or not
void receiving_bombs(void) {
    displayMap(placedShips[current_column], current_column);
    current_column++;
    if (current_column > 4)
    {
        current_column = 0;
    }
}

void check_bomb(char position) {
    uint8_t column = position & 0x0F;
    uint8_t row = (position >> 4) & 0x0F;
    uint8_t mask = (0x01 << row);
    if((placedShips[column] & mask) != 0) {
        ir_uart_putc('h');
    } else {
        ir_uart_putc('m');
    }

}


void finishGame(void) {
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (10);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    game_state = GAME_FINISHED;
    send('x');
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

    while (1) {
        pacer_wait();

        switch (game_state) {
            case PLACE_SHIPS:
                if (ir_uart_read_ready_p() && !recieved) {
                    char chr = ir_uart_getc();
                    if (chr == 'a') {  // Ensure byte is valid and matches 'a'
                        turn = 0;
                        recieved = true;
                    }
                }
                place_ships(&position, current_column, &turn, &game_state, &bothDone);
                break;
            case SEND_MAP:
                displayMap(placedShips[current_column], current_column);
                current_column = (current_column + 1) % 5;

                if (ir_uart_read_ready_p()) {
                    char chr = ir_uart_getc();
                    if (chr == 'b') {
                        bothDone = true;
                    }
                }

                if (bothDone) {
                    if (turn == 0) {
                    game_state = THEIR_TURN;
                } else if (turn == 1) {
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
                    if (chr == 'm') {
                        game_state = THEIR_TURN;
                        send ('n');
                    } else if (chr == 'h') {
                        hits++;
                        if (hits == 9) {
                            finishGame();
                        }
                        place_ship_on_map (missile, missileMap, &position);
                    } 
                }
                moveMissile(&position, missile, current_column);
                break;

            case THEIR_TURN:
                if (ir_uart_read_ready_p()) {
                    char chr = ir_uart_getc();
                    if (chr == 'n') {
                        game_state = YOUR_TURN;
                    } else if (chr == 'x') {
                        finishGame ();
                    } else if (chr <= 100) {
                        check_bomb(chr);
                    }
                }
                receiving_bombs();
                break;
            case GAME_FINISHED:
                if (hits == 9) {
                    tinygl_text ("W");
                } else {
                    tinygl_text ("L");
                }
                tinygl_update();
                break;
        }
    }
}
