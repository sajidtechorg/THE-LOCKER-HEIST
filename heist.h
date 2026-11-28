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

// ---------- module 7: exceptions ----------

// base class of every exception in the game
// penalty_kind tells the engine what the mistake costs: 0 nothing, 1 wrong turn, 2 wrong word
class GameException {
private:
    string message;
    int penalty_kind;

public:
    GameException(string text, int kind) {
        message = text;
        penalty_kind = kind;
    }
    virtual ~GameException() {
    }
    string what() const {
        return message;
    }
    int get_penalty_kind() const {
        return penalty_kind;
    }
};

// a text file is missing or corrupt
class FileException : public GameException {
public:
    FileException(string text) : GameException(text, 0) {
    }
};

// the player typed something that is not allowed, no penalty
class InvalidInputException : public GameException {
public:
    InvalidInputException(string text) : GameException(text, 0) {
    }
};

// a word typed by the player is wrong, costs a wrong word
class InvalidWordException : public GameException {
public:
    InvalidWordException(string text) : GameException(text, 2) {
    }
};

// a rack has no space left
class RackFullException : public GameException {
public:
    RackFullException(string text) : GameException(text, 0) {
    }
};

// a rack has nothing to take out
class RackEmptyException : public GameException {
public:
    RackEmptyException(string text) : GameException(text, 0) {
    }
};

// a board or array position is outside the allowed range
class InvalidPositionException : public GameException {
public:
    InvalidPositionException(string text) : GameException(text, 0) {
    }
};

// a board slot already holds a letter
class SlotFilledException : public GameException {
public:
    SlotFilledException(string text) : GameException(text, 0) {
    }
};

// the detective does not have enough credibility to interview someone, costs a wrong turn
class CredibilityException : public GameException {
public:
    CredibilityException(string text) : GameException(text, 1) {
    }
};

// an action was tried too early, costs a wrong turn
class ActionNotAllowedException : public GameException {
public:
    ActionNotAllowedException(string text) : GameException(text, 1) {
    }
};

// the detective accused the wrong person, costs a wrong turn
class WrongAccusationException : public GameException {
public:
    WrongAccusationException(string text) : GameException(text, 1) {
    }
};

// ---------- module 8: template ----------

// generic dynamic array used by cottage, scrabble and tile rack
// it owns its memory and makes deep copies, so those classes no longer need new and delete
template <class T>
class DynamicArray {
private:
    T* data;
    int length;

public:
    // empty array
    DynamicArray() {
        data = 0;
        length = 0;
    }

    // array with the given number of slots, all values start at zero
    DynamicArray(int size) {
        length = size;
        if (size > 0) {
            data = new T[size]();
        } else {
            data = 0;
            length = 0;
        }
    }

    // deep copy: a new block of memory is created and every element is copied
    DynamicArray(const DynamicArray<T>& other) {
        length = other.length;
        if (length > 0) {
            data = new T[length];
            for (int i = 0; i < length; i++) {
                data[i] = other.data[i];
            }
        } else {
            data = 0;
        }
    }

    // deep copy assignment, safe against self assignment
    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this != &other) {
            delete[] data;
            length = other.length;
            if (length > 0) {
                data = new T[length];
                for (int i = 0; i < length; i++) {
                    data[i] = other.data[i];
                }
            } else {
                data = 0;
            }
        }
        return *this;
    }

    ~DynamicArray() {
        delete[] data;
    }

    int size() const {
        return length;
    }

    // access with a range check
    T& operator[](int index) {
        if (index < 0 || index >= length) {
            throw InvalidPositionException("Array index is out of range");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= length) {
            throw InvalidPositionException("Array index is out of range");
        }
        return data[index];
    }
};

// ---------- structures ----------

// one slot of the scrabble board
struct Tile {
    char tileLetter;   // letter stored in the slot
    bool filled;       // true once a letter has been placed

    // module 9: stream operators, they are friends because they are not members
    friend ostream& operator<<(ostream& out, const Tile& tile);
    friend istream& operator>>(istream& in, Tile& tile);
};

// two tiles are equal when their letters match, capital or small
bool operator==(const Tile& first, const Tile& second);

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

    friend ostream& operator<<(ostream& out, const Location& place);
    friend istream& operator>>(istream& in, Location& place);
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

    // module 9: the engine reads and sets the culprit flag directly (friend class)
    friend class GameEngine;
    friend ostream& operator<<(ostream& out, const Character& person);
    friend istream& operator>>(istream& in, Character& person);

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

/* ---------- old module 5 version of TileRack (raw pointer), kept for reference ----------
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

    // add_tile throws RackFullException, remove_tile throws RackEmptyException
    // or InvalidPositionException
    void add_tile(char letter);
    char remove_tile(int index);
    int get_count() const;
    int get_capacity() const;
    bool is_full() const;
    bool is_empty() const;
    void clear();
    void display() const;

    // merges two racks into one with a space tile between them
    TileRack operator+(const TileRack& other) const;
};
*/

// holds the letters the detective has collected
// module 8: the tiles now live in a DynamicArray, so no raw pointer is needed
class TileRack {
private:
    DynamicArray<Tile> tiles;
    int count;

public:
    TileRack();
    TileRack(int capacity);
    TileRack(const TileRack& other);
    TileRack& operator=(const TileRack& other);
    ~TileRack();

