/** @file   button.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls the fucntionality of the pushbutton*/

#include <avr/io.h>
#include "button.h"


/** Return non-zero if button pressed.  */
int button_pressed_p (void)
{
    if ((PIND & (1<<7)) != 0) {
        return 1;
    } else {
        return 0;
    }
}


/** Initialise button1.  */
void button_init (void)
{
    DDRD &= ~(1<<7);
}
