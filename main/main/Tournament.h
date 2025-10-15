#pragma once
#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "Player.h"
#include <memory>
#include <iostream>

#include <fstream>
#include "Match.h"

class Tournament {
private:
    std::vector<std::unique_ptr<Player>> players;
    int rounds;

public:
    Tournament(int r) : rounds(r) {}

    void addPlayer(std::unique_ptr<Strategy> s) {
        players.push_back(std::make_unique<LPlayer>(std::move(s)));
    }

    void playAll() {
        for (size_t i = 0; i < players.size(); ++i) {
            for (size_t j = i + 1; j < players.size(); ++j) {
                Match match(*players[i], *players[j], rounds);
                match.play();
            }
        }
    }

    void printResults() const {
        std::cout << "\nFinal Scores:\n";
        for (auto& p : players) {
            std::cout << p->getName() << ": " << p->getScore() << "\n";
        }
    }

    void exportCSV(const std::string& filename = "results.csv") const {
        std::ofstream file(filename);
        file << "Strategy,Score\n";
        for (auto& p : players)
            file << p->getName() << "," << p->getScore() << "\n";
        file.close();
    }
};

#endif
