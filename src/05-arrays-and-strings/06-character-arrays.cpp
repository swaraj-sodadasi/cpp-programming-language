/*
 * =====================================================================================
 * CONCEPT        : Character Arrays (C-Style Strings) in C++
 * DESCRIPTION    : Comprehensive guide and implementation of Character Arrays:
 *                  1. Null Terminator ('\0')      : The special sentinel character marking the end 
 *                                                    of a valid C-style string buffer.
 *                  2. Memory Layout & Initialization: Difference between explicit element lists 
 *                                                    and string literal auto-null termination.
 *                  3. String Length vs Buffer Size: `strlen()` (character count) vs `sizeof()` (total bytes allocated).
 *                  4. Safe Input Handling         : Using `cin.getline()` to prevent buffer overflow.
 *                  5. Functions from `<cstring>`   : `strcpy_s` / `strncpy`, `strcat`, `strcmp`.
 *                  6. Functions from `<cctype>`    : Character classification and transformation (`toupper`, `isdigit`).
 *                  7. String Literals vs Arrays   : `const char*` (read-only literal) vs `char[]` (mutable buffer).
 *
 * TIME COMPLEXITY  : String Operations (`strlen`, `strcpy`, `strcmp`): O(N) linear time.
 * SPACE COMPLEXITY : Fixed Character Buffers: O(N) bytes.
 * =====================================================================================
 */

#include <iostream>
#include <cstring>
#include <cctype>
#include <limits>

using namespace std;

