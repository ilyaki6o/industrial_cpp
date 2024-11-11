#ifndef ANL_HEADER
#define ANL_HEADER

#include <memory>
#include <random>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


class ScheduleView {
public:
    virtual void print() const = 0;
    virtual long long get_loss() const = 0;
    virtual std::string to_string() const = 0;

    virtual ~ScheduleView() = default;
};


class Mutation {
public:
    virtual void perform(ScheduleView* view) = 0;

    virtual ~Mutation() = default;
};


class TemperatureLaw {
public:
    virtual double change(double temp, long long iter) const = 0;

    virtual ~TemperatureLaw() = default;
};


class SimulateAnnealing {
private:
    ScheduleView* curret_view;
    ScheduleView* best_view;
    Mutation& mutation;
    TemperatureLaw& temp_decrease_law;
    long long iter = 1;
    long long iter_with_imp = iter;

    double curret_temp;
    double start_temp;
    long long cur_loss;
    long long best_loss;

    int data_socket;

    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());
    std::uniform_real_distribution<> rand_p = std::uniform_real_distribution(0.0, 1.0);

public:
    SimulateAnnealing(
        double StartTemp,
        Mutation& MutationObj,
        ScheduleView* View,
        ScheduleView* BestView,
        TemperatureLaw& TempLaw,
        std::string SockName
    );

    void step();
    void update_loss(long long new_loss);
    void run();

    long long getIter(){
        return iter;
    }
};


class ImpScheduleView: public ScheduleView {
public:
    std::vector<int> work_time;
    std::map<int, std::vector<int>> schedule;
    std::map<int, int> works_bind;
    int numb_cpu;

    ImpScheduleView(){};

    ImpScheduleView(
        int CPUs,
        std::vector<int>& workTime
    ): numb_cpu(CPUs), work_time(workTime) {
        std::random_device rd;
        std::mt19937 gen = std::mt19937(rd());
        std::uniform_int_distribution<> dis_cpu(0, numb_cpu - 1);

        for (int i = 0; i < work_time.size(); i++){
            int cpu_numb = dis_cpu(gen);
            schedule[cpu_numb].push_back(i);
            works_bind[i] = cpu_numb;
        }
    }

    ImpScheduleView(
        int CPUs,
        std::vector<int>& workTime,
        std::map<int, std::vector<int>>& Schedule,
        std::map<int, int>& Bind
    ):
    numb_cpu(CPUs),
    work_time(workTime),
    schedule(Schedule),
    works_bind(Bind)
    {}

    void copy(ImpScheduleView* other);

    int getNumbWorks(){
        return work_time.size();
    }

    int getNumbCPU(){
        return numb_cpu;
    }

    void print() const override;
    long long get_loss() const override;
    std::string to_string() const override;

    ~ImpScheduleView() override = default;
};


class ImpMutation: public Mutation{
private:
    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());

public:
    void perform(ScheduleView* view) override;

    ~ImpMutation() override = default;
};


class BoltzmannLaw: public TemperatureLaw {
public:
    double change(double temp, long long iter) const override;;

    ~BoltzmannLaw() override = default;
};


class CauchyLaw: public TemperatureLaw {
public:
    double change(double temp, long long iter) const override;;

    ~CauchyLaw() override = default;
};


class ThirdLaw: public TemperatureLaw {
public:
    double change(double temp, long long iter) const override;;

    ~ThirdLaw() override = default;
};

#endif
