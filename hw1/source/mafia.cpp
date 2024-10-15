#include <iostream>
#include <string>
#include <vector>
#include <map>


#include <boost/asio.hpp>

#include "../includes/my_shared_ptr/shared_ptr.hpp"
#include "../includes/logger.hpp"

#include "../includes/characters/civilian.hpp"
#include "../includes/characters/commissioner.hpp"
#include "../includes/characters/mafia.hpp"
#include "../includes/characters/doctor.hpp"
#include "../includes/characters/maniac.hpp"



//test

#include <ranges>

//

using namespace boost::asio;
using namespace msp;


awaitable<void> nightPhase(std::map<std::string, shared_ptr<Player>>& players, Logger& logger){
    logger.nightPhase();
    std::cout << "~~~City falling asleep~~~" << std::endl;

    io_context io_local;
    posix::stream_descriptor in(io_local, ::dup(STDIN_FILENO));

    std::map<std::string, std::string> choices;

    for (const auto& [playerName, player]: players){
        if (player->isAlive() && (player->getRole() != Role::CIVILIAN)){
            auto registerChoice = [&choices, &playerName](std::exception_ptr e, std::string chosenName){
                choices[playerName] = chosenName;
            };

            co_spawn(io_local, player->nightChoice(players, logger, in), registerChoice);
        }

    }

    io_local.run();
    in.close();

    std::cout << "\n" << std::endl;

    std::string docName = "";

    for (const auto& [playerName, targetName]: choices){
        if (players[playerName]->getRole() != Role::DOCTOR){
            if (!targetName.empty()){
                players[playerName]->act(players[targetName]);
            }
        }else{
            docName = playerName;
        }
    }

    if (!docName.empty()){
        players[docName]->act(players[choices[docName]]);
    }

    std::cout << "\n" << std::endl;

    co_return;
}


awaitable<void> dayPhase(std::map<std::string, shared_ptr<Player>>& players, Logger& logger){
    logger.dayPhase();
    std::cout << "~~~City waking up~~~" << std::endl;

    for (const auto& [playerName, player]: players){
        player->resetVotes();
    }

    io_context io_local;
    posix::stream_descriptor in(io_local, ::dup(STDIN_FILENO));

    for (const auto& [playerName, player]: players){
        if (player->isAlive()){
            if (player->getRole() != Role::MAFIA){
                co_spawn(io_local, player->vote(players, logger, in), detached);
            }else{
                co_spawn(io_local, player->vote(players, logger, in, Role::MAFIA), detached);
            }
        }
    }

    io_local.run();
    in.close();

    int max_votes = 0;
    shared_ptr<Player> resVote;

    for (const auto& [playerName, player]: players){
        if (player->getVoteRes() >= max_votes){
            max_votes = player->getVoteRes();
            resVote = player;
        }
    }

    if (max_votes){
        std::cout << "In result of vote phase player " + resVote->getName() + "(" + resVote->getStrRole() + ") out of the game." << std::endl;
        logger.log("Player " + resVote->getName() + " is out as result of vote phase.");

        resVote->kill();
    }else{
        std::cerr << "Not enought players for vote phase." << std::endl;
    }

    co_return;
}


bool endGameCheck(std::map<std::string, shared_ptr<Player>>& players, Logger& logger){
    int mafiaAlive = 0;
    int civiliansAlive = 0;
    int maniacAlive = 0;

    auto mapValues = [](const auto& pair) { return pair.second; };
    auto chooseAlive = [](const auto& player) { return player->isAlive(); };

    auto alivePlayers = players | std::views::transform(mapValues) | std::views::filter(chooseAlive);

    for (const auto& player : alivePlayers) {
        if (player->getRole() == Role::MAFIA) {
            mafiaAlive++;
        } else if(player->getRole() == Role::MANIAC) {
            maniacAlive++;
            civiliansAlive++;
        }else{
            civiliansAlive++;
        }
    }

    if (mafiaAlive >= civiliansAlive) {
        logger.final("Mafia wins!");
        std::cout << "Mafia wins!" << std::endl;
        return true; // Мафия победила
    }

    if (mafiaAlive == 0 && maniacAlive == 0) {
        logger.final("Civilian wins!");
        std::cout << "Civilian wins!" << std::endl;
        return true; // Мирные победили
    }

    if (maniacAlive == 1 && civiliansAlive == 1){
        logger.final("Maniac wins!");
        std::cout << "Maniac wins!" << std::endl;
        return true; // Маньяк победил
    }

    return false; // Игра продолжается
}


Role addUser(std::map<std::string, shared_ptr<Player>>& players, Logger& logger, const std::string& playerName){
    // Доступные роли
    std::vector<Role> roles = {
        Role::CIVILIAN,
        Role::COMMISSAR,
        Role::DOCTOR,
        Role::MAFIA,
        Role::MANIAC
    };

    // Генерация случайного индекса
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, roles.size() - 1);
    int randomIndex = dis(gen);

    // Назначаем случайную роль
    Role randomRole = roles[randomIndex];

    // Создаем соответствующий объект игрока в зависимости от назначенной роли
    switch (randomRole) {
        case Role::CIVILIAN:
            players[playerName] = shared_ptr<Player>(new Civilian(playerName, PlayerStatus::USER));
            break;
        case Role::COMMISSAR:
            players[playerName] = shared_ptr<Player>(new Commissioner(playerName, PlayerStatus::USER));
            break;
        case Role::DOCTOR:
            players[playerName] = shared_ptr<Player>(new Doctor(playerName, PlayerStatus::USER));
            break;
        case Role::MAFIA:
            players[playerName] = shared_ptr<Player>(new Mafia(playerName, PlayerStatus::USER));
            break;
        case Role::MANIAC:
            players[playerName] = shared_ptr<Player>(new Maniac(playerName, PlayerStatus::USER));
            break;
        default:
            std::cerr << "Undefined role." << std::endl;
            return Role::DEFAULT;
    }

    // Логируем результат
    logger.log("Player " + playerName + " have role " + players[playerName]->getStrRole());
    std::cout << "Your role -> " + players[playerName]->getStrRole() << std::endl;

    return players[playerName]->getRole();
}