    // add_tile throws RackFullException, remove_tile throws RackEmptyException
    // or InvalidPositionException
    void add_tile(char letter);
    char remove_tile(int index);
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

/* ---------- old module 4 version of Cottage (raw pointer), kept for reference ----------
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

    // reads locations.txt, throws a FileException if it is missing or corrupt
    void load(const string& culprit_role);

    int get_location_count() const;
    bool is_valid_index(int index) const;
    bool is_searched(int index) const;
    const Location& get_location(int index) const;
    Clue search(int index);
    Clue peek(int index) const;
    void display() const;
};
*/

// the seven places of the cottage
// module 8: the locations now live in a DynamicArray
// module 10, aggregation (hollow diamond in the class diagram):
// aggregation means the whole holds a collection of parts, but each part is a simple
// data object that has its own meaning (a location struct is just a place with a letter)
// the cottage is the whole and the location array is the collection it is made of
class Cottage {
private:
    static const int total_locations = 7;
    DynamicArray<Location> locations;   // aggregation: the cottage aggregates its locations

public:
    Cottage();
    Cottage(const Cottage& other);
    Cottage& operator=(const Cottage& other);
    ~Cottage();

    // reads locations.txt, throws a FileException if it is missing or corrupt
    void load(const string& culprit_role);

    int get_location_count() const;
    bool is_valid_index(int index) const;
    bool is_searched(int index) const;
    const Location& get_location(int index) const;
    Clue search(int index);
    Clue peek(int index) const;
    void display() const;
};

/* ---------- old module 4 version of Scrabble (jagged array), kept for reference ----------
// the scrabble board: a jagged array, row 1 has 7 slots and row 2 has 8 slots
class Scrabble {
public:
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
    void place_tile(const Clue& clue);
    bool is_row_complete(WordRow row) const;
    bool is_row_solved(WordRow row) const;
    bool both_solved() const;
    void check_word(WordRow row, const string& typed);
    string get_word(WordRow row) const;
    void display() const;
};
*/

// the scrabble board: row 1 has 7 slots and row 2 has 8 slots
// module 8: the jagged array is replaced by two DynamicArray rows
class Scrabble {
private:
    DynamicArray<Tile> row1;
    DynamicArray<Tile> row2;
    string word1;
    string word2;
    bool solved1;
    bool solved2;

    friend ostream& operator<<(ostream& out, const Scrabble& board);
    friend istream& operator>>(istream& in, Scrabble& board);

public:
    Scrabble();
    Scrabble(const Scrabble& other);
    Scrabble& operator=(const Scrabble& other);
    ~Scrabble();

    void setup(const string& first_word, const string& second_word);
    void place_tile(const Clue& clue);
    bool is_row_complete(WordRow row) const;
    bool is_row_solved(WordRow row) const;
    bool both_solved() const;
    void check_word(WordRow row, const string& typed);
    string get_word(WordRow row) const;
    void display() const;
};

// ---------- module 6: score board (file handling) ----------

// keeps the results of finished games in leader/scoreBoard.txt
class ScoreBoard {
private:
    string file_name;

public:
    ScoreBoard();

    // adds one line to the end of the file, throws a FileException if it cannot be opened
    void add_score(const string& detective_name, bool won, int credibility,
                   int turns_left, const string& culprit_role);

    // prints the file as a table, throws a FileException if it is missing or corrupt
    void display() const;
};

// ---------- game engine ----------

// runs the whole game: menus, loading files and starting a new game
// module 12, singleton pattern: only one engine can ever exist, so the whole game
// runs as a single entity, the one object is reached with get_instance()
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

    // module 10, composition (filled diamond in the class diagram):
    // composition means the whole owns its parts, the parts are created with the engine
    // and destroyed with it, they cannot live without it
    // the engine is composed of a detective, a cottage, a scrabble board and two tile racks
    Detective detective;
    Cottage cottage;
    Scrabble scrabble;
    TileRack rack_word1;
    TileRack rack_word2;
    bool interviewed[total_characters];

    // module 10, association (plain arrow in the class diagram):
    // association means one class only knows and uses another, it does not own it
    // main() creates the score board and the engine just keeps a pointer to it, so the
    // score board stays alive even if the engine is gone
    ScoreBoard* score_board;

    // values read from rules.txt
    int max_turns;
    int starting_credibility;
    int wrong_turn_penalty;
    int wrong_word_penalty;

    // dynamic array of the eight people, the engine owns and deletes them
    Character** characters;
    Character* culprit;
    string culprit_role;

    // singleton: the constructor is private so nobody outside can create an engine
    // copying is not allowed either, it would make a second engine and the engine
    // owns raw pointers (copy functions are declared here and never defined)
    GameEngine();
    GameEngine(const GameEngine& other);
    GameEngine& operator=(const GameEngine& other);
    ~GameEngine();

    static bool has_space(const string& text);
    static Character* create_character(const string& role, const string& name,
                                       const string& nation, char letter, int position);
    string read_line();
    int read_choice(int low, int high);
    bool read_yes_no();

    void show_title() const;
    void show_main_menu() const;
    void show_about() const;
    void show_rules();

    void load_rules();
    void load_characters();
    void load_clue_words(string& first_word, string& second_word) const;
    void free_characters();
    void start_new_game();

    // module 13: screens and menus
    void show_status() const;
    void show_rack() const;
    void show_character_menu() const;
    void main_menu_step();
    void game_menu_step();

    // module 13: the actions of the game menu
    void visit_location();
    void interview_person();
    void view_board() const;
    void form_word();
    void accuse_suspect();
    void quit_game();
    void check_end_conditions();
    void finish_game();
    void clear_screen() const;

public:
    // access method: creates the engine on the first call and returns the same one every time
    // it returns a reference, so the caller must also store it as a reference
    static GameEngine& get_instance();

    void set_score_board(ScoreBoard* board);
    void start();
    bool is_running() const;
    void run_step();
    void report_error(const GameException& error);
};
