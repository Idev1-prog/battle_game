#include <iostream>
#include "Objects.h"

void Object::move_manual(int x_shift, int y_shift) {
    if (x_shift < 0 && _x < static_cast<size_t>(-x_shift)) _x = 0;
    else _x += x_shift;

    if (y_shift < 0 && _y < static_cast<size_t>(-y_shift)) _y = 0;
    else _y += y_shift;
}

void Object::move_rand() {
    int dx = RandomNum::get(-1, 1);
    int dy = RandomNum::get(-1, 1);

    if (dx < 0 && _x < static_cast<size_t>(-dx)) _x = 0;
    else _x += dx;

    if (dy < 0 && _y < static_cast<size_t>(-dy)) _y = 0;
    else _y += dy;
}

