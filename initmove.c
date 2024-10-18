/** @file   initmove.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Listens to navswitch controls to move a ship around the map*/

#include "initmove.h"
#include <stdint.h>
#include "navswitch.h"
#include "position.h"
#include "move.h"
#include "button.h"
#include "flipship.h"

uint8_t col_upper_lim;
uint8_t row_upper_lim;

/**
Determines the movement limit of the current ship so that it cannot move off the LED mat
*/
static void setLims(position_t* pos, uint8_t shipNum) {
    if (!pos->vertical) {
        col_upper_lim = 4;
        if (shipNum == 4) {
            row_upper_lim = 3;
        } else if (shipNum == 3) {
            row_upper_lim = 4;
        } else {
            row_upper_lim = 5;
        }
    } else {
        row_upper_lim = 6;
        if (shipNum == 4) {
            col_upper_lim = 1;
        } else if (shipNum == 3) {
            col_upper_lim = 2;
        } else {
            col_upper_lim = 3;
        }
    }   
}

/**
Places the current ship on the placed ships map
*/
static void placeShip(bool* placed, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos) {
    if (!(pos->vertical)) {
        placedShips[pos->column] |= ship << pos->row;
    } else {
        for (int i = 0; i < shipNum; i++) {
            vert_ship[i] = (0x01 << pos->row);
            placedShips[i + pos->column] |= vert_ship[i];
        }
    }
    *placed = !(*placed);
}

/**
Uses navigation switch controls to move the current ship around the map
 */
void move(bool* placed, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos)
{
    setLims (pos, shipNum);
    navswitch_update ();
    if (navswitch_push_event_p (NAVSWITCH_EAST)) { 
        down(col_upper_lim, ship, vert_ship, shipNum, pos);
    }
    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        up(ship, vert_ship, shipNum, pos);
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        left(ship, vert_ship, shipNum, pos);
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        right(row_upper_lim, ship, vert_ship, shipNum, pos);
    }

    if (navswitch_push_event_p (NAVSWITCH_PUSH))
    {
        placeShip(placed, ship, vert_ship, shipNum, pos);
    }

    if (button_pressed_p ())
    {
        flip(shipNum, ship, pos->vertical, vert_ship, (row_upper_lim + 1) - shipNum, col_upper_lim - (shipNum - 1), pos);
        pos->vertical = !(pos->vertical);
    }
}