#ifndef BASE_HPP
#define BASE_HPP

#include <string>
#include <iostream>
#include <random>
#include <ranges>
#include <map>

#include "../my_shared_ptr/shared_ptr.hpp"
#include "../logger.hpp"
#include <boost/asio.hpp>


using namespace boost::asio;


enum class Role{
    DEFAULT,
    CIVILIAN,
    COMMISSAR,
    DOCTOR,
    MAFIA,
    MANIAC,
};


enum class PlayerStatus{
    BOT,
    USER
};




class Player {
protected:
    std::string name;
    bool is_alive;
    Role role;
    PlayerStatus status;
    int target;


    int getRandomIndex(const int numbPlayers) {
        if (!numbPlayers) {
            return -1;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, numbPlayers - 1);

        int randomIndex = dist(gen);
        return randomIndex;
    }

    void getRandomPlayer(std::map<std::string, msp::shared_ptr<Player>>& players, msp::shared_ptr<Player>& chosenPlayer){
        auto mapValues = [](const auto& pair){ return pair.second; };
        auto chooseAlive = [](const auto& player){ return player->isAlive(); };

        auto alivePlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAlive);

        int randIndex = getRandomIndex(std::ranges::distance(alivePlayers));

        int i = 0;

        for (const auto& player: alivePlayers){
            if (i == randIndex){
                chosenPlayer = player;
                break;
            }

            i++;
        }

        return;
    }

    awaitable<std::string> read_input(posix::stream_descriptor& in){
        std::string line;

        auto res = co_await boost::asio::async_read_until(in, dynamic_string_buffer(line), "\n", use_awaitable);

        line.pop_back();

        co_return line;
    }

    template<std::ranges::range R>
    awaitable<std::string> requestToUser(posix::stream_descriptor& in, std::string query, R&& alivePlayersNames){
        if (!in.is_open()){
            std::cerr << "Input descriptor is already closed" << std::endl;
            co_return "";
        }

        int column = 0;

        std::cout << "Alive players:";

        for (const auto& playerName: alivePlayersNames){
            if (column % 3 != 0){
                std::cout << playerName << " ";
                column++;
            }else{
                std::cout << std::endl;
                std::cout << playerName << " ";
                column = 1;
            }
        }

        std::cout << "\n" << std::endl;

        if (!query.empty()){
            std::cout << query << std::endl;
        }

        int attempt = 0;

        while (attempt < 5){
            std::string chosenName = co_await read_input(in);

            auto it = std::ranges::find(alivePlayersNames, chosenName);

            if (it != std::ranges::end(alivePlayersNames)){
                co_return chosenName;
            }

            std::cout << "Can't find player with that name. Please try again" << std::endl;

            attempt++;
        }

        std::cout << "After 5 attempts, you were unable to enter the name correctly. Skip the current phase." << std::endl;

        co_return "";
    }

public:
    Player(
        const std::string& name,
        Role start_role = Role::DEFAULT,
        PlayerStatus start_status = PlayerStatus::BOT
    ) : name(name), is_alive(true), role(start_role), status(start_status){}

    virtual awaitable<void> performNightAction(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ){
        co_return;
    }

    virtual awaitable<void> vote(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ){
        if (players.empty()) {
            std::cerr << "There is no players" << std::endl;
            co_return;
        }

        msp::shared_ptr<Player> chosenPlayer{};

        if (this->getStatus() == PlayerStatus::BOT){
            getRandomPlayer(players, chosenPlayer);
        }else{
            auto mapValues = [](const auto& pair){ return pair.second; };
            auto chooseAlive = [](const auto& player){ return player->isAlive(); };
            auto getNames = [](const auto& player){ return player->getName(); };

            auto alivePlayersNames = players | std::views::transform(mapValues) | std::views::filter(chooseAlive) | std::views::transform(getNames);

            std::string chosenName = co_await requestToUser(in, "Enter name of alive player", alivePlayersNames);

            if (chosenName.empty()){
                getRandomPlayer(players, chosenPlayer);
            }else{
                chosenPlayer = players[chosenName];
            }
        }

        logger.log(
            "Игрок " + this->getName()
            + " (" + this->getStrRole() + ") "
            + "выбрал игрока " + chosenPlayer->getName()
            + " (" + chosenPlayer->getStrRole() +")"
        );

        chosenPlayer->voteUp();

        co_return;
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

    Role getRole(){
        return role;
    }

    std::string getStrRole(){
        switch (this->getRole())
        {
        case Role::CIVILIAN:
            return "Мирный";
        case Role::COMMISSAR:
            return "Комиссар";
        case Role::DOCTOR:
            return "Доктор";
        case Role::MAFIA:
            return "Мафия";
        case Role::MANIAC:
            return "Маньяк";
        default:
            return "Неопределено";
        }
    }

    std::string getName() const {
        return name;
    }

    PlayerStatus getStatus() const {
        return status;
    }

    void voteUp(){
        target++;
    }

    void resetVotes(){
        target = 0;
    }

    int getVoteRes(){
        return target;
    }

    virtual ~Player() = default;

    friend std::ostream& operator<<(std::ostream& os, const Player& player) {
        os << player.name << (player.is_alive ? " (жив)" : " (мертв)");
        return os;
    }
};

#endif