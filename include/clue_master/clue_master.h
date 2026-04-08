#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "clue_master/code_names.h"

using namespace std;

class ClueMaster
{

  public:
    ClueMaster();

    void check_words();

  private:
    bool load_words();

    float *get_vector(const string &word);

    float get_similarity(const string &word_1, const string &word_2);
    float get_similarity(const string &word, float *vec);
    float get_similarity(float *vec_1, float *vec_2);

    float get_avg_similarity(const string &word, string *words);

    CodeNames game;

    int D = 300;

    vector<string> words;
    vector<float> glove_vectors;
    unordered_map<string, int> word_to_idx;
};