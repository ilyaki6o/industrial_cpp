#ifndef MANIAC_HPP
#define MANIAC_HPP

#include "base.hpp"

class Maniac : public Player {
public:
    // Конструктор
    Maniac(const std::string& name, PlayerStatus player_status = PlayerStatus::BOT)
        : Player(name, Role::MANIAC, player_status) {}

    // Выполнение ночного действия - выбор игрока для убийства
    virtual awaitable<std::string> nightChoice(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ) override {
        if (players.empty()) {
            logger.log("There is no players.", LogLevel::WARNING);
            co_return "";
        }

        msp::shared_ptr<Player> chosenPlayer;

        if (this->getStatus() == PlayerStatus::BOT) {
            // Фильтруем живых игроков, исключая самого Маньяка
            auto mapValues = [](const auto& pair) { return pair.second; };
            auto chooseAlive = [this](const auto& player) {
                return player->isAlive() && player->getName() != this->getName();
            };

            auto potentialVictims = players | std::views::transform(mapValues) | std::views::filter(chooseAlive);

            // Выбираем случайного игрока
            chosenPlayer = getRandomItem(potentialVictims);
        } else {
            // Если игрок является USER, ему предоставляется список живых игроков, кроме самого Маньяка
            auto mapValues = [](const auto& pair) { return pair.second; };
            auto chooseAlive = [this](const auto& player) {
                return player->isAlive() && player->getName() != this->getName();
            };
            auto getNames = [](const auto& player) { return player->getName(); };

            auto potentialVictimsNames = players | std::views::transform(mapValues)
                                               | std::views::filter(chooseAlive)
                                               | std::views::transform(getNames);

            std::string chosenName = co_await requestToUser(in, "Enter name of player, that you want to kill", potentialVictimsNames);

            if (chosenName.empty()) {
                // Если имя не введено, выбираем случайного игрока
                auto potentialVictims = players | std::views::transform(mapValues) | std::views::filter(chooseAlive);
                chosenPlayer = getRandomItem(potentialVictims);
            } else {
                chosenPlayer = players[chosenName];
            }
        }

        // Убиваем выбранного игрока
        logger.log("Maniac(" + this->getName() + ") kill player " + chosenPlayer->getName() + "(" + chosenPlayer->getStrRole() + ").");

        if (this->getStatus() == PlayerStatus::USER){
            std::cout << "You kill player" + chosenPlayer->getName() << std::endl;
        }

        co_return chosenPlayer->getName();
    }

    virtual void act(msp::shared_ptr<Player>& target) override {
        target->kill();
        std::cout << "Maniac kill player " + target->getName() + "(" + target->getStrRole() + ")." << std::endl;
    }

    // Деструктор
    ~Maniac() override = default;
};

#endif // MANIAC_HPP
