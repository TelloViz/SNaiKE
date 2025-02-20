#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "AI/AIPlayer.hpp"

class ScoreLogger {
public:
    struct GameSession {
        int finalScore;
        std::vector<std::pair<AIStrategy, int>> strategyChanges;  // strategy and score at change
        int gameLength;  // time or moves
    };

    static void logGame(const GameSession& session) {
        std::ofstream file("snake_scores.csv", std::ios::app);
        if (!file.is_open()) return;

        // If file is empty, write header
        file.seekp(0, std::ios::end);
        if (file.tellp() == 0) {
            file << "FinalScore,GameLength,StrategyChanges\n";
        }

        // Write game data
        file << session.finalScore << ","
             << session.gameLength << ",\"";
        
        // Write strategy changes
        for (size_t i = 0; i < session.strategyChanges.size(); ++i) {
            const auto& change = session.strategyChanges[i];
            file << "Strategy" << static_cast<int>(change.first) 
                 << "@Score" << change.second;
            if (i < session.strategyChanges.size() - 1) {
                file << ";";
            }
        }
        file << "\"\n";
        file.close();
    }
};