#pragma once
// Engine.h
#pragma once
#include "Strategy.h"
#include <map>

// Configuration struct to hold all CLI parameters
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

    // Q1, Q2, Q3 entry point
    std::vector<StrategyResult> run_tournament();

    // Q4, Q5 entry point
    std::vector<std::vector<StrategyResult>> run_evolution();
};