// Helper function to safely clear leftover input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    constexpr size_t BUFFER_SIZE = 64;
    char userInputBuffer[BUFFER_SIZE] = {};

    // Dynamic input collection with stream flushing
    cout << "Enter a message string (max 63 chars, e.g., C++ Coding 2026): " << flush;
    cin.getline(userInputBuffer, BUFFER_SIZE);

    if (cin.fail()) {
        clearInputBuffer();
        cout << "Input exceeded buffer! Falling back to default message." << endl;
        strncpy(userInputBuffer, "C++ Coding 2026", BUFFER_SIZE - 1);
        userInputBuffer[BUFFER_SIZE - 1] = '\0';
    } else if (strlen(userInputBuffer) == 0) {
        strncpy(userInputBuffer, "C++ Coding 2026", BUFFER_SIZE - 1);
        userInputBuffer[BUFFER_SIZE - 1] = '\0';
    }

    // =====================================================================================
    // 1. DECLARATION, INITIALIZATION & THE NULL TERMINATOR ('\0')
    // =====================================================================================
    cout << "\n================ 1. DECLARATION & THE NULL TERMINATOR ================\n";
    
    char explicitArr[6] = {'H', 'e', 'l', 'l', 'o', '\0'}; // Explicit null termination
    char literalArr[]   = "World";                         // Automatically appends '\0' (size = 6)

    cout << "  - Explicit Array: \"" << explicitArr << "\"\n";
    cout << "  - Literal Array : \"" << literalArr  << "\"\n";

    // Memory Inspection showing individual ASCII codes including '\0'
    cout << "  - Bytes inspection of explicitArr including null terminator:\n    ";
    for (size_t i = 0; i < sizeof(explicitArr); ++i) {
        cout << "['" << (explicitArr[i] == '\0' ? "\\0" : string(1, explicitArr[i])) 
             << "' : ASCII " << static_cast<int>(explicitArr[i]) << "] ";
    }
    cout << "\n";

    // =====================================================================================
    // 2. BUFFER SIZE (sizeof) VS STRING LENGTH (strlen)
    // =====================================================================================
    cout << "\n================ 2. sizeof() VS strlen() ================\n";
    
    char textBuffer[30] = "Hello C++";

    cout << "  - Target Buffer Content : \"" << textBuffer << "\"\n";
    cout << "  - strlen(textBuffer)     : " << strlen(textBuffer) << " characters (Excludes '\\0')\n";
    cout << "  - sizeof(textBuffer)     : " << sizeof(textBuffer) << " bytes (Total stack memory allocated)\n";

    // =====================================================================================
    // 3. COMMON `<cstring>` OPERATIONS
    // =====================================================================================
    cout << "\n================ 3. `<cstring>` STRING OPERATIONS ================\n";
    
    char destination[64] = "Base: ";
    char source[] = "Engine Online";

    // String Copy (Safe bounded copy using strncpy)
    char copiedBuffer[30];
    strncpy(copiedBuffer, source, sizeof(copiedBuffer) - 1);
    copiedBuffer[sizeof(copiedBuffer) - 1] = '\0'; // Ensure null termination
    cout << "  - strncpy Copy Result   : \"" << copiedBuffer << "\"\n";

    // String Concatenation
    strncat(destination, source, sizeof(destination) - strlen(destination) - 1);
    cout << "  - strncat Concat Result : \"" << destination << "\"\n";

    // String Comparison (strcmp)
    char str1[] = "Apple";
    char str2[] = "Banana";
    int cmpResult = strcmp(str1, str2);

    cout << "  - strcmp(\"" << str1 << "\", \"" << str2 << "\") = " << cmpResult << " ";
    if (cmpResult < 0) {
        cout << "(\"" << str1 << "\" is lexicographically smaller than \"" << str2 << "\")\n";
    } else if (cmpResult > 0) {
        cout << "(\"" << str1 << "\" is lexicographically greater than \"" << str2 << "\")\n";
    } else {
        cout << "(Strings are identical)\n";
    }

    // =====================================================================================
    // 4. CHARACTER TRAVERSAL & `<cctype>` MANIPULATION
    // =====================================================================================
    cout << "\n================ 4. CHARACTER CLASSIFICATION & MUTATION ================\n";
    cout << "  - Processing User Input Buffer: \"" << userInputBuffer << "\"\n";

    int letterCount = 0, digitCount = 0, spaceCount = 0;
    
    // Copy for uppercase transformation
    char uppercaseBuffer[BUFFER_SIZE];
    strncpy(uppercaseBuffer, userInputBuffer, BUFFER_SIZE - 1);
    uppercaseBuffer[BUFFER_SIZE - 1] = '\0';

    // Traverse character array using pointer/index until '\0'
    for (size_t i = 0; uppercaseBuffer[i] != '\0'; ++i) {
        unsigned char ch = static_cast<unsigned char>(uppercaseBuffer[i]);

        if (isalpha(ch)) letterCount++;
        if (isdigit(ch)) digitCount++;
        if (isspace(ch)) spaceCount++;

        // Convert lowercase characters to uppercase in-place
        uppercaseBuffer[i] = static_cast<char>(toupper(ch));
    }

    cout << "  - Uppercase Converted : \"" << uppercaseBuffer << "\"\n";
    cout << "  - Character Metrics   : Letters = " << letterCount 
         << " | Digits = " << digitCount 
         << " | Whitespaces = " << spaceCount << "\n";

    // =====================================================================================
    // 5. STRING LITERAL POINTERS VS MUTABLE CHARACTER ARRAYS
    // =====================================================================================
    cout << "\n================ 5. CONST CHAR* VS CHAR ARRAYS ================\n";
    
    const char* literalPtr = "Immutable String Literal"; // Points to read-only string literal in data segment
    char mutableArr[]      = "Mutable Stack Array";     // Copy placed on stack (can be modified)

    mutableArr[0] = 'm'; // LEGAL: Modifying stack buffer

    cout << "  - Literal Pointer (`const char*`) : " << literalPtr << "\n";
    cout << "  - Modified Array  (`char[]`)       : " << mutableArr << "\n";

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ CHARACTER ARRAYS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Concept     | C-Style Character Array (`char[]`)| Modern C++ String (`std::string`) |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Termination Marker    | Null character (`'\\0'`)           | Managed internally by class       |\n"
         << "| Size Management       | Fixed at compile-time / manual    | Dynamic runtime heap resizing     |\n"
         << "| Memory Overhead       | Zero abstraction overhead         | Pointer + Capacity stack/heap obj |\n"
         << "| Safety against Overflow| Manual bounds checking (`strncpy`)| Automatic memory reallocation    |\n"
         << "| Passing to Functions  | Decays to `char*` raw pointer     | Value / Reference (`const string&`)|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}