// Strategies.cpp
#include "Strategy.h" // Includes common.h
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

// --- Utility: Random number generation ---
std::mt19937 rng(std::random_device{}());
void set_global_seed(unsigned int seed) {
    rng.seed(seed);
}

// --- Standard Strategy Implementations ---

class ALLC : public Strategy {
public:
    Move decide(const History&, const History&) override { return Move::C; }
    std::string name() const override { return "ALLC"; }
};

class ALLD : public Strategy {
public:
    Move decide(const History&, const History&) override { return Move::D; }
    std::string name() const override { return "ALLD"; }
};

class TFT : public Strategy {
public:
    Move decide(const History&, const History& opp_history) override {
        if (opp_history.empty()) return Move::C;
        return opp_history.back();
    }
    std::string name() const override { return "TFT"; }
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
    std::string name() const override { return "GRIM"; }
};

class PAVLOV : public Strategy {
public:
    Move decide(const History& self_history, const History& opp_history) override {
        if (self_history.empty()) return Move::C;
        Move my_last = self_history.back();
        Move opp_last = opp_history.back();
        // Win-Stay, Lose-Shift. A "win" is C-C or D-C. A "loss" is C-D or D-D.
        bool was_successful = (my_last == Move::C && opp_last == Move::C) || (my_last == Move::D && opp_last == Move::C);
        if (was_successful) return my_last; // Stay
        return my_last == Move::C ? Move::D : Move::C; // Shift
    }
    std::string name() const override { return "PAVLOV"; }
};

class RND : public Strategy {
    double p;
public:
    RND(double coop_prob) : p(coop_prob) {}
    Move decide(const History&, const History&) override {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(rng) < p ? Move::C : Move::D;
    }
    std::string name() const override {
        std::ostringstream s;
        s << "RND(" << std::fixed << std::setprecision(2) << p << ")";
        return s.str();
    }
};

class CTFT : public Strategy {
public:
    Move decide(const History& self_history, const History& opp_history) override {
        if (self_history.empty()) return Move::C;
        // If I defected by mistake and opponent retaliated, be contrite and cooperate.
        if (self_history.size() >= 2) {
            if (self_history[self_history.size() - 2] == Move::C && self_history.back() == Move::D && opp_history.back() == Move::D) {
                return Move::C;
            }
        }
        // Otherwise, play standard TFT.
        return opp_history.back();
    }
    std::string name() const override { return "CONTRITE"; }
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

        if (!probe_phase_complete) {
            probe_phase_complete = true;
            // Did they cooperate on rounds 1 and 2 despite my defection on round 2?
            if (opp_history[1] == Move::D && opp_history[2] == Move::C) {
                opponent_is_exploitable = true;
            }
        }

        if (opponent_is_exploitable) return Move::D;
        return opp_history.back(); // Revert to TFT
    }
    std::string name() const override { return "PROBER"; }
};

// --- Original Strategy 1 ---
class SuspiciousTFT : public Strategy {
public:
    Move decide(const History&, const History& opp_history) override {
        if (opp_history.empty()) return Move::D; // Start with defection
        return opp_history.back(); // Then play TFT
    }
    std::string name() const override { return "SUS_TFT"; }
};

// --- Original Strategy 2 ---
class AdaptivePunisher : public Strategy {
    int punishment_counter = 0;
    int rounds_to_punish = 0;
public:
    void reset() override { punishment_counter = 0; rounds_to_punish = 0; }
    Move decide(const History&, const History& opp_history) override {
        if (opp_history.empty()) return Move::C;
        if (rounds_to_punish > 0) {
            rounds_to_punish--;
            return Move::D;
        }
        if (opp_history.back() == Move::D) {
            punishment_counter++;
            rounds_to_punish = punishment_counter; // Punish for N rounds for Nth offense
            return Move::D;
        }
        punishment_counter = 0; // Reset on cooperation
        return Move::C;
    }
    std::string name() const override { return "ADAPT_PUNISH"; }
};

// --- Strategy Factory Implementation ---
std::unique_ptr<Strategy> createStrategy(const std::string& name) {
    std::string upper_name = name;
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

    if (upper_name == "ALLC") return std::make_unique<ALLC>();
    if (upper_name == "ALLD") return std::make_unique<ALLD>();
    if (upper_name == "TFT") return std::make_unique<TFT>();
    if (upper_name == "GRIM") return std::make_unique<GRIM>();
    if (upper_name == "PAVLOV") return std::make_unique<PAVLOV>();
    if (upper_name == "CONTRITE") return std::make_unique<CTFT>();
    if (upper_name == "PROBER") return std::make_unique<PROBER>();
    if (upper_name == "SUS_TFT") return std::make_unique<SuspiciousTFT>();
    if (upper_name == "ADAPT_PUNISH") return std::make_unique<AdaptivePunisher>();
    if (upper_name.rfind("RND", 0) == 0) {
        try {
            double p = std::stod(name.substr(3));
            return std::make_unique<RND>(p);
        }
        catch (...) {
            return std::make_unique<RND>(0.5); // Default
        }
    }
    throw std::runtime_error("Unknown strategy: " + name);
}