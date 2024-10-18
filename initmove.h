#ifndef INITMOVE_H
#define INITMOVE_H

#include <stdint.h>
#include "position.h"
#include "move.h"

void move(bool* placed, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos);

#endif
