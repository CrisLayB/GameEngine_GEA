#include "RandomSystem.h"

RandomSystem::RandomSystem()
    : mt(std::random_device{}()), dist(0, 255) {
    mt.seed(static_cast<unsigned long>(std::time(nullptr)));
}

int RandomSystem::getRandomInt() {
    return dist(mt);
}

std::array<int, 3> RandomSystem::getRandomIntArray() {
    std::array<int, 3> arr;
    for (int& val : arr) {
        val = getRandomInt();
    }
    return arr;
}
