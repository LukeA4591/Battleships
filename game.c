#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define NUM_COLS 5
#define NUM_ROWS 7

int16_t columns[3] = {-1, -1, -1};
uint16_t column;
uint16_t row;
bool large_placed = false;
bool med_placed = false;
bool small_placed = false;

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

static uint8_t large_ship = 0x0F;


static uint8_t med_ship = 0x07;
    

static uint8_t small_ship = 0x03;

void reset(void) {
    column = 0;
    row = 0;
}

bool collision_check(uint8_t ship, uint16_t newcolumn, uint16_t newrow)
{
    if ((map[newcolumn] & (ship << newrow)) == 0) {
        return true;
    }
    return false;
}

void displayMap(int current_column) {
    for (int i = 0; i < NUM_COLS; i++) {
        pio_output_high(cols[i]);
    }
    pio_output_low(cols[current_column]);
    for (size_t current_row = 0; current_row < 7; current_row++)
    {
        if ((map[current_column] >> current_row) & 1)
        {
            pio_output_low(rows[current_row]);
        }
        else
        {
            pio_output_high(rows[current_row]);
        }
    }
}

void move(bool* placed, uint8_t ship, uint8_t shipNum)
{
    uint8_t col_upper_lim = 4;
    uint8_t col_lower_lim = 0;
    uint8_t row_upper_lim;
    uint8_t row_lower_lim = 0;
    if (!large_placed) {
        row_upper_lim = 3;
    } else if (!med_placed) {
        row_upper_lim = 4;
    } else if (!small_placed) {
        row_upper_lim = 5;
    }
    navswitch_update ();
    if (navswitch_push_event_p (NAVSWITCH_EAST)){
        if ((column < col_upper_lim) /*&& collision_check(ship, column++, row)*/) {
            map[column + 1] = map[column];
            map[column] = 0x0;
            column++;
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        if ((column > col_lower_lim) /*&& collision_check(ship, column--, row)*/) {
            map[column - 1] = map[column];
            map[column] = 0x0;
            column--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if ((row > row_lower_lim) /*&& collision_check(ship, column, row--)*/) {
            map[column] >>= 1;
            row--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if ((row < row_upper_lim) /*&& collision_check(ship, column, row--)*/) {
            map[column] <<= 1;
            row++;
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_PUSH))
    {
        *placed = !(*placed);
        columns[shipNum] = column;
    }
}


void place_ship(uint8_t ship, uint8_t shipNum) {
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 5; j++){
            if ((map[j] & (ship << i)) == 0) {
                map[j] |= ship;
                columns[shipNum] = j;
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
    pacer_init (50);

    
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
        if (!large_placed) {
            if (ship1) {
                place_ship(large_ship, 0);
                ship1 = !ship1;
            }
            move(&large_placed, large_ship, 0);
            displayMap(column);
        } else if (!med_placed) {
            if (ship2){
                reset();
                place_ship (med_ship, 1);
                ship2 = !ship2;
            }
            move(&med_placed, med_ship, 1);
            displayMap(column);
        } else if (!small_placed) {
            if (ship3){
                reset();
                place_ship (small_ship, 2);
                ship3 = !ship3;
            }
            move(&small_placed, small_ship, 2);
            displayMap(column);
        }
    }
}
