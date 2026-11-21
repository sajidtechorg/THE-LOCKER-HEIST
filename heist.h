#pragma once
// lets visual studio accept time and file functions without the unsafe warnings
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

// ---------- enumerations ----------

// tells which of the two scrabble rows a letter belongs to
enum WordRow { Word1, Word2 };

// current state of the player's game
enum GameState { Menu, Playing, Won, Lost, Exit };

// options of the main menu
enum MenuChoice { NewGame = 1, LeaderBoard, About, Rules, ExitGame };

// ---------- structures ----------

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

// ---------- helper functions ----------

void clean_line(string& line);
string get_field(const string& line, int index);
string to_upper(const string& text);
string underscore_to_space(const string& text);
int role_index(const string& role);

// ---------- characters ----------

// base class of every person in the game
// it is abstract: display() and required_credibility() are pure virtual
class Character {
private:
    string characterName;
    string characterRole;
    string nationality;
    bool isCulprit;

public:
    Character(string name, string role, string nation, bool culprit);
    virtual ~Character();

    string get_name() const;
    string get_role() const;
    string get_nationality() const;
    bool is_culprit() const;
    void set_culprit(bool value);

    // letter, board position and row given in an interview
    // only the victim and the suspects really have one
    virtual char get_letter() const;
    virtual int get_position() const;
    virtual WordRow get_row() const;

    // lowest credibility the detective needs to interview this character
    virtual int required_credibility() const = 0;

    // every derived class shows itself in its own way
    virtual void display() const = 0;
};

// the player of the game
class Detective : public Character {
private:
    int credibility;
    int turns;

public:
    Detective();
    Detective(string name, int starting_credibility);

    int get_credibility() const;
    int get_turns() const;
    void lose_credibility(int amount);
    void use_turn();

    virtual int required_credibility() const;
    virtual void display() const;
};

// the business owner, interviewed by the detective but never a suspect
class Victim : public Character {
private:
    char letterClueChar;
    int boardPosChar;
    WordRow clueRowChar;

public:
    Victim(string name, string nation, char letter, int position);

    virtual char get_letter() const;
    virtual int get_position() const;
    virtual WordRow get_row() const;

    virtual int required_credibility() const;
    virtual void display() const;
};

// base class of the seven suspects, still abstract
class Suspect : public Character {
protected:
    char letterClueChar;
    int boardPosChar;
    WordRow clueRowChar;

public:
    Suspect(string name, string role, string nation, char letter, int position);

    virtual char get_letter() const;
    virtual int get_position() const;
    virtual WordRow get_row() const;
};

// suspects who know the owner through business
// virtual inheritance keeps a single copy of suspect inside pa (diamond problem)
class BusinessRelations : virtual public Suspect {
public:
    BusinessRelations(string name, string role, string nation, char letter, int position);
    virtual int required_credibility() const;   // 50, rule for partners
};

// suspects who work in the cottage, they can always be interviewed
class HouseHelp : public Suspect {
public:
    HouseHelp(string name, string role, string nation, char letter, int position);
    virtual int required_credibility() const;   // 0
};

// suspects close to the owner's family
class CloseRelations : virtual public Suspect {
public:
    CloseRelations(string name, string role, string nation, char letter, int position);
    virtual int required_credibility() const;   // 75, rule for the child
};

// partner 1 and partner 2 are both objects of this class
class Partner : public BusinessRelations {
public:
    Partner(string name, string role, string nation, char letter, int position);
    virtual void display() const;
};

// personal assistant, both a business relation and a close relation
class PA : public BusinessRelations, public CloseRelations {
public:
    PA(string name, string nation, char letter, int position);
    virtual int required_credibility() const;   // 35
    virtual void display() const;
};

class Cook : public HouseHelp {
public:
    Cook(string name, string nation, char letter, int position);
    virtual void display() const;
};

class Guard : public HouseHelp {
public:
    Guard(string name, string nation, char letter, int position);
    virtual void display() const;
};

class Servant : public HouseHelp {
public:
    Servant(string name, string nation, char letter, int position);
    virtual void display() const;
};

// the teenage child of the owner
class Child : public CloseRelations {
public:
    Child(string name, string nation, char letter, int position);
    virtual void display() const;
};

