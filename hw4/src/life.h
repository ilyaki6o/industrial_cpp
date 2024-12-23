#ifndef LIFE
#define LIFE

#include <string>
#include <vector>
#include <random>


enum class Initial{
    zeros,
    random,
};


class Game {
    int size_;
    std::vector<std::vector<bool>> field_;
    double p_alive = 0.75;

    Game step() const;

    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());

public:
    Game(int size, Initial state = Initial::zeros);
    Game(const std::vector<std::vector<bool>>& field);
    Game(const Game& other);

    int getSize() const { return size_; }
    std::vector<std::vector<bool>> getField() const { return field_; } 
    bool isStationary() const { return step() == *this; }

    std::vector<bool>& operator[] (int i) { return field_[i]; }
    bool operator== (const Game& other) const;
    Game& operator= (const Game& other);

    int countAlive() const;
    std::string toString() const;
    void PrintState(int state_in_sec) const;
    Game run(int numb_steps = 100, int state_in_sec = 0) const;
};

#endif
