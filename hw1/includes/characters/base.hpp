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
    int target_;


    template <std::ranges::range R>
    auto getRandomItem(R&& range) -> std::ranges::range_value_t<R> {
        // Get the size of the range
        auto size = std::ranges::distance(range);

        if (size == 0) {
            std::cerr << "The range is empty." << std::endl;
        }

        // Create a random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, size - 1);

        // Get a random index
        auto randomIndex = dis(gen);

        // Return the element at the random index
        return *std::ranges::next(std::ranges::begin(range), randomIndex);
    }

    awaitable<std::string> read_input(posix::stream_descriptor& in){
        std::string line;

        auto res = co_await boost::asio::async_read_until(in, dynamic_string_buffer(line), "\n", use_awaitable);

        line.pop_back();

        co_return line;
    }

    template<std::ranges::range R>
    awaitable<std::string> requestToUser(posix::stream_descriptor& in, std::string query, R&& targetList){
        if (!in.is_open()){
            std::cerr << "Input descriptor is already closed" << std::endl;
            co_return "";
        }

        int column = 0;

        std::cout << "Availabe targets:";

        for (const auto& target: targetList){
            if (column % 3 != 0){
                std::cout << target << " ";
                column++;
            }else{
                std::cout << std::endl;
                std::cout << target << " ";
                column = 1;
            }
        }

        std::cout << "\n" << std::endl;

        if (!query.empty()){
            std::cout << query << std::endl;
        }

        int attempt = 0;

        while (attempt < 5){
            std::string chosenTarget = co_await read_input(in);

            auto it = std::ranges::find(targetList, chosenTarget);

            if (it != std::ranges::end(targetList)){
                co_return chosenTarget;
            }

            std::cout << "Can't find that target. Please try again" << std::endl;

            attempt++;
        }

        std::cout << "After 5 attempts, you were unable to enter the target correctly. Skip the current phase." << std::endl;

        co_return "";
    }

public:
    Player(
        const std::string& name,
        Role start_role = Role::DEFAULT,
        PlayerStatus start_status = PlayerStatus::BOT
    ) : name(name), is_alive(true), role(start_role), status(start_status){}

    virtual awaitable<std::string> nightChoice(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ){
        co_return "";
    }

    virtual void act(msp::shared_ptr<Player>& target){}

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

        auto mapValues = [](const auto& pair){ return pair.second; };
        auto chooseAlive = [this](const auto& player){ return player->isAlive() && (player->getName() != this->getName()); };
        auto getNames = [](const auto& player){ return player->getName(); };

        if (this->getStatus() == PlayerStatus::BOT){
            auto alivePlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAlive);

            chosenPlayer = getRandomItem(alivePlayers);
        }else{
            auto alivePlayersNames = players | std::views::transform(mapValues) | std::views::filter(chooseAlive) | std::views::transform(getNames);

            std::string chosenName = co_await requestToUser(in, "Enter name of alive player", alivePlayersNames);

            if (chosenName.empty()){
                auto alivePlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAlive);

                chosenPlayer = getRandomItem(alivePlayers);
            }else{
                chosenPlayer = players[chosenName];
            }
        }

        logger.log(
            "Player " + this->getName()
            + " (" + this->getStrRole() + ") "
            + "vote for player " + chosenPlayer->getName()
            + " (" + chosenPlayer->getStrRole() +")"
        );

        if (this->getStatus() == PlayerStatus::USER){
            std::cout << "You vote for player " + chosenPlayer->getName() + " (" + chosenPlayer->getStrRole() + ")" << std::endl;
        }

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
            return "Civilian";
        case Role::COMMISSAR:
            return "Commissar";
        case Role::DOCTOR:
            return "Doctor";
        case Role::MAFIA:
            return "Mafia";
        case Role::MANIAC:
            return "Maniac";
        default:
            return "Undefined";
        }
    }

    std::string getName() const {
        return name;
    }

    PlayerStatus getStatus() const {
        return status;
    }

    void voteUp(){
        target_++;
    }

    void resetVotes(){
        target_ = 0;
    }

    int getVoteRes(){
        return target_;
    }

    virtual ~Player() = default;

    friend std::ostream& operator<<(std::ostream& os, const Player& player) {
        os << player.name << (player.is_alive ? " (alive)" : " (dead)");
        return os;
    }
};

#endif