bool initGame(std::map<std::string, shared_ptr<Player>>& players, Logger& logger){
    int N; // Количество игроков

    std::cout << "Enter number of players (N > 4): ";
    std::cin >> N;

    if (N <= 4) {
        std::cout << "Number of players must be more 4." << std::endl;
        return false;
    }

    char user_in_game_choice;
    bool user_in_game = false;
    std::string userLogin = "";

    std::cout << "Do you want to participate in the game? (y/n): ";
    std::cin >> user_in_game_choice;

    if (user_in_game_choice == 'y' || user_in_game_choice == 'Y') {
        user_in_game = true;
    }

    int k = 5;
    int mafia_count = N / k;
    if (mafia_count == 0) mafia_count = 1;
    int commissioner_count = 1;
    int doctor_count = 1;
    int maniac_count = 1;
    int civilian_count = N - mafia_count - 3;

    if (user_in_game){
        std::cout << "Please enter the login (it can't start with \"BOT\"): ";
        std::cin >> userLogin;

        if (userLogin.starts_with("BOT")){
            std::cout << "You can't choose that login. Start game again" << std::endl;
            return false;
        }

        Role userRole = addUser(players, logger, userLogin);

        switch (userRole) {
            case Role::CIVILIAN:
                civilian_count--;
                break;
            case Role::COMMISSAR:
                commissioner_count--;
                break;
            case Role::DOCTOR:
                doctor_count--;
                break;
            case Role::MAFIA:
                dynamic_cast<Mafia*>(players[userLogin].get())->setBoss();
                mafia_count--;
                break;
            case Role::MANIAC:
                maniac_count--;
                break;
            default:
                std::cerr << "Undefined role." << std::endl;
                return false;
        }
    }

    int cnt_players = 1;

    std::string donName = "";

    // Добавляем мафию
    for (int i = 0; i < mafia_count; ++i) {
        std::string botName = "BOT" + std::to_string(cnt_players);
        players[botName] = shared_ptr<Player>(new Mafia(botName));
        logger.log("Player " + botName + " have role Mafia");
        donName = botName;
        cnt_players++;
    }

    for (int i = 0; i < commissioner_count; ++i) {
        std::string botName = "BOT" + std::to_string(cnt_players);
        players[botName] = shared_ptr<Player>(new Commissioner(botName));
        logger.log("Player " + botName + " have role Commissioner");
        cnt_players++;
    }

    for (int i = 0; i < doctor_count; ++i) {
        std::string botName = "BOT" + std::to_string(cnt_players);
        players[botName] = shared_ptr<Player>(new Doctor(botName));
        logger.log("Player " + botName + " have role Doctor");
        cnt_players++;
    }

    for (int i = 0; i < maniac_count; ++i) {
        std::string botName = "BOT" + std::to_string(cnt_players);
        players[botName] = shared_ptr<Player>(new Maniac(botName));
        logger.log("Player " + botName + " have role Maniac");
        cnt_players++;
    }

    for (int i = 0; i < civilian_count; ++i) {
        std::string botName = "BOT" + std::to_string(cnt_players);
        players[botName] = shared_ptr<Player>(new Civilian(botName));
        logger.log("Player " + botName + " have role Civilian");
        cnt_players++;
    }

    if (!user_in_game || (user_in_game && players[userLogin]->getRole() != Role::MAFIA)){
        dynamic_cast<Mafia*>(players[donName].get())->setBoss();
    }

    return true;
}


awaitable<void> run_game(std::map<std::string, shared_ptr<Player>>& players, Logger& logger){
    int cnt_round = 1;

    while (true){
        logger.newRound(cnt_round);

        co_await nightPhase(players, logger);
        if (endGameCheck(players, logger)) break;
        co_await dayPhase(players, logger);
        if (endGameCheck(players, logger)) break;

        cnt_round++;
    }

    co_return;
}


awaitable<void> aiomain(io_context::executor_type& ex){
    //NOTE: Делаем локальный io_context для каждого асинхронного участка
    Logger logger("../logs");

    std::map<std::string, shared_ptr<Player>> players;

    if (!initGame(players, logger)){
        co_return;
    }

    //for (const auto& [playerName, player]: players){
    //    std::cout << player->getName() << " " << player->getStrRole() << std::endl;
    //    if (player->getRole() == Role::MAFIA){
    //        std::cout << dynamic_cast<Mafia*>(player.get())->isBoss() << std::endl;
    //    }
    //}

    co_await run_game(players, logger);

    co_return;
}


int main()
{
    using namespace msp;
    boost::asio::io_context io;
    auto ex = io.get_executor();

    co_spawn(io, aiomain(ex), detached);

    io.run();

    return 0;
}
