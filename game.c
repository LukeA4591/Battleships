#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include <stddef.h>

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

static const uint8_t bitmap[] =
        {
                0x01, 0x00, 0x00, 0x00, 0x00
        };

void displayShip(int current_column) {
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

    
    displayShip(0);


    /*
    while (1)
    {

        pacer_wait ();
        navswitch_update ();
        if (navswitch_push_event_p (NAVSWITCH_EAST)){

        }
            // Do something

        if (navswitch_push_event_p (NAVSWITCH_WEST)) {

        }
        // Do something
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {

        }
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {

        }
    }
    */
}
