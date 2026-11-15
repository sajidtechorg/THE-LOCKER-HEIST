#include "heist.h"

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

// removes the hidden carriage return that windows files can leave at the end
void GameEngine::clean_line(string& line) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }
}

// returns the field at the given index from a line separated by |
string GameEngine::get_field(const string& line, int index) {
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

// checks if the text contains a space
bool GameEngine::has_space(const string& text) {
    for (int i = 0; i < (int)text.size(); i++) {
        if (text[i] == ' ') {
            return true;
        }
    }
    return false;
}

// finds which column of characters.txt holds the letters for the culprit role
int GameEngine::column_for_role(const string& role) {
    if (role == "Cook") {
        return 4;
    } else if (role == "Guard") {
        return 5;
    } else if (role == "Servant") {
        return 6;
    } else if (role == "Partner 1") {
        return 7;
    } else if (role == "Partner 2") {
        return 8;
    } else if (role == "PA") {
        return 9;
    } else if (role == "Child") {
        return 10;
    }
    return -1;
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
int GameEngine::read_number() const {
    string input;
    getline(cin, input);
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
    int column = column_for_role(get_field(lines[pick], 1));
    if (column < 0) {
        cout << "Characters.txt is corrupt, a role is not recognised" << endl;
        return false;
    }

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
    return true;
}

// asks for the detective name and prepares a fresh game
void GameEngine::start_new_game() {
    string name = "";
    while (true) {
        cout << "Enter detective name: ";
        getline(cin, name);
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

    detective = Detective(name, starting_credibility);
    state = Playing;

    cout << "A culprit has been secretly chosen. The game begins!" << endl;
    cout << "Credibility: " << detective.get_credibility()
         << " | Turns used: " << detective.get_turns() << endl;
    show_characters();

    // the game menu is added in module 13, so go back to the main menu for now
    state = Menu;
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
            cout << "The leader board will be added in module 6" << endl;
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
            cout << "Invalid choice, please enter a number from 1 to 5" << endl;
        }
    }
}
