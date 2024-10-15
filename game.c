#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define NUM_COLS 5
#define NUM_ROWS 7

uint16_t prev_column;
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

static uint8_t bitmap[] =
        {
                0x00, 0x00, 0x00, 0x00, 0x00
        };

static uint8_t large_ship[] = 
        {
            0x0F, 0x00, 0x00, 0x00, 0x00
        };

static uint8_t med_ship[] = 
        {
            0x07, 0x00, 0x00, 0x00, 0x00
        };

static uint8_t small_ship[] = 
        {
            0x03, 0x00, 0x00, 0x00, 0x00
        };

void reset(void) {
    prev_column = -1;
    column = 0;
    row = 0;
}

void displayShip(int current_column, uint8_t ship[]) {
    for (int i = 0; i < NUM_COLS; i++) {
        pio_output_high(cols[i]);
    }
    pio_output_low(cols[current_column]);
    for (size_t current_row = 0; current_row < 7; current_row++)
    {
        if ((ship[current_column] >> current_row) & 1)
        {
            pio_output_low(rows[current_row]);
        }
        else
        {
            pio_output_high(rows[current_row]);
        }
    }
}

void move(uint8_t ship[], bool placed)
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
        if (column < col_upper_lim) {
            ship[column + 1] = ship[column];
            ship[column] = 0x0;
            prev_column = column;
            column++;
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        if (column > col_lower_lim) {
            ship[column - 1] = ship[column];
            ship[column] = 0x0;
            prev_column = column;
            column--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if (row > row_lower_lim) {
            ship[column] >>= 1;
            row--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if (row < row_upper_lim) {
            ship[column] <<= 1;
            row++;
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_PUSH))
    {
        placed = !placed;
    }
}

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
            move(large_ship, large_placed);
            displayShip(column, large_ship);
        } else if (!med_placed) {
            move(med_ship, med_placed);
            displayShip(column, med_ship);
        } else if (!small_placed) {
            move(small_ship, small_placed);
            displayShip(column, small_ship);
        }
    }
}
