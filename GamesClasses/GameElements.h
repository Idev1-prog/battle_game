#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <conio.h>
#include <utility>
#include <unordered_set>
#include "Utilities.h"
#include "Objects.h"
#include "TVector.h"

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

class Player {
protected:
	PlayerType _type;
	CharacterType _character_type;
	std::unique_ptr<Character> _character;
public:
	Player(PlayerType type = Indefinite, CharacterType character_type = TypeWarrior, std::unique_ptr<Character> character = nullptr) :
		_type(type),
		_character_type(character_type),
		_character(std::move(character)) {
	}
	~Player() = default;
	const Character* get_character() const noexcept {
		return _character.get();
	}
	const CharacterType get_character_type() const noexcept { return _character_type; }

	static Player create_random_player(PlayerType type, char symbol,
		size_t x = FIELD_SIDE - 1, size_t y = FIELD_SIDE - 1);
	static Player create_promt_player(PlayerType type, char symbol, int color,
		CharacterType cls, std::string name,
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

class Map {
	TVector<TVector<std::pair<char, int>>> _game_map;
	TVector<TVector<std::unique_ptr<Item>>> _logical_map;

public:
	Map(int items_count);
	TVector<TVector<std::pair<char, int>>> map() const noexcept { return _game_map; }
	//Item* request_item();
	//void delete_object();
private:
	/*
	* @return first x, second y
	*/
	std::pair<int, int> to_cartesian_coordinates(int coordinate) const;
};

class Game {
	Map _field;   // игровое поле
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
