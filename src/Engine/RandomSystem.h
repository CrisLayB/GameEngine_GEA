#ifndef RANDOMSYSTEM_H
#define RANDOMSYSTEM_H

#include <array>
#include <random>
#include <ctime>

class RandomSystem {
public:
    RandomSystem();

    int getRandomInt();
    std::array<int, 3> getRandomIntArray();

private:
    std::mt19937 mt;
    std::uniform_int_distribution<int> dist;
};

#endif // RANDOMSYSTEM_H
