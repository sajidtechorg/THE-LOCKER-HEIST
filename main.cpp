#include "heist.h"

int main() {
    // the score board is created here and only connected to the engine (association)
    ScoreBoard score_board;
    // the engine is a singleton, so it is taken with get_instance() as a reference
    GameEngine& engine = GameEngine::get_instance();
    engine.set_score_board(&score_board);
    engine.start();

    // every exception of the game is caught here, the game then goes on
    while (engine.is_running()) {
        try {
            engine.run_step();
        } catch (const GameException& error) {
            engine.report_error(error);
        } catch (...) {
            cout << "An unexpected error happened, the game will close" << endl;
            return 1;
        }
    }
    return 0;
}
