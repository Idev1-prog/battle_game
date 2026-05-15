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

void clear_input_and_wait() {
    while (_kbhit()) {
        _getch();
    }
    _getch();
}

std::string character_naming(CharacterType character_type) {
    switch (character_type) {
    case TypeWarrior:   return "Warrior";
    case TypeNecrolit:  return "Necrolit";
    case TypeTrickster: return "Trickster";
    default:            return "Unknown";
    }
}

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

void Player::item_event(std::unique_ptr<Item> item) {
    if (item == nullptr)
        throw std::invalid_argument("Параметр item не может быть nullptr");
    if (!item->is_active()) {
        item->use_item(*_character);
    }
    else
        throw std::logic_error("На данный момент объект такого типа существовать не может");
}

Player Player::create_random_player(PlayerType type, char symbol, size_t x, size_t y) {
    int char_type = RandomNum::get(0, 2);
    std::string basic_name = (type == User) ? "Random" : "AI";
    int basic_color = (type == User) ? USER_COLOR : COMPUTER_COLOR;
    std::unique_ptr<Character> character;

    switch (char_type) {
    case TypeWarrior:
        character = std::make_unique<Warrior>(x, y, symbol, basic_color, basic_name,
            RandomNum::get(80, 200),
            RandomNum::get(10, 25),
            RandomNum::get(5, 25));
        break;
    case TypeNecrolit:
        character = std::make_unique<Necrolit>(x, y, symbol, basic_color, basic_name,
            RandomNum::get(80, 200),
            RandomNum::get(10, 25));
        break;
    case TypeTrickster:
        character = std::make_unique<Trickster>(x, y, symbol, basic_color, basic_name,
            RandomNum::get(80, 200),
            RandomNum::get(10, 25),
            RandomNum::get(5, 15),
            RandomNum::get(10, 20));
        break;
    default:
        throw std::runtime_error("Unknown character type generated");
    }

    return std::move(Player(type, static_cast<CharacterType>(char_type), std::move(character)));
}

Player Player::create_promt_player(PlayerType type, char symbol, int color,
    CharacterType cls, std::string name, int hp, int power,
    int specific_stat1, int specific_stat2,
    size_t x, size_t y) {
    std::unique_ptr<Character> character;

    switch (cls) {
    case TypeWarrior:
        character = std::make_unique<Warrior>(x, y, symbol, color, name, hp, power, specific_stat1);
        break;
    case TypeNecrolit:
        character = std::make_unique<Necrolit>(x, y, symbol, color, name, hp, power);
        break;
    case TypeTrickster:
        character = std::make_unique<Trickster>(x, y, symbol, color, name, hp, power, specific_stat1, specific_stat2);
        break;
    default:
        throw std::invalid_argument("Invalid character type selected");
    }

    return std::move(Player(type, cls, std::move(character)));
}

std::pair<size_t, size_t> Map::to_cartesian_coordinates(size_t coordinate) const {
    if (coordinate > (FIELD_SIDE * FIELD_SIDE) - 1) {
        throw std::invalid_argument("Координаты выходят за возможные допсустимые пределы поля");
    }
    return { coordinate % FIELD_SIDE, coordinate / FIELD_SIDE };
}

void Map::synchronize_maps() {
    for (int y = 0; y < FIELD_SIDE; ++y) {
        for (int x = 0; x < FIELD_SIDE; ++x) {
            if (_logical_map[y][x] != nullptr) {
                _game_map[y][x].first = _logical_map[y][x]->sym();
                _game_map[y][x].second = _logical_map[y][x]->color_code();
            }
            else {
                _game_map[y][x].first = '.';
                _game_map[y][x].second = -1;
            }
        }
    }
}

Map::Map(int items_count): _game_map(FIELD_SIDE), _logical_map(FIELD_SIDE) {
    for (size_t i = 0; i < FIELD_SIDE; ++i) {
        _game_map[i].manual_memory_initialization(FIELD_SIDE);
        _logical_map[i].manual_memory_initialization(FIELD_SIDE);
    }


    float persent = ACTIVE_ITEMS_PERSENT;
    int active_items = items_count * (persent / 100);
    int unactive_items = items_count - active_items;
    for (int y = 0; y < FIELD_SIDE; ++y) {
        for (int x = 0; x < FIELD_SIDE; ++x) {
            _logical_map[y][x] = nullptr;
        }
    }

    std::unordered_set<int> excluded_values;
    int coordinate;
    for (int i = 0; i < unactive_items; ++i) {

        do {
            coordinate = RandomNum::get(1, (FIELD_SIDE * FIELD_SIDE) - 1);
        } while (excluded_values.find(coordinate) != excluded_values.end());
        excluded_values.insert(coordinate);

        auto cartesian_coordinates = to_cartesian_coordinates(coordinate);
        _logical_map[cartesian_coordinates.second][cartesian_coordinates.first] = 
            std::make_unique<BonusMalus>
            (
                cartesian_coordinates.first, cartesian_coordinates.second,
                UNACTIVE_ITEM_SYMBOL, ITEM_COLOR
            );
    }

    synchronize_maps();
}

