#ifndef PLACESHIPS_H
#define PLACESHIPS_H

#include "position.h"
#include "gamestate.h"

void place_ships(position_t* pos, int8_t* turn, game_state_t* game_state, bool* bothDone);

#endif