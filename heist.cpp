#include "heist.h"

// ---------- helper functions ----------

// removes the hidden carriage return that windows files can leave at the end
void clean_line(string& line) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }
}

// returns the field at the given index from a line separated by |
string get_field(const string& line, int index) {
    int current = 0;
    string field = "";
    for (int i = 0; i < (int)line.size(); i++) {
        if (line[i] == '|') {
            if (current == index) {
                return field;
            }
            current++;
            field = "";
        } else {
            field = field + line[i];
        }
    }
    if (current == index) {
        return field;
    }
    return "";
}

// turns every letter into a capital letter
string to_upper(const string& text) {
    string result = text;
    for (int i = 0; i < (int)result.size(); i++) {
        if (result[i] >= 'a' && result[i] <= 'z') {
            result[i] = result[i] - 'a' + 'A';
        }
    }
    return result;
}

// changes underscores to spaces so file names look nice on screen
string underscore_to_space(const string& text) {
    string result = text;
    for (int i = 0; i < (int)result.size(); i++) {
        if (result[i] == '_') {
            result[i] = ' ';
        }
    }
    return result;
}

// position of a culprit role in the data files, -1 for an unknown role
// the letters of that role sit in column 4 + index of characters.txt
// and in column 2 + index of locations.txt
int role_index(const string& role) {
    if (role == "Cook") {
        return 0;
    } else if (role == "Guard") {
        return 1;
    } else if (role == "Servant") {
        return 2;
    } else if (role == "Partner 1") {
        return 3;
    } else if (role == "Partner 2") {
        return 4;
    } else if (role == "PA") {
        return 5;
    } else if (role == "Child") {
        return 6;
    }
    return -1;
}

// ---------- module 9: operators of tile, location and character ----------

// capital version of one letter
static char upper_char(char letter) {
    if (letter >= 'a' && letter <= 'z') {
        return letter - 'a' + 'A';
    }
    return letter;
}

// a filled tile shows its letter, an empty one shows a blank box
ostream& operator<<(ostream& out, const Tile& tile) {
    if (tile.filled) {
        out << "[" << tile.tileLetter << "]";
    } else {
        out << "[ ]";
    }
    return out;
}

// reads one letter into a tile and marks it as filled
istream& operator>>(istream& in, Tile& tile) {
    in >> tile.tileLetter;
    tile.filled = true;
    return in;
}

// two tiles are equal when their letters match, capital or small
bool operator==(const Tile& first, const Tile& second) {
    return upper_char(first.tileLetter) == upper_char(second.tileLetter);
}

// shows the place name and whether it was searched
ostream& operator<<(ostream& out, const Location& place) {
    out << underscore_to_space(place.locName) << " - ";
    if (place.locSearchFlag) {
        out << "Searched";
    } else {
        out << "NOT searched";
    }
    return out;
}

// reads name, letter and position of a location
istream& operator>>(istream& in, Location& place) {
    in >> place.locName >> place.letterClueLoc >> place.boardPosLoc;
    return in;
}

// shows name and role of a character, works for every derived class
ostream& operator<<(ostream& out, const Character& person) {
    out << person.characterName << " (" << person.characterRole << ")";
    return out;
}

// reads name and nationality of a character
istream& operator>>(istream& in, Character& person) {
    in >> person.characterName >> person.nationality;
    return in;
}

// ---------- character ----------

Character::Character(string name, string role, string nation, bool culprit) {
    characterName = name;
    characterRole = role;
    nationality = nation;
    isCulprit = culprit;
}

// virtual destructor so derived objects are deleted correctly through a base pointer
Character::~Character() {
}

string Character::get_name() const {
    return characterName;
}

string Character::get_role() const {
    return characterRole;
}

string Character::get_nationality() const {
    return nationality;
}

bool Character::is_culprit() const {
    return isCulprit;
}

void Character::set_culprit(bool value) {
    isCulprit = value;
}

// default answers for characters that do not give a letter, like the detective
char Character::get_letter() const {
    return ' ';
}

int Character::get_position() const {
    return 0;
}

WordRow Character::get_row() const {
    return Word2;
}

// ---------- detective ----------

// default detective with no name and no credibility
Detective::Detective() : Character("", "Detective", "Malay", false) {
    credibility = 0;
    turns = 0;
}

// detective created with a name and starting credibility
Detective::Detective(string name, int starting_credibility)
    : Character(name, "Detective", "Malay", false) {
    credibility = starting_credibility;
    turns = 0;
}

int Detective::get_credibility() const {
    return credibility;
}

int Detective::get_turns() const {
    return turns;
}

// credibility never goes below zero
void Detective::lose_credibility(int amount) {
    credibility = credibility - amount;
    if (credibility < 0) {
        credibility = 0;
    }
}

// the game engine tells the detective when a turn is used
void Detective::use_turn() {
    turns++;
}

// the detective is never interviewed
int Detective::required_credibility() const {
    return 0;
}

void Detective::display() const {
    cout << get_name() << " (Detective) - Credibility: " << credibility
         << " | Turns used: " << turns << endl;
}

// ---------- victim ----------

Victim::Victim(string name, string nation, char letter, int position)
    : Character(name, "Victim", nation, false) {
    letterClueChar = letter;
    boardPosChar = position;
    clueRowChar = Word2;
}

char Victim::get_letter() const {
    return letterClueChar;
}

int Victim::get_position() const {
    return boardPosChar;
}

WordRow Victim::get_row() const {
    return clueRowChar;
}

// the owner can only be interviewed with high credibility
int Victim::required_credibility() const {
    return 80;
}

void Victim::display() const {
    cout << get_name() << " (Victim) - " << get_nationality()
         << " - Owner of the cottage" << endl;
}

// ---------- suspect and its groups ----------

Suspect::Suspect(string name, string role, string nation, char letter, int position)
    : Character(name, role, nation, false) {
    letterClueChar = letter;
    boardPosChar = position;
    clueRowChar = Word2;
}

char Suspect::get_letter() const {
    return letterClueChar;
}

int Suspect::get_position() const {
    return boardPosChar;
}

WordRow Suspect::get_row() const {
    return clueRowChar;
}

// the suspect part is built by the most derived class, so it is repeated here
BusinessRelations::BusinessRelations(string name, string role, string nation, char letter, int position)
    : Suspect(name, role, nation, letter, position) {
}

int BusinessRelations::required_credibility() const {
    return 50;
}

HouseHelp::HouseHelp(string name, string role, string nation, char letter, int position)
    : Suspect(name, role, nation, letter, position) {
}

int HouseHelp::required_credibility() const {
    return 0;
}

CloseRelations::CloseRelations(string name, string role, string nation, char letter, int position)
    : Suspect(name, role, nation, letter, position) {
}

int CloseRelations::required_credibility() const {
    return 75;
}

