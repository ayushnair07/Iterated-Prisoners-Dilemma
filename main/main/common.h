#pragma once
// common.h
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <random>

// A clear enum for moves.
enum class Move { C, D };

// A struct for the payoff matrix, making it easy to pass around.
// Using a template for the score type fulfills that requirement.
template<typename T>
struct PayoffMatrix {
    T T_temptation = 5.0;
    T R_reward = 3.0;
    T P_punishment = 1.0;
    T S_sucker = 0.0;

    void validate() const {
        if (!(T_temptation > R_reward && R_reward > P_punishment && P_punishment > S_sucker)) {
            throw std::runtime_error("Payoff values violate T > R > P > S inequality.");
        }
        if (!(2 * R_reward > T_temptation + S_sucker)) {
            std::cerr << "Warning: Payoffs violate 2R > T+S. Cooperation may not be stable." << std::endl;
        }
    }

    std::pair<T, T> get_scores(Move p1_move, Move p2_move) const {
        if (p1_move == Move::C && p2_move == Move::C) return { R_reward, R_reward };
        if (p1_move == Move::C && p2_move == Move::D)   return { S_sucker, T_temptation };
        if (p1_move == Move::D && p2_move == Move::C) return { T_temptation, S_sucker };
        // Both must have defected
        return { P_punishment, P_punishment };
    }
};

// Struct to hold final statistics for the leaderboard.
struct StrategyResult {
    std::string name;
    double mean_score = 0.0;
    double stdev = 0.0;
    double ci_lower = 0.0;
    double ci_upper = 0.0;
    int population = 0; // For evolutionary results

    // Static function to compute stats from a vector of scores.
    static StrategyResult compute(const std::string& name, const std::vector<double>& scores) {
        StrategyResult res;
        res.name = name;
        size_t n = scores.size();
        if (n == 0) return res;

        double sum = std::accumulate(scores.begin(), scores.end(), 0.0);
        res.mean_score = sum / n;

        double sq_sum = std::inner_product(scores.begin(), scores.end(), scores.begin(), 0.0);
        res.stdev = std::sqrt(std::max(0.0, sq_sum / n - res.mean_score * res.mean_score));

        double se = res.stdev / std::sqrt(n);
        res.ci_lower = res.mean_score - 1.96 * se;
        res.ci_upper = res.mean_score + 1.96 * se;
        return res;
    }
};

extern std::mt19937 rng;