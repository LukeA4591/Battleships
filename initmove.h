/** @file   initmove.h
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Provides function declarations for the functionality of attaching navswitch controls to the movememnt of ships*/

#ifndef INITMOVE_H
#define INITMOVE_H

#include <stdint.h>
#include "position.h"
#include "move.h"

/**
Uses navigation switch controls to move the current ship around the map
 */
void move(bool* placed, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos);

#endif
