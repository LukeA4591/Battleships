#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include "position.h"

void down(uint8_t col_upper_lim, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos);

void up (uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos);

void left (uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos);

void right (uint8_t row_upper_lim, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos);

#endif