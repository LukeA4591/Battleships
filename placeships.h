/** @file   placeships.h
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Provides function declartions for the functionaliy of placing of ships on a map */


#ifndef PLACESHIPS_H
#define PLACESHIPS_H

#include "position.h"
#include "gamestate.h"

/**
Method to switch between the current ships as they are being placed and handle their placing
 */
void placeShips(position_t* pos, int8_t* turn, game_state_t* game_state, bool* bothDone);

#endif