// ---------- the seven suspect roles ----------

Partner::Partner(string name, string role, string nation, char letter, int position)
    : Suspect(name, role, nation, letter, position),
      BusinessRelations(name, role, nation, letter, position) {
}

void Partner::display() const {
    cout << get_name() << " (" << get_role() << ") - " << get_nationality()
         << " - Business partner of the owner" << endl;
}

// pa builds the single suspect part itself because of the virtual inheritance
PA::PA(string name, string nation, char letter, int position)
    : Suspect(name, "PA", nation, letter, position),
      BusinessRelations(name, "PA", nation, letter, position),
      CloseRelations(name, "PA", nation, letter, position) {
}

// both parents give a different value, so pa decides which one is used
int PA::required_credibility() const {
    return 35;
}

void PA::display() const {
    cout << get_name() << " (PA) - " << get_nationality()
         << " - Right hand of the owner and knows the secrets" << endl;
}

Cook::Cook(string name, string nation, char letter, int position)
    : HouseHelp(name, "Cook", nation, letter, position) {
}

void Cook::display() const {
    cout << get_name() << " (Cook) - " << get_nationality()
         << " - Works in the kitchen" << endl;
}

Guard::Guard(string name, string nation, char letter, int position)
    : HouseHelp(name, "Guard", nation, letter, position) {
}

void Guard::display() const {
    cout << get_name() << " (Guard) - " << get_nationality()
         << " - Guards the cottage" << endl;
}

Servant::Servant(string name, string nation, char letter, int position)
    : HouseHelp(name, "Servant", nation, letter, position) {
}

void Servant::display() const {
    cout << get_name() << " (Servant) - " << get_nationality()
         << " - Serves in the cottage" << endl;
}

Child::Child(string name, string nation, char letter, int position)
    : Suspect(name, "Child", nation, letter, position),
      CloseRelations(name, "Child", nation, letter, position) {
}

void Child::display() const {
    cout << get_name() << " (Child) - " << get_nationality()
         << " - Panicked teenage child of the owner" << endl;
}

/* old module 4 version of Cottage (raw pointer), kept for reference
// ---------- module 4: cottage ----------

// empty cottage, load() fills it
Cottage::Cottage() {
    locations = 0;
    location_count = 0;
}

// deep copy: a new array is created and every location is copied into it
Cottage::Cottage(const Cottage& other) {
    location_count = other.location_count;
    if (location_count > 0) {
        locations = new Location[location_count];
        for (int i = 0; i < location_count; i++) {
            locations[i] = other.locations[i];
        }
    } else {
        locations = 0;
    }
}

// deep copy assignment, safe against self assignment
Cottage& Cottage::operator=(const Cottage& other) {
    if (this != &other) {
        delete[] locations;
        location_count = other.location_count;
        if (location_count > 0) {
            locations = new Location[location_count];
            for (int i = 0; i < location_count; i++) {
                locations[i] = other.locations[i];
            }
        } else {
            locations = 0;
        }
    }
    return *this;
}

// deletes the array, the locations inside it go with it
Cottage::~Cottage() {
    delete[] locations;
}

// reads locations.txt, every place gets the letter of the culprit role
// the old array is only replaced when the whole file was read correctly
void Cottage::load(const string& culprit_role) {
    int index = role_index(culprit_role);
    if (index < 0) {
        throw FileException("The culprit role is not known");
    }

    ifstream file("locations.txt");
    if (!file) {
        throw FileException("Locations.txt could not be found");
    }

    Location* loaded = new Location[total_locations];
    string line;
    getline(file, line);   // header line

    int count = 0;
    while (count < total_locations && getline(file, line)) {
        clean_line(line);
        if (line.empty()) {
            continue;
        }
        string letter = get_field(line, 2 + index);
        int position = atoi(get_field(line, 1).c_str());
        if (letter.empty() || position < 1 || position > total_locations) {
            delete[] loaded;
            throw FileException("Locations.txt is corrupt, a letter or position is wrong");
        }
        loaded[count].locName = get_field(line, 0);
        loaded[count].letterClueLoc = letter[0];
        loaded[count].boardPosLoc = position;
        loaded[count].locRow = Word1;
        loaded[count].locSearchFlag = false;
        count++;
    }

    if (count < total_locations) {
        delete[] loaded;
        throw FileException("Locations.txt is corrupt, it needs 7 places");
    }

    delete[] locations;
    locations = loaded;
    location_count = total_locations;
}

int Cottage::get_location_count() const {
    return location_count;
}

// checks that an index points to a real location
bool Cottage::is_valid_index(int index) const {
    return index >= 0 && index < location_count;
}

// the next four functions throw an InvalidPositionException for a wrong index
bool Cottage::is_searched(int index) const {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    return locations[index].locSearchFlag;
}

const Location& Cottage::get_location(int index) const {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    return locations[index];
}

// marks the place as searched and hands over its clue
Clue Cottage::search(int index) {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    locations[index].locSearchFlag = true;
    Clue clue = { locations[index].letterClueLoc, locations[index].boardPosLoc, locations[index].locRow };
    return clue;
}

// gives the clue of a place again without changing anything
Clue Cottage::peek(int index) const {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    Clue clue = { locations[index].letterClueLoc, locations[index].boardPosLoc, locations[index].locRow };
    return clue;
}

// shows all places with their search status
void Cottage::display() const {
    for (int i = 0; i < location_count; i++) {
        cout << "[" << (i + 1) << "] " << underscore_to_space(locations[i].locName) << " - "
             << (locations[i].locSearchFlag ? "Searched" : "NOT searched") << endl;
    }
}
*/

// ---------- cottage (module 8, uses DynamicArray) ----------

Cottage::Cottage() : locations(0) {
}

// deep copy: the DynamicArray copies its own memory
Cottage::Cottage(const Cottage& other) : locations(other.locations) {
}

Cottage& Cottage::operator=(const Cottage& other) {
    if (this != &other) {
        locations = other.locations;
    }
    return *this;
}

// nothing to delete here, the DynamicArray frees its own memory
Cottage::~Cottage() {
}

// reads locations.txt, every place gets the letter of the culprit role
// the old array is only replaced when the whole file was read correctly
void Cottage::load(const string& culprit_role) {
    int index = role_index(culprit_role);
    if (index < 0) {
        throw FileException("The culprit role is not known");
    }

    ifstream file("locations.txt");
    if (!file) {
        throw FileException("Locations.txt could not be found");
    }

    DynamicArray<Location> loaded(total_locations);
    string line;
    getline(file, line);   // header line

    int count = 0;
    while (count < total_locations && getline(file, line)) {
        clean_line(line);
        if (line.empty()) {
            continue;
        }
        string letter = get_field(line, 2 + index);
        int position = atoi(get_field(line, 1).c_str());
        if (letter.empty() || position < 1 || position > total_locations) {
            throw FileException("Locations.txt is corrupt, a letter or position is wrong");
        }
        Location place;
        place.locName = get_field(line, 0);
        place.letterClueLoc = letter[0];
        place.boardPosLoc = position;
        place.locRow = Word1;
        place.locSearchFlag = false;
        loaded[count] = place;
        count++;
    }

    if (count < total_locations) {
        throw FileException("Locations.txt is corrupt, it needs 7 places");
    }
    locations = loaded;
}

