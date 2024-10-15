#ifndef MAFIA_HPP
#define MAFIA_HPP

#include "base.hpp"
#include <set>

class Mafia : public Player {
private:
    bool is_boss = false; // Флаг, указывающий, является ли мафиози Боссом
    static inline std::set<std::string> mafiaMembersNames; // Список всех мафиози в игре
    static inline std::string bossName;

public:
    // Конструктор для создания мафиози с указанием, является ли он Боссом
    Mafia(const std::string& name, PlayerStatus status = PlayerStatus::BOT)
        : Player(name, Role::MAFIA, status) {
        mafiaMembersNames.insert(this->getName());
    }

    // Проверка, является ли мафиози Боссом
    bool isBoss() const {
        return is_boss;
    }

    void setBoss(){
        is_boss = true;
        bossName = this->getName();
    }

    // Устанавливает нового Босса, если текущий был убит
    template<std::ranges::range R>
    void assignNewBoss(R&& range, Logger& logger) {
        auto randomMafia = dynamic_cast<Mafia*>(getRandomItem(range).get());
        randomMafia->setBoss();
        logger.log("Old boss die. New boss: " + randomMafia->getName());
    }

    // Выполнение ночного действия - выбор жертвы для убийства
    virtual awaitable<std::string> nightChoice(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in
    ) override {
        if (players.empty()) {
            logger.log("There is no players.", LogLevel::WARNING);
            co_return "";
        }

        if (!players[bossName]->isAlive()){
            auto returnPlayer = [&players](const auto& name){ return players[name]; };
            auto chooseAlive = [](const auto& player){ return player->isAlive(); };
            auto aliveMafia = mafiaMembersNames | std::views::transform(returnPlayer) | std::views::filter(chooseAlive);

            assignNewBoss(aliveMafia, logger);
        }

        // Только Босс принимает окончательное решение о цели
        if (!is_boss) {
            co_return "";
        }

        // Выбираем жертву
        msp::shared_ptr<Player> chosenPlayer;

        auto mapValues = [](const auto& pair) { return pair.second; };
        auto chooseAliveAndNotMafia = [](const auto& player) {
            return player->isAlive() && player->getRole() != Role::MAFIA;
        };
        auto getNames = [](const auto& player) { return player->getName(); };

        if (this->getStatus() == PlayerStatus::BOT) {
            // Фильтруем живых игроков, которые не являются мафией
            auto aliveNonMafiaPlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAliveAndNotMafia);

            // Выбираем случайного игрока
            chosenPlayer = getRandomItem(aliveNonMafiaPlayers);
        } else {
            // Если игрок является USER, ему предоставляется список живых игроков, не являющихся мафией
            auto aliveNonMafiaPlayersNames = players | std::views::transform(mapValues)
                                                      | std::views::filter(chooseAliveAndNotMafia)
                                                      | std::views::transform(getNames);

            std::string chosenName = co_await requestToUser(in, "Enter player name, that you want to kill:", aliveNonMafiaPlayersNames);

            if (chosenName.empty()) {
                // Если имя не введено, выбираем случайного игрока
                auto aliveNonMafiaPlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAliveAndNotMafia);
                chosenPlayer = getRandomItem(aliveNonMafiaPlayers);
            } else {
                chosenPlayer = players[chosenName];
            }
        }

        // Убиваем выбранного игрока
        logger.log("Mafia boss chose player " + chosenPlayer->getName() + " (" + chosenPlayer->getStrRole() + ") and kill him.");

        if (this->getStatus() == PlayerStatus::USER){
            std::cout << "You chose player " + chosenPlayer->getName() + " to kill him." << std::endl;
        }

        std::string res = chosenPlayer->getName();

        co_return res;
    }

    virtual void act(msp::shared_ptr<Player>& target) override {
        if (is_boss){
            target->kill();
            std::cout << "Mafia kill " + target->getName() + "(" + target->getStrRole() + ")." << std::endl;
        }
    }

    virtual awaitable<void> vote(
        std::map<std::string, msp::shared_ptr<Player>>& players,
        Logger& logger,
        posix::stream_descriptor& in,
        Role exceptRole = Role::DEFAULT
    ) override {
        co_await Player::vote(players, logger, in, exceptRole);
    }
};

#endif