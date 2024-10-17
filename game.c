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
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define NUM_COLS 5
#define NUM_ROWS 7

typedef enum game_state
{           
    PLACE_SHIPS,
    SEND_MAP,
    YOUR_TURN,
    THEIR_TURN,
    GAME_FINISHED

} game_state_t;

static game_state_t game_state = PLACE_SHIPS;

uint8_t current_column = 0;
static uint8_t prev_column = 4;
static uint16_t column;
static uint16_t row;
bool bothDone = false;
bool placingShips = true;
bool ship1 = true;
bool ship2 = true;
bool ship3 = true;
bool recieved = false;
bool large_placed = false;
bool med_placed = false;
bool small_placed = false;
bool vertical = false;

uint8_t large_ship_vert[4] = {0x01, 0x01, 0x01, 0x01};
uint8_t med_ship_vert[3] = {0x01, 0x01, 0x01};
uint8_t small_ship_vert[2] = {0x01, 0x01};

static uint8_t large_ship = 0x0F;
static uint8_t med_ship = 0x07;
static uint8_t small_ship = 0x03;

uint8_t col_lower_lim = 0;
uint8_t row_lower_lim = 0;
uint8_t col_upper_lim;
uint8_t row_upper_lim;

int8_t turn = -1;

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

static uint8_t map[] =
        {
                0x00, 0x00, 0x00, 0x00, 0x00
        };

static uint8_t placedShips[] = 
        {
                0x00, 0x00, 0x00, 0x00, 0x00
        };
static uint8_t missileMap[] =
        {
                0x01, 0x00, 0x00, 0x00, 0x00
        };


void reset(void)
{
    column = 0;
    row = 0;
    vertical = false;
}

void resetMap(void) {
    for (int i = 0; i < NUM_COLS; i++) {
        map[i] = placedShips[i];
    }
}

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

bool collision_check(uint8_t ship, uint16_t newcolumn, uint16_t newrow)
{

    if ((placedShips[newcolumn] & (ship << newrow)) == 0x0) {
        return true;
    }
    return false;
}

