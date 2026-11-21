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
bool Cottage::load(const string& culprit_role) {
    int index = role_index(culprit_role);
    if (index < 0) {
        return false;
    }

    ifstream file("locations.txt");
    if (!file) {
        return false;
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
            return false;
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
        return false;
    }

    delete[] locations;
    locations = loaded;
    location_count = total_locations;
    return true;
}

int Cottage::get_location_count() const {
    return location_count;
}

// checks that an index points to a real location
bool Cottage::is_valid_index(int index) const {
    return index >= 0 && index < location_count;
}

// the caller must check is_valid_index() first
bool Cottage::is_searched(int index) const {
    return locations[index].locSearchFlag;
}

const Location& Cottage::get_location(int index) const {
    return locations[index];
}

// marks the place as searched and hands over its clue
Clue Cottage::search(int index) {
    locations[index].locSearchFlag = true;
    Clue clue = { locations[index].letterClueLoc, locations[index].boardPosLoc, locations[index].locRow };
    return clue;
}

// gives the clue of a place again without changing anything
Clue Cottage::peek(int index) const {
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
int Scrabble::place_tile(const Clue& clue) {
    if (board == 0) {
        return board_not_ready;
    }
    int row = (clue.clueRow == Word1) ? 0 : 1;
    if (clue.cluePosition < 1 || clue.cluePosition > row_size[row]) {
        return invalid_position;
    }
    Tile& slot = board[row][clue.cluePosition - 1];
    if (slot.filled) {
        return slot_filled;
    }
    slot.tileLetter = clue.clueLetter;
    slot.filled = true;
    return placed_ok;
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

// compares the typed word with the real word, true means correct
// module 7 turns a wrong word into an exception
bool Scrabble::check_word(WordRow row, const string& typed) {
    if (!is_row_complete(row) || is_row_solved(row)) {
        return false;
    }
    string target = (row == Word1) ? word1 : word2;
    if (to_upper(typed) != target) {
        return false;
    }
    if (row == Word1) {
        solved1 = true;
    } else {
        solved2 = true;
    }
    return true;
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

// stores a letter in the next free slot, false when the rack is full
bool TileRack::add_tile(char letter) {
    if (is_full()) {
        return false;
    }
    tiles[count].tileLetter = letter;
    tiles[count].filled = true;
    count++;
    return true;
}

// takes a letter out and shifts the rest to the left
bool TileRack::remove_tile(int index, char& letter) {
    if (index < 0 || index >= count) {
        return false;
    }
    letter = tiles[index].tileLetter;
    for (int i = index; i < count - 1; i++) {
        tiles[i] = tiles[i + 1];
    }
    tiles[count - 1].tileLetter = '\0';
    tiles[count - 1].filled = false;
    count--;
    return true;
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
bool ScoreBoard::add_score(const string& detective_name, bool won, int credibility,
                           int turns_left, const string& culprit_role) {
    // ios::app adds at the end and keeps the old lines
    ofstream file(file_name.c_str(), ios::app);
    if (!file) {
        return false;
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
    return true;
}

// reads the score board file and prints it as a table
bool ScoreBoard::display() const {
    ifstream file(file_name.c_str());
    if (!file) {
        cout << "The score board file could not be found" << endl;
        return false;
    }

    string line;
    if (!getline(file, line)) {
        cout << "The score board file is empty or corrupt" << endl;
        return false;
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
            cout << "The score board file is corrupt" << endl;
            return false;
        }
        cout << pad_text(get_field(line, 0), 16) << pad_text(get_field(line, 1), 8)
             << pad_text(get_field(line, 2), 13) << pad_text(get_field(line, 3), 12)
             << pad_text(get_field(line, 4), 12) << get_field(line, 5) << endl;
        rows++;
    }
    if (rows == 0) {
        cout << "No games have been played yet" << endl;
    }
    return true;
}

// ---------- game engine ----------

// sets default values and seeds the random number generator
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

// reads one line and converts it to a number, returns -1 for invalid input
// when the input ends the game exits, so a piped test cannot loop forever
int GameEngine::read_number() {
    string input;
    if (!getline(cin, input)) {
        state = Exit;
        return 0;
    }
    clean_line(input);
    if (input.empty() || input.size() > 5) {
        return -1;
    }
    for (int i = 0; i < (int)input.size(); i++) {
        if (input[i] < '0' || input[i] > '9') {
            return -1;
        }
    }
    return atoi(input.c_str());
}

void GameEngine::show_title() const {
    cout << "========================================================" << endl;
    cout << "                   THE LOCKER HEIST" << endl;
    cout << "         A detective game backed by OOP principles" << endl;
    cout << "========================================================" << endl;
}

void GameEngine::show_main_menu() const {
    cout << endl;
    cout << "-------- Main menu --------" << endl;
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
        cout << "About.txt could not be found" << endl;
        return;
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

// temporary list that shows polymorphism, module 13 replaces it with the character menu
void GameEngine::show_characters() const {
    cout << endl;
    cout << "-------- People in the cottage --------" << endl;
    for (int i = 0; i < total_characters; i++) {
        cout << "[" << (i + 1) << "] ";
        characters[i]->display();
    }
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
bool GameEngine::load_characters() {
    free_characters();

    ifstream file("characters.txt");
    if (!file) {
        cout << "Characters.txt could not be found" << endl;
        return false;
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
        cout << "Characters.txt is corrupt, it needs " << total_characters << " people" << endl;
        return false;
    }

    // line 0 is the victim, so the culprit is one of the seven suspects after it
    int pick = 1 + rand() % total_suspects;
    int index = role_index(get_field(lines[pick], 1));
    if (index < 0) {
        cout << "Characters.txt is corrupt, a role is not recognised" << endl;
        return false;
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
            cout << "Characters.txt is corrupt, a letter or position is wrong" << endl;
            free_characters();
            return false;
        }

        characters[i] = create_character(role, name, nation, letter[0], position);
        if (characters[i] == 0) {
            cout << "Characters.txt is corrupt, a role is not recognised" << endl;
            free_characters();
            return false;
        }
    }

    characters[pick]->set_culprit(true);
    culprit = characters[pick];
    culprit_role = culprit->get_role();
    return true;
}

// reads the two words of the culprit role from clue.txt
bool GameEngine::load_clue_words(string& first_word, string& second_word) const {
    ifstream file("clue.txt");
    if (!file) {
        cout << "Clue.txt could not be found" << endl;
        return false;
    }

    string line;
    getline(file, line);   // header line
    while (getline(file, line)) {
        clean_line(line);
        if (get_field(line, 0) == culprit_role) {
            first_word = get_field(line, 1);
            second_word = get_field(line, 2);
            if (first_word.size() != 7 || second_word.size() != 8) {
                cout << "Clue.txt is corrupt, word 1 needs 7 letters and word 2 needs 8" << endl;
                return false;
            }
            return true;
        }
    }
    cout << "Clue.txt is corrupt, the culprit role was not found" << endl;
    return false;
}

void GameEngine::set_score_board(ScoreBoard* board) {
    score_board = board;
}

// temporary test: saves the current state of the game as a finished game
// the real version is finish_game(), added with the game menu in module 13
void GameEngine::test_save_result() {
    if (score_board == 0) {
        cout << "The score board is not connected" << endl;
        return;
    }
    cout << "Save this game as a win? (Y = WIN, N = LOSE): ";
    string answer;
    getline(cin, answer);
    clean_line(answer);
    bool won = (answer == "Y" || answer == "y");
    int turns_left = max_turns - detective.get_turns();
    if (turns_left < 0) {
        turns_left = 0;
    }
    if (score_board->add_score(detective.get_name(), won, detective.get_credibility(),
                               turns_left, culprit_role)) {
        cout << "Result saved on the score board" << endl;
    } else {
        cout << "The result could not be saved" << endl;
    }
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

// temporary test: interview people and collect letters for word 2
// the credibility rule is only shown here, module 13 charges the penalties
void GameEngine::test_interview_people() {
    while (state != Exit) {
        cout << endl;
        cout << "====== Character menu (temporary test) ======" << endl;
        for (int i = 0; i < total_characters; i++) {
            cout << "[" << (i + 1) << "] " << characters[i]->get_name() << " ("
                 << characters[i]->get_role() << ") - "
                 << (interviewed[i] ? "Interviewed" : "NOT interviewed") << endl;
        }
        cout << "[0] Back" << endl;
        cout << "Pick: ";

        int pick = read_number();
        if (pick == 0) {
            return;
        }
        if (pick < 1 || pick > total_characters) {
            cout << "Invalid choice, please pick a number from the list" << endl;
            continue;
        }

        int index = pick - 1;
        Character* person = characters[index];
        if (interviewed[index]) {
            cout << "You have already interviewed " << person->get_name() << endl;
            continue;
        }

        // polymorphism: each character type knows its own required credibility
        if (detective.get_credibility() < person->required_credibility()) {
            cout << person->get_name() << " will only talk to a detective with at least "
                 << person->required_credibility() << " credibility" << endl;
            continue;
        }

        Clue clue = { person->get_letter(), person->get_position(), person->get_row() };
        int result = scrabble.place_tile(clue);
        if (result != Scrabble::placed_ok) {
            cout << "The letter could not be placed" << endl;
            continue;
        }
        rack_word2.add_tile(clue.clueLetter);
        interviewed[index] = true;
        cout << "Letter " << clue.clueLetter << " collected at position " << clue.cluePosition
             << " of Word 2." << endl;
        cout << "Letter has been placed." << endl;
        show_rack();
        scrabble.display();
    }
}

// temporary test: visit locations, collect letters and place them on the board
// the real version comes with the game menu in module 13
void GameEngine::test_visit_locations() {
    while (state != Exit) {
        cout << endl;
        cout << "====== Location menu (temporary test) ======" << endl;
        cottage.display();
        cout << "[0] Back" << endl;
        cout << "Pick: ";

        int pick = read_number();
        if (pick == 0) {
            return;
        }
        if (!cottage.is_valid_index(pick - 1)) {
            cout << "Invalid choice, please pick a number from the list" << endl;
            continue;
        }

        int index = pick - 1;
        if (cottage.is_searched(index)) {
            cout << "You have already searched this place. Search again? (Y/N): ";
            string answer;
            getline(cin, answer);
            if (answer == "Y" || answer == "y") {
                Clue again = cottage.peek(index);
                cout << "Letter " << again.clueLetter << " is at position " << again.cluePosition
                     << " of Word 1" << endl;
            }
            continue;
        }

        Clue clue = cottage.search(index);
        cout << "Letter " << clue.clueLetter << " collected at position " << clue.cluePosition
             << " of Word 1." << endl;
        int result = scrabble.place_tile(clue);
        if (result == Scrabble::placed_ok) {
            rack_word1.add_tile(clue.clueLetter);
            cout << "Letter has been placed." << endl;
        } else if (result == Scrabble::slot_filled) {
            cout << "That board slot already has a letter" << endl;
        } else {
            cout << "The letter could not be placed" << endl;
        }
        show_rack();
        scrabble.display();
    }
}

// asks for the detective name and prepares a fresh game
void GameEngine::start_new_game() {
    string name = "";
    while (true) {
        cout << "Enter detective name: ";
        if (!getline(cin, name)) {
            state = Exit;
            return;
        }
        clean_line(name);
        if (name.empty()) {
            cout << "The name cannot be empty" << endl;
        } else if (has_space(name)) {
            cout << "Please use an underscore ( _ ) or a dot ( . ) instead of a space" << endl;
        } else {
            break;
        }
    }

    load_rules();
    if (!load_characters()) {
        cout << "The game cannot start without the character data" << endl;
        return;
    }

    string first_word;
    string second_word;
    if (!load_clue_words(first_word, second_word)) {
        cout << "The game cannot start without the clue words" << endl;
        return;
    }
    if (!cottage.load(culprit_role)) {
        cout << "Locations.txt is missing or corrupt, the game cannot start" << endl;
        return;
    }
    scrabble.setup(first_word, second_word);

    detective = Detective(name, starting_credibility);
    rack_word1 = TileRack(7);
    rack_word2 = TileRack(8);
    for (int i = 0; i < total_characters; i++) {
        interviewed[i] = false;
    }
    state = Playing;

    cout << "A culprit has been secretly chosen. The game begins!" << endl;
    cout << "Credibility: " << detective.get_credibility()
         << " | Turns used: " << detective.get_turns() << endl;
    show_characters();
    cout << endl;
    scrabble.display();

    // temporary menu of modules 4 and 5, module 13 replaces it with the real game menu
    while (state != Exit) {
        cout << endl;
        cout << "---- Temporary test menu ----" << endl;
        show_rack();
        cout << "[1] Visit a location" << endl;
        cout << "[2] Interview a person" << endl;
        cout << "[3] View scrabble board" << endl;
        cout << "[4] Save a test result on the score board" << endl;
        cout << "[0] Back to main menu" << endl;
        cout << "Choice: ";
        int test_choice = read_number();
        if (test_choice == 0) {
            break;
        } else if (test_choice == 1) {
            test_visit_locations();
        } else if (test_choice == 2) {
            test_interview_people();
        } else if (test_choice == 3) {
            scrabble.display();
        } else if (test_choice == 4) {
            test_save_result();
        } else if (state != Exit) {
            cout << "Invalid choice, please enter a number from 0 to 4" << endl;
        }
    }

    // the game menu is added in module 13, so go back to the main menu for now
    if (state != Exit) {
        state = Menu;
    }
}

// main loop of the program
void GameEngine::run() {
    show_title();
    state = Menu;
    while (state != Exit) {
        show_main_menu();
        int choice = read_number();
        switch (choice) {
        case NewGame:
            start_new_game();
            break;
        case LeaderBoard:
            if (score_board == 0) {
                cout << "The score board is not connected" << endl;
            } else {
                score_board->display();
            }
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
        default:
            if (state != Exit) {
                cout << "Invalid choice, please enter a number from 1 to 5" << endl;
            }
        }
    }
}
