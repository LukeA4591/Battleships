#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t column;
    uint16_t row;
    bool vertical;
} position_t;

void reset(position_t *pos);

#endif // POSITION_H