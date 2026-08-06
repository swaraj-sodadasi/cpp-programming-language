/*
 * =====================================================================================
 * CONCEPT        : Common Mistakes in Arrays and Strings in C++ & How to Avoid Them
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the 7 most frequent bugs, security hazards, and memory traps:
 *
 *                  1. Off-by-One Errors & Out-of-Bounds Access (`arr[N]` vs `.at(N)`).
 *                  2. Array Decay Trap (Array function parameters decay to raw pointers).
 *                  3. Comparing C-Style Arrays/Strings with `==` (Pointer vs Value Comparison).
 *                  4. Missing Null Terminator ('\0') in Character Buffers.
 *                  5. Dangling `std::string_view` Hazards (Referencing local temporaries).
 *                  6. Unflushed Stream Buffer Newline Trap (`cin >>` before `getline()`).
 *                  7. Range-Loop Value Copying (Accidentally modifying copies instead of elements).
 *
 * TIME COMPLEXITY  : O(1) to O(N) depending on demonstration block.
 * SPACE COMPLEXITY : O(1) auxiliary space overhead.
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <stdexcept>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// MISTAKE 2 DEMONSTRATION: ARRAY POINTER DECAY
// Passing raw C-arrays to functions causes automatic decay into a raw pointer (`const int*`),
// destroying compile-time size information.
// =====================================================================================

void demonstrateDecayBug(const int arr[5]) {
    // RESOLVED: In C++, 'const int arr[5]' in a parameter list is syntactic sugar for 'const int* arr'.
    // Assigning to an explicit pointer demonstrates decay without triggering compiler warnings (-Wsizeof-array-argument).
    const int* decayedPtr = arr;
    cout << "  - [MISTAKE - Decayed Pointer] sizeof(decayed ptr) inside function = " 
         << sizeof(decayedPtr) << " bytes (Pointer size, NOT 5 * sizeof(int)!)\n";
}

void demonstrateDecayFix(const std::array<int, 5>& arr) {
    // FIX: std::array passed by reference preserves full type and container size info.
    cout << "  - [FIX - std::array Reference] sizeof(arr) inside function = " 
         << sizeof(arr) << " bytes (Full container size preserved!)\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a test index integer (e.g., 5 to trigger out-of-bounds safety check): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting index to 5." << endl;
        userInputValue = 5;
    }

    // =====================================================================================
    // MISTAKE 1: OFF-BY-ONE & OUT-OF-BOUNDS ACCESS
    // Accessing index N on an array of size N triggers Undefined Behavior (UB).
    // =====================================================================================
    cout << "\n================ 1. OFF-BY-ONE & OUT-OF-BOUNDS ACCESS ================\n";

    std::array<int, 5> numbers = {10, 20, 30, 40, 50}; // Valid indices: 0, 1, 2, 3, 4

    cout << "  - Array size: " << numbers.size() << " elements (Valid indices 0..4)\n";
    cout << "  - [MISTAKE]: `numbers[5]` is off-by-one! Undefined behavior / memory corruption.\n";

    // FIX: Use `.at(index)` for safe, bounds-checked access
    try {
        cout << "  - [FIX]: Accessing index " << userInputValue << " using `.at()` safety check...\n";
        int value = numbers.at(static_cast<size_t>(userInputValue));
        cout << "    Value at index " << userInputValue << " = " << value << "\n";
    } catch (const std::out_of_range& e) {
        cout << "    [EXCEPTIONAL SAFETY] Caught out_of_range exception: " << e.what() << "\n";
    }

    // =====================================================================================
    // MISTAKE 2: ARRAY POINTER DECAY IN FUNCTIONS
    // =====================================================================================
    cout << "\n================ 2. ARRAY POINTER DECAY TRAP ================\n";

    int rawArr[5] = {1, 2, 3, 4, 5};
    cout << "  - Outside function sizeof(rawArr) = " << sizeof(rawArr) << " bytes (5 ints * 4 bytes)\n";

    demonstrateDecayBug(rawArr);
    demonstrateDecayFix(numbers);

    // =====================================================================================
    // MISTAKE 3: COMPARING C-STYLE STRINGS / ARRAYS WITH `==`
    // Operator `==` on raw arrays or `char*` compares memory addresses, NOT content.
    // =====================================================================================
    cout << "\n================ 3. EQUALITY COMPARISON TRAP (`==`) ================\n";

    char str1[] = "Hello";
    char str2[] = "Hello";

    // MISTAKE: Direct `str1 == str2` compares addresses (Explicit unary '+' decays operands to pointers)
    cout << "  - [MISTAKE]: `+str1 == +str2` evaluates to: " 
         << (+str1 == +str2 ? "EQUAL" : "NOT EQUAL") 
         << " (Compares raw stack addresses!)\n";

    // FIX 1: Use strcmp for C-strings
    cout << "  - [FIX 1 - C-String]: `strcmp(str1, str2) == 0` evaluates to: " 
         << (strcmp(str1, str2) == 0 ? "EQUAL" : "NOT EQUAL") << "\n";

    // FIX 2: Use std::string or std::string_view for native value comparison
    std::string s1 = "Hello";
    std::string s2 = "Hello";
    cout << "  - [FIX 2 - std::string]: `s1 == s2` evaluates to: " 
         << (s1 == s2 ? "EQUAL" : "NOT EQUAL") << " (Compares actual string values!)\n";

    // =====================================================================================
    // MISTAKE 4: MISSING NULL TERMINATOR ('\0') IN CHARACTER ARRAYS
    // Reading character arrays without a trailing '\0' causes buffer overflow reads.
    // =====================================================================================
    cout << "\n================ 4. MISSING NULL TERMINATOR ('\\0') ================\n";

    char badCharArr[5] = {'C', '+', '+', '2', '0'}; // MISTAKE: No '\0'! Reading past buffer causes UB.
    (void)badCharArr;                               // RESOLVED: Explicitly void-cast to suppress -Wunused-variable warning

    char goodCharArr[6] = {'C', '+', '+', '2', '0', '\0'}; // FIX: Explicit null termination

    cout << "  - [MISTAKE]: Un-terminated char array risks reading adjacent dirty memory.\n";
    cout << "  - [FIX]: Properly null-terminated array length via strlen: " 
         << strlen(goodCharArr) << " characters (\"" << goodCharArr << "\")\n";

    // =====================================================================================
    // MISTAKE 5: DANGLING `std::string_view` HAZARDS
    // Storing a `std::string_view` bound to a destroyed temporary object.
    // =====================================================================================
    cout << "\n================ 5. DANGLING `std::string_view` HAZARDS ================\n";

    std::string_view danglingView;
    {
        std::string temporaryString = "Short Lived Data";
        danglingView = temporaryString; // Dangerous view binding
        cout << "  - Inside Scope: View content = \"" << danglingView << "\"\n";
    } // temporaryString is destroyed HERE!

    cout << "  - [MISTAKE WARNING]: 'danglingView' now points to deallocated memory!\n";
    cout << "  - [FIX]: Convert `std::string_view` to an owning `std::string` if persistence is needed.\n";

    // =====================================================================================
    // MISTAKE 6: UNFLUSHED STREAM BUFFER NEWLINE TRAP
    // Calling `getline()` after `cin >>` without flushing leftover '\n'.
    // =====================================================================================
    cout << "\n================ 6. UNFLUSHED I/O STREAM BUFFER TRAP ================\n";

    cout << "  - Previous operation used `cin >> userInputValue`.\n";
    cout << "  - [MISTAKE]: Calling `getline(cin, str)` immediately will read the leftover '\\n' and return empty!\n";

    // FIX: Clear flags and flush buffer
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "  - [FIX]: Stream buffer flushed successfully via `cin.ignore()`!\n";

    // =====================================================================================
    // MISTAKE 7: RANGE-LOOP VALUE COPYING VS REFERENCE MUTATION
    // Iterating by value creates local copies, preventing intended modifications.
    // =====================================================================================
    cout << "\n================ 7. RANGE-FOR VALUE COPYING VS REFERENCE ================\n";

    std::vector<int> data = {1, 2, 3, 4, 5};

    // MISTAKE: `for (auto val : data)` modifies local copy `val`, NOT `data`!
    for (auto val : data) {
        val *= 10;
    }
    cout << "  - [MISTAKE - Iterated by Value]: First element remains unmodified: " << data[0] << "\n";

    // FIX: Use reference `auto&` to modify elements in-place
    for (auto& val : data) {
        val *= 10;
    }
    cout << "  - [FIX - Iterated by Reference auto&]: First element modified: " << data[0] << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ COMMON MISTAKES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Common Pitfall        | Problem / Consequence             | Modern C++ Best Practice / Fix    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| `arr[N]` Off-by-one   | Undefined Behavior / Crash        | Use `.at(index)` for safety       |\n"
         << "| Raw Array Decay       | `sizeof()` returns pointer size   | Use `std::array<T, N>` / `ref`    |\n"
         << "| `cArr1 == cArr2`      | Compares pointer addresses        | Use `strcmp()` or `std::string`   |\n"
         << "| Un-terminated char[]  | Buffer overflow memory scan       | Always include trailing `'\\0'`      |\n"
         << "| Dangling string_view  | Reads deallocated heap/stack mem  | Copy to owning `std::string`      |\n"
         << "| Unflushed `cin` buffer| `getline()` reads leftover `'\\n'`  | Call `cin.ignore()` after `cin>>` |\n"
         << "| Range Loop Value Copy | Modifies copy, leaves vector as-is| Use `for (auto& elem : vec)`      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}