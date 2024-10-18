
#include "map.h"
#include "flipship.h"
#include "position.h"

void flipToVert (uint8_t shipNum, uint8_t vert_ship[], uint8_t col_lim, position_t* pos) {
    if (pos->column > col_lim) {
            pos->column = col_lim;
    }
    while (!vert_collision_check(shipNum, pos->column, pos->row)) {
        if (pos->column > 0) {
            pos->column--;
        } else {
            if (pos->row < 6) {
                pos->row++;
            } else {
                pos->row = 0;
            }
        }
    }
    for (int i = 0; i < shipNum; i++) {
        vert_ship[i] = (0x01 << pos->row);
    }
    for (int i = 0; i < shipNum; i++) {
        map[i + pos->column] |= vert_ship[i];
    }
}

void flipToHrz (uint8_t ship, uint8_t row_lim, position_t* pos) {
    if (pos->row > row_lim) {
        pos->row = row_lim;
    }
    while(!collision_check(ship, pos->column, pos->row, placedShips)) {
        if (pos->row > 0) {
            pos->row --;
        } else {
            if (pos->column < 4) {
                pos->column++;
            } else {
                pos->column = 0;
            }
        }
    }
    map[pos->column] |= (ship << pos->row);
}

void flip (uint8_t shipNum, uint8_t ship, bool vert, uint8_t vert_ship[], uint8_t row_lim, uint8_t col_lim, position_t* pos) {
    resetMap ();
    if (!vert) {
        flipToVert (shipNum, vert_ship, col_lim, pos);
    } else {
        flipToHrz (ship, row_lim, pos);
    }
}