#include "heist.h"

int main() {
    // temporary check that the module 1 types compile and work
    Tile t = { 'A', true };
    Location loc = { "Kitchen", 'E', 5, Word1, false };
    Clue c = { loc.letterClueLoc, loc.boardPosLoc, loc.locRow };
    GameState state = Menu;

    cout << "Tile: " << t.tileLetter << " filled=" << t.filled << endl;
    cout << "Clue: " << c.clueLetter << " at " << c.cluePosition << endl;
    cout << "State is menu: " << (state == Menu) << endl;
    return 0;
}
