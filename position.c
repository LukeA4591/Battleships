/** @file   position.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls the position point functionality*/

#include "position.h"

/**
Resets the position row and column to the origin, resets orientation to horizontal
*/
void reset(position_t* pos)
{
    pos->column = 0;
    pos->row = 0;
    pos->vertical = false;
}
