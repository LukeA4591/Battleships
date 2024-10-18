#ifndef MAPFUNCTIONS_H
#define MAPFUNCTIONS_H
#include <stdint.h>
#include "position.h"

void initLedMat(void);

void place_ship_on_map(uint8_t ship, uint8_t map[], position_t* pos);

void displayMap(uint8_t row_pattern, uint8_t current_column);

#endif