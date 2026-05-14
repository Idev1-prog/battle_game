#pragma once
#include <iostream>
#include <random>
#include <algorithm>

#define ACTIVE_ITEMS_PERSENT 0 // после необходимо заменить на 10
#define USER_SYMBOL '$'
#define COMPUTER_SYMBOL '%'
#define UNACTIVE_ITEM_SYMBOL '@'
#define ACTIVE_ITEM_SYMBOL '!'
#define USER_COLOR 0x0002 | 0x0008
#define COMPUTER_COLOR 0x0004 | 0x0008
#define ITEM_COLOR 0x0001 | 0x0008
#define FIELD_SIDE 12

// AI random
class RandomNum {
private:
    static std::mt19937& engine() {
        thread_local static std::mt19937 gen{ std::random_device{}() };
        return gen;
    }

public:
    static int get(int min, int max) {
        if (min > max) std::swap(min, max);
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine());
    }

    static double get(double min, double max) {
        if (min > max) std::swap(min, max);
        std::uniform_real_distribution<double> dist(min, max);
        return dist(engine());
    }

    // Sets the seed ONLY for the CURRENT stream generator
    static void seed(unsigned int value) {
        engine().seed(value);
    }
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

/*
class RandomNum {
private:
    static void init() {
        static bool initialized = false;
        if (!initialized) {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            initialized = true;
        }
    }
public:
    static int get(int min, int max) {
        init();
        return min + (std::rand() % (max - min + 1));
    }
};
*/