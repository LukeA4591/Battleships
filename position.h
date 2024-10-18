/** @file   move.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief Provides function delcartions for the position point functionality*/

#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t column;
    uint16_t row;
    bool vertical;
} position_t;

/**
Resets the position row and column to the origin, resets orientation to horizontal
*/
void reset(position_t *pos);

#endif // POSITION_H