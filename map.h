#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t map[5];

extern uint8_t placedShips[5];

extern uint8_t missileMap[5];

bool collision_check(uint8_t ship, uint16_t newcolumn, uint16_t newrow, uint8_t map[]);

bool vert_collision_check(uint8_t shipNum, uint16_t newcolumn, uint16_t newrow);



void resetMap(void);

#endif // MAP_H