bool vert_collision_check(uint8_t shipNum, uint16_t newcolumn, uint16_t newrow)
{
    uint8_t mask = 0x01 << newrow;
    for (int i = 0; i < shipNum; i++) {
        if (((placedShips[i + newcolumn]) & mask) != 0x0) {
            return false;
        }
    }
    return true;
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

void flipToVert (uint8_t shipNum, uint8_t vert_ship[], uint8_t col_lim) {
    if (column > col_lim) {
            column = col_lim;
    }
    while (!vert_collision_check(shipNum, column, row)) {
        if (column > 0) {
            column--;
        } else {
            if (row < 6) {
                row++;
            } else {
                row = 0;
            }
        }
    }
    for (int i = 0; i < shipNum; i++) {
        vert_ship[i] = (0x01 << row);
    }
    for (int i = 0; i < shipNum; i++) {
        map[i + column] |= vert_ship[i];
    }
}

void flipToHrz (uint8_t ship, uint8_t row_lim) {
    if (row > row_lim) {
        row = row_lim;
    }
    while(!collision_check(ship, column, row)) {
        if (row > 0) {
            row --;
        } else {
            if (column < 4) {
                column++;
            } else {
                column = 0;
            }
        }
    }
    map[column] |= (ship << row);
}

void flip (uint8_t shipNum, uint8_t ship, bool vert, uint8_t vert_ship[], uint8_t row_lim, uint8_t col_lim) {
    resetMap ();
    if (!vert) {
        flipToVert (shipNum, vert_ship, col_lim);
    } else {
        flipToHrz (ship, row_lim);
    }
}

void placeVertRow(uint8_t shipNum,uint8_t vert_ship[]) {
    for (int i = 0; i < shipNum; i++) {
        vert_ship[i] = (0x01 << row);
    }
    for (int i = 0; i < shipNum; i++) {
        map[i + column] |= vert_ship[i];
    }
}

void placeHrzRow (uint8_t ship) {
    map[column] = placedShips[column];
    map[column] |= (ship << row);
}

void placeVertCol(uint8_t shipNum, uint8_t vert_ship[]) {
    for (int i = 0; i < shipNum; i++) {
        map[i + column] |= vert_ship[i];
    }
}

void placeHrzColDown (uint8_t ship) {
    map[column + 1] |= (ship << row);
    map[column] = placedShips[column];
    column++;
}

void placeHrzColUp (uint8_t ship) {
    map[column - 1] |= (ship << row);
    map[column] = placedShips[column];
    column--;
}

void hrzDown (uint8_t lim, uint8_t ship) {
    if ((column < lim) && collision_check(ship, column + 1, row)) {
        placeHrzColDown (ship);
    }
}

void vertDown (uint8_t col_upper_lim, uint8_t shipNum, uint8_t vert_ship[]) {
    if ((column < col_upper_lim) && vert_collision_check(shipNum, column + 1, row)) {
        resetMap ();
        column++;
        placeVertCol (shipNum, vert_ship);
    }
}

void hrzUp (uint8_t ship) {
    if ((column > col_lower_lim) && collision_check(ship, column - 1, row)) {
        placeHrzColUp (ship);
    }
}

void vertUp (uint8_t vert_ship[], uint8_t shipNum) {
    if ((column > col_lower_lim) && vert_collision_check(shipNum, column - 1, row)) {
        resetMap ();
        column--;
        placeVertCol (shipNum, vert_ship);
    }
}

void hrzLeft (uint8_t ship) {
    if ((row > row_lower_lim) && collision_check(ship, column, row - 1)) {
        row--;
        placeHrzRow (ship);
    }
}

void vertLeft (uint8_t vert_ship[], uint8_t shipNum) {
    if ((row > row_lower_lim) && vert_collision_check(shipNum, column, row - 1)) {
        resetMap ();
        row--;
        placeVertRow(shipNum, vert_ship);
    }
}

void hrzRight (uint8_t row_upper_lim, uint8_t ship) {
    if ((row < row_upper_lim) && collision_check(ship, column, row + 1)) {
        row++;
        placeHrzRow (ship);
    }
}

void vertRight (uint8_t row_upper_lim, uint8_t shipNum, uint8_t vert_ship[]) {
    if ((row < row_upper_lim) && vert_collision_check(shipNum, column, row + 1)) {
        resetMap ();
        row++;
        placeVertRow(shipNum, vert_ship);
    }
}

void down(uint8_t col_upper_lim, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum) {
    if (!vertical) {
        hrzDown (col_upper_lim, ship);
    } else {
        vertDown (col_upper_lim, shipNum, vert_ship);
    }
}

void up (uint8_t ship, uint8_t vert_ship[], uint8_t shipNum) {
    if (!vertical) {
        hrzUp (ship);
    } else {
        vertUp (vert_ship, shipNum);
    }
}

void left (uint8_t ship, uint8_t vert_ship[], uint8_t shipNum) {
    if (!vertical) {
        hrzLeft (ship);
    } else {
        vertLeft (vert_ship, shipNum);
    }
}

void right (uint8_t row_upper_lim, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum) {
    if (!vertical) {
        hrzRight (row_upper_lim, ship);
    } else {
        vertRight (row_upper_lim, shipNum, vert_ship);
    }
}

void setLims(void) {
    if (!vertical) {
        col_upper_lim = 4;
        if (!large_placed) {
            row_upper_lim = 3;
        } else if (!med_placed) {
            row_upper_lim = 4;
        } else {
            row_upper_lim = 5;
        }
    } else {
        row_upper_lim = 6;
        if (!large_placed) {
            col_upper_lim = 1;
        } else if (!med_placed) {
            col_upper_lim = 2;
        } else {
            col_upper_lim = 3;
        }
    }   
}

void placeShip(bool* placed, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum) {
    if (!vertical) {
        placedShips[column] |= ship << row;
    } else {
        for (int i = 0; i < shipNum; i++) {
            vert_ship[i] = (0x01 << row);
            placedShips[i + column] |= vert_ship[i];
        }
    }
    *placed = !(*placed);
}

void move(bool* placed, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, bool* vert)
{
    setLims ();
    navswitch_update ();
    if (navswitch_push_event_p (NAVSWITCH_EAST)) { 
        down(col_upper_lim, ship, vert_ship, shipNum);
    }
    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        up(ship, vert_ship, shipNum);
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        left(ship, vert_ship, shipNum);
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        right(row_upper_lim, ship, vert_ship, shipNum);
        
    }

    if (navswitch_push_event_p (NAVSWITCH_PUSH))
    {
        placeShip(placed, ship, vert_ship, shipNum);
    }

    if (button_pressed_p ())
    {
        flip(shipNum, ship, vertical, vert_ship, (row_upper_lim + 1) - shipNum, col_upper_lim - (shipNum - 1));
        *vert = !(*vert);
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
        if (column < 4) {
            missileMap[column + 1] = missileMap[column];
            missileMap[column] = 0x0;
            column++;
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        if (column > 0) {
            missileMap[column - 1] = missileMap[column];
            missileMap[column] = 0x0;
            column--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if (row > 0) {
            missileMap[column] >>= 1;
            row--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if (row < 6) {
            missileMap[column] <<= 1;
            row++;
        }
    }

}


void place_ship_on_map(uint8_t ship) {
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 5; j++){
            if ((map[j] & (ship << i)) == 0) {
                map[j] |= (ship << i);
                column = j;
                row = i;
                return;
            }
        }
    }
}

void place_ships(void) {
    displayMap(map[current_column], current_column);
    current_column++;
    if (current_column > 4)
    {
        current_column = 0;
    }
    if (!large_placed) {
        if (ship1) {
            place_ship_on_map (large_ship);
            ship1 = !ship1;
        }
        move(&large_placed, large_ship, large_ship_vert, 4, &vertical);
    } else if (!med_placed) {
        if (ship2){
            reset();
            place_ship_on_map (med_ship);
            ship2 = !ship2;
        }
        move(&med_placed, med_ship, med_ship_vert, 3, &vertical);
    } else if (!small_placed) {
        if (ship3){
            reset();
            place_ship_on_map (small_ship);
            ship3 = !ship3;
        }
        move(&small_placed, small_ship, small_ship_vert, 2, &vertical);
    } else {
        if (turn == -1) {
            turn = 1;
            ir_uart_putc ('a');
            game_state = SEND_MAP;
        } else {
            ir_uart_putc ('b');
            bothDone = true;
            game_state = SEND_MAP;
        }
    }
}

int main (void)
{
    //char shipString[NUM_COLS * 2 + 1];
    //char mapString[NUM_COLS * 2 + 1];
    reset ();
    system_init ();
    navswitch_init ();
    pacer_init (500);
    initLedMat ();
    button_init();
    ir_uart_init ();

    /*
    tinygl_init (1000);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (20);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text("PLACE YOUR SHIPS");
    */

    while (1)
    {
        pacer_wait ();
        if(game_state == PLACE_SHIPS) {
            if (ir_uart_read_ready_p () && !recieved) {
                char chr = ir_uart_getc();
                if (chr == 'a') {
                    turn = 0;
                    recieved = true;
                }
            }
            place_ships();
        } else if (game_state == SEND_MAP) {
            if (ir_uart_read_ready_p ()) {
                char chr = ir_uart_getc();
                if (chr == 'b') {
                    bothDone = true;
                }
            }
            if (bothDone) {
                if (turn == 0) {
                    game_state = THEIR_TURN;
                } else if (turn == 1)
                {
                    game_state = YOUR_TURN;

                }
                
            }
        } else if (game_state == YOUR_TURN) {
            moveMissile();
        } else if (game_state == THEIR_TURN) {

        } else if (game_state == GAME_FINISHED) {
            
        }
        
    }
}
