#include <iostream>
#include "src/gen.h"


int POPSIZE = 100;
int FIELDSIZE = 50;
int LIFESTEPS = 100;
double PMUT = 1. / (FIELDSIZE * FIELDSIZE); 
double PCROSS = 0.8;


std::vector<std::vector<bool>> field = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


int main(void){
    population_ptr pop = std::make_shared<population>();
    std::shared_ptr<TSurvivalFunc> surv_func = std::make_shared<DeadCells>(LIFESTEPS);
    std::shared_ptr<TSelection> selection = std::make_shared<ProportionalSelection>();
    std::shared_ptr<TCrossover> crossover = std::make_shared<TwoPointCV>(PCROSS, POPSIZE);
    std::shared_ptr<TMutation> mutation = std::make_shared<BitMutation>(PMUT);
    
    for (int i = 0; i < POPSIZE; i++) {
        Game cur(FIELDSIZE, Initial::random);
        // int score = surv_func->surv_score(cur);

        // std::cout << "\t" << score << "\n" << cur.toString() << "\n" << std::endl;

        pop->push_back(cur);
    }

    std::cout << (1 == 1.) << std::endl;
    std::cout << "\n ------- \n" << std::endl;

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

    int best_score = surv_func->surv_score(*best_solution);

    Game best_after100 = best_solution->run(100, 0);
    // int best_score = best_solution->countAlive();


    std::cout << "\t" << best_score << "\t" << best_after100.countAlive() << std::endl;
    // std::cout << "\t" << best_score << "\n" << best_solution->toString() << std::endl;
    // for (const auto& individ: (*pop)) {
    //     int score = surv_func.surv_score(individ);
    //     std::cout << "\t" << score << "\n" << individ.toString() << "\n" << std::endl;
    // }

    // std::cout << "Before (" << s1.getSize() << "x" << s1.getSize() << "):\n" << surv_func.surv_score(s1) << "\n" << std::endl;
    //
    // s1 = s1.run(8);
    //
    // std::cout << "After (" << s1.getSize() << "x" << s1.getSize() << "):\n" << surv_func.surv_score(s1) << "\n" << std::endl;

    // std::cout << PMUT << std::endl;
    //
    // Game tmp(field);
    // 
    // std::cout << "Before\n" << tmp.toString() << std::endl; 
    //
    // Game after100 = tmp.run(10, 2);
    //
    // std::cout << "\nAfter: " << after100.countAlive() << "\n" << after100.toString() << std::endl; 

    return 0;
}
