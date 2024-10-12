#ifndef CIVILIAN_HPP
#define CIVILIAN_HPP

#include <string>
#include <map>

#include "base.hpp"
#include "../my_shared_ptr/shared_ptr.hpp"
#include <boost/asio.hpp>

class Civilian : public Player {
public:
    Civilian(const std::string& name, PlayerStatus status = PlayerStatus::BOT) : Player(name, Role::CIVILIAN, status) {}

    awaitable<void> vote(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ) override{
        if (players.empty()) {
            std::cerr << "There is no players" << std::endl;
            co_return;
        }

        if (this->getStatus() == PlayerStatus::BOT){
            co_await Player::vote(players, logger, in);
            co_return;
        }

        auto mapValues = [](const auto& pair){ return pair.second; };
        auto chooseAlive = [](const auto& player){ return player->isAlive(); };
        auto getNames = [](const auto& player){ return player->getName(); };

        auto alivePlayersNames = players | std::views::transform(mapValues) | std::views::filter(chooseAlive) | std::views::transform(getNames);

        std::string chosenName = co_await requestToUser(in, "Enter name of alive player", alivePlayersNames);

        if (chosenName.empty()){
            co_await Player::vote(players, logger, in);
            co_return;
        }

        msp::shared_ptr<Player> chosenPlayer = players[chosenName];

        logger.log(
            "Игрок " + this->getName()
            + " (" + this->getStrRole() + ") "
            + "выбрал игрока" + chosenPlayer->getName()
            + " (" + chosenPlayer->getStrRole() +") "
        );

        chosenPlayer->voteUp();

        co_return;
    }

    virtual ~Civilian() = default;
};


#endif