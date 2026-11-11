#pragma once
#include <iostream>
#include <string>
using namespace std;

// enumerations

// tells which of the two scrabble rows a letter belongs to
enum WordRow { Word1, Word2 };

// current state of the player's game
enum GameState { Menu, Playing, Won, Lost, Exit };

// options of the main menu
enum MenuChoice { NewGame = 1, LeaderBoard, About, Rules, ExitGame };

// structures

// one slot of the scrabble board
struct Tile {
    char tileLetter;   // letter stored in the slot
    bool filled;       // true once a letter has been placed
};

// a letter handed over by a location or a character
struct Clue {
    char clueLetter;   // the letter itself
    int cluePosition;  // its position on the board
    WordRow clueRow;   // row (word 1 or word 2) it belongs to
};

// one of the seven places in the cottage
struct Location {
    string locName;        // name of the place
    char letterClueLoc;    // letter found here
    int boardPosLoc;       // board position of that letter
    WordRow locRow;        // always Word1 for locations
    bool locSearchFlag;    // true once the detective has searched it
};
