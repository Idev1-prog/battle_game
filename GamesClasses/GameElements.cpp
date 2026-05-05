#include "GameElements.h"
#include <windows.h>
#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define IS_KEY_PRESSED 0x0001
#define MOVE_USER_DELAY_MS 300
#define MOVE_COMPUTER_DELAY_MS 150
#define SLEEP_MS 50

bool Player::move_right() noexcept {
    if (_character->x() < FIELD_SIDE - 1) {
        _character->move_manual(1, 0);
        return true;
    }
    return false;
}

bool Player::move_left() noexcept {
    if (_character->x() > 0) {
        _character->move_manual(-1, 0);
        return true;
    }
    return false;
}

bool Player::move_down() noexcept {
    if (_character->y() < FIELD_SIDE - 1) {
        _character->move_manual(0, 1);
        return true;
    }
    return false;
}

bool Player::move_up() noexcept {
    if (_character->y() > 0) {
        _character->move_manual(0, -1);
        return true;
    }
    return false;
}

bool Player::do_manual_move() noexcept {
    bool is_key_pressed = false;
    if (GetAsyncKeyState(KEY_W) & IS_KEY_PRESSED) {
        move_up();
        is_key_pressed = true;
    }
    else if (GetAsyncKeyState(KEY_S) & IS_KEY_PRESSED) { // возможна проблема
        move_down();
        is_key_pressed = true;
    }
    else if (GetAsyncKeyState(KEY_A) & IS_KEY_PRESSED) {
        move_left();
        is_key_pressed = true;
    }
    else if (GetAsyncKeyState(KEY_D) & IS_KEY_PRESSED) {
        move_right();
        is_key_pressed = true;
    }
    return is_key_pressed;
}

bool Player::do_move() noexcept {
    switch (_type) {
    case User:
        do_manual_move();
        return true;
    case Computer:
        _character->move_rand(FIELD_SIDE, FIELD_SIDE);
        return true;
    }
    return false;
}

Player Player::create_random_player(PlayerType type, char symbol, size_t x, size_t y) {
    int char_type = RandomNum::get(0, 2);
    std::unique_ptr<Character> character;

    switch (char_type) {
    case TypeWarrior:
        character = std::make_unique<Warrior>(x, y, "AI",
            RandomNum::get(80, 200),
            RandomNum::get(10, 25),
            RandomNum::get(5, 25));
        break;
    case TypeNecrolit:
        character = std::make_unique<Necrolit>(x, y, "AI",
            RandomNum::get(80, 200),
            RandomNum::get(10, 25));
        break;
    case TypeTrickster:
        character = std::make_unique<Trickster>(x, y, "AI",
            RandomNum::get(80, 200),
            RandomNum::get(10, 25),
            RandomNum::get(5, 15),
            RandomNum::get(10, 20));
        break;
    default:
        throw std::runtime_error("Unknown character type generated");
    }

    return std::move(Player(type, std::move(character), symbol));
}

Player Player::create_promt_player(PlayerType type, char symbol,
    CharacterType cls, std::string name, int hp, int power,
    int specific_stat1, int specific_stat2,
    size_t x, size_t y) {
    std::unique_ptr<Character> character;

    switch (cls) {
    case TypeWarrior:
        character = std::make_unique<Warrior>(x, y, name, hp, power, specific_stat1);
        break;
    case TypeNecrolit:
        character = std::make_unique<Necrolit>(x, y, name, hp, power);
        break;
    case TypeTrickster:
        character = std::make_unique<Trickster>(x, y, name, hp, power, specific_stat1, specific_stat2);
        break;
    default:
        throw std::invalid_argument("Invalid character type selected");
    }

    return std::move(Player(type, std::move(character), symbol));
}

