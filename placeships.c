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
bool ship1 = true;
bool ship2 = true;
bool ship3 = true;

void place_ships(position_t* pos, int8_t* turn, game_state_t* game_state, bool* bothDone) {
    displayMap(map[current_colu], current_colu);
    current_colu++;
    if (current_colu > NUM_COLS - 1)
    {
        current_colu = 0;
    }
    if (!large_placed) {
        if (ship1) {
            place_ship_on_map (large_ship, map, pos);
            ship1 = !ship1;
        }
        move(&large_placed, large_ship, large_ship_vert, largeShipNum, pos);
    } else if (!med_placed) {
        if (ship2){
            reset(pos);
            place_ship_on_map (med_ship, map, pos);
            ship2 = !ship2;
        }
        move(&med_placed, med_ship, med_ship_vert, medShipNum, pos);
    } else if (!small_placed) {
        if (ship3){
            reset(pos);
            place_ship_on_map (small_ship, map, pos);
            ship3 = !ship3;
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
    }
}