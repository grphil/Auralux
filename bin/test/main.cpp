#include "library/game/game.h"
#include "library/game/game_map.h"
#include "library/game/default_players.h"

#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
    TGame::TConfig config;
    {
        std::ifstream stream("small_map.txt");
        config.GameMap_ = LoadPlanarGraph([&stream]() {
            int x;
            stream >> x;
            return x;
        });
    }

    config.PlanetProductionMultiply_ = 100;

    TGame game(config);

    game.AddPlayer(std::make_unique<TAFKPlayer>());
    game.AddPlayer(std::make_unique<TStdinStdoutPlayer>(config.MaxPlayerShipMovesPerStep_));
    game.Process();

    auto errors = game.GetGameLogger().GetErrors();
    for (const auto& error : errors) {
        std::cout << error << std::endl;
    }

    PrintGameState(std::cout, game.GetGameLogger().GetFinalGameState());

    return 0;
}
