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

	void move_rand(size_t max_x = SIZE_MAX, size_t max_y = SIZE_MAX);
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
    Warrior(const std::string& name, int hp, int power, int armor)
        : Character(0, 0, name, hp, power),
        _armor((armor > 0) ? armor : 0) {
    }

    Warrior(size_t x, size_t y, const std::string& name, int hp, int power, int armor)
        : Character(x, y, name, hp, power),
        _armor((armor > 0) ? armor : 0) {
    }

    Warrior() : Warrior("Warrior", 150, 15, 5) {}

    Warrior(const Warrior& other) = default;

    inline int get_armor() const { return _armor; }
    void attack(Character&) override;
    int attaked(const Character&, int, int) override;
};

class Necrolit : public Character {
    bool _second_life = true;
public:
    Necrolit(const std::string& name, int hp, int power)
        : Character(0, 0, name, hp, power) {
    }

    Necrolit(size_t x, size_t y, const std::string& name, int hp, int power)
        : Character(x, y, name, hp, power) {
    }

    Necrolit() : Necrolit("Necrolit", 150, 15) {}

    Necrolit(const Necrolit& other) = default;

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
    Trickster(const std::string& name, int hp, int power, size_t evasion, size_t spiks)
        : Character(0, 0, name, hp, power),
        _evasion(evasion),
        _spiks(spiks) {
    }

    Trickster(size_t x, size_t y, const std::string& name, int hp, int power, size_t evasion, size_t spiks)
        : Character(x, y, name, hp, power),
        _evasion(evasion),
        _spiks(spiks) {
    }

    Trickster() : Trickster("Trickster", 150, 15, 5, 5) {}

    Trickster(const Trickster& other) = default;

    inline unsigned int evasion() const { return _evasion; }
    inline unsigned int spiks() const { return _spiks; }
    void attack(Character&) override;
    int attaked(const Character&, int, int) override;
};