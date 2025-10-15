#ifndef PAYOFF_H
#define PAYOFF_H


#include <utility>
#include <iostream>

struct Payoff {
    double T, R, P, S;
    Payoff(double t = 5, double r = 3, double p = 1, double s = 0) : T(t), R(r), P(p), S(s) {}
    std::pair<double, double> outcome(Move a, Move b) const {
        if (a == Move::C && b == Move::C) return { R,R };
        if (a == Move::C && b == Move::D) return { S,T };
        if (a == Move::D && b == Move::C) return { T,S };
        return { P,P };
    }
    bool validate(bool warn = true) const {
        bool ok = (T > R && R > P && P > S);
        if (!ok && warn) std::cerr << "Warning: payoffs do not satisfy T>R>P>S\n";
        if (!(2 * R > T + S) && warn) std::cerr << "Warning: 2R > T+S violated (cooperation may not be stable)\n";
        return ok;
    }
};

#endif
