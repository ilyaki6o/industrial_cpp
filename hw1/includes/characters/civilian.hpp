#ifndef CIVILIAN_HPP
#define CIVILIAN_HPP

#include <string>
#include <vector>

#include "base.hpp"
#include "../my_shared_ptr/shared_ptr.hpp"

class Civilian : public Player {
public:
    Civilian(const std::string& name) : Player(name, Role::CIVILIAN) {}

    std::string performNightAction(std::vector<msp::shared_ptr<Player>>& alivePlayers, Logger& logger) override {
        return "";
    }

    virtual ~Civilian() = default;
};


#endif