int Cottage::get_location_count() const {
    return locations.size();
}

// checks that an index points to a real location
bool Cottage::is_valid_index(int index) const {
    return index >= 0 && index < locations.size();
}

// the next four functions throw an InvalidPositionException for a wrong index
bool Cottage::is_searched(int index) const {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    return locations[index].locSearchFlag;
}

const Location& Cottage::get_location(int index) const {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    return locations[index];
}

// marks the place as searched and hands over its clue
Clue Cottage::search(int index) {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    Location& place = locations[index];
    place.locSearchFlag = true;
    Clue clue = { place.letterClueLoc, place.boardPosLoc, place.locRow };
    return clue;
}

// gives the clue of a place again without changing anything
Clue Cottage::peek(int index) const {
    if (!is_valid_index(index)) {
        throw InvalidPositionException("There is no such location");
    }
    const Location& place = locations[index];
    Clue clue = { place.letterClueLoc, place.boardPosLoc, place.locRow };
    return clue;
}

// shows all places with their search status
void Cottage::display() const {
    for (int i = 0; i < locations.size(); i++) {
        cout << "[" << (i + 1) << "] " << locations[i] << endl;
    }
}

/* old module 4 version of Scrabble (jagged array), kept for reference
// ---------- module 4: scrabble board ----------

// empty board, setup() gives it the real size
Scrabble::Scrabble() {
    board = 0;
    row_size[0] = 0;
    row_size[1] = 0;
    word1 = "";
    word2 = "";
    solved1 = false;
    solved2 = false;
}

// creates the jagged array: an array of two row pointers, each row has its own length
void Scrabble::allocate_board(int first_size, int second_size) {
    row_size[0] = first_size;
    row_size[1] = second_size;
    board = new Tile*[2];
    board[0] = new Tile[first_size];
    board[1] = new Tile[second_size];
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < row_size[r]; c++) {
            board[r][c].tileLetter = '\0';
            board[r][c].filled = false;
        }
    }
}

// deletes the rows first and then the array of row pointers
void Scrabble::free_board() {
    if (board != 0) {
        delete[] board[0];
        delete[] board[1];
        delete[] board;
        board = 0;
    }
    row_size[0] = 0;
    row_size[1] = 0;
}

// deep copy of the whole board
void Scrabble::copy_from(const Scrabble& other) {
    if (other.board != 0) {
        allocate_board(other.row_size[0], other.row_size[1]);
        for (int r = 0; r < 2; r++) {
            for (int c = 0; c < row_size[r]; c++) {
                board[r][c] = other.board[r][c];
            }
        }
    }
    word1 = other.word1;
    word2 = other.word2;
    solved1 = other.solved1;
    solved2 = other.solved2;
}

Scrabble::Scrabble(const Scrabble& other) {
    board = 0;
    row_size[0] = 0;
    row_size[1] = 0;
    copy_from(other);
}

Scrabble& Scrabble::operator=(const Scrabble& other) {
    if (this != &other) {
        free_board();
        copy_from(other);
    }
    return *this;
}

Scrabble::~Scrabble() {
    free_board();
}

// creates fresh empty rows and remembers the two target words from clue.txt
void Scrabble::setup(const string& first_word, const string& second_word) {
    free_board();
    word1 = to_upper(first_word);
    word2 = to_upper(second_word);
    allocate_board((int)word1.size(), (int)word2.size());
    solved1 = false;
    solved2 = false;
}

// puts a clue letter at its position, positions start at 1
void Scrabble::place_tile(const Clue& clue) {
    if (board == 0) {
        throw InvalidPositionException("The board is not ready");
    }
    int row = (clue.clueRow == Word1) ? 0 : 1;
    if (clue.cluePosition < 1 || clue.cluePosition > row_size[row]) {
        throw InvalidPositionException("That board position does not exist");
    }
    Tile& slot = board[row][clue.cluePosition - 1];
    if (slot.filled) {
        throw SlotFilledException("That board slot already has a letter");
    }
    slot.tileLetter = clue.clueLetter;
    slot.filled = true;
}

// a row is complete when every slot has a letter
bool Scrabble::is_row_complete(WordRow row) const {
    if (board == 0) {
        return false;
    }
    int r = (row == Word1) ? 0 : 1;
    for (int c = 0; c < row_size[r]; c++) {
        if (!board[r][c].filled) {
            return false;
        }
    }
    return row_size[r] > 0;
}

bool Scrabble::is_row_solved(WordRow row) const {
    return (row == Word1) ? solved1 : solved2;
}

bool Scrabble::both_solved() const {
    return solved1 && solved2;
}

// compares the typed word with the real word, a wrong word throws an exception
void Scrabble::check_word(WordRow row, const string& typed) {
    if (!is_row_complete(row)) {
        throw ActionNotAllowedException("That row is not complete yet");
    }
    if (is_row_solved(row)) {
        throw InvalidInputException("That word is already solved");
    }
    string target = (row == Word1) ? word1 : word2;
    if (to_upper(typed) != target) {
        throw InvalidWordException("That is not the right word");
    }
    if (row == Word1) {
        solved1 = true;
    } else {
        solved2 = true;
    }
}

string Scrabble::get_word(WordRow row) const {
    return (row == Word1) ? word1 : word2;
}

// prints both rows, a filled slot shows its letter
void Scrabble::display() const {
    cout << "Scrabble:" << endl;
    for (int r = 0; r < 2; r++) {
        cout << "  Word " << (r + 1) << ": ";
        for (int c = 0; c < row_size[r]; c++) {
            if (board != 0 && board[r][c].filled) {
                cout << "[" << board[r][c].tileLetter << "] ";
            } else {
                cout << "[ ] ";
            }
        }
        WordRow current = (r == 0) ? Word1 : Word2;
        if (is_row_solved(current)) {
            cout << " -> SOLVED";
        } else if (is_row_complete(current)) {
            cout << " -> COMPLETE";
        }
        cout << endl;
    }
}
*/

// ---------- scrabble board (module 8, uses DynamicArray) ----------

// empty board, setup() gives it the real size
Scrabble::Scrabble() : row1(0), row2(0) {
    word1 = "";
    word2 = "";
    solved1 = false;
    solved2 = false;
}

