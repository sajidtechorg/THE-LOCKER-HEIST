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

    // values read from rules.txt
    int max_turns;
    int starting_credibility;
    int wrong_turn_penalty;
    int wrong_word_penalty;

    // dynamic array of the eight people, the engine owns and deletes them
    Character** characters;
    Character* culprit;

    // the engine owns raw pointers, so copying it is not allowed
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);

    // helper functions for reading files and input
    static void clean_line(string& line);
    static string get_field(const string& line, int index);
    static bool has_space(const string& text);
    static int column_for_role(const string& role);
    static Character* create_character(const string& role, const string& name,
                                       const string& nation, char letter, int position);
    int read_number() const;

    void show_title() const;
    void show_main_menu() const;
    void show_about() const;
    void show_rules();
    void show_characters() const;

    void load_rules();
    bool load_characters();
    void free_characters();
    void start_new_game();

public:
    GameEngine();
    ~GameEngine();
    void run();
};
