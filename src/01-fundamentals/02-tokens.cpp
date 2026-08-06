/*
 * =====================================================================================
 * CONCEPT        : Tokens in C++ (Keywords, Identifiers, Literals, Operators, Punctuators)
 * DESCRIPTION    : Comprehensive implementation showcasing all 5 fundamental token categories:
 *                  1. Keywords    : Reserved words with pre-defined meaning (e.g., struct, auto, return)
 *                  2. Identifiers : User-defined names for variables/functions (e.g., TokenStats, analyzeToken)
 *                  3. Literals    : Direct constant values (e.g., 0, 0.0, true, "string")
 *                  4. Operators   : Symbols triggering mathematical/logical execution (e.g., ::, <<, >>, ++, ?:)
 *                  5. Punctuators : Delimiters governing C++ code structure (e.g., ;, {}, (), <>)
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Single linear scan over N user-supplied dynamic tokens.
 * SPACE COMPLEXITY : Best Case: O(N) — Storage allocated for storing N tokens in std::vector.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

// 1. KEYWORD DEMONSTRATION: 'using', 'namespace'
using namespace std;

// 1. KEYWORD: 'struct'
// 2. IDENTIFIER: 'TokenStats'
// 5. PUNCTUATOR: '{', '}', ';'
struct TokenStats {
    // 3. LITERALS: Default member initializers with numeric (0.0) and boolean (false) literals
    size_t length{0};
    double numericVal{0.0};
    bool isNumeric{false};
};

// 1. KEYWORD: 'auto', 'const', 'noexcept'
// 2. IDENTIFIER: 'analyzeToken', 'token'
// 4. OPERATOR: Trailing return type arrow '->'
auto analyzeToken(const string& token) noexcept -> TokenStats {
    // 2. IDENTIFIER: 'stats'
    TokenStats stats{};
    
    // 4. OPERATOR: Member access operator '.'
    stats.length = token.length();

    if (token.empty()) {
        return stats;
    }

    // 4. OPERATORS: Logic AND '&&', Inequality '!=', Unary Address-of '&'
    // Search for non-digit characters dynamically using C++ standard algorithms
    bool hasNonDigits = any_of(token.begin(), token.end(), [](char c) {
        return !isdigit(static_cast<unsigned char>(c)) && c != '.';
    });

    if (!hasNonDigits) {
        try {
            size_t processed = 0;
            stats.numericVal = stod(token, &processed);
            stats.isNumeric = (processed == token.length());
        } catch (...) {
            stats.isNumeric = false;
        }
    }

    return stats;
}

// 1. KEYWORD: 'int'
// 2. IDENTIFIER: 'main'
int main() {
    // 2. IDENTIFIER: 'count'
    size_t count = 0;

    // 3. LITERAL: String literal "Enter number of tokens to dynamically parse: "
    // 4. OPERATOR: Stream insertion '<<'
    cout << "Enter number of tokens to dynamically parse: " << flush;

    // 4. OPERATOR: Stream extraction '>>', Logical NOT '!'
    if (!(cin >> count) || count == 0) {
        cout << "Invalid count provided. Program terminated." << endl;
        return 0;
    }

    // 1. KEYWORD: 'vector' template
    // 2. IDENTIFIER: 'tokens'
    // 5. PUNCTUATOR: Angle brackets '<>', Parentheses '()'
    vector<string> tokens(count);

    cout << "Enter " << count << " space-separated tokens: " << flush;

    // 1. KEYWORD: 'for' loop
    // 4. OPERATOR: Less-than '<', Pre-increment '++'
    for (size_t i = 0; i < count; ++i) {
        cin >> tokens[i];
    }

    cout << "\n================ TOKEN ANALYSIS REPORT ================" << endl;

    // Loop index declared externally to guarantee universal compilation across standard versions (C++11 through C++23)
    size_t index = 0;

    // 1. KEYWORD: Standard range-based for loop
    // 4. OPERATOR: Reference operator '&', Compound statement
    for (const auto& currentToken : tokens) {
        // 2. IDENTIFIER: 'result'
        TokenStats result = analyzeToken(currentToken);

        // Display analyzed token metadata
        // 4. OPERATOR: Subscript '[]', Ternary operator '?:', Pre-increment '++'
        cout << "Token [" << ++index << "]: \"" << currentToken << "\"\n";
        cout << "  |- Token Length : " << result.length << " characters\n";
        cout << "  |- Is Numeric   : " << (result.isNumeric ? "Yes" : "No") << "\n";
        if (result.isNumeric) {
            cout << "  |- Numeric Val  : " << result.numericVal << "\n";
        }
        cout << "------------------------------------------------------" << endl;
    }

    // 1. KEYWORD: 'return'
    // 3. LITERAL: Integer literal '0'
    return 0;
}