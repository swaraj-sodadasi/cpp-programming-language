/*
 * =====================================================================================
 * CONCEPT        : C-Style Strings in C++ (Null-Terminated Character Arrays)
 * DESCRIPTION    : Comprehensive guide and implementation detailing C-style strings:
 *                  1. Memory Structure          : Sequential character array ending with a null terminator ('\0').
 *                  2. Declaration & Init        : String literals, explicit character arrays, and `const char*`.
 *                  3. `<cstring>` Functions     : `strlen`, `strncpy`, `strncat`, `strcmp`, `strchr`, `strstr`.
 *                  4. Safe Input Handling       : Reading lines safely using `std::cin.getline()`.
 *                  5. `<cctype>` Manipulations  : Character-level checks (`isalpha`, `isdigit`) and transformations (`toupper`).
 *                  6. Functions & Pointer Decay : Passing C-style strings as `const char*` parameters.
 *
 * TIME COMPLEXITY  : Length / Copy / Concatenation / Search: O(N) linear time.
 * SPACE COMPLEXITY : Stack-allocated character buffer: O(N) bytes.
 * =====================================================================================
 */

#include <iostream>
#include <cstring>
#include <cctype>
#include <iomanip>
#include <limits>
#include <cstddef>

using namespace std;

// Helper function to safely flush cin stream on input errors
void flushInputStream() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// =====================================================================================
// 1. PASSING C-STYLE STRINGS TO FUNCTIONS
// C-style strings decay to `const char*` pointers when passed to functions.
// =====================================================================================

void printCStyleStringDetails(const char* str, const string& label) {
    if (!str) return;
    
    size_t length = strlen(str);
    cout << "  - [" << left << setw(20) << label << "] Content: \"" << str << "\"\n";
    cout << "    Pointer Address : " << static_cast<const void*>(str) << "\n";
    cout << "    strlen() Length : " << length << " characters (Excludes '\\0')\n";
}

