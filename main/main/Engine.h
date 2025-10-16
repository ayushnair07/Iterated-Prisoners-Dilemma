#pragma once
#pragma once
#include "Strategy.h"
#include <map>

struct Config {
    int rounds = 100, repeats = 10, population = 50, generations = 50;
    unsigned int seed = 0;
    double epsilon = 0.0, mutation = 0.01;
    PayoffMatrix<double> payoffs;
    std::vector<std::string> strategies;
    bool evolve = false;
    bool apply_scb = false;
    std::string format = "text";
};

class Engine {
private:
    Config config;
    std::vector<std::unique_ptr<Strategy>> strategy_pool;

public:
    Engine(const Config& cfg);

    std::vector<StrategyResult> RunTournament();

    std::vector<std::vector<StrategyResult>> RunEvolution();
};