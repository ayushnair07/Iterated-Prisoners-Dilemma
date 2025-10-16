#include "Engine.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;

void set_global_seed(unsigned int seed);

Engine::Engine(const Config& cfg) : config(cfg) {
    set_global_seed(config.seed);
    config.payoffs.Validate();
    for (const auto& name : config.strategies) {
        strategy_pool.push_back(CreateStrategy(name));
    }
}

pair<double, double> RunMatch(Strategy& p1, Strategy& p2, int rounds, double epsilon, const PayoffMatrix<double>& payoffs) {
    History p1_hist, p2_hist;
    double p1_score = 0.0, p2_score = 0.0;
    uniform_real_distribution<double> dist(0.0, 1.0);

    p1.reset();
    p2.reset();

    for (int i = 0; i < rounds; ++i) {
        Move m1 = p1.decide(p1_hist, p2_hist);
        Move m2 = p2.decide(p2_hist, p1_hist);

        if (dist(rng) < epsilon) m1 = (m1 == Move::C ? Move::D : Move::C);
        if (dist(rng) < epsilon) m2 = (m2 == Move::C ? Move::D : Move::C);

        auto scores = payoffs.Get_Scores(m1, m2);
        p1_score += scores.first;
        p2_score += scores.second;

        p1_hist.push_back(m1);
        p2_hist.push_back(m2);
    }
    return { p1_score, p2_score };
}

vector<StrategyResult> Engine::RunTournament() {
    map<string, vector<double>> all_scores;
    for (const auto& s : strategy_pool) {
        all_scores[s->name()] = {};
    }

    for (int rep = 0; rep < config.repeats; ++rep) {
        for (size_t i = 0; i < strategy_pool.size(); ++i) {
            for (size_t j = i; j < strategy_pool.size(); ++j) {
                auto& p1 = *strategy_pool[i];
                auto& p2 = *strategy_pool[j];

                auto scores = RunMatch(p1, p2, config.rounds, config.epsilon, config.payoffs);

                if (i == j) { 
                    all_scores[p1.name()].push_back(scores.first);
                }
                else {
                    all_scores[p1.name()].push_back(scores.first);
                    all_scores[p2.name()].push_back(scores.second);
                }
            }
        }
    }

    vector<StrategyResult> results;
    for (const auto& s : strategy_pool) {
        results.push_back(StrategyResult::compute(s->name(), all_scores[s->name()]));
    }
    return results;
}


double GetSCB_Cost(const string& name) {
    if (name == "ALLC" || name == "ALLD" || name.rfind("RND", 0) == 0) return 1.0;
    if (name == "TFT" || name == "GRIM" || name == "SUS_TFT") return 2.0;
    if (name == "PAVLOV" || name == "CONTRITE" || name == "ADAPT_PUNISH") return 3.0;
    if (name == "PROBER") return 4.0;
    return 0.0;
}

vector<vector<StrategyResult>> Engine::RunEvolution() {
    vector<vector<StrategyResult>> evolution_history;

    vector<int> current_population(strategy_pool.size());
    for (size_t i = 0; i < strategy_pool.size(); ++i) {
        current_population[i] = config.population / strategy_pool.size();
    }

    for (int gen = 0; gen < config.generations; ++gen) {
        map<string, double> total_scores;
        for (const auto& s : strategy_pool) total_scores[s->name()] = 0.0;

        for (size_t i = 0; i < strategy_pool.size(); ++i) {
            for (size_t j = i; j < strategy_pool.size(); ++j) {
                if (current_population[i] == 0 || current_population[j] == 0) continue;

                auto& p1 = *strategy_pool[i];
                auto& p2 = *strategy_pool[j];
                auto scores = RunMatch(p1, p2, config.rounds, config.epsilon, config.payoffs);

                if (i == j) {
                    total_scores[p1.name()] += scores.first * current_population[i];
                }
                else {
                    total_scores[p1.name()] += scores.first * current_population[i] * current_population[j];
                    total_scores[p2.name()] += scores.second * current_population[j] * current_population[i];
                }
            }
        }

        vector<StrategyResult> gen_results;
        double total_fitness = 0;
        for (size_t i = 0; i < strategy_pool.size(); ++i) {
            if (current_population[i] == 0) continue;
            StrategyResult res;
            res.name = strategy_pool[i]->name();
            res.population = current_population[i];
            res.mean_score = total_scores[res.name] / res.population;
            if (config.apply_scb) res.mean_score -= GetSCB_Cost(res.name);
            total_fitness += res.mean_score * res.population;
            gen_results.push_back(res);
        }
        evolution_history.push_back(gen_results);

        if (total_fitness <= 0) break;

        vector<int> next_population(strategy_pool.size(), 0);
        int reproduced_count = 0;
        for (const auto& res : gen_results) {
            double proportion = (res.mean_score * res.population) / total_fitness;
            int num_offspring = static_cast<int>(round(proportion * config.population));
            auto it = find_if(strategy_pool.begin(), strategy_pool.end(), [&](const auto& s) { return s->name() == res.name; });
            size_t idx = distance(strategy_pool.begin(), it);
            next_population[idx] = num_offspring;
            reproduced_count += num_offspring;
        }

        while (reproduced_count < config.population) {
            next_population[rng() % strategy_pool.size()]++;
            reproduced_count++;
        }
        while (reproduced_count > config.population) {
            size_t idx = rng() % strategy_pool.size();
            if (next_population[idx] > 0) {
                next_population[idx]--;
                reproduced_count--;
            }
        }

        uniform_real_distribution<double> mut_dist(0.0, 1.0);
        for (int i = 0; i < config.population; ++i) {
            if (mut_dist(rng) < config.mutation) {
                int count = 0;
                size_t current_idx = 0;
                for (size_t k = 0; k < next_population.size(); ++k) {
                    count += next_population[k];
                    if (i < count) { current_idx = k; break; }
                }

                if (next_population[current_idx] > 0) {
                    next_population[current_idx]--;
                    next_population[rng() % strategy_pool.size()]++;
                }
            }
        }
        current_population = next_population;
    }
    return evolution_history;
}