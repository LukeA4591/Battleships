
#include "map.h"
#include "flipship.h"

void flipToVert (uint8_t shipNum, uint8_t vert_ship[], uint8_t col_lim, uint8_t column, uint8_t row) {
    if (column > col_lim) {
            column = col_lim;
    }
    while (!vert_collision_check(shipNum, column, row)) {
        if (column > 0) {
            column--;
        } else {
            if (row < 6) {
                row++;
            } else {
                row = 0;
            }
        }
    }
    for (int i = 0; i < shipNum; i++) {
        vert_ship[i] = (0x01 << row);
    }
    for (int i = 0; i < shipNum; i++) {
        map[i + column] |= vert_ship[i];
    }
}

void flipToHrz (uint8_t ship, uint8_t row_lim, uint8_t column, uint8_t row) {
    if (row > row_lim) {
        row = row_lim;
    }
    while(!collision_check(ship, column, row, placedShips)) {
        if (row > 0) {
            row --;
        } else {
            if (column < 4) {
                column++;
            } else {
                column = 0;
            }
        }
    }
    map[column] |= (ship << row);
}

void flip (uint8_t shipNum, uint8_t ship, bool vert, uint8_t vert_ship[], uint8_t row_lim, uint8_t col_lim, uint8_t column, uint8_t row) {
    resetMap ();
    if (!vert) {
        flipToVert (shipNum, vert_ship, col_lim, column, row);
    } else {
        flipToHrz (ship, row_lim, column, row);
    }
}