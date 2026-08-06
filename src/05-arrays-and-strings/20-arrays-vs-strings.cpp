/*
 * =====================================================================================
 * CONCEPT        : Arrays vs Strings in Modern C++
 * DESCRIPTION    : Comprehensive comparative analysis and code implementation comparing 
 *                  Array representations (`T[]`, `std::array`, `std::vector`) against 
 *                  String representations (`char[]`, `std::string`, `std::string_view`):
 *                  1. Purpose & Genericity        : Arrays store any element type T; Strings 
 *                                                    specifically store character/text data.
 *                  2. Memory & Sentinel Markers   : C-strings rely on null-terminator ('\0') sentinels;
 *                                                    Arrays rely strictly on size bounds.
 *                  3. Dedicated Operations        : Strings provide search (`find`), substring (`substr`), 
 *                                                    concatenation (`+`), and formatting; Arrays 
 *                                                    focus on indexed access, sorting, and linear transforms.
 *                  4. `std::vector<char>` vs `std::string`:
 *                                                    Both are dynamic heap buffers of chars, but `std::string`
 *                                                    provides SSO (Small String Optimization), text algorithms, 
 *                                                    and null-terminated `.c_str()` compatibility.
 *
 * TIME COMPLEXITY  : Element Access: O(1) for both | Search/Concat: O(N)
 * SPACE COMPLEXITY : Contiguous stack or heap buffers: O(N)
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <iomanip>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS FOR DEMONSTRATIONS
// =====================================================================================

// Display generic numeric array
template <typename T, size_t N>
void printArray(const std::array<T, N>& arr, const string& label) {
    cout << "  - [" << left << setw(24) << label << "] Size: " << arr.size() << " | Elements: [ ";
    for (size_t i = 0; i < arr.size(); ++i) {
        cout << arr[i] << (i + 1 < arr.size() ? ", " : " ");
    }
    cout << "]\n";
}

// Display dynamic vector
template <typename T>
void printVector(const std::vector<T>& vec, const string& label) {
    cout << "  - [" << left << setw(24) << label << "] Size: " << vec.size() << " | Elements: [ ";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << (i + 1 < vec.size() ? ", " : " ");
    }
    cout << "]\n";
}

// Display string with capacity / SSO metrics
void printStringDetails(const string& str, const string& label) {
    cout << "  - [" << left << setw(24) << label << "] Size: " << setw(2) << str.size() 
         << " | Capacity: " << setw(2) << str.capacity() 
         << " | Content: \"" << str << "\"\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userInputText;

    // Dynamic input collection with stream flushing
    cout << "Enter a sample phrase for comparison (e.g., C++2026 Engine): " << flush;
    getline(cin, userInputText);

    if (userInputText.empty()) {
        userInputText = "C++2026 Engine";
        cout << "Empty input detected. Defaulting to: \"" << userInputText << "\"\n";
    }

    // =====================================================================================
    // 1. GENERIC ELEMENT BUFFER (ARRAY) VS TEXT SENTINEL BUFFER (STRING)
    // =====================================================================================
    cout << "\n================ 1. RAW C-ARRAY VS C-STRING (SENTINEL DIFFERENCE) ================\n";

    int intArray[5] = {10, 20, 30, 40, 50}; // Generic integer buffer (No sentinel marker)
    char cString[]  = "Hello";              // Character buffer terminated by explicit '\0'

    cout << "  - Raw Integer Array size (sizeof)   : " << sizeof(intArray) << " bytes (5 ints * 4 bytes)\n";
    cout << "  - Raw Character C-String size       : " << sizeof(cString)  << " bytes (5 chars + 1 '\\0' byte)\n";
    cout << "  - C-String Active Length (strlen)   : " << strlen(cString)  << " characters (Excludes null terminator)\n";

    // =====================================================================================
    // 2. STL STATIC CONTAINERS: `std::array<T, N>` VS `std::string`
    // =====================================================================================
    cout << "\n================ 2. `std::array<T, N>` VS `std::string` ================\n";

    std::array<int, 5> stdNumArray = {1, 2, 3, 4, 5};
    std::string stdTextString = "Hello";

    printArray(stdNumArray, "std::array<int, 5>");
    printStringDetails(stdTextString, "std::string");

    cout << "\n  - [KEY DIFFERENCE]: Mutability & Types\n";
    cout << "    std::array stores fixed-type arbitrary data; std::string stores text characters.\n";

    // =====================================================================================
    // 3. DYNAMIC CONTAINERS: `std::vector<char>` VS `std::string`
    // Why std::string is NOT just a std::vector<char>
    // =====================================================================================
    cout << "\n================ 3. `std::vector<char>` VS `std::string` ================\n";

    std::vector<char> charVector = {'H', 'e', 'l', 'l', 'o'};
    std::string textString = "Hello";

    printVector(charVector, "std::vector<char>");
    printStringDetails(textString, "std::string");

    // Demonstrating specialized string capabilities NOT present in std::vector
    cout << "\n  - [TEXT-SPECIFIC CAPABILITIES IN std::string]:\n";

    // A. Concatenation Operator +
    textString += " World!";
    cout << "    1. Concatenation `+=`           : \"" << textString << "\"\n";

    // B. Pattern Search .find()
    size_t matchPos = textString.find("World");
    if (matchPos != string::npos) {
        cout << "    2. Pattern Search `.find()`      : Match for \"World\" found at index " << matchPos << "\n";
    }

    // C. Substring Extraction .substr()
    string subSlice = textString.substr(0, 5);
    cout << "    3. Substring Extraction `.substr()`: \"" << subSlice << "\"\n";

    // D. Null-Terminated C-API Interoperability .c_str()
    const char* cApiPtr = textString.c_str();
    cout << "    4. Legacy C-API `.c_str()` Addr   : " << static_cast<const void*>(cApiPtr) << "\n";

    // =====================================================================================
    // 4. INTEROPERABILITY & CONVERSION
    // Converting character containers to string_view
    // =====================================================================================
    cout << "\n================ 4. CONVERSION & VIEW INTEROPERABILITY ================\n";

    std::string_view userView(userInputText);
    cout << "  - User Text via `std::string_view`  : \"" << userView << "\"\n";
    cout << "  - View Size                        : " << userView.size() << " characters\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ ARRAYS VS STRINGS DETAILED COMPARISON ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Trait       | Array Types (`std::array`/`vector`)| String Types (`std::string`)      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Target Domain         | Generic sequential element buffer | Specialized text character buffer |\n"
         << "| Stored Element Types  | Any type `T` (ints, structs, obj) | Character types (`char`, `char8_t`)|\n"
         << "| Termination Marker    | None (Strict size boundaries)     | Null terminator (`'\\0'`) for C-API|\n"
         << "| Concatenation (`+`)   | Unsupported (Requires insert/push)| Built-in native `+` and `+=`      |\n"
         << "| Text Search Algorithms| Manual `std::find` iterators      | Built-in `.find()`, `.rfind()`    |\n"
         << "| Substring Extraction  | Manual iterator slicing           | Built-in `.substr(pos, len)`      |\n"
         << "| SSO Optimization      | None (vector always uses heap)    | Active (SSO avoids heap for short)|\n"
         << "| Format Conversion     | Manual parsing                    | `std::stoi`, `std::to_string`     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}