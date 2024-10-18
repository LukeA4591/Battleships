/** @file   mapfunctions.h
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Provides function delcarations for managing the display of the game the LED mat*/


#ifndef MAPFUNCTIONS_H
#define MAPFUNCTIONS_H
#include <stdint.h>
#include "position.h"

/**
Initialises the LedMat, all LEDs off
*/
void initLedMat(void);

/**
Displays a given row_pattern on the given column of the map
*/
void placeObjectOnMap(uint8_t ship, uint8_t map[], position_t* pos);

/**
Places the ship on the currently displayed map
 */
void displayMap(uint8_t row_pattern, uint8_t current_column);

/**
Resets the maps to play again
 */
void resetMaps(void);

#endif