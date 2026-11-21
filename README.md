# The Locker Heist

A console detective game written in C++ for the **CL1004 Object Oriented Programming** semester project (Fall 2026, FAST-NUCES).

A rich business owner has been found dead in his cottage. You are the detective. Search the cottage, interview the people who were there, collect letters, solve two scrabble words and accuse the right culprit, before your credibility or your turns run out.

## Team

|Name|Student ID|Section|
|-|-|-|
|Sajid Mehmood|25F-0761|BCS-3A|
|Haseen Ullah|25F-0770|BCS-3B|
||||

## How to run (Visual Studio 2022)

1. Clone the repository: `git clone https://github.com/sajidtechorg/THE-LOCKER-HEIST.git`
2. Create an empty C++ console project in Visual Studio and add `heist.h`, `heist.cpp` and `main.cpp`.
3. Keep the text files (`about.txt`, `characters.txt`, `clue.txt`, `locations.txt`, `rules.txt`) and the `leader` folder in the **project folder** (the same folder as the `.cpp` files). The game reads them with relative paths.
4. Build and run (Ctrl + F5).

The code also compiles with g++: `g++ -std=c++17 heist.cpp main.cpp -o heist`. Run it from the folder that holds the text files.

## How to play

|Item|Value|
|-|-|
|Maximum turns|25|
|Starting credibility|100|
|Wrong turn|-5 credibility|
|Wrong word|-10 credibility|
|Locations|7 (give the letters of Word 1, 7 letters)|
|People|8 (give the letters of Word 2, 8 letters)|

* Every action uses one turn, except viewing the scrabble board.
* Credibility needed to interview: owner 80, child 75, partners 50, personal assistant 35, house help (cook, guard, servant) none.
* Solve both words, then accuse the culprit. You win by accusing the correct person. You lose when credibility reaches zero or the turns run out.
* Finished games are saved on the leader board.

## Project structure

```
heist.h            declarations: enums, structs, exceptions, template, all classes
heist.cpp          definitions of every function
main.cpp           entry point only
about.txt          text of the About screen
characters.txt     the 8 people and the letters they give for every possible culprit
clue.txt           the two words (7 and 8 letters) of every possible culprit
locations.txt      the 7 locations and the letters they give for every possible culprit
rules.txt          maximum turns, credibility and penalties
leader/scoreBoard.txt   results of finished games
```

## Object oriented design

|Concept|Where it is used|
|-|-|
|Encapsulation|private data with getters, for example `Detective` keeps credibility private|
|Inheritance|`Character` -> `Detective`, `Victim`, `Suspect` -> `BusinessRelations`, `HouseHelp`, `CloseRelations` -> `Partner`, `PA`, `Cook`, `Guard`, `Servant`, `Child`|
|Multiple inheritance and the diamond problem|`PA` inherits from `BusinessRelations` and `CloseRelations`; both use `virtual public Suspect`|
|Abstract classes and polymorphism|`Character` has pure virtual `required\\\_credibility()` and `display()`|
|Dynamic memory|`Cottage`, `Scrabble` (jagged array), `TileRack`, and the array of character pointers|
|Rule of three and deep copy|copy constructor, `operator=` and destructor in classes that own memory|
|Operator overloading|`<<`, `>>`, `==`, and `+` (merges two tile racks)|
|File handling|all `.txt` files, and the append-only score board|
|Composition, aggregation, association|engine owns the detective and boards; cottage has locations; engine uses a score board it does not own|

## Modules

|Module|Content|Status|
|-|-|-|
|1|Base files, enums and structs|done|
|2|`GameEngine` and `Detective`|done|
|3|Character hierarchy with the diamond|done|
|4|`Cottage` and `Scrabble` with dynamic memory|done|
|5|`TileRack` and the interview clue system|done|
|6|`ScoreBoard` and file handling|done|
|7|Exception classes|in progress|
|8|Template `DynamicArray<T>`|planned|
|9 to 14|Operators, singleton engine, game menu, final testing|planned|

## 