Game::Game() : is_running(true), _field(10) {
    _computer = Player::create_random_player(Computer, COMPUTER_SYMBOL);

    int stat1 = 0, stat2 = 0, hp, power;
    std::string name;
    CharacterType character_type;

    int user_input;
    std::cout << "МЕНЮ СОЗДАНИЯ ПЕРСОНАЖА" << std::endl;
    while (true) {
        std::cout << PERS_CHOOSE_MENU << std::endl;
        std::cin >> user_input;
        if (user_input > 0 && user_input <= 4) break;
        std::cout << "Неверный ввод" << std::endl;
    }
    if (user_input == 4) {
        _user = Player::create_random_player(User, USER_SYMBOL, 0, 0);
        return;
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
            std::cout << "Броня: ";
            std::cin >> stat1;
            if (stat1 > 0) break;
            std::cout << "Неверное значение брони, введите его заново" << std::endl;
        }
        _user = Player::create_promt_player(User, USER_SYMBOL, USER_COLOR, TypeWarrior, name, hp, power, stat1);
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
        _user = Player::create_promt_player(User, USER_SYMBOL, USER_COLOR, TypeTrickster, name, hp, power, stat1, stat2);
        break;
    case TypeNecrolit:
        _user = Player::create_promt_player(User, USER_SYMBOL, USER_COLOR, TypeNecrolit, name, hp, power);
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

    auto map_screen = _field.map();
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (map_screen[user_y][user_x].first == UNACTIVE_ITEM_SYMBOL ||
            map_screen[user_y][user_x].first == ACTIVE_ITEM_SYMBOL) {
            _user.item_event(_field.request_item(user_x, user_y));
        }
        if (map_screen[comp_y][comp_x].first == UNACTIVE_ITEM_SYMBOL ||
            map_screen[comp_y][comp_x].first == ACTIVE_ITEM_SYMBOL) {
            _computer.item_event(_field.request_item(comp_x, comp_y));
        }
        _field.synchronize_maps();
    }

    for (size_t y = 0; y < FIELD_SIDE; ++y) {
        for (size_t x = 0; x < FIELD_SIDE; ++x) {
            if (x == user_x && y == user_y) {
                SetConsoleTextAttribute(handle, _user.get_character()->color_code());
                std::cout << _user.get_character()->sym();
            }
            else if (x == comp_x && y == comp_y) {
                SetConsoleTextAttribute(handle, _computer.get_character()->color_code());
                std::cout << _computer.get_character()->sym();
            }
            else {
                SetConsoleTextAttribute(handle,(map_screen[y][x].first == '.') ? saved_attributes : map_screen[y][x].second);
                std::cout << map_screen[y][x].first;
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

void Game::print_battle_info() {
    PrintInString print_in_string;

    print_in_string(">>> БОЙ <<<");

    print_in_string("\n\nВаш персонаж:");
    CharacterType user_character_type = _user.get_character_type();
    print_in_string("Класс персонажа:", character_naming(user_character_type));
    print_in_string("Имя:", _user.get_character()->name());
    print_in_string("HP:", _user.get_character()->health());
    print_in_string("Power:", _user.get_character()->power());

    print_in_string("\n\nПротивник:");
    CharacterType computer_character_type = _computer.get_character_type();
    print_in_string("Класс персонажа:", character_naming(computer_character_type));
    print_in_string("Имя:", _computer.get_character()->name());
    print_in_string("HP:", _computer.get_character()->health());
    print_in_string("Power:", _computer.get_character()->power());
}

bool Game::game_end() const noexcept {
    PrintInString print_in_string;

    bool user_status = _user.get_character()->is_alive();
    bool computer_status = _computer.get_character()->is_alive();
    if (user_status && computer_status) return false;
    else if (user_status && !computer_status) {
        print_in_string(">>> ВЫ ПОБЕДИЛИ!!! <<<");
    }
    else if (!user_status && computer_status) {
        print_in_string(">>> ВЫ ПРОИГРАЛИ!!! <<<");
    }
    else {
        print_in_string(">>> НИЧЬЯ <<<");
    }
    Sleep(5000);
    return true;
}

void Game::start_battle() {
    system("cls");
    bool is_active = true;
    while (is_active) {
        print_battle_info();
        if (game_end()) {
            is_active = false;
            break;
        }
        std::cout << "Для следующего хода нажмите любую клавишу" << std::endl;
        clear_input_and_wait();
        system("cls");
        _user.attack(_computer);
        _computer.attack(_user);
    }
}

void Game::start() {
    std::thread t_user([this]() { this->user_move(); });
    std::thread t_comp([this]() { this->computer_move(); });
    auto on_one_point = [this]() 
        {
        return this->_user.get_character()->x() == this->_computer.get_character()->x() && 
            this->_user.get_character()->y() == this->_computer.get_character()->y(); 
        };

    while (is_running) {
        if ((GetAsyncKeyState('Q') & IS_KEY_PRESSED) || on_one_point()) is_running = false;

        system("cls");
        print_field();
        std::this_thread::sleep_for(std::chrono::milliseconds(175));
    }

    if (t_user.joinable()) t_user.join();
    if (t_comp.joinable()) t_comp.join();

    Sleep(1000);
    start_battle();
    system("cls");
}

//void wait_for_key(DWORD key_code = 0x0D) {
//    std::cout << "Нажмите Enter для продолжения...";
//
//    // Сбрасываем состояние клавиши, чтобы не сработало старое нажатие
//    GetAsyncKeyState(key_code);
//
//    while (true) {
//        // Проверяем, нажата ли клавиша ПРЯМО СЕЙЧАС
//        if (GetAsyncKeyState(key_code) & 0x8000) {
//            // Небольшая задержка, чтобы избежать "дребезга" (multiple triggers)
//            std::this_thread::sleep_for(std::chrono::milliseconds(200));
//            break;
//        }
//        // Даём процессору отдохнуть, чтобы не грузить ядро
//        std::this_thread::sleep_for(std::chrono::milliseconds(50));
//    }
//} maybe I'll use it later