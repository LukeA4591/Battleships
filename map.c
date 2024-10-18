#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "map.h"

#define NUM_COLS 5

uint8_t map[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t placedShips[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t missileMap[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

bool collision_check(uint8_t ship, uint16_t newcolumn, uint16_t newrow, uint8_t map[])
{

    if ((map[newcolumn] & (ship << newrow)) == 0x0) {
        return true;
    }
    return false;
}

bool vert_collision_check(uint8_t shipNum, uint16_t newcolumn, uint16_t newrow)
{
    uint8_t mask = 0x01 << newrow;
    for (int i = 0; i < shipNum; i++) {
        if (((placedShips[i + newcolumn]) & mask) != 0x0) {
            return false;
        }
    }
    return true;
}

void resetMap(void) {
    for (int i = 0; i < NUM_COLS; i++) {
        map[i] = placedShips[i];
    }
}