// ---------- module 5: tile rack (dynamic memory) ----------

// holds the letters the detective has collected, stored in a dynamically allocated array
class TileRack {
private:
    Tile* tiles;
    int capacity;
    int count;

    void copy_from(const TileRack& other);

public:
    TileRack();
    TileRack(int size);
    TileRack(const TileRack& other);
    TileRack& operator=(const TileRack& other);
    ~TileRack();

    // return false when the rack is full or the index is wrong
    // module 7 turns these failures into exceptions
    bool add_tile(char letter);
    bool remove_tile(int index, char& letter);
    int get_count() const;
    int get_capacity() const;
    bool is_full() const;
    bool is_empty() const;
    void clear();
    void display() const;

    // merges two racks into one with a space tile between them
    TileRack operator+(const TileRack& other) const;
};

// ---------- module 4: game boards (dynamic memory) ----------

// the seven places of the cottage, stored in a dynamically allocated array
// aggregation: the cottage uses an array of location structs
class Cottage {
private:
    static const int total_locations = 7;
    Location* locations;
    int location_count;

public:
    Cottage();
    Cottage(const Cottage& other);
    Cottage& operator=(const Cottage& other);
    ~Cottage();

    // reads locations.txt, returns false if the file is missing or corrupt
    // module 7 turns this failure into an exception
    bool load(const string& culprit_role);

    int get_location_count() const;
    bool is_valid_index(int index) const;
    bool is_searched(int index) const;
    const Location& get_location(int index) const;
    Clue search(int index);
    Clue peek(int index) const;
    void display() const;
};

// the scrabble board: a jagged array, row 1 has 7 slots and row 2 has 8 slots
class Scrabble {
public:
    // results of place_tile(), module 7 turns these into exceptions
    static const int placed_ok = 0;
    static const int invalid_position = 1;
    static const int slot_filled = 2;
    static const int board_not_ready = 3;

private:
    Tile** board;
    int row_size[2];
    string word1;
    string word2;
    bool solved1;
    bool solved2;

    void allocate_board(int first_size, int second_size);
    void free_board();
    void copy_from(const Scrabble& other);

public:
    Scrabble();
    Scrabble(const Scrabble& other);
    Scrabble& operator=(const Scrabble& other);
    ~Scrabble();

    void setup(const string& first_word, const string& second_word);
    int place_tile(const Clue& clue);
    bool is_row_complete(WordRow row) const;
    bool is_row_solved(WordRow row) const;
    bool both_solved() const;
    bool check_word(WordRow row, const string& typed);
    string get_word(WordRow row) const;
    void display() const;
};

// ---------- game engine ----------

// runs the whole game: menus, loading files and starting a new game
class GameEngine {
private:
    // fallback values used when rules.txt is missing or corrupt
    static const int default_max_turns = 25;
    static const int default_credibility = 100;
    static const int default_wrong_turn = 5;
    static const int default_wrong_word = 10;
    // eight people in characters.txt, the first one is the victim
    static const int total_characters = 8;
    static const int total_suspects = 7;

    GameState state;
    Detective detective;
    Cottage cottage;
    Scrabble scrabble;
    TileRack rack_word1;
    TileRack rack_word2;
    bool interviewed[total_characters];

    // values read from rules.txt
    int max_turns;
    int starting_credibility;
    int wrong_turn_penalty;
    int wrong_word_penalty;

    // dynamic array of the eight people, the engine owns and deletes them
    Character** characters;
    Character* culprit;
    string culprit_role;

    // the engine owns raw pointers, so copying it is not allowed
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);

    static bool has_space(const string& text);
    static Character* create_character(const string& role, const string& name,
                                       const string& nation, char letter, int position);
    int read_number();

    void show_title() const;
    void show_main_menu() const;
    void show_about() const;
    void show_rules();
    void show_characters() const;

    void load_rules();
    bool load_characters();
    bool load_clue_words(string& first_word, string& second_word) const;
    void free_characters();
    void start_new_game();

    // temporary tests of modules 4 and 5, module 13 replaces them with the real game menu
    void show_rack() const;
    void test_visit_locations();
    void test_interview_people();

public:
    GameEngine();
    ~GameEngine();
    void run();
};
