#pragma once
#include "Utilities.h"
#include "Objects.h"

enum PlayerType {
	RealPlayer,
	Computer,
	Indefinite
};

class Player {
protected:
	PlayerType _type;
	std::unique_ptr<Character> _character;
public:
	Player(PlayerType type = Indefinite, std::unique_ptr<Character> character) :
		_type(type),
		_character(std::move(character)) {
	}
	~Player() = default;

	void do_move();
};
