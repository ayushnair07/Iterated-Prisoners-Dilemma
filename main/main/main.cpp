#include "Engine.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

vector<string> Split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

Config Parse_CLI(int argc, char* argv[]) {
    Config cfg;
    cfg.strategies = { "ALLC", "ALLD", "TFT", "GRIM", "PAVLOV" };
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--rounds" && i + 1 < argc) cfg.rounds = stoi(argv[++i]);
        else if (arg == "--repeats" && i + 1 < argc) cfg.repeats = stoi(argv[++i]);
        else if (arg == "--epsilon" && i + 1 < argc) cfg.epsilon = stod(argv[++i]);
        else if (arg == "--seed" && i + 1 < argc) cfg.seed = stoul(argv[++i]);
        else if (arg == "--payoffs" && i + 1 < argc) {
            auto p = Split(argv[++i], ',');
            if (p.size() == 4) {
                cfg.payoffs.T_temptation = stod(p[0]);
                cfg.payoffs.R_reward = stod(p[1]);
                cfg.payoffs.P_punishment = stod(p[2]);
                cfg.payoffs.S_sucker = stod(p[3]);
            }
        }
        else if (arg == "--strategies" && i + 1 < argc) cfg.strategies = Split(argv[++i], ',');
        else if (arg == "--format" && i + 1 < argc) cfg.format = argv[++i];
        else if (arg == "--evolve") cfg.evolve = true;
        else if (arg == "--population" && i + 1 < argc) cfg.population = stoi(argv[++i]);
        else if (arg == "--generations" && i + 1 < argc) cfg.generations = stoi(argv[++i]);
        else if (arg == "--mutation" && i + 1 < argc) cfg.mutation = stod(argv[++i]);
        else if (arg == "--scb") cfg.apply_scb = true;
    }
    return cfg;
}

void Print_Leaderboard(const vector<StrategyResult>& results) {
    auto sorted_results = results;
    sort(sorted_results.begin(), sorted_results.end(), [](const auto& a, const auto& b) {
        return a.mean_score > b.mean_score;
        });

    cout << left << setw(20) << "Strategy"
        << setw(15) << "Mean Score"
        << setw(15) << "Std Dev"
        << "95% CI" << endl;
    cout << string(70, '-') << endl;

    for (const auto& res : sorted_results) {
        cout << left << setw(20) << res.name
            << fixed << setprecision(3) << setw(15) << res.mean_score
            << setw(15) << res.stdev
            << "[" << res.ci_lower << ", " << res.ci_upper << "]" << endl;
    }
}

void Print_Evolution(const vector<vector<StrategyResult>>& history) {
    cout << "--- Evolutionary Dynamics ---" << endl;
    for (size_t gen = 0; gen < history.size(); ++gen) {
        cout << "Generation " << gen + 1 << ": ";
        for (const auto& res : history[gen]) {
            if (res.population > 0) {
                cout << res.name << "(" << res.population << ") ";
            }
        }
        cout << endl;
    }
}


int main(int argc, char* argv[]) {
    try {
        Config cfg = Parse_CLI(argc, argv);
        Engine engine(cfg);

        if (cfg.evolve) {
            auto history = engine.RunEvolution();
            Print_Evolution(history);
        }
        else {
            auto results = engine.RunTournament();
            Print_Leaderboard(results);
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}