#ifndef FLIPSHIP_H
#define FLIPSHIP_H

#include <stdint.h>
#include "map.h"

void flip (uint8_t shipNum, uint8_t ship, bool vert, uint8_t vert_ship[], uint8_t row_lim, uint8_t col_lim, uint8_t column, uint8_t row);

#endif // FLIPSHIP_H