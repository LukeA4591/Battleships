/** @file   game.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Battleships game*/


#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "button.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "timer.h"
#include "ir_uart.h"
#include "map.h"
#include "position.h"
#include "missile.h"
#include "gamestate.h"
#include "placeships.h"
#include "mapfunctions.h"
#include "send.h"
#include <stdbool.h>
#define NUM_COLS 5

static game_state_t game_state = START_SCREEN;

uint8_t current_col = 0;
uint8_t hits = 0;

position_t position;

bool bothDone = false;
bool recieved = false;
bool launch = true;

static uint8_t missile = 0x01;

uint8_t col_upper_lim;
uint8_t row_upper_lim;

int8_t playerOne = -1;

/**
Displays the map of placed ships
*/
void displayPlacedShips(void) {
    displayMap(placedShips[current_col], current_col);
    current_col++;
    if (current_col > 4)
    {
        current_col = 0;
    }
}

/**
Determines whether the received missile is a hit or miss, sends this information back to opponent
 */
void checkMissile(char position) {
    uint8_t column = position & 0x0F;
    uint8_t row = (position >> 4) & 0x0F;
    uint8_t mask = (0x01 << row);
    if((placedShips[column] & mask) != 0) {
        send('h'); //hit
    } else {
        send('m'); //miss
    }

}

/**
Prepares game over text and tells opponent to finish their game
 */
void finishGame(void) {
    if (hits == 9) {
        tinygl_text ("YOU WON! PUSH TO PLAY AGAIN");

    } else {
        tinygl_text ("YOU LOST PUSH TO PLAY AGAIN");
    }
    game_state = GAME_FINISHED;
    send('x');
}

/**
Initialises tinygl and sets start screen text
 */
void setStartScreen(void) {
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (15);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text("BATTLESHIPS PUSH TO START");
}

/**
Waits for navigation switch to be pushed before starting game
 */
void waitToStart(void) {
    navswitch_update ();
    if(navswitch_push_event_p(NAVSWITCH_PUSH)) {
        recieved = false;
        resetMaps();
        reset(&position);
        game_state = PLACE_SHIPS;
        bothDone = false;
        playerOne = -1;
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
    // main loop for the game
    while (1) {
        pacer_wait();
        // switch for game state
        switch (game_state) {
            case PLACE_SHIPS:
                if (ir_uart_read_ready_p() && !recieved) {
                    char chr = ir_uart_getc();
                    if (chr == 'a') {  // Ensure byte is valid and matches 'a'
                        playerOne = 0;
                        recieved = true;
                    }
                }
                placeShips(&position, &playerOne, &game_state, &bothDone); // place the ship
                tinygl_text("WAITING FOR OPPONENT");
                break;
            case SEND_MAP:
                tinygl_update();
                // wait for other player to place their ships
                if (ir_uart_read_ready_p()) {
                    char chr = ir_uart_getc();
                    if (chr == 'b') {
                        bothDone = true;
                    }
                }
                // set turns when both players have placed their ships
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
                    placeObjectOnMap (missile, missileMap, &position);
                    launch = !launch;
                }
                // after sending missile, listen for hit or miss.
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
                        placeObjectOnMap (missile, missileMap, &position); // if hit show on map
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
                        checkMissile(chr);
                    }
                }
                displayPlacedShips();
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

