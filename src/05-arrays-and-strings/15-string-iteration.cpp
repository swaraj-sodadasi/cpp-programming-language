/*
 * =====================================================================================
 * CONCEPT        : String Iteration Mechanisms in Modern C++ (`std::string`)
 * DESCRIPTION    : Comprehensive guide and practical code implementation of 6 distinct 
 *                  string iteration techniques:
 *                  1. Index-based Iteration (Subscript `[]` & `.at()`)
 *                  2. Range-based `for` Loop (By Value, Reference, Const Ref)
 *                  3. Standard STL Iterators (`begin()` / `end()`, `cbegin()` / `cend()`)
 *                  4. Reverse Iterators (`rbegin()` / `rend()`, `crbegin()` / `crend()`)
 *                  5. Pointer-based Traversal (Raw C-style pointer iteration via `.data()`)
 *                  6. Functional Algorithm Iteration (`std::for_each`)
 *
 * TIME COMPLEXITY  : Traversal across all mechanisms: O(N) linear time.
 * SPACE COMPLEXITY : O(1) auxiliary space footprint.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userInput;

    // Dynamic input collection with stream flushing
    cout << "Enter a string for iteration processing (e.g., C++ Engine 2026): " << flush;
    getline(cin, userInput);

    if (userInput.empty()) {
        userInput = "C++ Engine 2026";
        cout << "Empty input detected. Defaulting to: \"" << userInput << "\"\n";
    }

    // =====================================================================================
    // 1. INDEX-BASED ITERATION (Subscript `[]` & `.at()`)
    // Classic C-style array indexing approach with runtime size bound checks.
    // =====================================================================================
    cout << "\n================ 1. INDEX-BASED ITERATION (`[]` & `.at()`) ================\n";
    cout << "  - Traversing via index [i]: ";
    for (size_t i = 0; i < userInput.size(); ++i) {
        cout << userInput[i] << (i + 1 < userInput.size() ? "-" : "");
    }
    cout << "\n";

    cout << "  - Traversing via bounds-checked `.at(i)`: ";
    for (size_t i = 0; i < userInput.length(); ++i) {
        cout << userInput.at(i) << (i + 1 < userInput.length() ? "." : "");
    }
    cout << "\n";

    // =====================================================================================
    // 2. RANGE-BASED `for` LOOP (BY VALUE, REFERENCE, AND CONST REF)
    // Idiomatic Modern C++ traversal mechanics.
    // =====================================================================================
    cout << "\n================ 2. RANGE-BASED FOR LOOPS ================\n";

    // Read-only inspection using const reference (Avoids copy overhead)
    cout << "  - Range-for (const auto&): ";
    for (const char& ch : userInput) {
        cout << ch << " ";
    }
    cout << "\n";

    // In-place modification via non-const reference
    string mutableString = userInput;
    for (char& ch : mutableString) {
        if (std::islower(static_cast<unsigned char>(ch))) {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
    }
    cout << "  - Range-for (auto& modification to uppercase): \"" << mutableString << "\"\n";

    // =====================================================================================
    // 3. STANDARD STL ITERATORS (`begin()` / `end()`, `cbegin()` / `cend()`)
    // STL iterator interface compatible with standard algorithms and templates.
    // =====================================================================================
    cout << "\n================ 3. STANDARD STL ITERATORS ================\n";

    cout << "  - Forward Const Iterators (`cbegin()` -> `cend()`): ";
    for (auto it = userInput.cbegin(); it != userInput.cend(); ++it) {
        cout << *it << " ";
    }
    cout << "\n";

    // =====================================================================================
    // 4. REVERSE ITERATORS (`rbegin()` / `rend()`, `crbegin()` / `crend()`)
    // Iterates backwards from tail to head without manual index arithmetic.
    // =====================================================================================
    cout << "\n================ 4. REVERSE ITERATORS ================\n";

    cout << "  - Backward Traversal (`rbegin()` -> `rend()`): ";
    for (auto rit = userInput.crbegin(); rit != userInput.crend(); ++rit) {
        cout << *rit << " ";
    }
    cout << "\n";

    // =====================================================================================
    // 5. POINTER-BASED TRAVERSAL (RAW POINTER INTEROP)
    // Direct memory pointer traversal using contiguous underlying buffer `.data()`.
    // =====================================================================================
    cout << "\n================ 5. RAW POINTER TRAVERSAL ================\n";

    const char* ptrBegin = userInput.data();
    const char* ptrEnd   = userInput.data() + userInput.size();

    cout << "  - Pointer Increment (`const char*`): ";
    for (const char* ptr = ptrBegin; ptr != ptrEnd; ++ptr) {
        cout << *ptr << " ";
    }
    cout << "\n";

    // =====================================================================================
    // 6. FUNCTIONAL ALGORITHM ITERATION (`std::for_each`)
    // Functional approach applying a lambda expression or function object to each character.
    // =====================================================================================
    cout << "\n================ 6. FUNCTIONAL `std::for_each` ITERATION ================\n";

    int digitCount = 0;
    int alphaCount = 0;

    cout << "  - Applying std::for_each with Lambda: ";
    std::for_each(userInput.begin(), userInput.end(), [&digitCount, &alphaCount](char ch) {
        unsigned char uCh = static_cast<unsigned char>(ch);
        if (std::isdigit(uCh)) ++digitCount;
        if (std::isalpha(uCh)) ++alphaCount;
    });

    cout << "\n    Metrics Collected: Letters = " << alphaCount << " | Digits = " << digitCount << "\n";

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ STRING ITERATION MECHANISMS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Iteration Method      | Syntax Example                    | Key Characteristics & Best Use    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Subscript Index `[]`  | `str[i]`                          | Direct indexing; zero-overhead    |\n"
         << "| Bounds Checked `.at()`| `str.at(i)`                       | Safe access; throws out_of_range  |\n"
         << "| Range-for Loop        | `for (const char& c : str)`       | Most idiomatic, modern, & clean   |\n"
         << "| Standard Iterators    | `str.cbegin()` -> `str.cend()`    | Generic STL algorithm integration |\n"
         << "| Reverse Iterators     | `str.crbegin()` -> `str.crend()`  | Backward traversal without math   |\n"
         << "| Raw Pointer           | `for (const char* p = data)`      | High-performance C-API interop    |\n"
         << "| `std::for_each`       | `std::for_each(b, e, lambda)`     | Functional execution & capture    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}