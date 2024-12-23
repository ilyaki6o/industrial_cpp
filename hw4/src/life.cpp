#include "life.h"
#include <iostream>
#include <chrono>
#include <thread>


Game::Game(int size, Initial state) : size_(size) {
    gen.seed(rd());
    std::uniform_real_distribution<> rand_p = std::uniform_real_distribution(0.0, 1.0);

    for (int i = 0; i < size; ++i){
        field_.push_back({});

        for (int j = 0; j < size; ++j){
            if (state == Initial::zeros) {
                field_[i].push_back(false);
            }else if (state == Initial::random) {
                if (rand_p(gen) < p_alive){
                    field_[i].push_back(true);
                }else{
                    field_[i].push_back(false);
                }
            }
        }
    }
}

Game::Game(const std::vector<std::vector<bool>>& field) {
    int size = field.size();

    for (const auto& row: field){
        if (row.size() != size){
            throw std::exception();
        }
    }

    size_ = size;
    field_ = field;
}

Game::Game(const Game& other) {
    size_ = other.getSize();
    field_ = other.getField();
}

std::string Game::toString() const {
    std::string res = "";

    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            res += (field_[i][j] ? 'X' : '-');
        }

        res += '\n';
    }

    return res;
}

int Game::countAlive() const {
    int count_alive;

    for (const auto& row : field_) {
        for (const auto& cell : row) {
            if (cell) {
                count_alive++;
            }
        }
    }

    return count_alive;
}

bool Game::operator== (const Game& other) const {
    if (size_ != other.size_) return false;

    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            if (field_[i][j] != other.field_[i][j]) {
                return false;
            }
        }
    }

    return true;
}

Game& Game::operator= (const Game& other){
    if (this == &other) {
        return *this;
    } 

    size_ = other.size_;
    field_ = other.field_;

    return *this;
}

Game Game::step() const {
    Game field = *this;
    Game cur_copy = Game(field);
    int size = field.getSize();

    for (int i = 0; i < size; ++i) {
        int alive_left = 0;
        int alive_cur = 0;

        if (field[i][0]) alive_cur += 1;
        if (i + 1 < size && field[i + 1][0]) alive_cur += 1;
        if (i - 1 >= 0 && field[i - 1][0]) alive_cur += 1;

        for (int j = 0; j < size; ++j) {
            int alive_right = 0;

            if (j + 1 < size) {
                if (field[i][j + 1]) alive_right += 1;
                if (i + 1 < size && field[i + 1][j + 1]) alive_right += 1;
                if (i - 1 >= 0 && field[i - 1][j + 1]) alive_right += 1;
            }

            int alive_neighbours = alive_left + alive_cur + alive_right - (field[i][j] ? 1 : 0);

            if (alive_neighbours == 3) {
                cur_copy[i][j] = true;
            } else if (alive_neighbours == 2 && field[i][j]) {
                cur_copy[i][j] = true;
            }else {
                cur_copy[i][j] = false;
            }

            alive_left = alive_cur;
            alive_cur = alive_right;
        }
    }

    return cur_copy;
}

Game Game::run(int numb_steps, int state_in_sec) const {
    Game res = *this;

    if (state_in_sec) res.PrintState(state_in_sec);

    for (int i = 0; i < numb_steps; i++){
        res = res.step();
        
        if (state_in_sec) res.PrintState(state_in_sec);
    }   

    return res;
}

void Game::PrintState(int state_in_sec) const {
		system("clear");
        std::cout << std::endl;

		// std::cout << "\n\nstep " << (int)steps << "\n\n";
		//
        std::cout << this->toString() << std::endl;

		std::this_thread::sleep_for(std::chrono::nanoseconds(1000000000 / state_in_sec));
}
