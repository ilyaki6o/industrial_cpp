#ifndef BASE
#define BASE

#include <string>
#include <iostream>

#include "../my_shared_ptr/shared_ptr.hpp"
#include "../logger.hpp"

class Player {
protected:
    std::string name;
    bool is_alive;

public:
    Player(const std::string& name) : name(name), is_alive(true) {}

    virtual std::string performNightAction(std::vector<msp::shared_ptr<Player>>& alivePlayers, Logger& logger) {
        return "virtual func performNightAction";
    }

    virtual std::string vote(std::vector<msp::shared_ptr<Player>>& alivePlayers, Logger& logger) {
        return "virtual func vote";
    }

    bool isAlive() const {
        return is_alive;
    }

    void kill(){
        is_alive = false;
    }

    void setAlive(){
        is_alive = true;
    }

    std::string getName() const {
        return name;
    }

    virtual ~Player() = default;

    friend std::ostream& operator<<(std::ostream& os, const Player& player) {
        os << player.name << (player.is_alive ? " (жив)" : " (мертв)");
        return os;
    }
};

#endif