// deep copy of both rows
Scrabble::Scrabble(const Scrabble& other) : row1(other.row1), row2(other.row2) {
    word1 = other.word1;
    word2 = other.word2;
    solved1 = other.solved1;
    solved2 = other.solved2;
}

Scrabble& Scrabble::operator=(const Scrabble& other) {
    if (this != &other) {
        row1 = other.row1;
        row2 = other.row2;
        word1 = other.word1;
        word2 = other.word2;
        solved1 = other.solved1;
        solved2 = other.solved2;
    }
    return *this;
}

Scrabble::~Scrabble() {
}

// creates fresh empty rows and remembers the two target words from clue.txt
void Scrabble::setup(const string& first_word, const string& second_word) {
    word1 = to_upper(first_word);
    word2 = to_upper(second_word);
    row1 = DynamicArray<Tile>((int)word1.size());
    row2 = DynamicArray<Tile>((int)word2.size());
    solved1 = false;
    solved2 = false;
}

// puts a clue letter at its position, positions start at 1
void Scrabble::place_tile(const Clue& clue) {
    DynamicArray<Tile>& row = (clue.clueRow == Word1) ? row1 : row2;
    if (clue.cluePosition < 1 || clue.cluePosition > row.size()) {
        throw InvalidPositionException("That board position does not exist");
    }
    Tile& slot = row[clue.cluePosition - 1];
    if (slot.filled) {
        throw SlotFilledException("That board slot already has a letter");
    }
    slot.tileLetter = clue.clueLetter;
    slot.filled = true;
}

// a row is complete when every slot has a letter
bool Scrabble::is_row_complete(WordRow row) const {
    const DynamicArray<Tile>& tiles = (row == Word1) ? row1 : row2;
    if (tiles.size() == 0) {
        return false;
    }
    for (int i = 0; i < tiles.size(); i++) {
        if (!tiles[i].filled) {
            return false;
        }
    }
    return true;
}

bool Scrabble::is_row_solved(WordRow row) const {
    return (row == Word1) ? solved1 : solved2;
}

bool Scrabble::both_solved() const {
    return solved1 && solved2;
}

// compares the typed word with the real word, a wrong word throws an exception
void Scrabble::check_word(WordRow row, const string& typed) {
    if (!is_row_complete(row)) {
        throw ActionNotAllowedException("That row is not complete yet");
    }
    if (is_row_solved(row)) {
        throw InvalidInputException("That word is already solved");
    }
    string target = (row == Word1) ? word1 : word2;
    if (typed.size() != target.size()) {
        throw InvalidWordException("That is not the right word");
    }
    // module 9: every letter is compared with the == operator of tile
    for (int i = 0; i < (int)target.size(); i++) {
        Tile typed_tile = { typed[i], true };
        Tile target_tile = { target[i], true };
        if (!(typed_tile == target_tile)) {
            throw InvalidWordException("That is not the right word");
        }
    }
    if (row == Word1) {
        solved1 = true;
    } else {
        solved2 = true;
    }
}

string Scrabble::get_word(WordRow row) const {
    return (row == Word1) ? word1 : word2;
}

// the display function simply uses the << operator
void Scrabble::display() const {
    cout << *this;
}

// prints both rows of the board
ostream& operator<<(ostream& out, const Scrabble& board) {
    out << "Scrabble:" << endl;
    out << "  Word 1: ";
    for (int i = 0; i < board.row1.size(); i++) {
        out << board.row1[i] << " ";
    }
    if (board.solved1) {
        out << " -> SOLVED";
    } else if (board.is_row_complete(Word1)) {
        out << " -> COMPLETE";
    }
    out << endl;
    out << "  Word 2: ";
    for (int i = 0; i < board.row2.size(); i++) {
        out << board.row2[i] << " ";
    }
    if (board.solved2) {
        out << " -> SOLVED";
    } else if (board.is_row_complete(Word2)) {
        out << " -> COMPLETE";
    }
    out << endl;
    return out;
}

// reads the two target words into the board
istream& operator>>(istream& in, Scrabble& board) {
    string first;
    string second;
    in >> first >> second;
    board.setup(first, second);
    return in;
}

/* old module 5 version of TileRack (raw pointer), kept for reference
// ---------- tile rack ----------

// empty rack with no space
TileRack::TileRack() {
    tiles = 0;
    capacity = 0;
    count = 0;
}

// rack with room for the given number of tiles, every slot starts empty
TileRack::TileRack(int size) {
    count = 0;
    if (size > 0) {
        capacity = size;
        tiles = new Tile[capacity];
        for (int i = 0; i < capacity; i++) {
            tiles[i].tileLetter = '\0';
            tiles[i].filled = false;
        }
    } else {
        capacity = 0;
        tiles = 0;
    }
}

// copies the size, the count and every tile into new memory
void TileRack::copy_from(const TileRack& other) {
    capacity = other.capacity;
    count = other.count;
    if (capacity > 0) {
        tiles = new Tile[capacity];
        for (int i = 0; i < capacity; i++) {
            tiles[i] = other.tiles[i];
        }
    } else {
        tiles = 0;
    }
}

// deep copy: the new rack gets its own memory
TileRack::TileRack(const TileRack& other) {
    copy_from(other);
}

TileRack& TileRack::operator=(const TileRack& other) {
    if (this != &other) {
        delete[] tiles;
        copy_from(other);
    }
    return *this;
}

TileRack::~TileRack() {
    delete[] tiles;
}

// stores a letter in the next free slot
void TileRack::add_tile(char letter) {
    if (is_full()) {
        throw RackFullException("The rack is full");
    }
    tiles[count].tileLetter = letter;
    tiles[count].filled = true;
    count++;
}

// takes a letter out and shifts the rest to the left
char TileRack::remove_tile(int index) {
    if (is_empty()) {
        throw RackEmptyException("The rack is empty");
    }
    if (index < 0 || index >= count) {
        throw InvalidPositionException("There is no tile at that place in the rack");
    }
    char letter = tiles[index].tileLetter;
    for (int i = index; i < count - 1; i++) {
        tiles[i] = tiles[i + 1];
    }
    tiles[count - 1].tileLetter = '\0';
    tiles[count - 1].filled = false;
    count--;
    return letter;
}

int TileRack::get_count() const {
    return count;
}

int TileRack::get_capacity() const {
    return capacity;
}

bool TileRack::is_full() const {
    return count == capacity;
}

bool TileRack::is_empty() const {
    return count == 0;
}

void TileRack::clear() {
    for (int i = 0; i < capacity; i++) {
        tiles[i].tileLetter = '\0';
        tiles[i].filled = false;
    }
    count = 0;
}

// shows the letters in the order they were collected
void TileRack::display() const {
    if (count == 0) {
        cout << "(empty)";
        return;
    }
    for (int i = 0; i < count; i++) {
        if (tiles[i].tileLetter == ' ') {
            cout << "  ";
        } else {
            cout << tiles[i].tileLetter << " ";
        }
    }
}

// joins this rack, a space tile and the other rack into a new rack
TileRack TileRack::operator+(const TileRack& other) const {
    TileRack merged(capacity + other.capacity + 1);
    for (int i = 0; i < count; i++) {
        merged.add_tile(tiles[i].tileLetter);
    }
    merged.add_tile(' ');
    for (int i = 0; i < other.count; i++) {
        merged.add_tile(other.tiles[i].tileLetter);
    }
    return merged;
}
*/

