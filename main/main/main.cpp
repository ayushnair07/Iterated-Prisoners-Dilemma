
#include "Engine.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>


std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

Config parse_cli(int argc, char* argv[]) {
    Config cfg;
    cfg.strategies = { "ALLC", "ALLD", "TFT", "GRIM", "PAVLOV" }; 
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--rounds" && i + 1 < argc) cfg.rounds = std::stoi(argv[++i]);
        else if (arg == "--repeats" && i + 1 < argc) cfg.repeats = std::stoi(argv[++i]);
        else if (arg == "--epsilon" && i + 1 < argc) cfg.epsilon = std::stod(argv[++i]);
        else if (arg == "--seed" && i + 1 < argc) cfg.seed = std::stoul(argv[++i]);
        else if (arg == "--payoffs" && i + 1 < argc) {
            auto p = split(argv[++i], ',');
            if (p.size() == 4) {
                cfg.payoffs.T_temptation = std::stod(p[0]);
                cfg.payoffs.R_reward = std::stod(p[1]);
                cfg.payoffs.P_punishment = std::stod(p[2]);
                cfg.payoffs.S_sucker = std::stod(p[3]);
            }
        }
        else if (arg == "--strategies" && i + 1 < argc) cfg.strategies = split(argv[++i], ',');
        else if (arg == "--format" && i + 1 < argc) cfg.format = argv[++i];
        else if (arg == "--evolve") cfg.evolve = true;
        else if (arg == "--population" && i + 1 < argc) cfg.population = std::stoi(argv[++i]);
        else if (arg == "--generations" && i + 1 < argc) cfg.generations = std::stoi(argv[++i]);
        else if (arg == "--mutation" && i + 1 < argc) cfg.mutation = std::stod(argv[++i]);
        else if (arg == "--scb") cfg.apply_scb = true;
    }
    return cfg;
}


void print_leaderboard(const std::vector<StrategyResult>& results) {
    auto sorted_results = results;
    std::sort(sorted_results.begin(), sorted_results.end(), [](const auto& a, const auto& b) {
        return a.mean_score > b.mean_score;
        });

    std::cout << std::left << std::setw(20) << "Strategy"
        << std::setw(15) << "Mean Score"
        << std::setw(15) << "Std Dev"
        << "95% CI" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (const auto& res : sorted_results) {
        std::cout << std::left << std::setw(20) << res.name
            << std::fixed << std::setprecision(3) << std::setw(15) << res.mean_score
            << std::setw(15) << res.stdev
            << "[" << res.ci_lower << ", " << res.ci_upper << "]" << std::endl;
    }
}

void print_evolution(const std::vector<std::vector<StrategyResult>>& history) {
    std::cout << "--- Evolutionary Dynamics ---" << std::endl;
    for (size_t gen = 0; gen < history.size(); ++gen) {
        std::cout << "Generation " << gen + 1 << ": ";
        for (const auto& res : history[gen]) {
            if (res.population > 0) {
                std::cout << res.name << "(" << res.population << ") ";
            }
        }
        std::cout << std::endl;
    }
}

// Main function
int main(int argc, char* argv[]) {
    try {
        Config cfg = parse_cli(argc, argv);
        Engine engine(cfg);

        if (cfg.evolve) {
            auto history = engine.run_evolution();
            print_evolution(history);
        }
        else {
            auto results = engine.run_tournament();
            print_leaderboard(results);
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}