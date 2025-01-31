/** @file   placeships.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls the placing of ships on a map */

#include "placeships.h"
#include "map.h"
#include "position.h"
#include "initmove.h"
#include "mapfunctions.h"
#include "send.h"

#define NUM_COLS 5

bool large_placed = false;
bool med_placed = false;
bool small_placed = false;
static uint8_t large_ship = 0x0F;
static uint8_t med_ship = 0x07;
static uint8_t small_ship = 0x03;
uint8_t large_ship_vert[4] = {0x01, 0x01, 0x01, 0x01};
uint8_t med_ship_vert[3] = {0x01, 0x01, 0x01};
uint8_t small_ship_vert[2] = {0x01, 0x01};
uint8_t largeShipNum = 4;
uint8_t medShipNum = 3;
uint8_t smallShipNum = 2;
uint8_t current_colu = 0;
bool placeLarge = true;
bool planeMed = true;
bool placeSmall = true;


/**
Method to switch between the current ships as they are being placed and handle their placing
 */
void placeShips(position_t* pos, int8_t* turn, game_state_t* game_state, bool* bothDone) {
    displayMap(map[current_colu], current_colu);
    current_colu++;
    if (current_colu > NUM_COLS - 1)
    {
        current_colu = 0;
    }
    if (!large_placed) {
        if (placeLarge) {
            placeObjectOnMap (large_ship, map, pos);
            placeLarge = !placeLarge;
        }
        move(&large_placed, large_ship, large_ship_vert, largeShipNum, pos);
    } else if (!med_placed) {
        if (planeMed){
            reset(pos);
            placeObjectOnMap (med_ship, map, pos);
            planeMed = !planeMed;
        }
        move(&med_placed, med_ship, med_ship_vert, medShipNum, pos);
    } else if (!small_placed) {
        if (placeSmall){
            reset(pos);
            placeObjectOnMap (small_ship, map, pos);
            placeSmall = !placeSmall;
        }
        move(&small_placed, small_ship, small_ship_vert, smallShipNum, pos);
    } else {
        if (*turn == -1) {
            *turn = 1;
            send ('a');
            *game_state = SEND_MAP;
        } else {
            send ('b');
            *bothDone = true;
            *game_state = SEND_MAP;
        }
        placeLarge = true;
        planeMed = true;
        placeSmall = true;
        large_placed = false;
        med_placed = false;
        small_placed = false;
    }
}