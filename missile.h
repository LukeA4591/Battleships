/** @file   missile.h
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Provides function delcarations for the functionality of missiles/shooting in battleships*/

#ifndef MOVEMISSILE_H
#define MOVEMISSILE_H

#include "position.h"

/**
Uses navigation switch controls to move the missile cursor round the map
 */
void moveMissile(position_t* pos, uint8_t missile);

#endif // MOVEMISSILE_H