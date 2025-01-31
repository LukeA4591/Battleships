/** @file   missile.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls the functionality for missiles/shooting in battleships*/

#include "missile.h"
#include "map.h"
#include "navswitch.h"
#include "position.h"
#include "mapfunctions.h"
#include "send.h"

uint8_t current_column = 0;

/**
Sends the position of the shot missile to the opponent
*/
void shootMissile(position_t* pos) {
    char position = (pos->row << 4) | (pos->column & 0x0F); //encode column and row into single char
    send (position);
}

/**
Uses navigation switch controls to move the missile cursor round the map
 */
void moveMissile(position_t* pos, uint8_t missile) {
    displayMap(missileMap[current_column], current_column);
    current_column++;
    if (current_column > 4)
    {
        current_column = 0;
    }
    navswitch_update ();
    if (navswitch_push_event_p (NAVSWITCH_EAST)){
        if ((pos->column < 4) && collisionCheck(missile, pos->column + 1, pos->row, missileMap)) {
            missileMap[pos->column + 1] |= (0x01 << pos->row);
            missileMap[pos->column] ^= (0x01 << pos->row);
            pos->column++;
        }
    }

    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        if ((pos->column > 0) && collisionCheck(missile, pos->column - 1, pos->row, missileMap)) {
            missileMap[pos->column - 1] |= (0x01 << pos->row);
            missileMap[pos->column] ^= (0x01 << pos->row);
            pos->column--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if ((pos->row > 0) && collisionCheck(missile, pos->column, pos->row - 1, missileMap)) {
            missileMap[pos->column] ^= (0x01 << pos->row);
            pos->row--;
            missileMap[pos->column] |= (0x01 << pos->row);
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if ((pos->row < 6) && collisionCheck(missile, pos->column, pos->row + 1, missileMap)) {
            missileMap[pos->column] ^= (0x01 << pos->row);
            pos->row++;
            missileMap[pos->column] |= (0x01 << pos->row);
        }
    }

    if(navswitch_push_event_p(NAVSWITCH_PUSH)) {
        shootMissile(pos);
    }
}