#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tile.h"

class CodeNames
{

  private:
    // 2d vector
    std::string board[25];
    std::unordered_map<std::string, Tile> all_words;
    std::unordered_map<Tile, std::vector<std::string>> word_categories;

  public:
    CodeNames();

    bool word_in_group(std::string word, Tile word_type);
    std::vector<std::string> get_group(Tile word_type);

    void print_board();
};