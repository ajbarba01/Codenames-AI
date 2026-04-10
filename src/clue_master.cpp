#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "clue_master/clue_master.h"
#include "clue_master/tile.h"
#include "clue_master/util.h"

ClueMaster::ClueMaster()
{
    load_words();
    vector<Hint> hints = get_next_best();

    game.print_board();

    cout << "-------------------------------HINTS------------------------------"
            "----"
         << endl;

    for (int i = 0; i < hints.size(); i++)
    {
        Hint hint = hints[i];
        cout << hint.clue << ": [";
        for (int k = 0; k < hint.target_words.size(); k++)
        {
            cout << hint.target_words[k] << ", ";
        }
        cout << "], " << hint.rating << endl;
    }

    cout << "-----------------------------------------------------------------"
            "----"
         << endl;
}

void ClueMaster::check_words()
{
    ifstream file("data/words-eng.txt");
    vector<string> board_words = game.get_all_words();

    for (const string &word : board_words)
    {
        if (word_to_idx.count(word) == 0)
        {
            cout << word << endl;
        }
    }
}

bool ClueMaster::is_valid_clue(const string &clue)
{
    const string *board = game.get_board();
    for (int i = 0; i < 25; i++)
    {
        const string &word = board[i];
        if (clue == word)
            return false;
        if (word.find(clue) != string::npos)
            return false;
        if (clue.find(word) != string::npos)
            return false;
    }
    return true;
}

vector<Hint> ClueMaster::get_next_best()
{
    vector<Hint> valid_hints;

    for (const string &clue : words)
    {
        unordered_map<string, float> sims = get_similarities(clue);

        vector<string> sorted_sims;
        for (const auto &pair : sims)
            sorted_sims.push_back(pair.first);

        sort(sorted_sims.begin(), sorted_sims.end(),
             [&](const string &a, const string &b)
             { return sims[a] > sims[b]; });

        bool is_valid = !sorted_sims.empty() &&
                        game.word_in_group(sorted_sims[0], BLUE) &&
                        is_valid_clue(clue);

        if (!is_valid)
            continue;

        int num_for = 0;
        for (const string &word : sorted_sims)
        {
            if (!game.word_in_group(word, BLUE))
                break;
            if (++num_for == 4)
                break;
        }

        vector<string> words_for(sorted_sims.begin(),
                                 sorted_sims.begin() + num_for);

        float avg_blue = get_avg_similarity(clue, words_for);
        float avg_red = get_avg_similarity_tile(sims, RED);
        float avg_bystander = get_avg_similarity_tile(sims, BYSTANDER);
        float avg_assassin = get_avg_similarity_tile(sims, ASSASSIN);

        float rating =
            num_for * avg_blue - 2 * avg_red - avg_bystander - avg_assassin;
        valid_hints.push_back({clue, words_for, num_for, rating});
    }

    sort(valid_hints.begin(), valid_hints.end(),
         [](const Hint &a, const Hint &b) { return a.rating > b.rating; });

    if (valid_hints.size() > 10)
        valid_hints.resize(10);

    return valid_hints;
}

float ClueMaster::get_avg_similarity_tile(
    const unordered_map<string, float> &sims, Tile tile)
{
    vector<string> word_group = game.get_group(tile);

    if (word_group.empty())
        return 0.0f;

    float sim_sum = 0.0f;
    for (const string &word : word_group)
    {
        auto it = sims.find(word);
        if (it != sims.end())
            sim_sum += it->second;
    }

    return sim_sum / word_group.size();
}

const unordered_map<string, float>
ClueMaster::get_similarities(const string &word)
{
    unordered_map<string, float> similarities;
    const string *board = game.get_board();

    for (int i = 0; i < 25; i++)
    {
        similarities[board[i]] = get_similarity(word, board[i]);
    }

    return similarities;
}

float *ClueMaster::get_vector(const string &word)
{
    auto it = word_to_idx.find(word);
    if (it == word_to_idx.end())
        return nullptr;

    return &glove_vectors[it->second * D];
}

float ClueMaster::get_similarity(const string &word_1, const string &word_2)
{
    return get_similarity(get_vector(word_1), get_vector(word_2));
}

float ClueMaster::get_similarity(const string &word, float *vec)
{
    return get_similarity(get_vector(word), vec);
}

float ClueMaster::get_similarity(float *vec_1, float *vec_2)
{
    if (!vec_1 || !vec_2)
        return 0.0f;

    float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;

    for (int i = 0; i < D; i++)
    {
        dot += vec_1[i] * vec_2[i];
        norm1 += vec_1[i] * vec_1[i];
        norm2 += vec_2[i] * vec_2[i];
    }

    float denom = sqrt(norm1) * sqrt(norm2);
    return denom == 0.0f ? 0.0f : dot / denom;
}

float ClueMaster::get_avg_similarity(const string &clue,
                                     const vector<string> &words)
{
    if (words.empty())
        return 0.0f;

    float sum = 0.0f;
    for (const string &word : words)
        sum += get_similarity(clue, word);

    return sum / words.size();
}

bool ClueMaster::load_words()
{
    int num_words = 50000;

    string words_file = "data/top_" + to_string(num_words) + ".txt";
    string glove_file = "data/top_" + to_string(num_words) + ".bin";

    ifstream txt(words_file);
    if (!txt)
    {
        cerr << "Failed to open txt file\n";
        return false;
    }

    words.reserve(num_words);

    string line;
    int idx = 0;
    while (getline(txt, line))
    {
        string word = normalize_word(line);
        words.push_back(word);
        word_to_idx[word] = idx++;
    }

    txt.close();

    ifstream bin(glove_file, ios::binary);
    if (!bin)
    {
        cerr << "Failed to open glove file\n";
        return false;
    }

    glove_vectors.resize(num_words * D);
    bin.read(reinterpret_cast<char *>(glove_vectors.data()),
             glove_vectors.size() * sizeof(float));
    bin.close();

    return true;
}