// ---------- tile rack (module 8, uses DynamicArray) ----------

// empty rack with no space
TileRack::TileRack() : tiles(0) {
    count = 0;
}

// rack with room for the given number of tiles
TileRack::TileRack(int capacity) : tiles(capacity) {
    count = 0;
}

// deep copy, the dynamic array copies its own memory
TileRack::TileRack(const TileRack& other) : tiles(other.tiles) {
    count = other.count;
}

TileRack& TileRack::operator=(const TileRack& other) {
    if (this != &other) {
        tiles = other.tiles;
        count = other.count;
    }
    return *this;
}

TileRack::~TileRack() {
}

// stores a letter in the next free slot
void TileRack::add_tile(char letter) {
    if (is_full()) {
        throw RackFullException("The rack is full");
    }
    tiles[count].tileLetter = letter;
    tiles[count].filled = true;
    count++;
}

// takes a letter out and shifts the rest to the left
char TileRack::remove_tile(int index) {
    if (is_empty()) {
        throw RackEmptyException("The rack is empty");
    }
    if (index < 0 || index >= count) {
        throw InvalidPositionException("There is no tile at that place in the rack");
    }
    char letter = tiles[index].tileLetter;
    for (int i = index; i < count - 1; i++) {
        tiles[i] = tiles[i + 1];
    }
    tiles[count - 1].tileLetter = '\0';
    tiles[count - 1].filled = false;
    count--;
    return letter;
}

int TileRack::get_count() const {
    return count;
}

int TileRack::get_capacity() const {
    return tiles.size();
}

bool TileRack::is_full() const {
    return count == tiles.size();
}

bool TileRack::is_empty() const {
    return count == 0;
}

void TileRack::clear() {
    for (int i = 0; i < tiles.size(); i++) {
        tiles[i].tileLetter = '\0';
        tiles[i].filled = false;
    }
    count = 0;
}

// shows the letters in the order they were collected
void TileRack::display() const {
    if (count == 0) {
        cout << "(empty)";
        return;
    }
    for (int i = 0; i < count; i++) {
        if (tiles[i].tileLetter == ' ') {
            cout << "  ";
        } else {
            cout << tiles[i].tileLetter << " ";
        }
    }
}

// joins this rack, a space tile and the other rack into a new rack
TileRack TileRack::operator+(const TileRack& other) const {
    TileRack merged(tiles.size() + other.tiles.size() + 1);
    for (int i = 0; i < count; i++) {
        merged.add_tile(tiles[i].tileLetter);
    }
    merged.add_tile(' ');
    for (int i = 0; i < other.count; i++) {
        merged.add_tile(other.tiles[i].tileLetter);
    }
    return merged;
}

// adds spaces on the right so table columns line up
static string pad_text(const string& text, int width) {
    string result = text;
    while ((int)result.size() < width) {
        result = result + " ";
    }
    return result;
}

// ---------- score board ----------

ScoreBoard::ScoreBoard() {
    file_name = "leader/scoreBoard.txt";
}

// adds one line to the end of the score board file
void ScoreBoard::add_score(const string& detective_name, bool won, int credibility,
                           int turns_left, const string& culprit_role) {
    // ios::app adds at the end and keeps the old lines
    ofstream file(file_name.c_str(), ios::app);
    if (!file) {
        throw FileException("The score board file could not be opened");
    }

    // date in dd-mm-yy format
    time_t now = time(0);
    tm* local = localtime(&now);
    int day = local->tm_mday;
    int month = local->tm_mon + 1;
    int year = (local->tm_year + 1900) % 100;

    // spaces would break the | format, so Partner 1 is saved as Partner_1
    string role = culprit_role;
    for (int i = 0; i < (int)role.size(); i++) {
        if (role[i] == ' ') {
            role[i] = '_';
        }
    }

    file << detective_name << "|" << (won ? "WIN" : "LOSE") << "|" << credibility << "|"
         << (turns_left < 10 ? "0" : "") << turns_left << "|" << role << "|"
         << (day < 10 ? "0" : "") << day << "-" << (month < 10 ? "0" : "") << month
         << "-" << (year < 10 ? "0" : "") << year << endl;
}

// reads the score board file and prints it as a table
void ScoreBoard::display() const {
    ifstream file(file_name.c_str());
    if (!file) {
        throw FileException("The score board file could not be found");
    }

    string line;
    if (!getline(file, line)) {
        throw FileException("The score board file is empty or corrupt");
    }

    cout << endl;
    cout << "-------- Leader/score board --------" << endl;
    cout << pad_text("Detective", 16) << pad_text("Result", 8) << pad_text("Credibility", 13)
         << pad_text("Turns left", 12) << pad_text("Culprit", 12) << "Date" << endl;

    int rows = 0;
    while (getline(file, line)) {
        clean_line(line);
        if (line.empty()) {
            continue;
        }
        // a valid line has exactly five separators
        int bars = 0;
        for (int i = 0; i < (int)line.size(); i++) {
            if (line[i] == '|') {
                bars++;
            }
        }
        if (bars != 5) {
            throw FileException("The score board file is corrupt");
        }
        cout << pad_text(get_field(line, 0), 16) << pad_text(get_field(line, 1), 8)
             << pad_text(get_field(line, 2), 13) << pad_text(get_field(line, 3), 12)
             << pad_text(get_field(line, 4), 12) << get_field(line, 5) << endl;
        rows++;
    }
    if (rows == 0) {
        cout << "No games have been played yet" << endl;
    }
}

// ---------- game engine ----------

// sets default values and seeds the random number generator
// static local variable: it is created once, the first time this function runs,
// and it lives until the program ends
GameEngine& GameEngine::get_instance() {
    static GameEngine instance;
    return instance;
}

GameEngine::GameEngine() {
    state = Menu;
    max_turns = default_max_turns;
    starting_credibility = default_credibility;
    wrong_turn_penalty = default_wrong_turn;
    wrong_word_penalty = default_wrong_word;
    characters = 0;
    culprit = 0;
    culprit_role = "";
    score_board = 0;
    for (int i = 0; i < total_characters; i++) {
        interviewed[i] = false;
    }
    srand((unsigned int)time(0));
}

// frees every character created by the engine
GameEngine::~GameEngine() {
    free_characters();
}

