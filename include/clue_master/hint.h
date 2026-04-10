#include <string>
#include <vector>

using namespace std;

struct Hint
{
    string clue;
    vector<string> target_words;
    int num_target_words;
    float rating;
};