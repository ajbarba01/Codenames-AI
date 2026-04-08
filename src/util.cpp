#include <algorithm>

#include "clue_master/util.h"

std::string normalize_word(std::string word)
{
    // lower case
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);

    // whitespace
    word.erase(word.begin(), std::find_if(word.begin(), word.end(), [](char c)
                                          { return !std::isspace(c); }));

    word.erase(std::find_if(word.rbegin(), word.rend(),
                            [](char c) { return !std::isspace(c); })
                   .base(),
               word.end());

    return word;
}