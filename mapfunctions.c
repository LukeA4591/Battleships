/** @file   mapfunctions.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module to control the display of the game on the LED mat*/

#include <mapfunctions.h>
#include <stddef.h>
#include "pio.h"

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



static uint8_t prev_column = 4;


/**
Initialises the LedMat, all LEDs off
*/
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

/**
Displays a given row_pattern on the given column of the map
*/
void displayMap(uint8_t row_pattern, uint8_t current_column)
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

/**
Places the ship on the currently displayed map
 */
void placeObjectOnMap(uint8_t ship, uint8_t map[], position_t* pos) {
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 5; j++){
            if ((map[j] & (ship << i)) == 0) {
                map[j] |= (ship << i);
                pos->column = j;
                pos->row = i;
                return;
            }
        }
    }
}
