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

    std::string vote(std::vector<msp::shared_ptr<Player>>& alivePlayers, Logger& logger) override {
        if (alivePlayers.empty()) {
            std::cerr << "alivePlayers vector is empty!" << std::endl;
            return "";
        }

        int randIndex = getRandomPlayer(alivePlayers.size());
        msp::shared_ptr<Player> chosenPlayer = alivePlayers[randIndex];
        logger.log(
            "Игрок " + this->getName()
            + " (" + this->getStrRole() + ") "
            + "выбрал игрока" + chosenPlayer->getName()
            + " (" + chosenPlayer->getStrRole() +") "
        );

        return chosenPlayer->getName();
    }

    std::string getStrRole(){
        return "Мирный";
    }

    virtual ~Civilian() = default;
};


#endif