// Function modifying a C-style string in-place
void convertToUppercaseInPlace(char* str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        str[i] = static_cast<char>(toupper(static_cast<unsigned char>(str[i])));
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    constexpr size_t BUFFER_CAPACITY = 64;
    char userBuffer[BUFFER_CAPACITY] = {};

    // Dynamic input collection with stream flushing
    cout << "Enter a phrase or sentence (max 63 chars, e.g., C++ Programming 2026): " << flush;
    cin.getline(userBuffer, BUFFER_CAPACITY);

    if (cin.fail()) {
        flushInputStream();
        cout << "Input exceeded buffer bounds! Defaulting to fallback string." << endl;
        strncpy(userBuffer, "C++ Programming 2026", BUFFER_CAPACITY - 1);
        userBuffer[BUFFER_CAPACITY - 1] = '\0';
    } else if (strlen(userBuffer) == 0) {
        strncpy(userBuffer, "C++ Programming 2026", BUFFER_CAPACITY - 1);
        userBuffer[BUFFER_CAPACITY - 1] = '\0';
    }

    // =====================================================================================
    // 1. DECLARATION, INITIALIZATION & THE NULL TERMINATOR ('\0')
    // =====================================================================================
    cout << "\n================ 1. DECLARATION & THE NULL TERMINATOR ================\n";

    char explicitArr[6] = {'H', 'e', 'l', 'l', 'o', '\0'}; // Explicit null termination
    char literalArr[]   = "World";                         // Size automatically deduced as 6 ('W','o','r','l','d','\0')
    const char* ptrLiteral = "Immutable Literal";          // Read-only pointer to string literal in data segment

    printCStyleStringDetails(explicitArr, "Explicit Array");
    printCStyleStringDetails(literalArr, "Literal Array");
    printCStyleStringDetails(ptrLiteral, "String Pointer");

    // Memory Bytes Inspection including '\0'
    cout << "\n  - Memory Byte View of literalArr (\"World\"):\n    ";
    for (size_t i = 0; i < sizeof(literalArr); ++i) {
        cout << "[" << (literalArr[i] == '\0' ? "\\0" : string(1, literalArr[i])) 
             << " : " << static_cast<int>(literalArr[i]) << "] ";
    }
    cout << "\n";

    // =====================================================================================
    // 2. BUFFER SIZE (sizeof) VS STRING LENGTH (strlen)
    // =====================================================================================
    cout << "\n================ 2. sizeof() VS strlen() ================\n";

    char staticBuffer[32] = "Data Stream";

    cout << "  - Buffer Content   : \"" << staticBuffer << "\"\n";
    cout << "  - strlen() Result  : " << strlen(staticBuffer) << " characters (Active characters before '\\0')\n";
    cout << "  - sizeof() Result  : " << sizeof(staticBuffer) << " bytes (Total memory allocated on stack)\n";

    // =====================================================================================
    // 3. SAFE MANIPULATION VIA `<cstring>`
    // =====================================================================================
    cout << "\n================ 3. `<cstring>` MANIPULATION FUNCTIONS ================\n";

    char target[64] = "Header: ";
    char appendChunk[] = "Payload Systems";

    // Safe Bounded Copy (strncpy)
    char copyDest[32];
    strncpy(copyDest, appendChunk, sizeof(copyDest) - 1);
    copyDest[sizeof(copyDest) - 1] = '\0'; // Guarantee null termination
    cout << "  - strncpy Result   : \"" << copyDest << "\"\n";

    // Safe Bounded Concatenation (strncat)
    strncat(target, appendChunk, sizeof(target) - strlen(target) - 1);
    cout << "  - strncat Result   : \"" << target << "\"\n";

    // String Comparison (strcmp)
    char s1[] = "Alpha";
    char s2[] = "Beta";
    int comparison = strcmp(s1, s2);
    cout << "  - strcmp(\"" << s1 << "\", \"" << s2 << "\") = " << comparison << " ";
    if (comparison < 0) {
        cout << "(\"" << s1 << "\" comes before \"" << s2 << "\" lexicographically)\n";
    } else if (comparison > 0) {
        cout << "(\"" << s1 << "\" comes after \"" << s2 << "\" lexicographically)\n";
    } else {
        cout << "(Strings are equal)\n";
    }

    // Substring Search (strstr) and Character Search (strchr)
    const char* subMatch = strstr(target, "Systems");
    if (subMatch) {
        cout << "  - strstr Found     : Substring \"Systems\" starts at index " << (subMatch - target) << "\n";
    }

    const char* charMatch = strchr(target, 'P');
    if (charMatch) {
        cout << "  - strchr Found     : Character 'P' starts at index " << (charMatch - target) << "\n";
    }

    // =====================================================================================
    // 4. CHARACTER ANALYSIS & IN-PLACE MUTATION VIA `<cctype>`
    // =====================================================================================
    cout << "\n================ 4. CHARACTER ANALYSIS & IN-PLACE MUTATION ================\n";
    cout << "  - User Buffer Input : \"" << userBuffer << "\"\n";

    int digits = 0, alphas = 0, spaces = 0;
    for (size_t i = 0; userBuffer[i] != '\0'; ++i) {
        unsigned char ch = static_cast<unsigned char>(userBuffer[i]);
        if (isdigit(ch)) digits++;
        if (isalpha(ch)) alphas++;
        if (isspace(ch)) spaces++;
    }

    cout << "  - Character Metrics : Letters = " << alphas 
         << " | Digits = " << digits 
         << " | Spaces = " << spaces << "\n";

    // In-place transformation call
    convertToUppercaseInPlace(userBuffer);
    cout << "  - Converted Buffer  : \"" << userBuffer << "\"\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ C-STYLE STRINGS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature               | C-Style String (`char[]` / `char*`)| Modern C++ String (`std::string`) |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Ending Sentinel       | Requires null character (`'\\0'`)   | Managed internally by class       |\n"
         << "| Resizability          | Fixed stack/heap array length     | Dynamic runtime heap growth       |\n"
         << "| Memory Overhead       | Zero (pure char array)            | Small object header + heap buffer |\n"
         << "| Comparison            | Must use `strcmp()` / `strncmp()` | Native operators (`==`, `<`, `>`) |\n"
         << "| Buffer Safety         | Vulnerable to overflow if unbounded| Automatic reallocation safety     |\n"
         << "| Function Decay        | Decays to `const char*` pointer   | Passed as object value/reference  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}