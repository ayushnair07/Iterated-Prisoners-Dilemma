#pragma once
#include "common.h"
#include <string>
#include <vector>
#include <memory>
#include <random>

using History = std::vector<Move>;

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual Move decide(const History& self_history, const History& opp_history) = 0;
    virtual std::string name() const = 0;
    virtual void reset() {} // For stateful strategies
};

// Factory function to create strategies from names. Declaration here.
std::unique_ptr<Strategy> createStrategy(const std::string& name);