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



//test

#include <ranges>

//

using namespace boost::asio;
using namespace msp;


awaitable<std::string> read_input(posix::stream_descriptor& in){
    std::string line;

    auto res = co_await boost::asio::async_read_until(in, dynamic_string_buffer(line), "\n", use_awaitable);

    line.pop_back();

    co_return line;
}


awaitable<void> send_output(posix::stream_descriptor& out, std::string& str){
    auto res = co_await boost::asio::async_write(out, dynamic_string_buffer(str), use_awaitable);

    co_return;
}

awaitable<void> read_wrapper(posix::stream_descriptor& in){
    std::string res = co_await read_input(in);
    std::cout << res << std::endl;

    co_return;
}


awaitable<void> print_hello(){
    std::cout << "Hello, world!" << std::endl;
    co_return;
}


awaitable<void> aiomain(io_context::executor_type& ex){
    //NOTE: Делаем локальный io_context для каждого асинхронного участка
    Logger logger("../logs");

    shared_ptr<Player> com(new Commissioner("batya", PlayerStatus::USER));

    std::map<std::string, shared_ptr<Player>> players;
    players["p1"] = shared_ptr<Player>(new Civilian("p1"));
    players["m1"] = shared_ptr<Player>(new Mafia("m1"));
    players["m2"] = shared_ptr<Player>(new Mafia("m2"));
    dynamic_cast<Mafia*>(players["m2"].get())->setBoss();
    players["batya"] = com;

    std::cout << *com << std::endl;

    for (int i = 1; i < 3; i++){
        logger.newRound(i);
        io_context io_local;
        posix::stream_descriptor in(io_local, ::dup(STDIN_FILENO));
        posix::stream_descriptor out(io_local, ::dup(STDOUT_FILENO));

        std::map<std::string, std::string> choices;
        std::string resName = com->getName();

        auto registerChoice = [&choices, &resName](std::exception_ptr e, std::string chosenName){
            choices[resName] = chosenName;
        };

        co_spawn(io_local, com->nightChoice(players, logger, in), registerChoice);
        co_spawn(io_local, players["p1"]->vote(players, logger, in), detached);
        co_spawn(io_local, players["m1"]->vote(players, logger, in), detached);
        co_spawn(io_local, players["m2"]->vote(players, logger, in), detached);

        io_local.run();
        in.close();
        out.close();

        for (const auto& [playerName, targetName]: choices){
            players[playerName]->act(players[targetName]);
        }
    }


    std::cout << "p1 " << players["p1"]->getVoteRes() << std::endl;
    std::cout << "m2 " << players["m2"]->getVoteRes() << std::endl;
    std::cout << "m1 " << players["m1"]->getVoteRes() << std::endl;


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