// deletes the characters through base pointers, the virtual destructor does the rest
void GameEngine::free_characters() {
    if (characters != 0) {
        for (int i = 0; i < total_characters; i++) {
            delete characters[i];
        }
        delete[] characters;
        characters = 0;
    }
    culprit = 0;
}

// checks if the text contains a space
bool GameEngine::has_space(const string& text) {
    for (int i = 0; i < (int)text.size(); i++) {
        if (text[i] == ' ') {
            return true;
        }
    }
    return false;
}

// builds the right character object for a role, returns 0 for an unknown role
Character* GameEngine::create_character(const string& role, const string& name,
                                        const string& nation, char letter, int position) {
    if (role == "Victim") {
        return new Victim(name, nation, letter, position);
    } else if (role == "Cook") {
        return new Cook(name, nation, letter, position);
    } else if (role == "Guard") {
        return new Guard(name, nation, letter, position);
    } else if (role == "Servant") {
        return new Servant(name, nation, letter, position);
    } else if (role == "Partner 1" || role == "Partner 2") {
        return new Partner(name, role, nation, letter, position);
    } else if (role == "PA") {
        return new PA(name, nation, letter, position);
    } else if (role == "Child") {
        return new Child(name, nation, letter, position);
    }
    return 0;
}

// reads a full line, an ended input stream stops the game
string GameEngine::read_line() {
    string input;
    if (!getline(cin, input)) {
        state = Exit;
        throw InvalidInputException("The input has ended");
    }
    clean_line(input);
    return input;
}

// reads a number between low and high, anything else is an invalid input
int GameEngine::read_choice(int low, int high) {
    string input = read_line();
    bool valid = !input.empty() && input.size() <= 5;
    for (int i = 0; valid && i < (int)input.size(); i++) {
        if (input[i] < '0' || input[i] > '9') {
            valid = false;
        }
    }
    int number = valid ? atoi(input.c_str()) : -1;
    if (number < low || number > high) {
        throw InvalidInputException("Please enter a number from " + string(1, (char)('0' + low))
                                    + " to " + string(1, (char)('0' + high)));
    }
    return number;
}

// reads y or n
bool GameEngine::read_yes_no() {
    string input = read_line();
    if (input == "Y" || input == "y") {
        return true;
    }
    if (input == "N" || input == "n") {
        return false;
    }
    throw InvalidInputException("Please answer with Y or N");
}

void GameEngine::show_title() const {
    cout << "========================================================" << endl;
    cout << "                   THE LOCKER HEIST" << endl;
    cout << "         A detective game backed by OOP principles" << endl;
    cout << "========================================================" << endl;
}

void GameEngine::show_main_menu() const {
    cout << endl;
    cout << "-------- Main Menu --------" << endl;
    cout << "[1] New game" << endl;
    cout << "[2] View leader/score board" << endl;
    cout << "[3] About" << endl;
    cout << "[4] Rules" << endl;
    cout << "[5] Exit" << endl;
    cout << "Choice: ";
}

// prints about.txt line by line
void GameEngine::show_about() const {
    ifstream file("about.txt");
    if (!file) {
        throw FileException("About.txt could not be found");
    }
    string line;
    cout << endl;
    while (getline(file, line)) {
        clean_line(line);
        cout << line << endl;
    }
}

// reads the rules and shows them to the player
void GameEngine::show_rules() {
    load_rules();
    cout << endl;
    cout << "-------- Rules --------" << endl;
    cout << "Maximum turns: " << max_turns << endl;
    cout << "Starting credibility: " << starting_credibility << endl;
    cout << "Every wrong turn costs " << wrong_turn_penalty << " credibility" << endl;
    cout << "Every wrong word costs " << wrong_word_penalty << " credibility" << endl;
    cout << "You win by accusing the correct culprit" << endl;
    cout << "You lose when credibility reaches zero or the turns run out" << endl;
    cout << "Viewing the scrabble board is free and does not use a turn" << endl;
}

// reads rules.txt, falls back to the default values if it is missing or corrupt
void GameEngine::load_rules() {
    max_turns = default_max_turns;
    starting_credibility = default_credibility;
    wrong_turn_penalty = default_wrong_turn;
    wrong_word_penalty = default_wrong_word;

    ifstream file("rules.txt");
    if (!file) {
        cout << "Rules.txt not found, using the default rules" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        clean_line(line);
        int pos = (int)line.find('=');
        if (pos < 0) {
            continue;
        }
        string key = line.substr(0, pos);
        int value = atoi(line.substr(pos + 1).c_str());
        // penalties are stored as positive numbers
        if (value < 0) {
            value = -value;
        }
        if (key == "Max_Turns") {
            max_turns = value;
        } else if (key == "Starting_Credibility") {
            starting_credibility = value;
        } else if (key == "Every_Wrong_Turn") {
            wrong_turn_penalty = value;
        } else if (key == "Every_Wrong_Word") {
            wrong_word_penalty = value;
        }
    }

    // a zero value means the file is corrupt
    if (max_turns == 0 || starting_credibility == 0) {
        cout << "Rules.txt is corrupt, using the default rules" << endl;
        max_turns = default_max_turns;
        starting_credibility = default_credibility;
        wrong_turn_penalty = default_wrong_turn;
        wrong_word_penalty = default_wrong_word;
    }
}

// reads characters.txt, picks the culprit and creates the eight character objects
// every character gets the letter from the column of the culprit role
void GameEngine::load_characters() {
    free_characters();

    ifstream file("characters.txt");
    if (!file) {
        throw FileException("Characters.txt could not be found");
    }

    string lines[total_characters];
    string line;
    getline(file, line);   // header line

    int count = 0;
    while (count < total_characters && getline(file, line)) {
        clean_line(line);
        if (line.empty()) {
            continue;
        }
        lines[count] = line;
        count++;
    }
    if (count < total_characters) {
        throw FileException("Characters.txt is corrupt, it needs 8 people");
    }

    // line 0 is the victim, so the culprit is one of the seven suspects after it
    int pick = 1 + rand() % total_suspects;
    int index = role_index(get_field(lines[pick], 1));
    if (index < 0) {
        throw FileException("Characters.txt is corrupt, a role is not recognised");
    }
    int column = 4 + index;

    characters = new Character*[total_characters];
    for (int i = 0; i < total_characters; i++) {
        characters[i] = 0;
    }

    for (int i = 0; i < total_characters; i++) {
        string name = get_field(lines[i], 0);
        string role = get_field(lines[i], 1);
        string nation = get_field(lines[i], 2);
        int position = atoi(get_field(lines[i], 3).c_str());
        string letter = get_field(lines[i], column);

        if (letter.empty() || position < 1 || position > total_characters) {
            free_characters();
            throw FileException("Characters.txt is corrupt, a letter or position is wrong");
        }

        characters[i] = create_character(role, name, nation, letter[0], position);
        if (characters[i] == 0) {
            free_characters();
            throw FileException("Characters.txt is corrupt, a role is not recognised");
        }
    }

    characters[pick]->isCulprit = true;   // allowed because GameEngine is a friend of Character
    culprit = characters[pick];
    culprit_role = culprit->get_role();
}

