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

    Move nextMove() {
        return strategy->decide(myMoves, oppMoves);
    }

    void recordMoves(Move myMove, Move oppMove) {
        myMoves.push_back(myMove);
        oppMoves.push_back(oppMove);
    }

    void addScore(int s) { score += s; }
    int getScore() const { return score; }

    const std::string getName() const { return strategy->name(); }
    const std::vector<Move>& getMyMoves() const { return myMoves; }
};

#endif
