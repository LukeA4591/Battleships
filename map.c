/** @file   map.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls functionality for managing maps and their collision checking*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "map.h"

#define NUM_COLS 5

uint8_t map[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t placedShips[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t missileMap[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

/** 
Checks that a given movement will not collide with any other map objects

 */
bool collisionCheck(uint8_t ship, uint16_t newcolumn, uint16_t newrow, uint8_t map[])
{

    if ((map[newcolumn] & (ship << newrow)) == 0x0) {
        return true;
    }
    return false;
}

/**
Checks that a given movement of a vertical ship will not collide with any other ships
 */
bool vertCollisionCheck(uint8_t shipNum, uint16_t newcolumn, uint16_t newrow)
{
    uint8_t mask = 0x01 << newrow;
    for (int i = 0; i < shipNum; i++) {
        if (((placedShips[i + newcolumn]) & mask) != 0x0) {
            return false;
        }
    }
    return true;
}

/**
Resets the current map to be equivalent to the map of placed ships
*/
void resetMap(void) {
    for (int i = 0; i < NUM_COLS; i++) {
        map[i] = placedShips[i];
    }
}