// reads the two words of the culprit role from clue.txt
void GameEngine::load_clue_words(string& first_word, string& second_word) const {
    ifstream file("clue.txt");
    if (!file) {
        throw FileException("Clue.txt could not be found");
    }

    string line;
    getline(file, line);   // header line
    while (getline(file, line)) {
        clean_line(line);
        if (get_field(line, 0) == culprit_role) {
            first_word = get_field(line, 1);
            second_word = get_field(line, 2);
            if (first_word.size() != 7 || second_word.size() != 8) {
                throw FileException("Clue.txt is corrupt, word 1 needs 7 letters and word 2 needs 8");
            }
            return;
        }
    }
    throw FileException("Clue.txt is corrupt, the culprit role was not found");
}

void GameEngine::set_score_board(ScoreBoard* board) {
    score_board = board;
}

// ---------- module 13: screens, menus and game actions ----------

// shows the numbers the detective must watch
void GameEngine::show_status() const {
    cout << "Credibility: " << detective.get_credibility()
         << " | Turns used: " << detective.get_turns() << " of " << max_turns << endl;
}

// shows both racks merged into one clue, using the + operator of the rack
void GameEngine::show_rack() const {
    cout << "Rack: ";
    if (rack_word1.is_empty() && rack_word2.is_empty()) {
        cout << "(empty)" << endl;
        return;
    }
    TileRack merged = rack_word1 + rack_word2;
    merged.display();
    cout << endl;
}

// lists all eight people with their interview status, uses the << operator of character
void GameEngine::show_character_menu() const {
    cout << endl;
    cout << "====== Character menu ======" << endl;
    for (int i = 0; i < total_characters; i++) {
        cout << "[" << (i + 1) << "] " << *characters[i] << " - "
             << (interviewed[i] ? "Interviewed" : "NOT interviewed") << endl;
    }
    cout << "[0] Back" << endl;
    cout << "============================" << endl;
}

// one pass of the game menu, every action is one turn except viewing the board
void GameEngine::game_menu_step() {
    cout << endl;
    cout << "--- TURN " << (detective.get_turns() + 1) << " ---" << endl;
    show_status();
    show_rack();
    cout << "-------- Game Menu --------" << endl;
    cout << "[1] Visit a Location" << endl;
    cout << "[2] Interview a Person" << endl;
    cout << "[3] View Scrabble Board (free - no turn)" << endl;
    cout << "[4] Form a Word" << endl;
    cout << "[5] Accuse a Suspect" << endl;
    cout << "[6] Quit" << endl;
    cout << "Choice: ";

    int choice = read_choice(1, 6);
    switch (choice) {
    case 1:
        visit_location();
        break;
    case 2:
        interview_person();
        break;
    case 3:
        view_board();
        break;
    case 4:
        form_word();
        break;
    case 5:
        accuse_suspect();
        break;
    case 6:
        quit_game();
        break;
    }
    check_end_conditions();
}

// visiting a location gives a letter for word 1
void GameEngine::visit_location() {
    cout << endl;
    cout << "====== Location menu ======" << endl;
    cottage.display();
    cout << "[0] Back" << endl;
    cout << "===========================" << endl;
    cout << "Pick: ";

    int pick = read_choice(0, cottage.get_location_count());
    if (pick == 0) {
        return;
    }
    int index = pick - 1;
    string place_name = underscore_to_space(cottage.get_location(index).locName);

    if (cottage.is_searched(index)) {
        cout << "You have already searched the " << place_name << ". Search again? (Y/N): ";
        if (!read_yes_no()) {
            return;
        }
        // repeating a search shows the same letter again, it only wastes a turn
        Clue again = cottage.peek(index);
        cout << "Letter " << again.clueLetter << " is at position " << again.cluePosition
             << " of Word 1 (already placed)" << endl;
        detective.use_turn();
        detective.lose_credibility(wrong_turn_penalty);
        cout << "That was a wasted turn, you lose " << wrong_turn_penalty << " credibility" << endl;
        return;
    }

    cout << "You have NOT searched the " << place_name << " yet. Search? (Y/N): ";
    if (!read_yes_no()) {
        return;
    }
    // the tile is placed first, so a failure leaves the place unsearched
    Clue clue = cottage.peek(index);
    scrabble.place_tile(clue);
    rack_word1.add_tile(clue.clueLetter);
    cottage.search(index);
    detective.use_turn();
    cout << "Letter " << clue.clueLetter << " collected at position " << clue.cluePosition
         << " of Word 1." << endl;
    cout << "Letter has been placed." << endl;
}

// interviewing a person gives a letter for word 2
void GameEngine::interview_person() {
    show_character_menu();
    cout << "Pick: ";

    int pick = read_choice(0, total_characters);
    if (pick == 0) {
        return;
    }
    int index = pick - 1;
    Character* person = characters[index];

    if (interviewed[index]) {
        cout << "You have already interviewed " << person->get_name() << ". Interview again? (Y/N): ";
        if (!read_yes_no()) {
            return;
        }
        // repeating an interview shows the same letter again, it only wastes a turn
        cout << "Letter " << person->get_letter() << " is at position " << person->get_position()
             << " of Word 2 (already placed)" << endl;
        detective.use_turn();
        detective.lose_credibility(wrong_turn_penalty);
        cout << "That was a wasted turn, you lose " << wrong_turn_penalty << " credibility" << endl;
        return;
    }

    // polymorphism: each character type knows its own required credibility
    if (detective.get_credibility() < person->required_credibility()) {
        throw CredibilityException(person->get_name() + " will only talk to a detective with at least "
                                   + to_string(person->required_credibility()) + " credibility");
    }

    cout << "You have NOT interviewed " << person->get_name() << " yet. Interview? (Y/N): ";
    if (!read_yes_no()) {
        return;
    }
    Clue clue = { person->get_letter(), person->get_position(), person->get_row() };
    scrabble.place_tile(clue);
    rack_word2.add_tile(clue.clueLetter);
    interviewed[index] = true;
    detective.use_turn();
    // polymorphism: display() is virtual, so the real class of the person decides the text
    cout << "You interviewed: ";
    person->display();
    cout << "Letter " << clue.clueLetter << " collected at position " << clue.cluePosition
         << " of Word 2." << endl;
    cout << "Letter has been placed." << endl;
}

// showing the board is free and uses no turn
void GameEngine::view_board() const {
    cout << endl;
    show_rack();
    cout << scrabble;
}

