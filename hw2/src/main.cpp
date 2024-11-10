#include "struct.h"
#include <iostream>

int main(){
    std::vector<int> works = std::vector({1, 2, 3, 4, 10, 12, 3, 100, 3, 5, 17, 2, 5, 4});
    ImpScheduleView* cur_schedule = new ImpScheduleView(5, works);
    ImpScheduleView* best_schedule = new ImpScheduleView();
    best_schedule->copy(cur_schedule);

    best_schedule->print();
    std::cout << best_schedule->get_loss() << std::endl;

    std::cout << std::endl;

    ImpMutation mutation = ImpMutation();
    BoltzmannLaw temp_law = BoltzmannLaw();
    SimulateAnnealing res = SimulateAnnealing(1000, mutation, cur_schedule, best_schedule, temp_law);
    res.run();

    best_schedule->print();
    std::cout << best_schedule->get_loss() << std::endl;

    std::cout << "\nIterations: " << res.getIter() << std::endl;

    delete cur_schedule;
    delete best_schedule;

    return 0;
}

//int main(){
//    std::vector<int> works = std::vector({1, 2, 3, 4, 10, 12, 3, 100, 3, 5, 17, 2, 5, 4});
//    ImpScheduleView* cur_schedule = new ImpScheduleView(5, works);
//    ImpMutation mutation = ImpMutation();
//
//    cur_schedule->print();
//    std::cout<< cur_schedule->get_loss() << std::endl;
//    mutation.perform(cur_schedule);
//
//    std::cout << std::endl;
//
//    cur_schedule->print();
//    std::cout<< cur_schedule->get_loss() << std::endl;
//
//    return 0;
//}