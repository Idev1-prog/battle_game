#include <iostream>
#include "Objects.h"

void Object::move_manual(int x_shift, int y_shift) {
    if (x_shift < 0 && _x < static_cast<size_t>(-x_shift)) _x = 0;
    else _x += x_shift;

    if (y_shift < 0 && _y < static_cast<size_t>(-y_shift)) _y = 0;
    else _y += y_shift;
}

void Object::move_rand(size_t max_x, size_t max_y) {
    int dir = RandomNum::get(0, 4);

    if (dir == 1 && _x + 1 < max_x) _x++;
    else if (dir == 2 && _x > 0) _x--;
    else if (dir == 3 && _y + 1 < max_y) _y++;
    else if (dir == 4 && _y > 0) _y--;
}

