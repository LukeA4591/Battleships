/** @file   move.h
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Provides function delcartions for the functionality of the movement of objects on a map*/

#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include "position.h"
#include "initmove.h"

/**
 * move ship down
 * @param colUpperLim
 * @param ship
 * @param vertShip
 * @param shipNum
 * @param pos
 */
void down(uint8_t colUpperLim, uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos);

/**
 * move ship up
 * @param ship
 * @param vertShip
 * @param shipNum
 * @param pos
 */
void up (uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos);

/**
 * move ship left
 * @param ship
 * @param vertShip
 * @param shipNum
 * @param pos
 */
void left (uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos);

/**
 * move ship right
 * @param rowUpperLim
 * @param ship
 * @param vertShip
 * @param shipNum
 * @param pos
 */
void right (uint8_t rowUpperLim, uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos);

#endif