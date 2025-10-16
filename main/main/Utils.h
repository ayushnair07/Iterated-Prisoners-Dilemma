#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <tuple>
#include "common.h"
#include <utility>


inline std::pair<int, int> Payoff(Move a, Move b) {
    if (a == Move::C && b == Move::C) return { 3, 3 };
    if (a == Move::C && b == Move::D) return { 0, 5 };
    if (a == Move::D && b == Move::C) return { 5, 0 };
    return { 1, 1 };
}

#endif
