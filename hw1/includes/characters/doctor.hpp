#ifndef DOCTOR_HPP
#define DOCTOR_HPP

#include "base.hpp"

class Doctor : public Player {
private:
    std::string last_healed_player; // Имя игрока, которого Док лечил прошлой ночью

public:
    // Конструктор
    Doctor(const std::string& name, PlayerStatus player_status = PlayerStatus::BOT)
        : Player(name, Role::DOCTOR, player_status) {}

    // Метод для проверки, может ли Доктор лечить указанного игрока
    bool canHeal(const std::string& player_name) const {
        return last_healed_player != player_name;
    }

    // Выполнение ночного действия - выбор игрока для лечения
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
            // Фильтруем живых игроков, которые не лечились прошлой ночью
            auto mapValues = [](const auto& pair) { return pair.second; };
            auto chooseAliveAndCanBeHealed = [this](const auto& player) {
                return player->isAlive() && canHeal(player->getName());
            };

            auto healablePlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAliveAndCanBeHealed);

            // Выбираем случайного игрока
            chosenPlayer = getRandomItem(healablePlayers);
        } else {
            // Если игрок является USER, ему предоставляется список живых игроков, которых можно лечить
            auto mapValues = [](const auto& pair) { return pair.second; };
            auto chooseAliveAndCanBeHealed = [this](const auto& player) {
                return player->isAlive() && canHeal(player->getName());
            };
            auto getNames = [](const auto& player) { return player->getName(); };

            auto healablePlayersNames = players | std::views::transform(mapValues)
                                                  | std::views::filter(chooseAliveAndCanBeHealed)
                                                  | std::views::transform(getNames);

            std::string chosenName = co_await requestToUser(in, "Enter the name of player, that you want to heal", healablePlayersNames);

            if (chosenName.empty()) {
                // Если имя не введено, выбираем случайного игрока
                auto healablePlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAliveAndCanBeHealed);
                chosenPlayer = getRandomItem(healablePlayers);
            } else {
                chosenPlayer = players[chosenName];
            }
        }

        // Лечим выбранного игрока
        last_healed_player = chosenPlayer->getName(); // Запоминаем последнего вылеченного игрока
        logger.log("Doctor choose for heal " + chosenPlayer->getName() + " (" + chosenPlayer->getStrRole() + ").");

        if (this->getStatus() == PlayerStatus::USER){
            std::cout << "You choose to heal player " + chosenPlayer->getName() << std::endl;
        }

        co_return chosenPlayer->getName();
    }

    virtual void act(msp::shared_ptr<Player>& target) override {
        target->setAlive();
        std::cout << "Doctor healed player " + target->getName() << std::endl;
    }

    // Деструктор
    ~Doctor() override = default;
};

#endif // DOCTOR_HPP
