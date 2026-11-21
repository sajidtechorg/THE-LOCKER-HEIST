#include "heist.h"

int main() {
    // the score board is created here and only connected to the engine (association)
    ScoreBoard score_board;
    GameEngine engine;
    engine.set_score_board(&score_board);
    engine.run();
    return 0;
}
