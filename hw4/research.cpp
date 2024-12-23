#include "src/gen.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <fstream>


int POPSIZE = 100;
int FIELDSIZE = 50;
int LIFESTEPS = 100;
double PMUT_INIT = 1. / (FIELDSIZE * FIELDSIZE); 
double PCROSS = 0.8;


int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 9; ++i) {
        threads.emplace_back(std::thread([=]() {
            double mutation_prob = PMUT_INIT * (std::pow(1.5, i));
            double min_value = FIELDSIZE * FIELDSIZE;
            double max_value = 0;
            long max_time = 0;

            std::shared_ptr<Game> best_in_series = std::make_shared<Game>(FIELDSIZE);

            for (int j = 0; j < 10; ++j) {
                std::shared_ptr<TSurvivalFunc> surv_func = std::make_shared<DeadCells>(LIFESTEPS);
                std::shared_ptr<TSelection> selection = std::make_shared<ProportionalSelection>();
                std::shared_ptr<TCrossover> crossover = std::make_shared<TwoPointCV>(PCROSS, POPSIZE);
                std::shared_ptr<TMutation> mutation = std::make_shared<BitMutation>(mutation_prob);
                
                population_ptr pop = std::make_shared<population>();

                for (int i = 0; i < POPSIZE; i++) {
                    pop->push_back(Game(FIELDSIZE, Initial::random));
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
                int best_score = surv_func->surv_score(*best_solution);

                unsigned time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now() - now).count();

                if (min_value > best_score) {
                    best_in_series = best_solution;
                    min_value = best_score;
                }

                if (max_value < best_score) {
                    max_value = best_score;
                }

                if (max_time < time) {
                    max_time = time;
                }
                
                //TODO

                std::string filename = "result/series_" + std::to_string(i + 1) + "_run_" + std::to_string(j + 1) + "_sol";

                std::ofstream out_orig(filename + ".txt");
                out_orig << best.ToString();
                out_orig.close();

                std::ofstream out_end(filename + "_after100.txt");
                out_end << best.SkipConditions(100).ToString();
                out_end.close();
            }

            std::cout << i << ": " << min_value << ", " << max_value << ", " << max_time << std::endl;
        }));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
