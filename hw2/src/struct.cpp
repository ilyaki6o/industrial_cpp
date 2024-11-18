#include "struct.h"
#include <iostream>
#include <algorithm>

int MAX_ITER_WITHOUT_IMP = 100;
int STEPS_WIHOUT_DECR = 10;

double BoltzmannLaw::change(double temp, long long iter) const {
    return temp / std::log(1 + iter);
}

double CauchyLaw::change(double temp, long long iter) const {
    return temp / (1 + iter);
}

double ThirdLaw::change(double temp, long long iter) const {
    return temp * std::log(1 + iter) / (1 + iter);
}

void SimulateAnnealing::step(){
    for (int i = 0; i < STEPS_WIHOUT_DECR; ++i){
        mutation.perform(curret_view);

        long long new_loss = curret_view->get_loss();

        if (new_loss < cur_loss){
            update_loss(new_loss);
        }else{
            long long df = new_loss - best_loss;
            double p = std::exp(-df / curret_temp);

            if (rand_p(gen) < p){
                update_loss(new_loss);
            }
        }
    }
}

void SimulateAnnealing::update_loss(long long new_loss){
    if (new_loss < best_loss){
        ImpScheduleView* imp_best = dynamic_cast<ImpScheduleView*>(best_view);
        ImpScheduleView* imp_cur = dynamic_cast<ImpScheduleView*>(curret_view);
        imp_best->copy(imp_cur);
        iter_with_imp = iter;
        best_loss = new_loss;
    }

    cur_loss = new_loss;
}

void SimulateAnnealing::run(){
    gen.seed(rd());

    while(true){
        step();

        if (iter - iter_with_imp > MAX_ITER_WITHOUT_IMP){
            break;
        }else{
            curret_temp = temp_decrease_law.change(start_temp, iter);
        }

        iter++;
    }

    std::string best_in_str = best_view->to_string();

    const char* best_in_char = best_in_str.c_str();

    int ret = write(data_socket, best_in_char, strlen(best_in_char) + 1);

    close(data_socket);
}

void ImpMutation::perform(ScheduleView* view){
    ImpScheduleView* imp_view = dynamic_cast<ImpScheduleView*>(view);

    gen.seed(rd());
    std::uniform_int_distribution<> dis_works(0, imp_view->getNumbWorks() - 1);
    std::uniform_int_distribution<> dis_cpu(0, imp_view->getNumbCPU() - 1);

    int chosen_work = dis_works(gen);
    int chosen_cpu = dis_cpu(gen);

    int old_cpu = imp_view->works_bind[chosen_work];
    imp_view->works_bind[chosen_work] = chosen_cpu;
    imp_view->schedule[chosen_cpu].push_back(chosen_work);
    std::vector<int>& tmp = imp_view->schedule[old_cpu];
    auto work_iter = std::find(tmp.begin(), tmp.end(), chosen_work);
    tmp.erase(work_iter);
}

long long ImpScheduleView::get_loss() const {
    long long loss = 0;

    for (auto& [key, value]: schedule){
        long long start = 0;

        for (auto work: value){
            loss += start + work_time[work];
            start += work_time[work];
        }
    }

    return loss;
}

void ImpScheduleView::copy(ImpScheduleView* other){
    work_time = other->work_time;
    schedule = other->schedule;
    works_bind = other->works_bind;
    numb_cpu = other->numb_cpu;
}

void ImpScheduleView::print() const {
    for (auto& [key, value]: schedule){
        std::cout << key << ": [";

        for (auto work: value){
            std::cout << work << "[" << work_time[work] << "], ";
        }

        std::cout << "]" << std::endl;
    }
}


SimulateAnnealing::SimulateAnnealing(
    double StartTemp,
    Mutation& MutationObj,
    ScheduleView* View,
    ScheduleView* BestView,
    TemperatureLaw& TempLaw,
    std::string SockName
):
start_temp(StartTemp),
mutation(MutationObj),
temp_decrease_law(TempLaw),
curret_view(View),
best_view(BestView)
{
    struct sockaddr_un addr;
    int ret;

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (data_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SockName.c_str(), sizeof(addr.sun_path) - 1);

    ret = connect (data_socket, (const struct sockaddr *) &addr,
                   sizeof(struct sockaddr_un));

    if (ret == -1) {
        fprintf(stderr, "The server is down.\n");
        exit(EXIT_FAILURE);
    }

    cur_loss = curret_view->get_loss();
    best_loss = best_view->get_loss();
}

std::string ImpScheduleView::to_string() const {
    std::string res = std::to_string(this->numb_cpu) + "\n";
    res += std::to_string(this->get_loss()) + "\n";

    for (auto& [key, value]: schedule){
        res += std::to_string(key);

        for (auto work: value){
            res += " ";
            res += std::to_string(work);
        }

        res += "\n";
    }

    return res;
}