Game::Game() : is_running(true) {
    for (auto& row : _field) {
        std::fill(std::begin(row), std::end(row), '.');
    }
    _computer = Player::create_random_player(Computer, COMPUTER_SYMBOL);

    int stat1 = 0, stat2 = 0, hp, power;
    std::string name;
    CharacterType character_type;

    int user_input;
    std::cout << "МЕНЮ СОЗДАНИЯ ПЕРСОНАЖА" << std::endl;
    while (true) {
        std::cout << PERS_CHOOSE_MENU << std::endl;
        std::cin >> user_input;
        if (user_input > 0 && user_input <= 3) break;
        std::cout << "Неверный ввод" << std::endl;
    }
    character_type = static_cast<CharacterType>(user_input - 1);
    std::cout << "Отлично, теперь самое время выбрать имя и основные харрактеристики" << std::endl;
    std::cout << "Имя: ";
    std::cin >> name;
    while (true) {
        std::cout << "HP: ";
        std::cin >> hp;
        if (hp > 0) break;
        std::cout << "Неверное HP, введите его еще раз" << std::endl;
    }
    while (true) {
        std::cout << "Power: ";
        std::cin >> power;
        if (power > 0) break;
        std::cout << "Неверное Power, введите его еще раз" << std::endl;
    }
    if (character_type != TypeNecrolit)
        std::cout << "Теперь добавим персонажу особых статов" << std::endl;

    switch (character_type) {
    case TypeWarrior:
        while (true) {
            std::cout << "Броня: " << std::endl;
            std::cin >> stat1;
            if (stat1 > 0) break;
            std::cout << "Неверное значение брони, введите его заново" << std::endl;
        }
        _user = Player::create_promt_player(User, USER_SYMBOL, TypeWarrior, name, hp, power, stat1);
        break;
    case TypeTrickster:
        while (true) {
            std::cout << "Уклонение: ";
            std::cin >> stat1;
            if (stat1 > 0) break;
            std::cout << "Неверное значение уклонения, введите его заново" << std::endl;
        }
        while (true) {
            std::cout << "Шипы: ";
            std::cin >> stat2;
            if (stat2 > 0) break;
            std::cout << "Неверное значение шипов, введите его заново" << std::endl;
        }
        _user = Player::create_promt_player(User, USER_SYMBOL, TypeTrickster, name, hp, power, stat1, stat2);
        break;
    case TypeNecrolit:
        _user = Player::create_promt_player(User, USER_SYMBOL, TypeNecrolit, name, hp, power);
        break;
    default:
        throw std::logic_error("Incorrect character typing");
    }
    system("cls");
}

void Game::user_move() {
    auto last_move_time = std::chrono::high_resolution_clock::now();

    while (is_running) {
        auto now = std::chrono::high_resolution_clock::now();
        auto time_between_moves =
            std::chrono::duration_cast<std::chrono::milliseconds>
            (now - last_move_time).count();

        bool is_key_pressed = false;

        if (time_between_moves > MOVE_USER_DELAY_MS) {            
            {
                std::lock_guard<std::mutex> lock(mtx); 
                is_key_pressed = _user.do_move();
            }
        }

        if (is_key_pressed) {
            last_move_time = now;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MS));
        }
    }
}

void Game::computer_move() {
    while (is_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(MOVE_COMPUTER_DELAY_MS));
        {
            std::lock_guard<std::mutex> lock(mtx);
            _computer.do_move();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(MOVE_COMPUTER_DELAY_MS));
    }
}

void Game::print_field() {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO console_info;
    GetConsoleScreenBufferInfo(handle, &console_info);
    WORD saved_attributes = console_info.wAttributes;
    CONSOLE_CURSOR_INFO cursor_info;
    GetConsoleCursorInfo(handle, &cursor_info);
    cursor_info.bVisible = false;
    SetConsoleCursorInfo(handle, &cursor_info);

    size_t user_x = _user.get_character()->x();
    size_t user_y = _user.get_character()->y();
    size_t comp_x = _computer.get_character()->x();
    size_t comp_y = _computer.get_character()->y();

    for (size_t y = 0; y < FIELD_SIDE; ++y) {
        for (size_t x = 0; x < FIELD_SIDE; ++x) {
            if (x == user_x && y == user_y) {
                SetConsoleTextAttribute(handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                std::cout << USER_SYMBOL;
            }
            else if (x == comp_x && y == comp_y) {
                SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
                std::cout << COMPUTER_SYMBOL;
            }
            else {
                SetConsoleTextAttribute(handle, saved_attributes);
                std::cout << _field[y][x];
            }
            std::cout << ' ';
        }
        std::cout << '\n';
    }

    SetConsoleTextAttribute(handle, saved_attributes);
    cursor_info.bVisible = true;
    SetConsoleCursorInfo(handle, &cursor_info);
    std::cout << "\nWASD - Move | Q - Quit\n";
}


void Game::start() {
    std::thread t_user([this]() { this->user_move(); });
    std::thread t_comp([this]() { this->computer_move(); });

    while (is_running) {
        if (GetAsyncKeyState('Q') & IS_KEY_PRESSED) is_running = false;

        system("cls");
        print_field();
        std::this_thread::sleep_for(std::chrono::milliseconds(175));
    }

    if (t_user.joinable()) t_user.join();
    if (t_comp.joinable()) t_comp.join();
}