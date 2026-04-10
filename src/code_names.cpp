#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

#include "clue_master/code_names.h"
#include "clue_master/tile.h"
#include "clue_master/util.h"

using namespace std;

CodeNames::CodeNames()
{
    ifstream file("data/words-eng.txt");
    string line;

    while (getline(file, line))
        all_words.push_back(normalize_word(line));

    file.close();

    mt19937 rng{random_device{}()};

    sample(all_words.begin(), all_words.end(), begin(board), size(board), rng);

    Tile tiles[] = {Tile::BLUE, Tile::RED, Tile::BYSTANDER, Tile::ASSASSIN};
    int word_nums[] = {9, 8, 7, 1};
    int index = 0;

    for (int i = 0; i < 4; i++)
    {
        Tile tile = tiles[i];
        int count = word_nums[i];

        for (int w = index; w < index + count; w++)
        {
            word_categories[tile].push_back(board[w]);
            words[board[w]] = tile;
        }

        index += count;
    }

    shuffle(board, board + size(board), rng);
}

bool CodeNames::word_in_group(const string &word, Tile word_type)
{
    auto it = words.find(word);
    return it != words.end() && it->second == word_type;
}

const vector<string> CodeNames::get_group(Tile word_type)
{
    auto it = word_categories.find(word_type);
    if (it == word_categories.end())
        return {};
    return it->second;
}

void CodeNames::print_board()
{
    const string RESET = "\033[0m";
    const string BLUE = "\033[34m";
    const string RED = "\033[31m";
    const string YELLOW = "\033[33m";
    const string BLACK = "\033[30;47m";

    cout << "----------------------------CodeNames "
            "Board----------------------------"
         << endl;
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            const string &word = board[y * 5 + x];
            Tile tile = words[word];

            string color;
            if (tile == Tile::BLUE)
                color = BLUE;
            else if (tile == Tile::RED)
                color = RED;
            else if (tile == Tile::BYSTANDER)
                color = YELLOW;
            else
                color = BLACK;

            cout << color << left << setw(15) << word << RESET;
        }
        cout << "\n";
    }
    cout << "------------------------------------------------------------------"
            "-----"
         << endl
         << endl;
}