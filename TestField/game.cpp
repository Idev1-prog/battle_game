#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <mutex>
#include <atomic>
#include <conio.h>
#include <windows.h>
#include <chrono>

#define N 10
#define M 15

#define MOVE_USER_DELAY_MS 400
#define MOVE_COMPUTER_DELAY_MS 200
#define SLEEP_MS 50

#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_ENTER 0x0D

#define IS_KEY_PRESSED 0x8000

#define GREEN 10
#define RED 12

// ============================================================================
// ====================== ќЅЏя¬Ћ≈Ќ»≈  Ћј——ј PLAYER ============================
// ============================================================================

class Player {
    size_t _x, _y;
    static size_t max_x;
    static size_t max_y;
    char _symbol;
    int _color_code;
    int _points;
public:
    // конструктор
    Player(size_t x, size_t y, char symbol, int color_code) : _x(x), _y(y),
        _symbol(symbol), _color_code(color_code), _points(0) {
    }

    // геттеры
    inline size_t x() const noexcept;
    inline size_t y() const noexcept;
    inline int color() const noexcept;
    inline char symbol() const noexcept;
    inline int points() const noexcept;

    // методы перемещени€
    bool move_right() noexcept;
    bool move_left() noexcept;
    bool move_up() noexcept;
    bool move_down() noexcept;

    bool do_manual_move() noexcept;
    void do_rand_move() noexcept;

    inline void increase_points() noexcept;
};

// ============================================================================
// ======================= ќЅЏя¬Ћ≈Ќ»≈  Ћј——ј GAME =============================
// ============================================================================

class Game {
    std::vector<std::vector<char>> field;   // игровое поле
    Player user;                            // пользователь
    Player computer;                        // компьютер

    // дл€ параллельной работы
    std::atomic<bool> is_running;    // обща€ дл€ потоков переменна€
    std::mutex mtx;                  // мьютекс*

    /*
    * ћьютекс - исключительный доступ к общему ресурсу, дл€ другого потока
    * доступ блокируетс€.
    */

public:
    Game();

    bool chech_win(Player player) const noexcept;
    void user_move();
    void computer_move();
    void print_field();

    void start();
};

// ============================================================================
// ====================== –≈јЋ»«ј÷»я  Ћј——ј PLAYER ============================
// ============================================================================

inline size_t Player::x() const noexcept { return  _x; }
inline size_t Player::y() const noexcept { return  _y; }
inline int Player::color() const noexcept { return _color_code; }
inline char Player::symbol() const noexcept { return _symbol; }
inline int Player::points() const noexcept { return _points; }

bool Player::move_right() noexcept {
    if (_x < max_x - 1) {
        _x++;
        return true;
    }
    return false;
}

bool Player::move_left() noexcept {
    if (_x > 0) {
        _x--;
        return true;
    }
    return false;
}

bool Player::move_down() noexcept {
    if (_y < max_y - 1) {
        _y++;
        return true;
    }
    return false;
}

