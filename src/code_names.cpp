#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdio.h>

#include "clue_master/code_names.h"
#include "clue_master/tile.h"
#include "clue_master/util.h"

using namespace std;

CodeNames::CodeNames()
{
    // gen board

    ifstream file("data/words-eng.txt");
    string line;

    while (getline(file, line))
    {
        all_words.push_back(normalize_word(line));
    }

    file.close();

    sample(all_words.begin(), all_words.end(), begin(board), size(board),
           mt19937{random_device{}()});

    Tile tiles[] = {Tile::BLUE, Tile::RED, Tile::BYSTANDER, Tile::ASSASSIN};
    int word_nums[] = {9, 8, 7, 1};

    Tile current_tile;
    int current_num;
    int index = 0;

    for (int i = 0; i < 4; i++)
    {
        current_num = word_nums[i];
        current_tile = tiles[i];

        for (int w = index; w < current_num + index; w++)
        {
            string word = board[w];
            word_categories[current_tile].push_back(word);
            words[word] = current_tile;
        }

        index += current_num;
    }

    shuffle(board, board + size(board), mt19937{random_device{}()});
}

bool CodeNames::word_in_group(const string &word, Tile word_type)
{
    return (words.count(word) > 0);
}

vector<string> CodeNames::get_group(Tile word_type)
{
    return word_categories[word_type];
}

void CodeNames::print_board()
{
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            string word = board[y * 5 + x];
            cout << word + ", ";
        }
        cout << endl;
    }
}
