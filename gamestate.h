#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef enum
{           
    PLACE_SHIPS,
    SEND_MAP,
    YOUR_TURN,
    THEIR_TURN,
    GAME_FINISHED,
    START_SCREEN

} game_state_t;

#endif