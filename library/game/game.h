//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_H
#define AURALUX_GAME_H

#include "game_state.h"
#include "player.h"

class TGame {
public:
    struct TConfig {
        int MaxSteps_ = 10000;

        int StartShipsCount_ = 100;

        int ShipsToCapturePlanet_ = 100;
        std::vector<int> ShipsToUpgradePlanet_ = {0, 100, 200, 400};
        std::vector<int> PerLevelPlanetArmor_ = {0, 100, 200, 300};
        int PlanetProductionMultiply_ = 1;

        int MaxShipGroupsInSpace_ = 1000;

        TGameMap GameMap_;
    };

private:
    struct TPlayerInfo {
        std::unique_ptr<IPlayer> PlayerEngine_;
        int PlayerId_;
        bool IsDead_;
        bool IsDisqualified_;
    };

public:
    explicit TGame(const TConfig& config);

    void AddPlayer(std::unique_ptr<IPlayer> player);

    void Process();

private:
    void Init();
    void Step();
    void PrePlayerMove(int playerId);
    void PlayerMove(int playerId);

    bool IsPlayerDead(int playerId);
    int GetShipGroupsInSpace(int playerId);

    bool IsValidPlanetId(int planetId);

private:
    const TConfig Config_;
    TGameState GameState_;
    std::vector<TLastShipMoves> LastShipMovesByPlayer_;
    std::vector<TPlayerInfo> Players_;
};

#endif // AURALUX_GAME_H