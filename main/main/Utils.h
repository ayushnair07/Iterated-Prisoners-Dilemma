#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <tuple>
#include "common.h"
#include <utility>

// Returns the payoff for both players given their moves
inline std::pair<int, int> payoff(Move a, Move b) {
    if (a == Move::C && b == Move::C) return { 3, 3 };
    if (a == Move::C && b == Move::D) return { 0, 5 };
    if (a == Move::D && b == Move::C) return { 5, 0 };
    return { 1, 1 }; // D,D
}

#endif
