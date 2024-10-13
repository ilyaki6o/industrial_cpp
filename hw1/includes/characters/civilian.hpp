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

    virtual ~Civilian() = default;
};


#endif