/** @file   fileship.h
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Provides function delcartions for the functionality of rotating ships on a map*/

#ifndef FLIPSHIP_H
#define FLIPSHIP_H

#include <stdint.h>
#include "map.h"
#include "position.h"

/**
Flips the orientation of the current ship
*/
void flip (uint8_t shipNum, uint8_t ship, bool vert, uint8_t vert_ship[], uint8_t row_lim, uint8_t col_lim, position_t* pos);

#endif // FLIPSHIP_H