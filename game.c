#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include <stddef.h>
#include <stdio.h>

#define NUM_COLS 5
#define NUM_ROWS 7

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
                0x01, 0x00, 0x00, 0x00, 0x00
        };
uint16_t prev_column = -1;
uint16_t column = 0;
uint16_t row = 0;

void displayShip(int current_column) {
    for (int i = 0; i < NUM_COLS; i++) {
        pio_output_high(cols[i]);
    }
    pio_output_low(cols[current_column]);
    for (size_t current_row = 0; current_row < 7; current_row++)
    {
        if ((bitmap[current_column] >> current_row) & 1)
        {
            pio_output_low(rows[current_row]);
        }
        else
        {
            pio_output_high(rows[current_row]);
        }
    }
}

int main (void)
{
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
        displayShip(column);
        pacer_wait ();
        navswitch_update ();
        if (navswitch_push_event_p (NAVSWITCH_EAST)){
            if (column < 4) {
                bitmap[column + 1] = bitmap[column];
                bitmap[column] = 0x0;
                prev_column = column;
                column++;
            }
        }

        if (navswitch_push_event_p (NAVSWITCH_WEST)) {
            if (column > 0) {
                bitmap[column - 1] = bitmap[column];
                bitmap[column] = 0x0;
                prev_column = column;
                column--;
            }
        }
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            if (row > 0) {
                bitmap[column] >>= 1;
                row--;
            }
        }
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            if (row < 6) {
                bitmap[column] <<= 1;
                row++;
            }
        }
    }
}
// test