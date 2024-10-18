/** @file   map.h
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Provides function delcartions for managing maps and their collision checking*/

#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t map[5];

extern uint8_t placedShips[5];

extern uint8_t missileMap[5];

/** 
Checks that a given movement will not collide with any other map objects
 */
bool collisionCheck(uint8_t ship, uint16_t newcolumn, uint16_t newrow, uint8_t map[]);

/**
Checks that a given movement of a vertical ship will not collide with any other ships
 */
bool vertCollisionCheck(uint8_t shipNum, uint16_t newcolumn, uint16_t newrow);

/**
Resets the current map to be equivalent to the map of placed ships
*/
void resetMap(void);

#endif // MAP_H