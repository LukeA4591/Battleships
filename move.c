/** @file   move.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls the functionality of the movement of objects on a map*/

#include "move.h"
#include "map.h"
#include "position.h"

#define col_lower_lim 0
#define row_lower_lim 0

void placeVertRow(uint8_t shipNum,uint8_t vert_ship[], position_t* pos) {
    for (int i = 0; i < shipNum; i++) {
        vert_ship[i] = (0x01 << pos->row);
    }
    for (int i = 0; i < shipNum; i++) {
        map[i + pos->column] |= vert_ship[i];
    }
}

void placeHrzRow (uint8_t ship, position_t* pos) {
    map[pos->column] = placedShips[pos->column];
    map[pos->column] |= (ship << pos->row);
}

void placeVertCol(uint8_t shipNum, uint8_t vert_ship[], position_t* pos) {
    for (int i = 0; i < shipNum; i++) {
        map[i + pos->column] |= vert_ship[i];
    }
}

void placeHrzColDown (uint8_t ship, position_t* pos) {
    map[pos->column + 1] |= (ship << pos->row);
    map[pos->column] = placedShips[pos->column];
    pos->column++;
}

void placeHrzColUp (uint8_t ship, position_t* pos) {
    map[pos->column - 1] |= (ship << pos->row);
    map[pos->column] = placedShips[pos->column];
    pos->column--;
}

void hrzDown (uint8_t lim, uint8_t ship, position_t* pos) {
    if ((pos->column < lim) && collision_check(ship, pos->column + 1, pos->row, placedShips)) {
        placeHrzColDown (ship, pos);
    }
}

void vertDown (uint8_t col_upper_lim, uint8_t shipNum, uint8_t vert_ship[], position_t* pos) {
    if ((pos->column < col_upper_lim) && vert_collision_check(shipNum, pos->column + 1, pos->row)) {
        resetMap ();
        pos->column++;
        placeVertCol (shipNum, vert_ship, pos);
    }
}

void hrzUp (uint8_t ship, position_t* pos) {
    if ((pos->column > col_lower_lim) && collision_check(ship, pos->column - 1, pos->row, placedShips)) {
        placeHrzColUp (ship, pos);
    }
}

void vertUp (uint8_t vert_ship[], uint8_t shipNum, position_t* pos) {
    if ((pos->column > col_lower_lim) && vert_collision_check(shipNum, pos->column - 1, pos->row)) {
        resetMap ();
        pos->column--;
        placeVertCol (shipNum, vert_ship, pos);
    }
}

void hrzLeft (uint8_t ship, position_t* pos) {
    if ((pos->row > row_lower_lim) && collision_check(ship, pos->column, pos->row - 1, placedShips)) {
        pos->row--;
        placeHrzRow (ship, pos);
    }
}

void vertLeft (uint8_t vert_ship[], uint8_t shipNum, position_t* pos) {
    if ((pos->row > row_lower_lim) && vert_collision_check(shipNum, pos->column, pos->row - 1)) {
        resetMap ();
        pos->row--;
        placeVertRow(shipNum, vert_ship, pos);
    }
}

void hrzRight (uint8_t row_upper_lim, uint8_t ship, position_t* pos) {
    if ((pos->row < row_upper_lim) && collision_check(ship, pos->column, pos->row + 1, placedShips)) {
        pos->row++;
        placeHrzRow (ship, pos);
    }
}

void vertRight (uint8_t row_upper_lim, uint8_t shipNum, uint8_t vert_ship[], position_t* pos) {
    if ((pos->row < row_upper_lim) && vert_collision_check(shipNum, pos->column, pos->row + 1)) {
        resetMap ();
        pos->row++;
        placeVertRow(shipNum, vert_ship, pos);
    }
}

void down(uint8_t col_upper_lim, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzDown (col_upper_lim, ship, pos);
    } else {
        vertDown (col_upper_lim, shipNum, vert_ship, pos);
    }
}

void up (uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzUp (ship, pos);
    } else {
        vertUp (vert_ship, shipNum, pos);
    }
}

void left (uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzLeft (ship, pos);
    } else {
        vertLeft (vert_ship, shipNum, pos);
    }
}

void right (uint8_t row_upper_lim, uint8_t ship, uint8_t vert_ship[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzRight (row_upper_lim, ship, pos);
    } else {
        vertRight (row_upper_lim, shipNum, vert_ship, pos);
    }
}