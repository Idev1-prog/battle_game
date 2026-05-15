#include <iostream>
#include "Objects.h"

BonusMalus::BonusMalus(size_t x, size_t y, char sym, int color) : Item(x, y, sym, color, false) {
	int type_num = RandomNum::get(0, 2);
	switch (type_num) {
	case 0:
		_type = Health;
		_value = RandomNum::get(-10, 30);
		return;
	case 1:
		_type = Power;
		_value = RandomNum::get(-5, 15);
		return;
	case 2:
		_type = SpecificalStats;
		_value = RandomNum::get(-2, 7);
		return;
	default:
		throw std::logic_error("Ошибка инициализации параметров _value и/или _type");
	}
}

inline void BonusMalus::use_item(Character& target) noexcept {
	target.update_stat(_type, _value);
}