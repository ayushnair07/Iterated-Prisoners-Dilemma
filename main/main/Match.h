#ifndef MATCH_H
#define MATCH_H

#include "Player.h" 
#include "Payoff.h"
#include <random>
#include <vector>
#include <memory>


struct LPlayer {
    std::unique_ptr<Strategy> strat;
    std::vector<Move> my, opp;
    double total = 0.0;
    LPlayer(Strategy* s) : strat(s) {}
    void reset_for_match() { my.clear(); opp.clear(); total = 0.0; strat->reset(); }
    std::string name() const { return strat->name(); }
};

struct Match {
    LPlayer& A, & B;
    int rounds;
    double epsilon;
    Payoff pf;
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> uni;
    Match(LPlayer& a, LPlayer& b, int r, double e, const Payoff& p, uint64_t seed)
        : A(a), B(b), rounds(r), epsilon(e), pf(p), rng(seed), uni(0.0, 1.0) {
    }
    void Play() {
        A.reset_for_match(); B.reset_for_match();
        for (int i = 0; i < rounds; ++i) {
            Move mA = A.strat->decide(A.my, A.opp);
            Move mB = B.strat->decide(B.my, B.opp);
            if (uni(rng) < epsilon) mA = (mA == Move::C ? Move::D : Move::C);
            if (uni(rng) < epsilon) mB = (mB == Move::C ? Move::D : Move::C);
            auto pr = pf.outcome(mA, mB);
            A.total += pr.first;
            B.total += pr.second;
            A.my.push_back(mA); A.opp.push_back(mB);
            B.my.push_back(mB); B.opp.push_back(mA);
        }
    }
};

#endif// MATCH_H
