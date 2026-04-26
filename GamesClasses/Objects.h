#pragma once
#include <iostream>
#include "Utilities.h"

class Object {
protected:
	size_t _x;
	size_t _y;
public:
	Object(size_t x = 0, size_t y = 0) :
		_x(x),
		_y(y) {
	}
	Object(const Object& other) :
		_x(other._x),
		_y(other._y) { 
	}
	virtual ~Object() = default;

	void move_rand();
	void move_manual(int, int);

    size_t x() const noexcept { return _x; }
    size_t y() const noexcept { return _y; }
};

class Character : public Object {
protected:
	std::string _name;
	int _health;
	int _power;
public:
	Character() : Object() , _health(100), _power(10), _name("Unknown") {}
	Character(const Character& other) :
		Object(other),
		_health(other._health),
		_power(other._power),
		_name(other._name) {
	}
	Character(size_t x, size_t y, const std::string& name, int hp, int power) :
		Object(x, y),
		_name(name),
		_health(hp),
		_power(power) {
	}

	inline int health() const noexcept { return _health; }
	inline int power() const noexcept { return _power; }
	inline std::string name() const noexcept { return _name; }

	virtual void attack(Character&) = 0;
	virtual int attaked(const Character&, int, int) = 0;
	virtual bool is_alive() { return _health > 0; }
};

class Warrior : public Character {
    int _armor;
public:
    Warrior() : Character(0, 0, "Warrior", 150, 15), _armor(5) {}

    Warrior(const std::string& name, int hp, int power, int armor, size_t x = 0, size_t y = 0) :
        Character(x, y, name, hp, power),
        _armor((armor > 0) ? armor : 0) {
    }

    Warrior(const Warrior& other) :
        Character(other),
        _armor(other._armor) {
    }

    inline int get_armor() const { return _armor; }

    void attack(Character&) override;
    int attaked(const Character&, int, int) override;
};

class Necrolit : public Character {
    bool _second_life = true;
public:
    Necrolit() : Character(0, 0, "Necrolit", 150, 15) {}

    Necrolit(const std::string& name, int hp, int power, size_t x = 0, size_t y = 0) :
        Character(x, y, name, hp, power) {
    }

    Necrolit(const Necrolit& other) :
        Character(other),
        _second_life(other._second_life) {
    }

    inline bool get_second_life() const { return _second_life; }

    void attack(Character&) override;
    int attaked(const Character&, int, int) override;

private:
    void reincarnation() noexcept;
};

class Trickster : public Character {
    size_t _evasion;
    size_t _spiks;
public:
    Trickster() : Character(0, 0, "Trickster", 150, 15), _evasion(5), _spiks(5) {}

    Trickster(const std::string& name, int hp, int power, size_t evasion, size_t spiks, size_t x = 0, size_t y = 0) :
        Character(x, y, name, hp, power),
        _evasion(evasion),
        _spiks(spiks) {
    }

    Trickster(const Trickster& other) :
        Character(other),
        _evasion(other._evasion),
        _spiks(other._spiks) {
    }

    inline unsigned int evasion() const { return _evasion; }
    inline unsigned int spiks() const { return _spiks; }

    void attack(Character&) override;
    int attaked(const Character&, int, int) override;
};