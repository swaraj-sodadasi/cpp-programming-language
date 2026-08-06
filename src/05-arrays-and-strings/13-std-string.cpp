/*
 * =====================================================================================
 * CONCEPT        : `std::string` in C++ (Dynamic Sequential Character Container)
 * DESCRIPTION    : Comprehensive implementation detailing `std::string` mechanics:
 *                  1. Constructors & SSO        : Small String Optimization (SSO) stack allocations vs
 *                                                 dynamic heap buffer allocations.
 *                  2. Capacity & Storage        : `size()`, `length()`, `capacity()`, `reserve()`, 
 *                                                 `shrink_to_fit()`, and `empty()`.
 *                  3. Bounds Safety & Interop   : Safe indexing `.at()`, `operator[]`, `.front()`, 
 *                                                 `.back()`, `.c_str()`, and `.data()`.
 *                  4. In-Place Mutations        : `+=`, `.append()`, `.push_back()`, `.pop_back()`, 
 *                                                 `.insert()`, `.erase()`, and `.replace()`.
 *                  5. Pattern Searching         : `.find()`, `.rfind()`, `.find_first_of()`, `.substr()`, 
 *                                                 and `std::string::npos`.
 *                  6. Numeric Conversions       : `std::to_string()`, `std::stoi()`, `std::stod()`.
 *
 * TIME COMPLEXITY  : Access: O(1) | Append (Amortized): O(1) | Search/Substring/Copy: O(N)
 * SPACE COMPLEXITY : Dynamic memory footprint: O(N)
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER FUNCTION: PRINT DETAILED STRING METRICS
// =====================================================================================
void printStringMetrics(const string& str, const string& label) {
    cout << "  - [" << left << setw(22) << label << "] Content: \"" << str << "\"\n";
    cout << "    Length / Size   : " << str.size() << " chars\n";
    cout << "    Capacity        : " << str.capacity() << " chars\n";
    cout << "    Is SSO Active?  : " << (str.capacity() < 32 ? "Yes (Stack Buffer)" : "No (Heap Buffer)") << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userInput;

    // Dynamic input collection with stream flushing
    cout << "Enter a base text string for std::string processing (e.g., C++2026 Engine): " << flush;
    getline(cin, userInput);

    if (userInput.empty()) {
        userInput = "C++2026 Engine";
        cout << "Empty input detected. Defaulting to: \"" << userInput << "\"\n";
    }

    // =====================================================================================
    // 1. INITIALIZATION & CONSTRUCTORS
    // =====================================================================================
    cout << "\n================ 1. CONSTRUCTORS & SSO ALLOCATION ================\n";

    string defaultStr;                           // Empty string
    string literalStr = "High Performance";       // Direct string literal
    string fillStr(12, '=');                     // Fill constructor (12 equal signs)
    string copyStr = literalStr;                // Copy constructor
    string subInitStr(literalStr, 5, 11);        // Substring constructor (From index 5, count 11)

    printStringMetrics(defaultStr, "Default String");
    printStringMetrics(literalStr, "Literal String");
    printStringMetrics(fillStr, "Fill Constructor");
    printStringMetrics(subInitStr, "Substring Init");

    // =====================================================================================
    // 2. CAPACITY & MEMORY MANAGEMENT
    // =====================================================================================
    cout << "\n================ 2. CAPACITY & BUFFER MANAGEMENT ================\n";

    string buffer = "Initial Buffer State";
    cout << "  - Initial Capacity     : " << buffer.capacity() << " chars\n";

    // Pre-allocating heap space to avoid multiple reallocations
    buffer.reserve(128);
    cout << "  - After .reserve(128)  : " << buffer.capacity() << " chars\n";

    buffer.append(" with extended string content to increase size.");
    cout << "  - Size after append    : " << buffer.size() << " chars\n";

    // Shrinking capacity down to match string length
    buffer.shrink_to_fit();
    cout << "  - After .shrink_to_fit(): " << buffer.capacity() << " chars\n";

    // =====================================================================================
    // 3. ELEMENT ACCESS & C-STRING INTEROPERABILITY
    // =====================================================================================
    cout << "\n================ 3. ELEMENT ACCESS & C-API INTERACTION ================\n";

    cout << "  - Unchecked Index `[0]`   : '" << userInput[0] << "'\n";
    cout << "  - Bounds-Checked `.at(0)`  : '" << userInput.at(0) << "'\n";
    cout << "  - First Char `.front()`    : '" << userInput.front() << "'\n";
    cout << "  - Last Char `.back()`      : '" << userInput.back() << "'\n";

    // Out-of-bounds safety check
    try {
        cout << "  - Testing out-of-bounds access `.at(500)`...\n";
        char ch = userInput.at(500);
        (void)ch;
    } catch (const std::out_of_range& e) {
        cout << "  - [EXCEPTIONAL SAFETY] Caught std::out_of_range: " << e.what() << "\n";
    }

    // C-String pointer interoperability
    const char* cStrPtr = userInput.c_str();
    cout << "  - Legacy C-String `.c_str()` Address : " << static_cast<const void*>(cStrPtr) << "\n";

    // =====================================================================================
    // 4. IN-PLACE MODIFICATIONS
    // =====================================================================================
    cout << "\n================ 4. IN-PLACE MODIFICATIONS ================\n";

    string pipeline = "Data";
    pipeline += " Processing";                   // Append operator +=
    pipeline.append(" Subsystem");               // .append() method
    pipeline.push_back('!');                     // Push single character
    cout << "  - After Append & Push   : \"" << pipeline << "\"\n";

    pipeline.pop_back();                         // Remove last character
    cout << "  - After .pop_back()     : \"" << pipeline << "\"\n";

    pipeline.insert(4, " Stream");              // Insert at index 4
    cout << "  - After .insert(4, ...) : \"" << pipeline << "\"\n";

    pipeline.erase(4, 7);                        // Erase 7 characters from index 4
    cout << "  - After .erase(4, 7)    : \"" << pipeline << "\"\n";

    pipeline.replace(0, 4, "Core");              // Replace index 0..4 with "Core"
    cout << "  - After .replace(0, 4)  : \"" << pipeline << "\"\n";

    // =====================================================================================
    // 5. PATTERN SEARCHING & SUBSTRINGS
    // =====================================================================================
    cout << "\n================ 5. PATTERN SEARCHING & SUBSTRINGS ================\n";

    string corpus = "The quick brown fox jumps over the lazy dog";
    cout << "  - Corpus Text: \"" << corpus << "\"\n";

    size_t foxIdx = corpus.find("fox");
    if (foxIdx != string::npos) {
        cout << "  - `.find(\"fox\")` matched at index        : " << foxIdx << "\n";
    }

    size_t lastTheIdx = corpus.rfind("the");
    if (lastTheIdx != string::npos) {
        cout << "  - `.rfind(\"the\")` last match at index   : " << lastTheIdx << "\n";
    }

    size_t missingIdx = corpus.find("cat");
    if (missingIdx == string::npos) {
        cout << "  - Search for \"cat\" returned `std::string::npos` (Not Found)\n";
    }

    // Substring extraction
    string targetWord = corpus.substr( foxIdx != string::npos ? foxIdx : 0, 9 );
    cout << "  - Extracted `.substr()` slice          : \"" << targetWord << "\"\n";

    // =====================================================================================
    // 6. NUMERIC CONVERSIONS & PARSING
    // =====================================================================================
    cout << "\n================ 6. NUMERIC CONVERSIONS ================\n";

    int intVal = 4096;
    double doubleVal = 271.828;

    // Numbers to std::string
    string strInt = std::to_string(intVal);
    string strDouble = std::to_string(doubleVal);
    cout << "  - `std::to_string(int)`      : \"" << strInt << "\"\n";
    cout << "  - `std::to_string(double)`   : \"" << strDouble << "\"\n";

    // std::string to Numbers
    string parseNumStr = "1024";
    string parseDecStr = "3.14159";

    int parsedInt = std::stoi(parseNumStr);
    double parsedDouble = std::stod(parseDecStr);

    cout << "  - `std::stoi(\"" << parseNumStr << "\") + 10`  : " << (parsedInt + 10) << "\n";
    cout << "  - `std::stod(\"" << parseDecStr << "\") * 2.0`: " << (parsedDouble * 2.0) << "\n";

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ STD::STRING MECHANICS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Method      | `std::string` Container           | Legacy C-Style String (`char*`)   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Memory Management     | Automatic SSO Stack / Heap Buffer | Manual Buffer Sizing / Fixed      |\n"
         << "| Bounds Safety         | `.at()` throws `std::out_of_range`| None (Buffer Overflow Risk)       |\n"
         << "| Size Lookup           | O(1) time complexity (`.size()`)  | O(N) time complexity (`strlen()`) |\n"
         << "| Concatenation         | Native `+`, `+=`, `.append()`     | `strcat()` / `strncat()`          |\n"
         << "| Comparison            | Value-based `==`, `<`, `>`        | `strcmp()` / `strncmp()`          |\n"
         << "| C-API Interop         | `.c_str()` / `.data()` null-term  | Native char pointer               |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}