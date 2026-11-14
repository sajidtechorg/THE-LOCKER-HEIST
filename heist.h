#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
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

// the player of the game
// note: in module 3 this will be changed to inherit from character
class Detective {
private:
    string detective_name;
    int credibility;
    int turns;

public:
    Detective();
    Detective(string name, int starting_credibility);

    string get_name() const;
    int get_credibility() const;
    int get_turns() const;

    void set_name(string name);
    void lose_credibility(int amount);
    void use_turn();
};

// runs the whole game: menus, loading files and starting a new game
class GameEngine {
private:
    // fallback values used when rules.txt is missing or corrupt
    static const int default_max_turns = 25;
    static const int default_credibility = 100;
    static const int default_wrong_turn = 5;
    static const int default_wrong_word = 10;
    // number of suspects in characters.txt (victim not included)
    static const int total_suspects = 7;

    GameState state;
    Detective detective;

    // values read from rules.txt
    int max_turns;
    int starting_credibility;
    int wrong_turn_penalty;
    int wrong_word_penalty;

    // the secretly chosen culprit
    string culprit_name;
    string culprit_role;

    // helper functions for reading files and input
    static void clean_line(string& line);
    static string get_field(const string& line, int index);
    static bool has_space(const string& text);
    int read_number() const;

    void show_title() const;
    void show_main_menu() const;
    void show_about() const;
    void show_rules();

    void load_rules();
    bool load_culprit();
    void start_new_game();

public:
    GameEngine();
    void run();
};
