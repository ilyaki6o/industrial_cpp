#ifndef COMMISSAR_HPP
#define COMMISSAR_HPP

#include <set>

#include "base.hpp"


enum class ACTION{
    CHECK,
    KILL,
};


class Commissioner : public Player {
private:
    std::set<std::string> foundMafiasNames; // Вектор для хранения найденных мафий
    std::set<std::string> notMafiasNames;
    ACTION currentAction;

public:
    // Конструктор
    Commissioner(
        const std::string& name,
        PlayerStatus start_status = PlayerStatus::BOT
    ) : Player(name, Role::COMMISSAR, start_status),
        currentAction(ACTION::CHECK) {}

    // Метод выполнения ночного действия
    virtual awaitable<std::string> nightChoice(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ) override {
        if (players.empty()) {
            logger.log("There is no players.", LogLevel::WARNING);
            co_return "";
        }

        // Если у комиссара есть статус USER, он может выбрать, убивать ли мафию
        if (!foundMafiasNames.empty()){
            if (this->getStatus() == PlayerStatus::USER) {
                std::string choice = co_await requestToUser(
                    in,
                    "There are recognized mafias. You want to kill one?",
                    std::set{"yes", "no"} | std::views::all
                );

                if (choice == "yes"){
                    auto chooseAlive = [&players](const auto& name) { return players[name]->isAlive(); };
                    auto aliveMafiasNames = foundMafiasNames | std::views::filter(chooseAlive);

                    std::string chosenMafName = co_await requestToUser(in, "Enter name of mafia player", aliveMafiasNames);

                    if (chosenMafName.empty()){
                        chosenMafName = getRandomItem(aliveMafiasNames);
                    }

                    //players[chosenMafName]->kill();
                    currentAction = ACTION::KILL;
                    logger.log("Commissioner " + this->getName() + " chose to kill mafia: " + players[chosenMafName]->getName());

                    std::cout << "You chose to kill mafia: " + players[chosenMafName]->getName() << std::endl;
                    co_return chosenMafName;
                }
            }else{
                auto chooseAlive = [&players](const auto& name) { return players[name]->isAlive(); };
                auto aliveMafiasNames = foundMafiasNames | std::views::filter(chooseAlive);

                std::string chosenMafName = getRandomItem(aliveMafiasNames);

                currentAction = ACTION::KILL;
                logger.log("Commissioner " + this->getName() + " chose to kill mafia: " + players[chosenMafName]->getName());

                co_return chosenMafName;
            }
        }

        // Выполняем проверку, если мафия не убита
        std::string chosenPlayerName;

        auto mapValues = [](const auto& pair) { return pair.second; };
        auto chooseAlive = [this](const auto& player) { return player->isAlive() && (player->getName() != this->getName()); };
        auto getName = [](const auto& player) { return player->getName(); };
        auto notRecognized = [this](const auto& name) {
            return !(foundMafiasNames.count(name) || notMafiasNames.count(name));
        };
        auto notRecognizedPlayersNames = players | std::views::transform(mapValues)
                                    | std::views::filter(chooseAlive)
                                    | std::views::transform(getName)
                                    | std::views::filter(notRecognized);

        if (this->getStatus() == PlayerStatus::BOT) {
            chosenPlayerName = getRandomItem(notRecognizedPlayersNames);
        } else {
            std::string chosenName = co_await requestToUser(in, "Enter name of alive player for role check", notRecognizedPlayersNames);

            if (chosenName.empty()) {
                chosenPlayerName = getRandomItem(notRecognizedPlayersNames);
            } else {
                chosenPlayerName = chosenName;
            }
        }

        auto chosenPlayer = players[chosenPlayerName];

        currentAction = ACTION::CHECK;

        // Выполняем проверку статуса игрока
        if (chosenPlayer->getRole() == Role::MAFIA) {
            foundMafiasNames.insert(chosenPlayerName); // Записываем мафию в список найденных
        }else{
            notMafiasNames.insert(chosenPlayerName);
        }

        logger.log("Commissioner " + this->getName() + " checked player " + chosenPlayer->getName() + ". He is " + chosenPlayer->getStrRole());

        if (this->getStatus() == PlayerStatus::USER){
            std::cout << "You checked player " + chosenPlayer->getName() + ". He is " + chosenPlayer->getStrRole() << std::endl;
        }

        co_return chosenPlayerName;
    }

    virtual void act(msp::shared_ptr<Player>& target) override {
        if (currentAction == ACTION::CHECK){
            std::cout << "Commissioner checked player " + target->getName() + "." << std::endl;
        }else if (currentAction == ACTION::KILL){
            target->kill();
            foundMafiasNames.erase(target->getName());
            std::cout << "Commissioner kill player " + target->getName() + "(" + target->getStrRole() + ")." << std::endl;
        }
    }

    // Голосование (Комиссар действует как обычный игрок)
    virtual awaitable<void> vote(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in,
        Role exceptRole = Role::DEFAULT
    ) override {
        co_await Player::vote(players, logger, in);
    }
};


#endif