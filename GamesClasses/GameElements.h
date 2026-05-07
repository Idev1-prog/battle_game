#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <conio.h>
#include "Utilities.h"
#include "Objects.h"

#define FIELD_SIDE 12
#define USER_SYMBOL '$'
#define COMPUTER_SYMBOL '%'
#define PERS_CHOOSE_MENU "Выберите тип персонажа и впишите соответсвующую цифру:\n\
1. Воин\n\
2. Некролит\n\
3. Ловкач\n\
4. Рандом"

enum PlayerType {
	User,
	Computer,
	Indefinite
};

enum CharacterType {
	TypeWarrior,
	TypeNecrolit,
	TypeTrickster
};

struct PrintInString {
	// Case 1: Single argument (handles 1 item)
	template<typename T>
	void operator()(const T& str) const {
		std::cout << str << std::endl;
	}

	// Case 2: Two or more arguments (handles 2+ items)
	// Note: We capture the first two explicitly to print them with a space, 
	// then pass the rest to the helper.
	template<typename T, typename Q, typename... Other>
	void operator()(const T& str1, const Q& str2, const Other&... strn) const {
		std::cout << str1 << " " << str2;
		print_tail(strn...); // Pass remaining args to recursive helper
		std::cout << std::endl;
	}

private:
	// Recursive step: prints the current 'first' and calls itself for the rest
	template<typename T, typename... Other>
	void print_tail(const T& first, const Other&... next) const {
		std::cout << " " << first;
		print_tail(next...);
	}

	// Base case: stops recursion when no arguments are left
	void print_tail() const {}
};

class Player {
protected:
	PlayerType _type;
	CharacterType _character_type;
	std::unique_ptr<Character> _character;
	char _personal_symbol;
public:
	Player(PlayerType type = Indefinite, CharacterType character_type = TypeWarrior, std::unique_ptr<Character> character = nullptr, char symbol = '%') :
		_type(type),
		_character_type(character_type),
		_character(std::move(character)),
		_personal_symbol(symbol) {
	}
	~Player() = default;
	const Character* get_character() const noexcept {
		return _character.get();
	}
	const CharacterType get_character_type() const noexcept { return _character_type; }

	static Player create_random_player(PlayerType type, char symbol,
		size_t x = FIELD_SIDE - 1, size_t y = FIELD_SIDE - 1);
	static Player create_promt_player(PlayerType type, char symbol, CharacterType cls, std::string name,
		int hp, int power, int specific_stat1 = 0, int specific_stat2 = 0, size_t x = 0, size_t y = 0);

	Player(Player&& other) noexcept = default;
	Player& operator=(Player&& other) noexcept = default;

	void attack(Player& target) noexcept {
		if (_character && target._character) {
			_character->attack(*target._character);
		}
	}
	bool do_move() noexcept;

	
private:
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

	void user_move();
	void computer_move();
	void print_field();
	void start_battle();

	void start();

private:
	void print_battle_info();
	bool game_end() const noexcept;
	friend std::string character_naming(CharacterType character_type);
};
