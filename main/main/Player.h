#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "Strategy.h"
#include <memory>


class Player {
private:
    std::unique_ptr<Strategy> strategy;
    std::vector<Move> myMoves;
    std::vector<Move> oppMoves;
    int score = 0;

public:
    Player(std::unique_ptr<Strategy> s) : strategy(std::move(s)) {}

    Move NextMove() {
        return strategy->decide(myMoves, oppMoves);
    }

    void RecordMoves(Move myMove, Move oppMove) {
        myMoves.push_back(myMove);
        oppMoves.push_back(oppMove);
    }

    void AddScore(int s) { score += s; }
    int GetScore() const { return score; }

    const std::string GetName() const { return strategy->name(); }
    const std::vector<Move>& GetMyMoves() const { return myMoves; }
};

#endif
