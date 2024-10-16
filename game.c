#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include "button.h"
#include "led.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define NUM_COLS 5
#define NUM_ROWS 7

static uint16_t column;
static uint16_t row;
bool large_placed = false;
bool med_placed = false;
bool small_placed = false;
bool vertical = false;

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

static uint8_t large_ship = 0x0F;

static uint8_t med_ship = 0x07;

static uint8_t small_ship = 0x03;

void reset(void) {
    column = 0;
    row = 0;
    vertical = false;
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

static uint8_t prev_column = 4;
static void displayMap(uint8_t row_pattern, uint8_t current_column)
{

    pio_output_high(cols[prev_column]);


    for (size_t current_row = 0; current_row < 7; current_row++)
    {
        if ((row_pattern >> current_row) & 1) 
        {
            pio_output_low(rows[current_row]);  
        } 
        else 
        {
            pio_output_high(rows[current_row]); 
        }
    }
    pio_output_low(cols[current_column]);
    
    prev_column = current_column;
}

uint8_t large_ship_vert[4] = {0x01, 0x01, 0x01, 0x01};

uint8_t med_ship_vert[3] = {0x01, 0x01, 0x01};

uint8_t small_ship_vert[2] = {0x01, 0x01};

void flip (uint8_t shipNum, uint8_t ship, bool vert, uint8_t vert_ship[], uint8_t row_lim, uint8_t col_lim) {
    for (int i = 0; i < NUM_COLS; i++) {
        map[i] = placedShips[i];
    }
    if (!vert) {
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
    } else {
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
}
void move(bool* placed, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, bool* vert)
{
    uint8_t col_lower_lim = 0;
    uint8_t row_lower_lim = 0;
    uint8_t col_upper_lim;
    uint8_t row_upper_lim;
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
    navswitch_update ();
    if (navswitch_push_event_p (NAVSWITCH_EAST)) { 
        if (!vertical) {
            if ((column < col_upper_lim) && collision_check(ship, column + 1, row)) {
                map[column + 1] |= (ship << row);
                map[column] = placedShips[column];
                column++;
            }
        } else {
            if ((column < col_upper_lim) && vert_collision_check(shipNum, column + 1, row)) {
                for (int i = 0; i < NUM_COLS; i++) {
                    map[i] = placedShips[i];
                }
                column++;
                for (int i = 0; i < shipNum; i++) {
                    map[i + column] |= vert_ship[i];
                }
            }
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        if (!vertical) {
            if ((column > col_lower_lim) && collision_check(ship, column - 1, row)) {
            map[column - 1] |= (ship << row);
            map[column] = placedShips[column];
            column--;
            }
        } else {
            if ((column > col_lower_lim) && vert_collision_check(shipNum, column - 1, row)) {
                for (int i = 0; i < NUM_COLS; i++) {
                    map[i] = placedShips[i];
                }
                column--;
                for (int i = 0; i < shipNum; i++) {
                    map[i + column] |= vert_ship[i];
                }
            }
        
        }
        
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if (!vertical) {
            if ((row > row_lower_lim) && collision_check(ship, column, row - 1)) {
                row--;
                map[column] = placedShips[column];
                map[column] |= (ship << row);
            }
        } else {
            if ((row > row_lower_lim) && vert_collision_check(shipNum, column, row - 1)) {
                for (int i = 0; i < NUM_COLS; i++) {
                    map[i] = placedShips[i];
                }
                row--;
                for (int i = 0; i < shipNum; i++) {
                    vert_ship[i] = (0x01 << row);
                }
                for (int i = 0; i < shipNum; i++) {
                    map[i + column] |= vert_ship[i];
                }
            }
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if (!vertical) {
            if ((row < row_upper_lim) && collision_check(ship, column, row + 1)) {
                row++;
                map[column] = placedShips[column];
                map[column] |= (ship << row);
            }
        } else {
            if ((row < row_upper_lim) && vert_collision_check(shipNum, column, row + 1)) {
                for (int i = 0; i < NUM_COLS; i++) {
                    map[i] = placedShips[i];
                }
                row++;
                for (int i = 0; i < shipNum; i++) {
                    vert_ship[i] = (0x01 << row);
                }
                for (int i = 0; i < shipNum; i++) {
                    map[i + column] |= vert_ship[i];
                }
            }
        }
        
    }

    if (navswitch_push_event_p (NAVSWITCH_PUSH))
    {
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

    if (button_pressed_p ())
    {
        flip(shipNum, ship, vertical, vert_ship, (row_upper_lim + 1) - shipNum, col_upper_lim - (shipNum - 1));
        *vert = !(*vert);
    }
}


void place_ship(uint8_t ship) {
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 5; j++){
            if ((map[j] & (ship << i)) == 0) {
                map[j] |= ship;
                column = j;
                row = i;
                return;
            }
        }
    }
}

bool ship1 = true;
bool ship2 = true;
bool ship3 = true;

int main (void)
{
    reset ();
    system_init ();
    navswitch_init ();
    pacer_init (1000);
    led_init ();
    button_init ();

    uint8_t current_column = 0;
    
    for (size_t i = 0; i < 7; i++)
    {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
    }
    for (size_t i = 0; i < 5; i++)
    {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }

    while (1)
    {
        pacer_wait ();
        displayMap(map[current_column], current_column);
        current_column++;
        if (current_column > 4)
        {
            current_column = 0;
        }
        if (!large_placed) {
            if (ship1) {
                place_ship(large_ship);
                ship1 = !ship1;
            }
            move(&large_placed, large_ship, large_ship_vert, 4, &vertical);
        } else if (!med_placed) {
            if (ship2){
                reset();
                place_ship (med_ship);
                ship2 = !ship2;
            }
            move(&med_placed, med_ship, med_ship_vert, 3, &vertical);
        } else if (!small_placed) {
            if (ship3){
                reset();
                place_ship (small_ship);
                ship3 = !ship3;
            }
            move(&small_placed, small_ship, small_ship_vert, 2, &vertical);
        }
    }
}

