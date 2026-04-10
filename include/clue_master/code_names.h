#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tile.h"

using namespace std;

class CodeNames
{

  public:
    CodeNames();

    const string *get_board() { return board; }
    const vector<string> get_all_words() { return all_words; }

    bool word_in_group(const string &word, Tile word_type);
    const vector<string> get_group(Tile word_type);

    void print_board();

  private:
    string board[25];
    vector<string> all_words;
    unordered_map<string, Tile> words;
    unordered_map<Tile, vector<string>> word_categories;
};