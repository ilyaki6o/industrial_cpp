#include <iostream>
#include "src/gen.h"
#include <chrono>
#include <cmath>
#include <fstream>


int POPSIZE = 100;
int FIELDSIZE = 50;
int LIFESTEPS = 100;
double PMUT_INIT = 1. / (FIELDSIZE * FIELDSIZE); 
double PCROSS = 0.8;


Game str_to_game(std::string filename) {
    std::ifstream input_file(filename);
    std::string line;

    std::vector<std::vector<bool>> field;

    while (std::getline(input_file, line)) {
        int len = line.length();
        std::vector<bool> buf;

        for (int j = 0; j < len; ++j) {
            buf.push_back(line[j] == '-' ? false : true);
        }

        field.push_back(buf);
    }

    input_file.close();

    return Game(field);
}

void research() {
    double mutation_prob = PMUT_INIT * (std::pow(1.5, 0));
    double min_value = FIELDSIZE * FIELDSIZE;
    double max_value = 0;
    long max_time = 0;

    std::shared_ptr<Game> best_in_series = std::make_shared<Game>(FIELDSIZE);

    std::shared_ptr<TSurvivalFunc> surv_func = std::make_shared<DeadCells>(LIFESTEPS);
    std::shared_ptr<TSelection> selection = std::make_shared<ProportionalSelection>();
    std::shared_ptr<TCrossover> crossover = std::make_shared<TwoPointCV>(PCROSS, POPSIZE);
    std::shared_ptr<TMutation> mutation = std::make_shared<BitMutation>(mutation_prob);
    
    population_ptr pop = std::make_shared<population>();

    for (int i = 0; i < POPSIZE; i++) {
        Game cur = Game(FIELDSIZE, Initial::random);

        std::cerr << cur.toString() << std::endl;

        pop->push_back(cur);
    }

    auto now = std::chrono::system_clock::now();

    GenAlgo main_cycle(
            surv_func,
            selection,
            crossover,
            mutation,
            pop,
            POPSIZE,
            FIELDSIZE
            );

    std::shared_ptr<Game> best_solution = main_cycle.run();
    Game best_after100 = best_solution->run(100);
    int count_alive = best_after100.countAlive();

    unsigned time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - now).count();

    if (min_value > count_alive) {
        min_value = count_alive;
    }

    if (max_value < count_alive) {
        max_value = count_alive;
    }

    if (max_time < time) {
        max_time = time;
    }

    std::cout << min_value << ", " << max_value << ", " << max_time << std::endl;
}


int main(void){
    Game tmp = str_to_game("fields/planer.txt");

    // std::cout << "Before\n" << tmp.toString() << std::endl; 

    Game after100 = tmp.run(24, 2);

    // std::cout << "\nAfter: " << after100.countAlive() << "\n" << after100.toString() << std::endl; 
    //
    //

    // research();

    return 0;
}
