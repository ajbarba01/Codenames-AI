#include <cmath>
#include <fstream>
#include <iostream>

#include "clue_master/clue_master.h"
#include "clue_master/util.h"

ClueMaster::ClueMaster()
{
    if (!load_words())
    {
        return;
    }

    // string *board = game.get_board();
    // for (int i = 0; i < 25; i++)
    // {
    //     string word = board[i];
    //     cout << word << ": " << to_string(get_similarity(word, "dog")) <<
    //     endl;
    // }
}

void ClueMaster::check_words()
{
    ifstream file("data/words-eng.txt");
    string line;

    vector<string> words = game.get_all_words();

    for (int i = 0; i < words.size(); i++)
    {
        string &word = words[i];
        if (word_to_idx.count(word) == 0)
        {
            cout << word << endl;
        }
    }
}
float *ClueMaster::get_vector(const string &word)
{
    auto it = word_to_idx.find(word);
    if (it == word_to_idx.end())
        return nullptr;

    int idx = it->second;

    return &glove_vectors[idx * D];
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
    float dot = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;

    for (int i = 0; i < D; i++)
    {
        dot += vec_1[i] * vec_2[i];
        norm1 += vec_1[i] * vec_1[i];
        norm2 += vec_2[i] * vec_2[i];
    }

    float denom = sqrt(norm1) * sqrt(norm2);

    if (denom == 0.0f)
        return 0.0f;

    return dot / denom;
}

float ClueMaster::get_avg_similarity(const string &word, string *words)
{
    return 0.0f;
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
