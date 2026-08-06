/*
 * =====================================================================================
 * CONCEPT        : Best Practices for Arrays and Strings in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade implementation highlighting
 *                  critical safety, performance, and API design rules:
 *
 *                  1. Prefer `std::array` / `std::vector` over C-Style Arrays
 *                     - Prevents pointer decay, retains size, provides `.at()` bounds checking.
 *                  2. Use `std::string_view` for Read-Only String Parameters
 *                     - Accepts `std::string`, `const char*`, and string literals without 
 *                       heap allocations.
 *                  3. Pre-allocate Memory with `.reserve()`
 *                     - Prevents frequent reallocation overhead when building dynamic 
 *                       `std::vector` or `std::string` buffers.
 *                  4. Pass Containers by `const&` or `std::string_view` / `std::span`
 *                     - Eliminates unnecessary O(N) object copy costs.
 *                  5. Prefer Range-Based `for` Loops with `const auto&`
 *                     - Eliminates indexing bugs and redundant element copying.
 *                  6. Avoid Raw Pointer Arithmetic & Insecure C-Functions (`strcpy`, `strcat`)
 *                     - Eliminates buffer overflow vulnerabilities.
 *                  7. Mind `std::string_view` Lifetimes
 *                     - Never return a view referencing a temporary or destroyed string object.
 *
 * TIME COMPLEXITY  : Access: O(1) | Append (with reserve): Amortized O(1) | Search: O(N)
 * SPACE COMPLEXITY : Modern STL containers maintain minimal zero/O(1) abstraction overhead.
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <iomanip>
#include <stdexcept>
#include <cstddef>

using namespace std;

// =====================================================================================
// RULE 1 & 2: BEST PRACTICE FOR FUNCTION PARAMETERS
// Use `std::string_view` for read-only string views (Zero allocations for char* or std::string)
// Use `const std::vector<T>&` or `std::span` for read-only array containers.
// =====================================================================================

void analyzeText(std::string_view sv, std::string_view label) {
    cout << "  - [" << left << setw(26) << label << "] Size: " << sv.size() 
         << " | Content: \"" << sv << "\"\n";
}

void processArraySafely(const std::vector<int>& data) {
    cout << "  - Traversed " << data.size() << " elements safely via const reference:\n    [ ";
    for (const auto& elem : data) { // Rule: const auto& avoids copies
        cout << elem << " ";
    }
    cout << "]\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userInput;

    // Dynamic input collection with stream flushing
    cout << "Enter a base string for best practice demonstration (e.g., C++ Memory Safety): " << flush;
    getline(cin, userInput);

    if (userInput.empty()) {
        userInput = "C++ Memory Safety";
        cout << "Empty input detected. Defaulting to: \"" << userInput << "\"\n";
    }

    // =====================================================================================
    // 1. DO: PREFER `std::array` AND `std::vector` OVER RAW C-ARRAYS
    // =====================================================================================
    cout << "\n================ 1. MODERN CONTAINERS VS RAW ARRAYS ================\n";

    // BAD: int rawArr[5] = {1, 2, 3, 4, 5}; -> Decays to raw pointer, loses size info.
    // GOOD: Fixed-size stack array with zero overhead
    std::array<int, 5> safeStackArray = {10, 20, 30, 40, 50};

    // GOOD: Dynamic-size heap array with RAII memory management
    std::vector<int> safeDynamicVector = {100, 200, 300, 400, 500};

    cout << "  - std::array size preserved    : " << safeStackArray.size() << " elements\n";
    cout << "  - std::vector size preserved   : " << safeDynamicVector.size() << " elements\n";

    // Safe bounds checking demonstration
    try {
        cout << "  - Attempting out-of-bounds access with .at(10)...\n";
        int val = safeStackArray.at(10);
        (void)val;
    } catch (const std::out_of_range& e) {
        cout << "  - [BEST PRACTICE] Caught out_of_range exception safely: " << e.what() << "\n";
    }

    // =====================================================================================
    // 2. DO: USE `std::string_view` TO AVOID TEMPORARY ALLOCATIONS
    // =====================================================================================
    cout << "\n================ 2. ZERO-ALLOCATION PARAMETERS (`std::string_view`) ================\n";

    // All of these calls run WITHOUT creating any temporary std::string heap copies!
    analyzeText(userInput, "Passing std::string");
    analyzeText("Literal C-String Data", "Passing C-String Literal");
    analyzeText(std::string_view(userInput).substr(0, 3), "Passing O(1) Substring View");

    // =====================================================================================
    // 3. DO: PRE-ALLOCATE MEMORY WITH `.reserve()`
    // =====================================================================================
    cout << "\n================ 3. PRE-ALLOCATION WITH `.reserve()` ================\n";

    constexpr size_t EXPECTED_ELEMENTS = 1000;

    std::vector<int> optimizedVector;
    optimizedVector.reserve(EXPECTED_ELEMENTS); // Allocates capacity upfront!

    std::string optimizedString;
    optimizedString.reserve(128);                // Prevents heap reallocations during string builds

    cout << "  - Vector Capacity after .reserve(" << EXPECTED_ELEMENTS << "): " 
         << optimizedVector.capacity() << " (Zero reallocations on push_back)\n";
    cout << "  - String Capacity after .reserve(128): " 
         << optimizedString.capacity() << " (Zero reallocations on append)\n";

    // =====================================================================================
    // 4. DO: USE CONST REFERENCES IN RANGE LOOPS TO PREVENT COPIES
    // =====================================================================================
    cout << "\n================ 4. EFFICIENT ITERATION ================\n";

    std::vector<std::string> largeStringList = {"Performance", "Security", "Maintainability", "Safety"};

    cout << "  - Iterating via `const auto&` (Zero copy overhead):\n";
    for (const auto& str : largeStringList) {
        cout << "    * " << str << " (Addr: " << static_cast<const void*>(str.data()) << ")\n";
    }

    // =====================================================================================
    // 5. DON'T: DANGLING `std::string_view` (LIFETIME SAFETY)
    // =====================================================================================
    cout << "\n================ 5. AVOIDING LIFETIME TRAPS ================\n";

    std::string_view dangerousView;
    {
        std::string temporaryString = "Temporary Local Value";
        dangerousView = temporaryString; // Dangerous if accessed outside scope!
        cout << "  - Inside Scope: View is valid -> \"" << dangerousView << "\"\n";
    } // temporaryString is DESTRUCTED here!

    cout << "  - [CRITICAL BEST PRACTICE WARNING]: 'dangerousView' is now dangling!\n";
    cout << "    Rule: Never store a std::string_view referencing a short-lived temporary.\n";

    // Safe conversion to an owning std::string when persistence is required
    std::string persistentCopy(userInput);
    cout << "  - Safe Persistent Copy Created: \"" << persistentCopy << "\"\n";

    // =====================================================================================
    // SUMMARY CHECKLIST TABLE
    // =====================================================================================
    cout << "\n================ ARRAYS & STRINGS BEST PRACTICES SUMMARY ================\n";
    cout << "+-----------------------------------+-----------------------------------+-----------------------------------+\n"
         << "| Category                          | Anti-Pattern (AVOID)              | Best Practice (PREFER)            |\n"
         << "+-----------------------------------+-----------------------------------+-----------------------------------+\n"
         << "| Fixed-Size Stack Arrays           | `int arr[N]` (Decays to pointer)  | `std::array<int, N>`              |\n"
         << "| Dynamic Heap Arrays               | `int* arr = new int[N]`           | `std::vector<int>`                |\n"
         << "| Read-Only String Function Param   | `const std::string&` / `char*`    | `std::string_view`                |\n"
         << "| Read-Only Array Function Param    | `const int* arr, size_t len`      | `std::span<const int>` / `const&` |\n"
         << "| Dynamic Building                  | Unbounded `push_back()` / `+=`    | Pre-allocate via `.reserve()`     |\n"
         << "| Out-of-Bounds Protection          | Unchecked `arr[i]` on untrusted i | Bounds-checked `arr.at(i)`        |\n"
         << "| Loop Iteration                    | `for (auto item : container)`     | `for (const auto& item : container)`|\n"
         << "| String Manipulation               | C-functions (`strcpy`, `strcat`)  | Modern `std::string` / `format`   |\n"
         << "+-----------------------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}