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
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>



#define NUM_COLS 5
#define NUM_ROWS 7

typedef enum game_state
{
    START_SCREEN,
    PLACE_SHIPS,
    SEND_MAP,
    YOUR_TURN,
    THEIR_TURN,
    GAME_FINISHED

} game_state_t;

static game_state_t game_state = START_SCREEN;

uint8_t current_column = 0;
uint8_t hits = 0;
static uint8_t prev_column = 4;
uint8_t largeShipNum = 4;
uint8_t medShipNum = 3;
uint8_t smallShipNum = 2;

position_t position;

bool bothDone = false;
bool placingShips = true;
bool ship1 = true;
bool ship2 = true;
bool ship3 = true;
bool recieved = false;
bool large_placed = false;
bool med_placed = false;
bool small_placed = false;
bool launch = true;
bool switch_on = false;

uint8_t large_ship_vert[4] = {0x01, 0x01, 0x01, 0x01};
uint8_t med_ship_vert[3] = {0x01, 0x01, 0x01};
uint8_t small_ship_vert[2] = {0x01, 0x01};

static uint8_t large_ship = 0x0F;
static uint8_t med_ship = 0x07;
static uint8_t small_ship = 0x03;
static uint8_t missile = 0x01;

uint8_t col_upper_lim;
uint8_t row_upper_lim;

int8_t playerOne = -1;

void place_ship(uint8_t ship);

static const pio_t rows[] =
        {
                LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
                LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
                LEDMAT_ROW7_PIO
        };

static const pio_t cols[] =
        {
                LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
                LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
        };





void initLedMat(void) {
    for (size_t i = 0; i < 7; i++)
    {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }
    for (size_t i = 0; i < 5; i++)
    {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }
}


static void displayMap(uint8_t row_pattern, uint8_t current_column)
{
    pio_output_high(cols[prev_column]);
    for (size_t current_row = 0; current_row < 7; current_row++) {
        if ((row_pattern >> current_row) & 1) {
            pio_output_low(rows[current_row]);  
        } 
        else {
            pio_output_high(rows[current_row]); 
        }
    }
    pio_output_low(cols[current_column]);
    prev_column = current_column;
}

void send (char chr) {
    ir_uart_putc(chr);
}

//Send position to opponent, opponent returns true 0 for miss, 1 for hit
void shootMissile(void) {
    char pos = (position.row << 4) | (position.column & 0x0F); //encode column and row into single char
    send (pos);
}

//opponent will wait for a bomb and return whether it is a hit or not
void displayPlacedShips(void) {
    displayMap(placedShips[current_column], current_column);
    current_column++;
    if (current_column > 4)
    {
        current_column = 0;
    }
}

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


void moveMissile(void) {
    displayMap(missileMap[current_column], current_column);
    current_column++;
    if (current_column > 4)
    {
        current_column = 0;
    }
    navswitch_update ();
    if (navswitch_push_event_p (NAVSWITCH_EAST)){
        if ((position.column < 4) && collisionCheck(missile, position.column + 1, position.row, missileMap)) {
            missileMap[position.column + 1] |= (0x01 << position.row);
            missileMap[position.column] ^= (0x01 << position.row);
            position.column++;
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        if ((position.column > 0) && collisionCheck(missile, position.column - 1, position.row, missileMap)) {
            missileMap[position.column - 1] |= (0x01 << position.row);
            missileMap[position.column] ^= (0x01 << position.row);
            position.column--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if ((position.row > 0) && collisionCheck(missile, position.column, position.row - 1, missileMap)) {
            missileMap[position.column] ^= (0x01 << position.row);
            position.row--;
            missileMap[position.column] |= (0x01 << position.row);
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if ((position.row < 6) && collisionCheck(missile, position.column, position.row + 1, missileMap)) {
            missileMap[position.column] ^= (0x01 << position.row);
            position.row++;
            missileMap[position.column] |= (0x01 << position.row);
        }
    }

    if(navswitch_push_event_p(NAVSWITCH_PUSH)) {
        shootMissile();
    }
}


void placeShipOnMap(uint8_t ship, uint8_t map[]) {
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 5; j++){
            if ((map[j] & (ship << i)) == 0) {
                map[j] |= (ship << i);
                position.column = j;
                position.row = i;
                return;
            }
        }
    }
}

void placeShips(void) {
    displayMap(map[current_column], current_column);
    current_column++;
    if (current_column > NUM_COLS - 1)
    {
        current_column = 0;
    }
    if (!large_placed) {
        if (ship1) {
            placeShipOnMap (large_ship, map);
            ship1 = !ship1;
        }
        move(&large_placed, large_ship, large_ship_vert, largeShipNum, &position);
    } else if (!med_placed) {
        if (ship2){
            reset(&position);
            placeShipOnMap (med_ship, map);
            ship2 = !ship2;
        }
        move(&med_placed, med_ship, med_ship_vert, medShipNum, &position);
    } else if (!small_placed) {
        if (ship3){
            reset(&position);
            placeShipOnMap (small_ship, map);
            ship3 = !ship3;
        }
        move(&small_placed, small_ship, small_ship_vert, smallShipNum, &position);
    } else {
        if (playerOne == -1) {
            playerOne = 1;
            send ('a');
            game_state = SEND_MAP;
            
        } else {
            send ('b');
            bothDone = true;
            game_state = SEND_MAP;
            
        }
    }
}

/*
Finishes the game and tells opponent to finish their game
*/
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
                placeShips();
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
                    placeShipOnMap (missile, missileMap);
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
                        placeShipOnMap (missile, missileMap);
                    } 
                }
                moveMissile();
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
                break;
            case START_SCREEN:
                tinygl_update();
                waitToStart();
                break;
        }
    }
}
