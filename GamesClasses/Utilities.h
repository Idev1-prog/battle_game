#include <iostream>

#include <random>
#include <algorithm>

// AI random
class RandomNum {
private:
    static std::mt19937& engine() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
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

    static void seed(unsigned int value) {
        engine().seed(value);
    }
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