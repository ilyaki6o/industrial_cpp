#include "struct.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include <sys/wait.h>

std::string FILE_NAME_PATERN = "./sockets/sock_";
int BUFFER_SIZE = 2048;
int MAX_ITER_WITHOUT_IMP_PARAL = 10;

namespace fs = std::filesystem;

ImpScheduleView* parse_to_view(std::string str_view, std::vector<int>& work_time){
    std::map<int, std::vector<int>> schedule;
    std::map<int, int> works_bind;

    std::istringstream stream(str_view); // Поток для работы с данными строки
    int num_processors;
    long long loss;

    stream >> num_processors >> loss; // Чтение количества процессоров и loss

    // Обработка оставшихся данных в строке
    std::string line;
    std::getline(stream, line); // Пропускаем остаток первой строки после `loss`

    for (int i = 0; i < num_processors; ++i) {
        std::getline(stream, line); // Чтение строки для каждого процессора
        std::istringstream line_stream(line);

        int processor_id;
        line_stream >> processor_id; // Чтение номера процессора

        int task;
        while (line_stream >> task) { // Чтение номеров задач для процессора
            schedule[processor_id].push_back(task);
            works_bind[task] = processor_id;
        }
    }

    ImpScheduleView* recieve_view = new ImpScheduleView(
                                                num_processors,
                                                work_time,
                                                schedule,
                                                works_bind
                                                );

    return recieve_view;
}

long long parse_to_loss(std::string str_view){
    std::istringstream stream(str_view); // Поток для работы с данными строки
    int num_processors;
    long long loss;

    stream >> num_processors >> loss; // Чтение количества процессоров и loss
                                      //
    return loss;
}

std::vector<int> generate_works(int numb_works){
    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());
    gen.seed(rd());
    std::uniform_int_distribution<> rand_data(1, 150);
    std::vector<int> works;

    for (int i = 0; i < numb_works; i++){
        works.push_back(rand_data(gen));
    }

    return works;
}

void createPath(fs::path path){
    if (fs::exists(path)){
        return;
    }else{
        createPath(path.parent_path());
    }

    fs::create_directories(path);

    return;
}

int main(int argc, char* argv[]){
    if (argc != 4) {
        std::cout << "Please use the next scheme: file tasks_num processes_num processors_num" << std::endl;
        return 0;
    }

    int numb_works = std::stoi(argv[1]);
    int numb_cpu = std::stoi(argv[2]);
    int processors = std::stoi(argv[3]);

    char buffer[BUFFER_SIZE];
    std::vector<int> works = generate_works(numb_works);
    std::vector<int> sockets;

    int ret;

    for (int i = 0; i < processors; i++){
        struct sockaddr_un addr;
        int listen_socket = socket(AF_UNIX, SOCK_STREAM, 0);

        if (listen_socket == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        memset(&addr, 0, sizeof(struct sockaddr_un));

        addr.sun_family = AF_UNIX;
        std::string file_name = FILE_NAME_PATERN + std::to_string(i) + ".sock";

        if (!fs::exists(file_name)) {
            createPath(fs::path(file_name));
        }

        remove(file_name.c_str());

        strncpy(addr.sun_path, file_name.c_str(), sizeof(addr.sun_path) - 1);

        ret = bind(listen_socket, (const struct sockaddr *) &addr,
               sizeof(struct sockaddr_un));

        if (ret == -1) {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        ret = listen(listen_socket, 20);

        if (ret == -1) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        sockets.push_back(listen_socket);
    }


    ImpMutation mutation = ImpMutation();
    BoltzmannLaw temp_law = BoltzmannLaw();

    ImpScheduleView* best_schedule = new ImpScheduleView();
    long long best_loss;
    std::string best_str_view;

    long long iter = 0;
    long long iter_with_imp = iter;

    while(iter - iter_with_imp <= MAX_ITER_WITHOUT_IMP_PARAL){
        ImpScheduleView* cur_schedule = new ImpScheduleView(numb_cpu, works);

        if (iter == 0){
            best_schedule->copy(cur_schedule);

            best_loss = best_schedule->get_loss();
            best_str_view = best_schedule->to_string();
        }

        pid_t pid, wpid;
        int status = 0;

        for (int i = 0; i < processors; i++){
            if ((pid = fork()) < 0){
                perror("fork error");
                exit(EXIT_FAILURE);
            }else if(pid == 0){
                std::string file_name = FILE_NAME_PATERN + std::to_string(i) + ".sock";
                SimulateAnnealing res = SimulateAnnealing(1000, mutation, cur_schedule, best_schedule, temp_law, file_name);

                res.run();

                exit(0);
            }
        }

        bool update_best = false;

        for (auto socket : sockets){
            int data_socket = accept(socket, NULL, NULL);

            if (data_socket == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            std::string str_view = "";

            while(read(data_socket, buffer, BUFFER_SIZE) != 0){
                buffer[BUFFER_SIZE - 1] = 0;
                str_view += std::string(buffer);
            }

            long long cur_loss = parse_to_loss(str_view);

            if (cur_loss < best_loss){
                update_best = true;
                best_loss = cur_loss;
                best_str_view = str_view;
            }

            close(data_socket);
        }

        if (update_best){
            iter_with_imp = iter;

            delete(best_schedule);
            best_schedule = parse_to_view(best_str_view, works);
            best_loss = best_schedule->get_loss();
        }

        while((wpid = wait(&status)) > 0);

        iter++;
        delete(cur_schedule);
    }

    for (auto socket : sockets){
        close(socket);
    }

    std::cout << best_loss << std::endl;
    best_schedule->print();

    delete(best_schedule);

    return 0;
}

//int main(){
//    std::vector<int> works = std::vector({1, 2, 3, 4, 10, 12, 3, 100, 3, 5, 17, 2, 5, 4});
//    ImpScheduleView* cur_schedule = new ImpScheduleView(5, works);
//    ImpScheduleView* best_schedule = new ImpScheduleView();
//    best_schedule->copy(cur_schedule);
//
//    best_schedule->print();
//    std::cout << best_schedule->get_loss() << std::endl;
//
//    std::cout << std::endl;
//
//    ImpMutation mutation = ImpMutation();
//    BoltzmannLaw temp_law = BoltzmannLaw();
//    SimulateAnnealing res = SimulateAnnealing(1000, mutation, cur_schedule, best_schedule, temp_law);
//    res.run();
//
//    best_schedule->print();
//    std::cout << best_schedule->get_loss() << std::endl;
//
//    std::cout << "\nIterations: " << res.getIter() << std::endl;
//
//    delete cur_schedule;
//    delete best_schedule;
//
//    return 0;
//}

//int main(){
//    std::vector<int> works = std::vector({1, 2, 3, 4, 10, 12, 3, 100, 3, 5, 17, 2, 5, 4});
//    ImpScheduleView* cur_schedule = new ImpScheduleView(5, works);
//    ImpMutation mutation = ImpMutation();
//
//    cur_schedule->print();
//    std::cout<< cur_schedule->get_loss() << std::endl;
//    std::string cur_s = cur_schedule->to_string();
//
//    ImpScheduleView* new_view = parse_to_view(cur_s.c_str(), works);
//    std::cout << std::endl;
//
//    new_view->print();
//    std::cout << parse_to_loss(cur_s.c_str()) << std::endl;
//
//    delete cur_schedule;
//    delete new_view;
//
//    return 0;
//}