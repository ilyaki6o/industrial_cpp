#ifndef GEN_ALGO
#define GEN_ALGO

#include "life.h"
#include <memory>
#include <vector>


using population = std::vector<Game>;
using population_ptr = std::shared_ptr<population>;


class TSurvivalFunc {
public:
	virtual int surv_score(const Game& field) = 0;
	virtual ~TSurvivalFunc() = default;
};


class TSelection {
public:
	virtual population_ptr select(const population_ptr& pop, const std::vector<int>& surv) = 0;
	virtual ~TSelection() = default;
};


class TCrossover {
public:
	virtual population_ptr crossing(const population_ptr& pop) = 0;
	virtual ~TCrossover() = default;

};


class TMutation {
public:
	virtual void mutate(population_ptr& pop) = 0;
	virtual ~TMutation() = default;
};


class DeadCells: public TSurvivalFunc {
	int numbSteps;

public:
	DeadCells(int steps = 100) : numbSteps(steps) {};

	int surv_score(const Game& field) override;
};


class ProportionalSelection: public TSelection {
    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());

public:
	population_ptr select(const population_ptr& pop, const std::vector<int>& scores) override;
};


class TwoPointCV: public TCrossover {
	double crossover_p;
	int Npop;

    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());

public:
	TwoPointCV(double CrossoverP = 0.8, int pop_size = 100) : crossover_p(CrossoverP), Npop(pop_size) {}; 
	
	population_ptr crossing(const population_ptr& pop) override;
};


class BitMutation: public TMutation {
	double Pmut;

    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());
public:
	BitMutation(double mut_p) : Pmut(mut_p) {}
	
	void mutate(population_ptr& pop) override;
};


class GenAlgo {
	std::shared_ptr<TSurvivalFunc> surv_func;	
	std::shared_ptr<TSelection> selection;
	std::shared_ptr<TCrossover> crossover;
	std::shared_ptr<TMutation> mutation;
	
	population_ptr pop;
	int population_size;
	int field_size;
	
public:
	GenAlgo(
			std::shared_ptr<TSurvivalFunc> SurvFunc,	
			std::shared_ptr<TSelection> Selection,
			std::shared_ptr<TCrossover> Crossover,
			std::shared_ptr<TMutation> Mutation,
			population_ptr Population,
			int PopSize,
			int FieldSize
		   ) :
		surv_func(SurvFunc),
		selection(Selection),
		crossover(Crossover),
		mutation(Mutation),
		pop(Population),
		population_size(PopSize),
		field_size(FieldSize) {}

	std::shared_ptr<Game> run();
	int selectBest(const population_ptr pop, const std::vector<int>& scores, std::shared_ptr<Game> best_individ);
};

#endif