// ---------- module 14: completion conditions ----------

// the detective types the word of a complete row
void GameEngine::form_word() {
    bool ready1 = scrabble.is_row_complete(Word1) && !scrabble.is_row_solved(Word1);
    bool ready2 = scrabble.is_row_complete(Word2) && !scrabble.is_row_solved(Word2);
    if (!ready1 && !ready2) {
        throw ActionNotAllowedException("No unsolved row is complete yet, collect more letters");
    }

    cout << endl;
    cout << "====== Form a word ======" << endl;
    cout << "[1] Word 1 - " << (ready1 ? "ready" : (scrabble.is_row_solved(Word1) ? "solved" : "not complete")) << endl;
    cout << "[2] Word 2 - " << (ready2 ? "ready" : (scrabble.is_row_solved(Word2) ? "solved" : "not complete")) << endl;
    cout << "[0] Back" << endl;
    cout << "Pick: ";

    int pick = read_choice(0, 2);
    if (pick == 0) {
        return;
    }
    WordRow row = (pick == 1) ? Word1 : Word2;

    cout << "Type the word (0 to go back): ";
    string typed = to_upper(read_line());
    if (typed == "0") {
        return;
    }

    // throws when the row is not complete, already solved, or the word is wrong
    scrabble.check_word(row, typed);
    detective.use_turn();
    cout << "Correct! " << typed << " is solved" << endl;

    if (scrabble.both_solved()) {
        cout << endl;
        cout << "Both words solved: " << scrabble.get_word(Word1) << " + " << scrabble.get_word(Word2)
             << " points to the " << culprit_role << "!" << endl;
        cout << "Use [5] Accuse a suspect to name the culprit" << endl;
    }
}

// accusing the right person wins the game
void GameEngine::accuse_suspect() {
    if (!scrabble.both_solved()) {
        throw ActionNotAllowedException("Solve both words before accusing someone");
    }

    cout << endl;
    cout << "====== Suspects ======" << endl;
    for (int i = 1; i < total_characters; i++) {
        cout << "[" << i << "] " << *characters[i] << endl;
    }
    cout << "[0] Back" << endl;
    cout << "Accuse: ";

    int pick = read_choice(0, total_suspects);
    if (pick == 0) {
        return;
    }

    // suspects start at index 1 because index 0 is the victim
    Character* accused = characters[pick];
    if (accused->isCulprit) {
        detective.use_turn();
        cout << accused->get_name() << " is the culprit!" << endl;
        state = Won;
    } else {
        throw WrongAccusationException(accused->get_name() + " is not the culprit");
    }
}

// asks first, then goes back to the main menu, nothing is saved on the score board
void GameEngine::quit_game() {
    cout << "Quit this game and go back to the main menu? (Y/N): ";
    if (read_yes_no()) {
        cout << "You left the investigation" << endl;
        state = Menu;
    }
}

// the game is lost when credibility or turns run out
void GameEngine::check_end_conditions() {
    if (state != Playing) {
        return;
    }
    if (detective.get_credibility() <= 0 || detective.get_turns() >= max_turns) {
        state = Lost;
    }
}

void GameEngine::clear_screen() const {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}

// shows the result, saves it on the score board and returns to the main menu
void GameEngine::finish_game() {
    bool won = (state == Won);
    int turns_left = max_turns - detective.get_turns();
    if (turns_left < 0) {
        turns_left = 0;
    }

    cout << endl;
    cout << "========================================================" << endl;
    cout << " " << detective.get_name() << (won ? " WIN!" : " LOSE!") << endl;
    cout << " Final credibility: " << detective.get_credibility()
         << " | Turns used: " << detective.get_turns()
         << " | Culprit: " << culprit->get_role() << " " << culprit->get_name() << endl;
    cout << "========================================================" << endl;

    // go back to the main menu first, so an error while saving cannot repeat this screen
    state = Menu;
    cout << "Press enter to continue";
    string wait;
    getline(cin, wait);
    clear_screen();

    if (score_board == 0) {
        throw FileException("The score board is not connected, the result was not saved");
    }
    score_board->add_score(detective.get_name(), won, detective.get_credibility(),
                           turns_left, culprit->get_role());
}

// asks for the detective name and prepares a fresh game
void GameEngine::start_new_game() {
    string name = "";
    while (true) {
        cout << "Enter detective name: ";
        name = read_line();
        if (name.empty()) {
            cout << "The name cannot be empty" << endl;
        } else if (has_space(name)) {
            cout << "Please use an underscore ( _ ) or a dot ( . ) instead of a space" << endl;
        } else {
            break;
        }
    }

    // each loader throws a FileException if its file is missing or corrupt
    load_rules();
    load_characters();

    string first_word;
    string second_word;
    load_clue_words(first_word, second_word);
    cottage.load(culprit_role);
    scrabble.setup(first_word, second_word);

    detective = Detective(name, starting_credibility);
    rack_word1 = TileRack(7);
    rack_word2 = TileRack(8);
    for (int i = 0; i < total_characters; i++) {
        interviewed[i] = false;
    }
    state = Playing;

    cout << "A culprit has been secretly chosen. The game begins!" << endl;
    view_board();
}

// one pass of the main menu
void GameEngine::main_menu_step() {
    show_main_menu();
    int choice = read_choice(1, 5);
    switch (choice) {
    case NewGame:
        start_new_game();
        break;
    case LeaderBoard:
        if (score_board == 0) {
            throw FileException("The score board is not connected");
        }
        score_board->display();
        break;
    case About:
        show_about();
        break;
    case Rules:
        show_rules();
        break;
    case ExitGame:
        cout << "Goodbye, detective" << endl;
        state = Exit;
        break;
    }
}

void GameEngine::start() {
    show_title();
    state = Menu;
}

bool GameEngine::is_running() const {
    return state != Exit;
}

// the engine decides what happens next, main() calls this inside a try block
void GameEngine::run_step() {
    if (state == Menu) {
        main_menu_step();
    } else if (state == Playing) {
        game_menu_step();
    } else if (state == Won || state == Lost) {
        finish_game();
    }
}

// prints the message of an exception and charges the penalty it carries
void GameEngine::report_error(const GameException& error) {
    cout << "Error: " << error.what() << endl;

    if (state != Playing) {
        return;
    }
    if (error.get_penalty_kind() == 1) {
        detective.use_turn();
        detective.lose_credibility(wrong_turn_penalty);
        cout << "That was a wrong turn, you lose " << wrong_turn_penalty << " credibility" << endl;
    } else if (error.get_penalty_kind() == 2) {
        detective.use_turn();
        detective.lose_credibility(wrong_word_penalty);
        cout << "That was a wrong word, you lose " << wrong_word_penalty << " credibility" << endl;
    }
    check_end_conditions();
}
