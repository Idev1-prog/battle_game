#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include "Utilities.h"
#include "Objects.h"

#define FIELD_SIDE 12
#define USER_SYMBOL '$'
#define COMPUTER_SYMBOL '%'

enum PlayerType {
	User,
	Computer,
	Indefinite
};

class Player {
protected:
	PlayerType _type;
	std::unique_ptr<Character> _character;
	char _personal_symbol;
public:
	Player(PlayerType type = Indefinite, std::unique_ptr<Character> character = nullptr, char symbol = '%') :
		_type(type),
		_character(std::move(character)),
		_personal_symbol(symbol) {
	}
	~Player() = default;
	const Character* get_character() const noexcept {
		return _character.get();
	}
	Player& operator=(Player&& other) noexcept = default;

	bool do_move() noexcept;
	//static Player create_random_player(PlayerType type, char symbol); TBD - вернет полнценно собранного пользователья (для списка инициализации)
private:
	//static std::unique_ptr<Character> generate_random_character(); TBD - генератор случайного персонажа

	bool move_right() noexcept;
	bool move_left() noexcept;
	bool move_up() noexcept;
	bool move_down() noexcept;

	bool do_manual_move() noexcept;
};

class Game {
	char _field[FIELD_SIDE][FIELD_SIDE];   // игровое поле
	Player _user;                            // пользователь
	Player _computer;                        // компьютер

	// для параллельной работы
	std::atomic<bool> is_running;    // общая для потоков переменная
	std::mutex mtx;                  // мьютекс*

	/*
	* Мьютекс - исключительный доступ к общему ресурсу, для другого потока
	* доступ блокируется.
	*/

public:
	Game();
	// Game(); Будущий инициализатор по уже выбранным классам

	//bool chech_win(Player player) const noexcept;
	void user_move();
	void computer_move();
	void print_field();

	void start();
};
