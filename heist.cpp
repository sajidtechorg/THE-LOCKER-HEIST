#include "heist.h"

// ---------- detective ----------

// default detective with no name and no credibility
Detective::Detective() {
    detective_name = "";
    credibility = 0;
    turns = 0;
}

// detective created with a name and starting credibility
Detective::Detective(string name, int starting_credibility) {
    detective_name = name;
    credibility = starting_credibility;
    turns = 0;
}

string Detective::get_name() const {
    return detective_name;
}

int Detective::get_credibility() const {
    return credibility;
}

int Detective::get_turns() const {
    return turns;
}

void Detective::set_name(string name) {
    detective_name = name;
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

// ---------- game engine ----------

// sets default values and seeds the random number generator
GameEngine::GameEngine() {
    state = Menu;
    max_turns = default_max_turns;
    starting_credibility = default_credibility;
    wrong_turn_penalty = default_wrong_turn;
    wrong_word_penalty = default_wrong_word;
    culprit_name = "";
    culprit_role = "";
    srand((unsigned int)time(0));
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

// reads the seven suspects from characters.txt and picks one as the culprit
bool GameEngine::load_culprit() {
    ifstream file("characters.txt");
    if (!file) {
        cout << "Characters.txt could not be found" << endl;
        return false;
    }

    string names[total_suspects];
    string roles[total_suspects];
    string line;

    getline(file, line);   // header line
    getline(file, line);   // victim line, the victim is never the culprit

    int count = 0;
    while (count < total_suspects && getline(file, line)) {
        clean_line(line);
        if (line.empty()) {
            continue;
        }
        names[count] = get_field(line, 0);
        roles[count] = get_field(line, 1);
        count++;
    }

    if (count < total_suspects) {
        cout << "Characters.txt is corrupt, it needs " << total_suspects << " suspects" << endl;
        return false;
    }

    int pick = rand() % total_suspects;
    culprit_name = names[pick];
    culprit_role = roles[pick];
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
    if (!load_culprit()) {
        cout << "The game cannot start without the character data" << endl;
        return;
    }

    detective = Detective(name, starting_credibility);
    state = Playing;

    cout << "A culprit has been secretly chosen. The game begins!" << endl;
    cout << "Credibility: " << detective.get_credibility()
         << " | Turns used: " << detective.get_turns() << endl;

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
