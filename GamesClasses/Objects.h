#pragma once
#include <iostream>
#include "Utilities.h"

enum FeatureType {
    Health,
    Power,
    SpecificalStats
};

class Object {
protected:
	size_t _x;
	size_t _y;
    char _sym;
    int _color_code;
public:
	Object(size_t x = 0, size_t y = 0, char sym = '0', int color_code = 0) :
		_x(x),
		_y(y),
        _sym(sym),
        _color_code(color_code){
	}
	Object(const Object& other) :
		_x(other._x),
		_y(other._y), 
        _sym(other._sym),
        _color_code(other._color_code){ 
	}
    Object(Object&& other) noexcept = default;
	virtual ~Object() = default;

	void move_rand(size_t max_x = SIZE_MAX, size_t max_y = SIZE_MAX);
	void move_manual(int, int);

    size_t x() const noexcept { return _x; }
    size_t y() const noexcept { return _y; }
    char sym() const noexcept { return _sym; }
    int color_code() const noexcept { return _color_code; }

    Object& operator=(Object&& other) noexcept = default;
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
	Character(size_t x, size_t y, char sym, int color ,const std::string& name, int hp, int power) :
		Object(x, y, sym, color),
		_name(name),
		_health(hp),
		_power(power) {
	}

	inline int health() const noexcept { return _health; }
	inline int power() const noexcept { return _power; }
	inline std::string name() const noexcept { return _name; }

    virtual void update_stat(FeatureType, int) = 0;
	virtual void attack(Character&) = 0;
	virtual int attaked(const Character&, int, int) = 0;
	virtual bool is_alive() const noexcept { return _health > 0; }
};

class Warrior : public Character {
    int _armor;
public:
    Warrior(const std::string& name, int hp, int power, int armor)
        : Character(0, 0, '0', 0, name, hp, power),
        _armor((armor > 0) ? armor : 0) {
    }

    Warrior(size_t x, size_t y, char sym, int color, const std::string& name, int hp, int power, int armor)
        : Character(x, y, sym, color, name, hp, power),
        _armor((armor > 0) ? armor : 0) {
    }

    Warrior() : Warrior("Warrior", 150, 15, 5) {}

    Warrior(const Warrior& other) = default;

    inline int get_armor() const { return _armor; }
    void update_stat(FeatureType, int) override;
    void attack(Character&) override;
    int attaked(const Character&, int, int) override;
};

class Necrolit : public Character {
    bool _second_life = true;
public:
    Necrolit(const std::string& name, int hp, int power)
        : Character(0, 0, '0', 0, name, hp, power) {
    }

    Necrolit(size_t x, size_t y, char sym, int color, const std::string& name, int hp, int power)
        : Character(x, y, sym, color, name, hp, power) {
    }

    Necrolit() : Necrolit("Necrolit", 150, 15) {}

    Necrolit(const Necrolit& other) = default;

    inline bool get_second_life() const { return _second_life; }
    void update_stat(FeatureType, int) override;
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
        : Character(0, 0, '0', 0, name, hp, power),
        _evasion(evasion),
        _spiks(spiks) {
    }

    Trickster(size_t x, size_t y, char sym, int color, const std::string& name, int hp, int power, size_t evasion, size_t spiks)
        : Character(x, y, sym, color, name, hp, power),
        _evasion(evasion),
        _spiks(spiks) {
    }

    Trickster() : Trickster("Trickster", 150, 15, 5, 5) {}

    Trickster(const Trickster& other) = default;

    inline unsigned int evasion() const { return _evasion; }
    inline unsigned int spiks() const { return _spiks; }
    void update_stat(FeatureType, int) override;
    void attack(Character&) override;
    int attaked(const Character&, int, int) override;
};

class Item : public Object {
protected:
    bool _is_active;
public:
    Item() : Object(), _is_active(false) {};
    Item(size_t x, size_t y, char sym, int color, bool is_active) :
        Object(x, y, sym, color),
        _is_active(is_active) {
    };
    Item(const Item& other) :
        Object(other),
        _is_active(other._is_active) {
    };
    bool is_active() const noexcept { return _is_active; }

    virtual void use_item(Character&) noexcept = 0;
};

class BonusMalus : public Item {
    int _value;
    FeatureType _type;
public:
    BonusMalus(size_t x, size_t y, char sym, int color);

    inline void use_item(Character&) noexcept override;
};