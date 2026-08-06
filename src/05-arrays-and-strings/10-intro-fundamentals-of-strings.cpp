/*
 * =====================================================================================
 * CONCEPT        : Introduction and Fundamentals of Strings in C++ (`std::string`)
 * DESCRIPTION    : Comprehensive guide and implementation of std::string in C++:
 *                  1. What is std::string?         : A dynamic, heap-managed sequential container 
 *                                                    of characters provided by `<string>`.
 *                  2. SSO (Small String Opt.)     : Stack buffer optimization avoiding heap allocation 
 *                                                    for small strings (typically <= 15-23 chars).
 *                  3. Initialization & Construction: Default, literal, fill, substring, and copy constructors.
 *                  4. Capacity & Memory Management : `size()`, `length()`, `capacity()`, `reserve()`, 
 *                                                    `shrink_to_fit()`, and `empty()`.
 *                  5. Element Access & Interop     : `operator[]`, `.at()`, `.front()`, `.back()`, 
 *                                                    `.c_str()`, and `.data()`.
 *                  6. Modification & Concatenation : `+`, `+=`, `.append()`, `.push_back()`, 
 *                                                    `.insert()`, `.erase()`, and `.replace()`.
 *                  7. Searching & Substring        : `.find()`, `.rfind()`, `.substr()`, and `std::string::npos`.
 *                  8. Conversions & Parsing        : `std::to_string()`, `std::stoi()`, `std::stod()`.
 *
 * TIME COMPLEXITY  : Access: O(1) | Append (Amortized): O(1) | Copy / Concatenate / Search: O(N)
 * SPACE COMPLEXITY : Dynamic heap/stack buffer: O(N)
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <cstddef>

using namespace std;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userInput;

    // Dynamic input collection with stream flushing
    cout << "Enter a text string for std::string demonstration (e.g., Hello C++2026): " << flush;
    getline(cin, userInput);

    if (userInput.empty()) {
        userInput = "Hello C++2026";
        cout << "Empty input detected. Defaulting to sample string: \"" << userInput << "\"\n";
    }

    // =====================================================================================
    // 1. INITIALIZATION AND CONSTRUCTION STYLES
    // =====================================================================================
    cout << "\n================ 1. DECLARATION & INITIALIZATION ================\n";

    string defaultStr;                         // Empty string ("")
    string literalStr = "Modern C++ Engine";   // Direct literal initialization
    string copyStr = literalStr;              // Copy initialization
    string fillStr(10, '*');                   // Fill constructor (10 asterisks)
    string subStrInit(literalStr, 7, 3);       // Substring constructor (From index 7, length 3 -> "C++")

    cout << "  - Default String   : \"" << defaultStr << "\" (empty: " << (defaultStr.empty() ? "Yes" : "No") << ")\n";
    cout << "  - Literal String   : \"" << literalStr << "\"\n";
    cout << "  - Copy String      : \"" << copyStr << "\"\n";
    cout << "  - Fill Constructor : \"" << fillStr << "\"\n";
    cout << "  - Substring Init   : \"" << subStrInit << "\"\n";

    // =====================================================================================
    // 2. CAPACITY AND MEMORY MANAGEMENT (SSO & CAPACITY)
    // =====================================================================================
    cout << "\n================ 2. CAPACITY & MEMORY MANAGEMENT ================\n";

    string capacityDemo = "Short String"; // Triggers Small String Optimization (SSO)

    cout << "  - String Content   : \"" << capacityDemo << "\"\n";
    cout << "  - .length() / .size(): " << capacityDemo.length() << " characters\n";
    cout << "  - Initial .capacity(): " << capacityDemo.capacity() << " characters (SSO buffer active)\n";

    // Triggering capacity growth
    capacityDemo.reserve(100); // Pre-allocates heap storage for at least 100 characters
    cout << "  - After .reserve(100) capacity : " << capacityDemo.capacity() << " characters\n";

    capacityDemo.shrink_to_fit(); // Requests reducing capacity to fit current size
    cout << "  - After .shrink_to_fit() capacity: " << capacityDemo.capacity() << " characters\n";

    // =====================================================================================
    // 3. ELEMENT ACCESS & C-STRING INTEROPERABILITY
    // =====================================================================================
    cout << "\n================ 3. ELEMENT ACCESS & C-STRING INTERACTION ================\n";

    cout << "  - Unchecked Access `[0]`   : '" << userInput[0] << "'\n";
    cout << "  - Bounds-Checked `.at(0)`  : '" << userInput.at(0) << "'\n";
    cout << "  - First Char `.front()`    : '" << userInput.front() << "'\n";
    cout << "  - Last Char `.back()`      : '" << userInput.back() << "'\n";

    // Exception safety with .at()
    try {
        cout << "  - Testing out-of-bounds access `.at(1000)`...\n";
        char invalidChar = userInput.at(1000);
        (void)invalidChar;
    } catch (const std::out_of_range& e) {
        cout << "  - [EXCEPTIONAL SAFETY] Caught std::out_of_range: " << e.what() << "\n";
    }

    // Interoperability with legacy C-APIs using .c_str() and .data()
    const char* rawCStr = userInput.c_str();
    cout << "  - Native C-String `.c_str()` pointer: " << static_cast<const void*>(rawCStr) 
         << " -> Content: \"" << rawCStr << "\"\n";

    // =====================================================================================
    // 4. STRING MODIFICATION & CONCATENATION
    // =====================================================================================
    cout << "\n================ 4. MODIFICATION & CONCATENATION ================\n";

    string buffer = "Data";
    buffer += " Processing";                 // Operator +=
    buffer.append(" Pipeline");              // Method .append()
    buffer.push_back('!');                   // Append single character
    cout << "  - After Append/Push      : \"" << buffer << "\"\n";

    buffer.pop_back();                       // Removes last character
    cout << "  - After .pop_back()      : \"" << buffer << "\"\n";

    buffer.insert(5, "Stream ");            // Insert substring at index 5
    cout << "  - After .insert(5, ...)  : \"" << buffer << "\"\n";

    buffer.erase(5, 7);                      // Erase 7 characters starting at index 5
    cout << "  - After .erase(5, 7)     : \"" << buffer << "\"\n";

    buffer.replace(0, 4, "Core");            // Replace 4 characters at index 0 with "Core"
    cout << "  - After .replace(0, ...) : \"" << buffer << "\"\n";

    // =====================================================================================
    // 5. SEARCHING & SUBSTRING OPERATIONS
    // =====================================================================================
    cout << "\n================ 5. SEARCHING & SUBSTRINGS ================\n";

    string searchText = "The quick brown fox jumps over the lazy dog";
    cout << "  - Target Text: \"" << searchText << "\"\n";

    size_t foundPos = searchText.find("fox");
    if (foundPos != string::npos) {
        cout << "  - Found substring \"fox\" at index: " << foundPos << "\n";
    }

    size_t missingPos = searchText.find("cat");
    if (missingPos == string::npos) {
        cout << "  - Substring \"cat\" NOT found (Returned std::string::npos)\n";
    }

    // Extracting substring
    string extractedSub = searchText.substr(10, 9); // Index 10, length 9
    cout << "  - Extracted `.substr(10, 9)` : \"" << extractedSub << "\"\n";

    // =====================================================================================
    // 6. NUMERIC CONVERSIONS & PARSING
    // =====================================================================================
    cout << "\n================ 6. NUMERIC CONVERSIONS ================\n";

    int intValue = 2026;
    double doubleValue = 99.95;

    // Numbers to std::string
    string strFromInt = std::to_string(intValue);
    string strFromDouble = std::to_string(doubleValue);
    cout << "  - std::to_string(int)    : \"" << strFromInt << "\"\n";
    cout << "  - std::to_string(double) : \"" << strFromDouble << "\"\n";

    // std::string to Numbers
    string intStr = "1048576";
    string doubleStr = "3.14159265";

    int parsedInt = std::stoi(intStr);
    double parsedDouble = std::stod(doubleStr);

    cout << "  - std::stoi(\"" << intStr << "\") + 1    : " << (parsedInt + 1) << "\n";
    cout << "  - std::stod(\"" << doubleStr << "\") * 2 : " << (parsedDouble * 2.0) << "\n";

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ STD::STRING FUNDAMENTALS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Metric / Operation    | `std::string` Class               | Legacy C-Style (`char[]`)         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Memory Management     | Dynamic automatic heap / SSO      | Manual buffer sizing / fixed      |\n"
         << "| Bounds Safety         | `.at()` throws out_of_range       | None (Buffer overflow vulnerabilities)|\n"
         << "| Concatenation         | Simple `+` / `+=` / `.append()`   | Requires `strcat` / `strncat`     |\n"
         << "| Comparison            | Native `==`, `<`, `>` operators   | Requires `strcmp`                 |\n"
         << "| Size Overhead         | 24/32-byte object header + buffer | 1 byte per char + 1 byte for `\\0` |\n"
         << "| STL Integration       | Full Iterator & Algorithm support | Pointer-range only                |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}