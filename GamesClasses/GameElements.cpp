#include "GameElements.h"
#include <windows.h>
#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define IS_KEY_PRESSED 0x0001
#define MOVE_USER_DELAY_MS 450
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
    else if (GetAsyncKeyState(KEY_S) & IS_KEY_PRESSED) {
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

Game::Game() : is_running(true) {
    for (auto& row : _field) {
        std::fill(std::begin(row), std::end(row), '.');
    }
    auto userChar = std::make_unique<Warrior>(0, 0, "Player", 100, 10, 5);
    auto compChar = std::make_unique<Warrior>(FIELD_SIDE - 1, FIELD_SIDE - 1, "AI", 100, 10, 5);

    _user = std::move(Player(User, std::move(userChar), USER_SYMBOL));
    _computer = std::move(Player(Computer, std::move(compChar), COMPUTER_SYMBOL));
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