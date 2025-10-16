#pragma once
#include "common.h"
#include <string>
#include <vector>
#include <memory>
#include <random>

using namespace std;

using History = vector<Move>;

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual Move decide(const History& self_history, const History& opp_history) = 0;
    virtual string name() const = 0;
    virtual void reset() {}
};

unique_ptr<Strategy> CreateStrategy(const string& name);