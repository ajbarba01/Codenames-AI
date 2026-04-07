
#include "CodeNames.h"
#include "Tile.h"
#include "util.h"

#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <random>
#include <iostream>


using namespace std;


CodeNames::CodeNames()
{
    // gen board

    vector<string> words;
    ifstream file("data/words-eng.txt");
    string line;

    while (getline(file, line)) {
        words.push_back(normalize_word(line));
    }

    sample(words.begin(), words.end(), this->board, size(this->board), mt19937{random_device{}()});

    Tile tiles[] = { Tile::BLUE, Tile::RED, Tile::BYSTANDER, Tile::ASSASSIN };
    int word_nums[] = { 9, 8, 8, 1 };

    Tile current_tile;
    int current_num;
    int index = 0;
    for (int i = 0; i < 4; i++) {
        current_num = word_nums[i];
        current_tile = tiles[i];

        for (int w = index; w < current_num + index; w++) {
            string word = this->board[w];
            this->word_categories[current_tile].push_back(word);
            this->all_words[word] = current_tile;
        }

        index += current_num;
    }

    shuffle(this->board, this->board + size(this->board), mt19937{random_device{}()});
}

bool CodeNames::word_in_group(string word, Tile word_type) {
    return (this->all_words.count(word) > 0);
}

vector<string> CodeNames::get_group(Tile word_type)
{
    return this->word_categories[word_type];
}

void CodeNames::print_board()
{
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            string word = this->board[y * 5 + x];
            cout << word + ", ";
        }
        cout << endl;
    }
}
