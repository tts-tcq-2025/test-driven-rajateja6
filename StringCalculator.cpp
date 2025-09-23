#include "StringCalculator.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <regex>

using namespace std;

static vector<string> split(const string& str_input, const vector<string>& delimiters) {
    string pattern = "(";
    for (size_t i = 0; i < delimiters.size(); i++) {
        if (i > 0) pattern += "|";
        pattern += regex_replace(delimiters[i], regex(R"([\^\$\.\|\?\*\+\(\)\[\{\\])"), R"(\$&)");
    }
    pattern += ")";
    regex re(pattern);

    sregex_token_iterator it(str_input.begin(), str_input.end(), re, -1);
    sregex_token_iterator end;
    vector<string> tokens;
    while (it != end) {
        tokens.push_back(*it++);
    }
    return tokens;
}

int StringCalculator::Add(const string& input) {
    if (input.empty()) return 0;

    vector<string> delimiters = {",", "\n"};
    string body = input;

    // Handle custom delimiter(s)
    if (input.rfind("//", 0) == 0) {
        size_t newlinePos = input.find("\n");
        string header = input.substr(2, newlinePos - 2);
        body = input.substr(newlinePos + 1);

        if (header.size() > 0 && header[0] == '[') {
            regex re(R"(\[(.*?)\])");
            auto begin = sregex_iterator(header.begin(), header.end(), re);
            auto end = sregex_iterator();
            delimiters.clear();
            for (auto it = begin; it != end; ++it) {
                delimiters.push_back((*it)[1]);
            }
        } else {
            delimiters = {header};
        }
    }

    vector<string> tokens = split(body, delimiters);

    vector<int> negatives;
    long long sum = 0;

    for (auto& token : tokens) {
        if (token.empty()) continue;
        int num = stoi(token);
        if (num < 0) {
            negatives.push_back(num);
        } else if (num <= 1000) {
            sum += num;
        }
    }

    if (!negatives.empty()) {
        string msg = "negatives not allowed:";
        for (int n : negatives) {
            msg += " " + to_string(n);
        }
        throw invalid_argument(msg);
    }

    return static_cast<int>(sum);
}
