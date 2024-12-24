#include "gen.h"
#include <utility>
#include <iostream>


int DeadCells::surv_score(const Game& field) {
    int field_size = field.getSize();
    const Game& after_run = field.run(numbSteps);

    if (after_run.isStationary()){
        return 0; 
    }

    return field_size * field_size - after_run.countAlive(); 
}

population_ptr ProportionalSelection::select(
        const population_ptr& pop,
        const std::vector<int>& scores        
) {
    gen.seed(rd());
    std::uniform_real_distribution<> rand_p = std::uniform_real_distribution(0.0, 1.0);

    population_ptr selected = std::make_shared<population>();   
    int sum_score = 0;

    for (const auto& score: scores) sum_score += score;

    double mean_score = (double)sum_score / pop->size();

    if (!mean_score) return pop;

    for (int i = 0; i < pop->size(); i++){
        for (int j = 1; j <= scores[i] / mean_score; j++){
                selected->push_back((*pop)[i]);
        }

        if (rand_p(gen) < (scores[i] / mean_score) - (int)(scores[i] / mean_score)) {
            selected->push_back((*pop)[i]);
        }
    }

    if (selected->size() == 0) {
        std::cout << "WARNING: selected size == 0: mean_score == " << mean_score;
        std::cout << " sum_score == " << sum_score << " score_size: " << scores.size() << std::endl;
    }

    return selected;
}


population_ptr TwoPointCV::crossing(const population_ptr& pop) {
    if (pop->size() == 0) {
        std::cout << "(crossing) Pop size == 0" << std::endl;
        throw std::exception();  
    } 

    gen.seed(rd());
    std::uniform_real_distribution<> rand_p(0.0, 1.0);
    std::uniform_int_distribution<> rand_individ(0, pop->size() - 1);
    
    int row_size = (*pop)[0].getSize();
    int bit_lenght = row_size * row_size;

    std::uniform_int_distribution<> rand_point(0, bit_lenght - 1);

    int p1 = rand_point(gen);
    int p2 = rand_point(gen);

    while (p1 == p2) p2 = rand_point(gen);
    if (p1 > p2) std::swap(p1, p2);

    int p1_i = p1 / row_size, p1_j = p1 % row_size;
    int p2_i = p2 / row_size, p2_j = p2 % row_size;

    population_ptr new_pop = std::make_shared<population>();   

    while (new_pop->size() < Npop) {
        int idx1 = rand_individ(gen);
        int idx2 = rand_individ(gen);

        while (idx1 == idx2) idx2 = rand_individ(gen);

        if (rand_p(gen) < crossover_p) {
            std::vector<std::vector<bool>> child1, child2;           

            for (int i = 0; i < row_size; i++){
                std::vector<bool> row_c1, row_c2;

                for (int j = 0; j < row_size; j++){
                    if (
                            (i == p1_i && j >= p1_j)
                            || (i > p1_i && i < p2_i)
                            || (i == p2_i && j <= p2_j)
                    ){
                        row_c1.push_back((*pop)[idx2][i][j]);
                        row_c2.push_back((*pop)[idx1][i][j]);
                    }else {
                        row_c1.push_back((*pop)[idx1][i][j]);
                        row_c2.push_back((*pop)[idx2][i][j]);
                    }
                }

                child1.push_back(row_c1);
                child2.push_back(row_c2);
            }

            Game tmp1 = Game(child1);
            Game tmp2 = Game(child2);

            new_pop->push_back(tmp1);
            new_pop->push_back(tmp2);
        }
    }

    return new_pop;
}


void BitMutation::mutate(population_ptr& pop) {
    gen.seed(rd());
    std::uniform_real_distribution<> rand_p(0.0, 1.0);

    for (auto& individ: (*pop)){
        for (int i = 0; i < individ.getSize(); i++){
            for (int j = 0; j < individ.getSize(); j++){
                if (rand_p(gen) < Pmut){
                    individ[i][j] = !individ[i][j];
                }  
            }
        } 
    }
}


int GenAlgo::selectBest(const population_ptr pop, const std::vector<int>& scores, std::shared_ptr<Game> best_individ) {
    if (pop->size() == 0) {
        std::cout << "(selectBest) Pop size == 0" << std::endl;
        throw std::exception();
    }

    int best_score = scores[0];
    *best_individ = (*pop)[0];

    for (int i = 0; i < population_size; ++i) {
        int cur_score = scores[i];
        
        if (cur_score > best_score) {
            best_score = cur_score;
            *best_individ = (*pop)[i];
        }
    }

    if (best_score < 0) {
        std::cout << "(selectBest) Best score < 0" << std::endl;
        throw std::exception();    
    }

    return best_score;
}


std::shared_ptr<Game> GenAlgo::run() {
    std::vector<int> surv_scores(population_size);

    for (int i = 0; i < population_size; ++i) {
        surv_scores[i] = surv_func->surv_score((*pop)[i]);
    }

    std::shared_ptr<Game> best_solution = std::make_shared<Game>(field_size);
    int best_score = selectBest(pop, surv_scores, best_solution);

    int steps_without_inc = 0;

    while (steps_without_inc < 50) {
        population_ptr selected = selection->select(pop, surv_scores);
        pop = crossover->crossing(selected);
        mutation->mutate(pop);

        for (int i = 0; i < population_size; ++i) {
            surv_scores[i] = surv_func->surv_score((*pop)[i]);
        }

        std::shared_ptr<Game> best_in_pop = std::make_shared<Game>(field_size);
        int pop_best_score = selectBest(pop, surv_scores, best_in_pop);

        if (pop_best_score > best_score) {
            best_score = pop_best_score;
            *best_solution = *best_in_pop;
            steps_without_inc = 0;
        }else {
            steps_without_inc++;
        }
    }

    return best_solution;
}
