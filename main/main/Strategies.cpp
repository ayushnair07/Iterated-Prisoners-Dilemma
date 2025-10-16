#include "Strategy.h"
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

using namespace std;

mt19937 rng(random_device{}());
void set_global_seed(unsigned int seed) {
    rng.seed(seed);
}


class ALLC : public Strategy {
public:
    Move decide(const History&, const History&) override { return Move::C; }
    string name() const override { return "ALLC"; }
};

class ALLD : public Strategy {
public:
    Move decide(const History&, const History&) override { return Move::D; }
    string name() const override { return "ALLD"; }
};

class TFT : public Strategy {
public:
    Move decide(const History&, const History& opp_history) override {
        if (opp_history.empty()) return Move::C;
        return opp_history.back();
    }
    string name() const override { return "TFT"; }
};

class GRIM : public Strategy {
    bool triggered = false;
public:
    void reset() override { triggered = false; }
    Move decide(const History&, const History& opp_history) override {
        if (triggered) return Move::D;
        if (!opp_history.empty() && opp_history.back() == Move::D) {
            triggered = true;
            return Move::D;
        }
        return Move::C;
    }
    string name() const override { return "GRIM"; }
};

class PAVLOV : public Strategy {
public:
    Move decide(const History& self_history, const History& opp_history) override {
        if (self_history.empty()) return Move::C;
        Move my_last = self_history.back();
        Move opp_last = opp_history.back();
        bool was_successful = (my_last == Move::C && opp_last == Move::C) || (my_last == Move::D && opp_last == Move::C);
        if (was_successful) return my_last;
        return my_last == Move::C ? Move::D : Move::C;
    }
    string name() const override { return "PAVLOV"; }
};

class RND : public Strategy {
public:
    Move decide(const History&, const History&) override {
        uniform_real_distribution<double> dist(0.0, 1.0);
             return dist(rng) < 0.5 ? Move::C : Move::D;
    }
    string name() const override {
        // The name is now just "RND"
        return "RND";
    }
};

class CTFT : public Strategy {
public:
    Move decide(const History& self_history, const History& opp_history) override {
        if (self_history.empty()) return Move::C;
        if (self_history.size() >= 2) {
            if (self_history[self_history.size() - 2] == Move::C && self_history.back() == Move::D && opp_history.back() == Move::D) {
                return Move::C;
            }
        }
        return opp_history.back();
    }
    string name() const override { return "CONTRITE"; }
};

class PROBER : public Strategy {
    bool opponent_is_exploitable = false;
    bool probe_phase_complete = false;
public:
    void reset() override {
        opponent_is_exploitable = false;
        probe_phase_complete = false;
    }
    Move decide(const History& self_history, const History& opp_history) override {
        size_t round = self_history.size();
        if (round == 0) return Move::C;
        if (round == 1) return Move::D;
        if (round == 2) return Move::C;
        if (round == 3) return Move::C;

        if (!probe_phase_complete) {
            probe_phase_complete = true;
            if (opp_history[1] == Move::C && opp_history[2] == Move::C&& opp_history[3] == Move::C) {
                opponent_is_exploitable = true;
            }
        }

        if (opponent_is_exploitable) return Move::D;
        return opp_history.back();
    }
    string name() const override { return "PROBER"; }
};

// Own Statergy 1 - It starts by playing Defect (D),It plays exactly like standard TFT, copying whatever the opponent did on the previous move.
class SuspiciousTFT : public Strategy {
public:
    Move decide(const History&, const History& opp_history) override {
        if (opp_history.empty()) return Move::D;
        return opp_history.back();
    }
    string name() const override { return "SUS_TFT"; }
};

// Own Statergy 2 - Cooperate on even rounds, Defect on odd rounds
class ALTERNATE : public Strategy {
public:
    Move decide(const History& self_history, const History&) override {
        if (self_history.size() % 2 == 0) {
            return Move::C;
        }
        else {
            return Move::D;
        }
    }
    string name() const override { return "ALTERNATE"; }
};


unique_ptr<Strategy> CreateStrategy(const string& name) {
    string upper_name = name;
    transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

    if (upper_name == "ALLC") return make_unique<ALLC>();
    if (upper_name == "ALLD") return make_unique<ALLD>();
    if (upper_name == "TFT") return make_unique<TFT>();
    if (upper_name == "GRIM") return make_unique<GRIM>();
    if (upper_name == "PAVLOV") return make_unique<PAVLOV>();
    if (upper_name == "CONTRITE") return make_unique<CTFT>();
    if (upper_name == "PROBER") return make_unique<PROBER>();
    if (upper_name == "SUS_TFT") return make_unique<SuspiciousTFT>();
    if (upper_name == "ALTERNATE") return make_unique<ALTERNATE>();
    if (upper_name == "RND") return make_unique<RND>();
    throw runtime_error("Unknown strategy: " + name);
}