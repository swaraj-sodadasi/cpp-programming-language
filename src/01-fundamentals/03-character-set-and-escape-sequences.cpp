/*
 * =====================================================================================
 * CONCEPT        : Character Set and Escape Sequences in C++
 * DESCRIPTION    : Comprehensive demonstration of:
 *                  1. C++ Execution Character Set: Alphabets (A-Z, a-z), Digits (0-9),
 *                     Punctuation, Whitespace, and Control characters using <cctype>.
 *                  2. Standard Escape Sequences:
 *                     - Formatting: \n (Newline), \t (Tab), \r (Carriage Return), \b (Backspace)
 *                     - Delimiters: \\ (Backslash), \' (Single Quote), \" (Double Quote)
 *                     - Literals  : \0 (Null Terminator), \xHH (Hexadecimal), \NNN (Octal)
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Single linear scan over N characters in dynamic input.
 * SPACE COMPLEXITY : Best Case: O(1) — Constant auxiliary memory space allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <string_view>
#include <cctype>
#include <iomanip>

using namespace std;

/**
 * @brief Classifies each character in dynamic input according to C++ character set rules.
 * @param input String view referencing the user input buffer.
 */
void classifyCharacterSet(string_view input) noexcept {
    cout << "\n================ C++ CHARACTER SET CLASSIFICATION ================" << endl;
    cout << left << setw(14) << "Character" 
         << setw(12) << "ASCII (Dec)" 
         << setw(12) << "ASCII (Hex)" 
         << "Category" << endl;
    cout << string(60, '-') << endl;

    for (char ch : input) {
        auto uc = static_cast<unsigned char>(ch);
        
        string category;
        if (isalpha(uc))      category = "Alphabet (A-Z, a-z)";
        else if (isdigit(uc)) category = "Decimal Digit (0-9)";
        else if (ispunct(uc)) category = "Punctuation / Symbol";
        else if (isspace(uc)) category = "Whitespace";
        else if (iscntrl(uc)) category = "Control Character";
        else                  category = "Extended / Other";

        // Display non-printable escape characters clearly
        string charRepresentation;
        switch (ch) {
            case '\n': charRepresentation = "\\n (Newline)"; break;
            case '\t': charRepresentation = "\\t (Tab)"; break;
            case '\r': charRepresentation = "\\r (CR)"; break;
            case ' ':  charRepresentation = "' ' (Space)"; break;
            default:   charRepresentation = string(1, ch); break;
        }

        cout << left << setw(14) << charRepresentation
             << setw(12) << static_cast<int>(uc)
             << "0x" << hex << uppercase << setw(10) << static_cast<int>(uc) << dec
             << category << endl;
    }
}

/**
 * @brief Demonstrates standard C++ escape sequences with dynamic payload.
 * @param dynamicPayload User-provided string to embed in escape sequence demonstrations.
 */
void demonstrateEscapeSequences(string_view dynamicPayload) {
    cout << "\n================ C++ ESCAPE SEQUENCES DEMONSTRATION ================" << endl;

    // 1. Newline (\n) and Horizontal Tab (\t)
    cout << "1. Tab (\\t) & Newline (\\n):\n";
    cout << "\tColumn 1\tColumn 2\t" << dynamicPayload << "\n";

    // 2. Escaped Backslash (\\), Single Quote (\'), and Double Quote (\")
    cout << "\n2. Literal Symbols (\\\\, \\', \\\"):\n";
    cout << "\tPath: C:\\Program Files\\CPP_Project\\\n";
    cout << "\tQuote: \'" << dynamicPayload << "\' in \"Modern C++\"\n";

    // 3. Backspace (\b) and Carriage Return (\r)
    cout << "\n3. Terminal Controls (\\b, \\r):\n";
    cout << "\tLoading... \b\b\b\bDone!   \n"; // Overwrites trailing dots using backspaces
    cout << "\tOverwritten\r\t" << dynamicPayload << "\n"; // Moves cursor to start of line

    // 4. Hexadecimal (\xHH) and Octal (\NNN) Escape Sequences
    cout << "\n4. Hexadecimal (\\x) & Octal (\\NNN) Escape Values:\n";
    cout << "\tHexadecimal (\\x43\\x2B\\x2B) -> \x43\x2B\x2B\n"; // Hex representation for 'C++'
    cout << "\tOctal (\\103\\053\\053)       -> \103\053\053\n"; // Octal representation for 'C++'

    // 5. Null Terminator (\0)
    cout << "\n5. Null Character (\\0) String Termination:\n";
    const char nullArray[] = {'C', '+', '+', '\0', 'S', 'e', 'c', 'r', 'e', 't', '\0'};
    cout << "\tRaw Buffer Output: " << nullArray << " (Truncates at first \\0)\n";
}

int main() {
    string dynamicInput;
    string dynamicPayload;

    // Dynamic input stream handling with explicit prompt flushing
    cout << "Enter a string to classify character set categories: " << flush;
    if (!getline(cin, dynamicInput) || dynamicInput.empty()) {
        dynamicInput = "C++23!\t\n";
    }

    cout << "Enter dynamic text for escape sequence demo: " << flush;
    if (!getline(cin, dynamicPayload) || dynamicPayload.empty()) {
        dynamicPayload = "Payload";
    }

    // Execute character classification and escape sequence demonstration
    classifyCharacterSet(dynamicInput);
    demonstrateEscapeSequences(dynamicPayload);

    return 0;
}