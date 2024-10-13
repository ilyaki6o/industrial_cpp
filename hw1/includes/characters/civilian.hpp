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

    virtual awaitable<void> vote(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ) override {
        co_await Player::vote(players, logger, in);
    }

    virtual ~Civilian() = default;
};


#endif