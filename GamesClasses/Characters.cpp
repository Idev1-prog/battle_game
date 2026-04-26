#include <iostream>
#include "Objects.h"

// Warrior

void Warrior::attack(Character& other) {
	int total_dmg = _power + RandomNum::get(5, 20);
	int reflected_dmg = other.attaked(*this, total_dmg, 0);

	if (reflected_dmg > 0) {
		_health -= reflected_dmg;
	}
}

int Warrior::attaked(const Character& other, int dmg, int pure_damage) {
	pure_damage += std::max(0, dmg - _armor);
	_health -= pure_damage;
	return 0;
}

// Necrolit

void Necrolit::reincarnation() noexcept {
	_health = 100;
	_power = 15;
	_second_life = false;
	//move_rand(); TBD maybe
}

void Necrolit::attack(Character& other) {
	int total_dmg = _power + RandomNum::get(3, 10);
	int reflected_dmg;

	if (_second_life) {
		reflected_dmg = other.attaked(*this, total_dmg, 0);
	}
	else {
		reflected_dmg = other.attaked(*this, 0, total_dmg);
	}

	if (reflected_dmg > 0) {
		_health -= reflected_dmg;
	}

	if (!is_alive() && _second_life) reincarnation();
}

int Necrolit::attaked(const Character& other, int dmg, int pure_damage) {
	_health -= (pure_damage + dmg);

	if (!is_alive() && _second_life) reincarnation();

	return 0;
}

// Trickster

void Trickster::attack(Character& other) {
	int total_dmg = _power + RandomNum::get(5, 10);
	int reflected_dmg = other.attaked(*this, total_dmg, 0);

	if (reflected_dmg > 0) {
		_health -= reflected_dmg;
	}
}

int Trickster::attaked(const Character& other, int dmg, int pure_damage) {
	int reflected_dmg = (dmg * static_cast<int>(_spiks)) / 100;

	if (RandomNum::get(1, 100) > _evasion) {
		_health -= (pure_damage + dmg);
	}

	return reflected_dmg;
}