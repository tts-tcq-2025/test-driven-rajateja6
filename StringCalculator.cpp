#include "StringCalculator.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <cstddef>

namespace {

// Holds parsing result for header.
struct DelimiterInfo {
    bool hasCustom = false;
    std::vector<std::string> delimiters; // always at least 2 if custom: {custom, "\n"} ; default: {",","\n"}
    std::size_t numbersStart = 0;
};

// CCN = 2
DelimiterInfo parseDelimiterSection(const std::string& input) {
    DelimiterInfo info;
    if (input.rfind("//", 0) != 0) {
        info.hasCustom = false;
        info.delimiters = {",", "\n"};
        info.numbersStart = 0;
        return info;
    }

    std::size_t newlinePos = input.find('\n');
    if (newlinePos == std::string::npos) {
        throw std::runtime_error("invalid delimiter");
    }
    std::string header = input.substr(2, newlinePos - 2);

    std::string custom;
    if (!header.empty() && header.front() == '[') {
        // Bracket form expected: //[...] only ONE delimiter allowed in scope.
        // Reject multiple: detect additional '[' after first char.
        std::size_t closing = header.rfind(']');
        if (closing == std::string::npos || closing == 1) {
            throw std::runtime_error("invalid delimiter");
        }
        // Ensure only one bracketed section: another '[' inside (besides first) suggests multi-delimiter.
        std::size_t another = header.find('[', 1);
        if (another != std::string::npos) {
            throw std::runtime_error("invalid delimiter");
        }
        // Extract inside brackets
        custom = header.substr(1, closing - 1);
        if (custom.empty()) {
            throw std::runtime_error("invalid delimiter");
        }
    } else {
        // Single-char delimiter required
        if (header.size() != 1) {
            throw std::runtime_error("invalid delimiter");
        }
        custom = header;
    }

    info.hasCustom = true;
    info.delimiters = {custom, "\n"}; // newline retained per kata
    info.numbersStart = newlinePos + 1;
    return info;
}

// CCN = 3 (loops + small branch)
std::vector<std::string> tokenize(const std::string& numbers, const std::vector<std::string>& delimiters) {
    std::vector<std::string> tokens;
    std::string current;
    std::size_t i = 0;
    while (i < numbers.size()) {
        bool matched = false;
        for (const auto& d : delimiters) {
            if (!d.empty() && numbers.compare(i, d.size(), d) == 0) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                } else {
                    // Empty token (e.g., consecutive delimiters) -> skip; not specified as error.
                }
                i += d.size();
                matched = true;
                break;
            }
        }
        if (!matched) {
            current.push_back(numbers[i]);
            ++i;
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

// CCN = 2
int toInt(const std::string& s) {
    if (s.empty()) {
        // Skip empty token (can arise from malformed input like "1,\n")
        throw std::runtime_error("invalid number");
    }
    // Simple manual parse (no spaces expected)
    bool negative = false;
    std::size_t idx = 0;
    if (s[0] == '-') {
        negative = true;
        idx = 1;
        if (idx == s.size()) {
            throw std::runtime_error("invalid number");
        }
    }
    int value = 0;
    for (; idx < s.size(); ++idx) {
        char c = s[idx];
        if (c < '0' || c > '9') {
            throw std::runtime_error("invalid number");
        }
        value = value * 10 + (c - '0');
    }
    return negative ? -value : value;
}

// CCN = 2
std::string buildNegativeMessage(const std::vector<int>& negatives) {
    std::string msg = "negatives not allowed: ";
    for (std::size_t i = 0; i < negatives.size(); ++i) {
        if (i > 0) msg += ",";
        msg += std::to_string(negatives[i]);
    }
    return msg;
}

} // namespace

// CCN = 3 (branches: empty, custom/default parse, negatives handling)
int StringCalculator::Add(const std::string& input) {
    if (input.empty()) {
        return 0;
    }

    auto info = parseDelimiterSection(input);
    const std::string numbersPart = input.substr(info.numbersStart);

    std::vector<std::string> tokens = tokenize(numbersPart, info.delimiters);
    int sum = 0;
    std::vector<int> negatives;
    for (const auto& token : tokens) {
        if (token.empty()) {
            continue; // skip any accidental empty token
        }
        int value = toInt(token);
        if (value < 0) {
            negatives.push_back(value);
            continue; // still gather all negatives
        }
        if (value <= 1000) {
            sum += value;
        }
    }
    if (!negatives.empty()) {
        throw std::runtime_error(buildNegativeMessage(negatives));
    }
    return sum;
}
