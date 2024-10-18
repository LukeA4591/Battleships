/** @file   move.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls the functionality of the movement of objects on a map*/

#include "move.h"
#include "map.h"
#include "position.h"
#include "initmove.h"

#define colLowerLim 0
#define rowLowerLim 0

void placeVertRow(uint8_t shipNum, uint8_t vertShip[], position_t* pos) {
    for (int i = 0; i < shipNum; i++) {
        vertShip[i] = (0x01 << pos->row);
    }
    for (int i = 0; i < shipNum; i++) {
        map[i + pos->column] |= vertShip[i];
    }
}

void placeHrzRow (uint8_t ship, position_t* pos) {
    map[pos->column] = placedShips[pos->column];
    map[pos->column] |= (ship << pos->row);
}

void placeVertCol(uint8_t shipNum, uint8_t vertShip[], position_t* pos) {
    for (int i = 0; i < shipNum; i++) {
        map[i + pos->column] |= vertShip[i];
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
    if ((pos->column < lim) && collisionCheck(ship, pos->column + 1, pos->row, placedShips)) {
        placeHrzColDown (ship, pos);
    }
}

void vertDown (uint8_t colUpperLim, uint8_t shipNum, uint8_t vertShip[], position_t* pos) {
    if ((pos->column < colUpperLim) && vertCollisionCheck(shipNum, pos->column + 1, pos->row)) {
        resetMap ();
        pos->column++;
        placeVertCol (shipNum, vertShip, pos);
    }
}

void hrzUp (uint8_t ship, position_t* pos) {
    if ((pos->column > colLowerLim) && collisionCheck(ship, pos->column - 1, pos->row, placedShips)) {
        placeHrzColUp (ship, pos);
    }
}

void vertUp (uint8_t vertShip[], uint8_t shipNum, position_t* pos) {
    if ((pos->column > colLowerLim) && vertCollisionCheck(shipNum, pos->column - 1, pos->row)) {
        resetMap ();
        pos->column--;
        placeVertCol (shipNum, vertShip, pos);
    }
}

void hrzLeft (uint8_t ship, position_t* pos) {
    if ((pos->row > rowLowerLim) && collisionCheck(ship, pos->column, pos->row - 1, placedShips)) {
        pos->row--;
        placeHrzRow (ship, pos);
    }
}

void vertLeft (uint8_t vertShip[], uint8_t shipNum, position_t* pos) {
    if ((pos->row > rowLowerLim) && vertCollisionCheck(shipNum, pos->column, pos->row - 1)) {
        resetMap ();
        pos->row--;
        placeVertRow(shipNum, vertShip, pos);
    }
}

void hrzRight (uint8_t rowUpperLim, uint8_t ship, position_t* pos) {
    if ((pos->row < rowUpperLim) && collisionCheck(ship, pos->column, pos->row + 1, placedShips)) {
        pos->row++;
        placeHrzRow (ship, pos);
    }
}

void vertRight (uint8_t rowUpperLim, uint8_t shipNum, uint8_t vertShip[], position_t* pos) {
    if ((pos->row < rowUpperLim) && vertCollisionCheck(shipNum, pos->column, pos->row + 1)) {
        resetMap ();
        pos->row++;
        placeVertRow(shipNum, vertShip, pos);
    }
}

void down(uint8_t colUpperLim, uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzDown (colUpperLim, ship, pos);
    } else {
        vertDown (colUpperLim, shipNum, vertShip, pos);
    }
}

void up (uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzUp (ship, pos);
    } else {
        vertUp (vertShip, shipNum, pos);
    }
}

void left (uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzLeft (ship, pos);
    } else {
        vertLeft (vertShip, shipNum, pos);
    }
}

void right (uint8_t rowUpperLim, uint8_t ship, uint8_t vertShip[], uint8_t shipNum, position_t* pos) {
    if (!pos->vertical) {
        hrzRight (rowUpperLim, ship, pos);
    } else {
        vertRight (rowUpperLim, shipNum, vertShip, pos);
    }
}