bool Player::move_up() noexcept {
    if (_y > 0) {
        _y--;
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

void Player::do_rand_move() noexcept {
    // C++ оформление генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> direction(1, 4);   // W, A, S, D

    bool is_move = false;
    while (1) {
        // генерируем новое направление и координаты
        int rand_direction = direction(gen);

        switch (rand_direction) {
        case 1:  // W
            is_move = move_up();
            break;
        case 2:  // A
            is_move = move_left();
            break;
        case 3:  // S
            is_move = move_down();
            break;
        case 4:  // D
            is_move = move_right();
            break;
        }

        if (is_move) break;
    }
}

inline void Player::increase_points() noexcept {
    _points++;
}

// инициализаци€ статических переменных дл€ класса
// (осуществл€етс€ вне классов и функций)
size_t Player::max_x = N;
size_t Player::max_y = M;

// ============================================================================
// ======================= –≈јЋ»«ј÷»я  Ћј——ј GAME =============================
// ============================================================================

Game::Game() : field(M, std::vector<char>(N, '.')), is_running(true),
user(0, 0, '@', GREEN), computer(N - 1, M - 1, '@', RED) {
}

bool Game::chech_win(Player player) const noexcept {
    return player.points() >= N * M * 0.05;
}

void Game::user_move() {
    auto last_move_time = std::chrono::high_resolution_clock::now();

    while (is_running) {
        auto now = std::chrono::high_resolution_clock::now();
        auto time_between_moves =
            std::chrono::duration_cast<std::chrono::milliseconds>
            (now - last_move_time).count();
        bool is_key_pressed = false;

        // --- ƒ¬»∆≈Ќ»≈ »√–ќ ј ---
        if (time_between_moves > MOVE_USER_DELAY_MS) {
            is_key_pressed = user.do_manual_move();
        }

        if (is_key_pressed) last_move_time = now;

        // --- ¬џѕќЋЌ≈Ќ»≈ ƒ≈…—“¬»я »√–ќ ќћ ---
        // (установка *, если он нажмЄт enter)
        if (GetAsyncKeyState(KEY_ENTER) & IS_KEY_PRESSED) {
            // ---  –»“»„≈— јя —≈ ÷»я: запись в поле ---
            // захватываем мьютекс дл€ безопасной записи
            std::lock_guard<std::mutex> lock(mtx);

            // ставим * только в пустую клетку ('.')
            if (field[user.y()][user.x()] == '.') {
                field[user.y()][user.x()] = '*';
                user.increase_points();
                if (chech_win(user)) is_running = false;
            }

            // защита от спама
            std::this_thread::sleep_for(
                std::chrono::milliseconds(SLEEP_MS)
            );
            is_key_pressed = true;
        }

        if (!is_key_pressed) {
            // защита от спама
            std::this_thread::sleep_for(
                std::chrono::milliseconds(SLEEP_MS)
            );
        }
    }
}

void Game::computer_move() {
    // C++ оформление генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> enter(0, 1);  // 2 варианта:
    // компьютер нажал enter (1)
    // или нет (0)

    while (is_running) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MOVE_COMPUTER_DELAY_MS)
        );

        // --- ƒ¬»∆≈Ќ»≈  ќћѕ№ё“≈–ј ---
        size_t old_x = computer.x();
        size_t old_y = computer.y();

        computer.do_rand_move();

        // --- ¬џѕќЋЌ≈Ќ»≈ ƒ≈…—“¬»я  ќћѕ№ё“≈–ќћ ---
        // (установка ?, если компьютер "нажал" enter)
        int rand_enter_press = enter(gen);
        if (rand_enter_press) {
            // ---  –»“»„≈— јя —≈ ÷»я: запись в поле ---
            // захватываем мьютекс дл€ безопасной записи
            std::lock_guard<std::mutex> lock(mtx);

            // ставим ? только в пустую клетку ('.')
            if (field[old_y][old_x] == '.') {
                field[old_y][old_x] = '?';
                computer.increase_points();
                if (chech_win(computer)) is_running = false;
            }
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(MOVE_COMPUTER_DELAY_MS)
        );
    }
}

void Game::print_field() {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO console_info;
    GetConsoleScreenBufferInfo(handle, &console_info);
    WORD saved_attributes = console_info.wAttributes;

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == user.y() && j == user.x()) {
                SetConsoleTextAttribute(handle, user.color());
                std::cout << user.symbol();
            }
            else if (i == computer.y() && j == computer.x()) {
                SetConsoleTextAttribute(handle, computer.color());
                std::cout << computer.symbol();
            }
            else {
                SetConsoleTextAttribute(handle, saved_attributes);
                std::cout << field[i][j];
            }
            SetConsoleTextAttribute(handle, saved_attributes);
            std::cout << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "\nINSTRUCTION: Use WASD to move. ENTER to place '*'.";
    std::cout << "\nFILL 5% OF FIELD TO WIN!";
}

void Game::start() {
    // запускаем в работу ход пользовател€ и ход компьютера одновременно,
    // параметр внутри скобок - л€мбда-выражение с захватом:
    // [this]() { this->user_move(); }
    std::thread userThread([this]() { this->user_move(); });
    std::thread compThread([this]() { this->computer_move(); });

    // пока пользовательский и компьютерный потоки выполн€ют обработки ходов
    // и мен€ют массив field, основной поток стирает поле и рисует заново
    while (is_running) {
        system("cls");

        // отрисовка происходит без блокировки мьютекса,
        // но есть решени€ и лучше, можете почитать
        print_field();

        // дл€ перерисовки добавлена задержка 0.3 сек.
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    userThread.join();
    compThread.join();

    // сообщение, которое будет выведено после завершени€ работы потоков
    std::cout << "END OF GAME!" << std::endl;
}

// ============================================================================
// ======================= √Ћј¬Ќјя ‘”Ќ ÷»я MAIN() =============================
// ============================================================================

int main() {
    Game game;
    game.start();

    system("pause");
    system("pause");
    system("pause");
    system("pause");
    return 0;
}

// ============================= END FILE =====================================
