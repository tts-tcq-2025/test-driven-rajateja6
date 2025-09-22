#ifndef STRING_CALCULATOR_H
#define STRING_CALCULATOR_H

#include <string>

class StringCalculator {
public:
    // Returns the sum according to the defined rules.
    // Throws std::runtime_error for:
    //   - negatives (message: "negatives not allowed: <list>")
    //   - invalid delimiter header(s)
    static int Add(const std::string& input);
};

#endif // STRING_CALCULATOR_H
