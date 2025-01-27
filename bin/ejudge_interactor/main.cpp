//
// Created by Egor Chunaev on 14.06.2021.
//

#include "library/testlib/testlib.h"

#include "library/game/game.h"
#include "library/game/game_map.h"
#include "library/game/game_result.h"
#include "library/game/default_players.h"

#include <cassert>

class TEjudgePlayer : public TTextPlayer {
public:
    using TTextPlayer::TTextPlayer;

    std::string ReadLine() override {
        return ouf.readLine();
    }
    void WriteLine(const std::string& line) override {
        std::cout << line << std::endl;
    }
    void OnTurnEnd() override {
    }
};

std::pair<std::vector<std::unique_ptr<IPlayer>>, int> ReadPlayers(const TGame::TConfig& gameConfig) {
    std::vector<std::unique_ptr<IPlayer>> players;

    int ejudgePlayerId = -1;
    for (size_t i = 0; i < gameConfig.GameMap_.StartPlanets_.size(); ++i) {
        std::string playerType = inf.readToken();

        if (auto defaultPlayer = CreateDefaultPlayer(playerType)) {
            players.push_back(std::move(defaultPlayer));
        } else if (playerType == "ejudge") {
            assert(ejudgePlayerId == -1);
            ejudgePlayerId = static_cast<int>(i);
            players.push_back(std::make_unique<TEjudgePlayer>(gameConfig.MaxPlayerShipMovesPerStep_));
        } else {
            assert(false);
        }
    }
    assert(ejudgePlayerId != -1);

    return {std::move(players), ejudgePlayerId};
}

int main(int argc, char *argv[]) {
    registerInteraction(argc, argv);

    TGame::TConfig config;
    config.GameMap_ = LoadPlanarGraph([]() {
        return inf.readInt();
    });
    // Only 10000 moves in ejudge
    config.MaxSteps_ = 10000;

    TGame game(config);

    auto [players, ejudgePlayerId] = ReadPlayers(config);
    for (auto& player : players) {
        game.AddPlayer((std::move(player)));
    }
    game.Process();

    const auto& gameLogger = game.GetGameLogger();
    const auto& errors = gameLogger.GetErrors();
    const auto& finalGameState = gameLogger.GetFinalGameState();
    const auto gameResult = GetGameResult(gameLogger);

    PrintHumanReadableGameResult(tout, gameResult);
    tout << '\n';
    PrintGameState(tout, finalGameState, /* planetInfoOnly = */ true);

    size_t maxErrorsInOutput = 50;
    for (size_t i = 0; i < std::min(maxErrorsInOutput, errors.size()); ++i) {
        tout << errors[i] << '\n';
    }
    if (errors.size() > maxErrorsInOutput) {
        tout << "...\n";
    }
    tout.flush();

    if (gameResult.Result_ != TGameResult::EResult::ONE_WINNER) {
        quitf(_wa, "Game result is '%s' but must be one winner", (gameResult.Result_ == TGameResult::EResult::NO_WINNER ? "no winner" : "draw"));
    }

    if (gameResult.WinnerId_ != ejudgePlayerId + 1) {
        quitf(_wa, "Winner is %d, but ejudge player id is %d", gameResult.WinnerId_, ejudgePlayerId + 1);
    }

    if (!errors.empty()) {
        quitf(_wa, "Errors not empty (strict format checking in ejudge)");
    }

    quitf(_ok, "Winner is %d", ejudgePlayerId + 1);
}
