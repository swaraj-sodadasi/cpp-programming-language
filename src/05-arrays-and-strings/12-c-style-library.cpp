/*
 * =====================================================================================
 * CONCEPT        : C-Style Standard Libraries in C++ (`<cstdio>`, `<cstring>`, `<cmath>`, `<cstdlib>`, `<cctype>`)
 * DESCRIPTION    : Comprehensive guide and implementation of C standard library headers in C++:
 *                  1. C Headers in C++ Scope      : Using modern C++ wrapper headers (e.g., `<cstdio>` instead 
 *                                                   of `<stdio.h>`) placing symbols in `std::`.
 *                  2. `<cstdio>` Input / Output   : `snprintf`, `sscanf`, `printf`, and `puts` with format specifiers.
 *                  3. `<cstring>` String Utilities: `strlen`, `strncpy`, `strncat`, `strcmp`, `memset`, `memcpy`.
 *                  4. `<cstdlib>` Memory & Env    : `malloc`, `free`, `strtol`, `strtod`, `abs`, `rand`, `qsort`, `bsearch`.
 *                  5. `<cmath>` Math Functions     : `pow`, `sqrt`, `ceil`, `floor`, `abs`, `sin`, `cos`.
 *                  6. `<cctype>` Character Checks  : `isalpha`, `isdigit`, `isalnum`, `isspace`, `toupper`, `tolower`.
 *
 * TIME COMPLEXITY  : String/Memory ops: O(N) | Sorting (`qsort`): O(N log N) | Math ops: O(1)
 * SPACE COMPLEXITY : Fixed stack/heap memory footprint: O(N)
 * =====================================================================================
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cctype>

using namespace std;

// Comparator callback function for std::qsort and std::bsearch
int compareIntegers(const void* a, const void* b) {
    int intA = *static_cast<const int*>(a);
    int intB = *static_cast<const int*>(b);
    return (intA - intB);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double mathInputValue = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter a floating-point number for <cmath> tests (e.g., 25.75): " << flush;
    if (!(cin >> mathInputValue) || mathInputValue <= 0.0) {
        cout << "Invalid input. Defaulting math test value to 25.75." << endl;
        mathInputValue = 25.75;
    }

    // =====================================================================================
    // 1. `<cstdio>` - C FORMATTED INPUT & OUTPUT
    // =====================================================================================
    cout << "\n================ 1. `<cstdio>` C-STYLE FORMATTED I/O ================\n";

    char formattedBuffer[128] = {};
    const char* appName = "Enterprise Engine";
    int versionMajor = 2026;

    // Safe Bounded Formatted String Printing (`snprintf`)
    int writtenBytes = std::snprintf(formattedBuffer, sizeof(formattedBuffer), 
                                     "System: %s | Release: %d | MathVal: %.2f", 
                                     appName, versionMajor, mathInputValue);

    cout << "  - std::snprintf Output Buffer : \"" << formattedBuffer << "\"\n";
    cout << "  - Written Bytes Count         : " << writtenBytes << " bytes\n";

    // Parsing Formatted Data (`sscanf`)
    char parsedApp[32] = {};
    int parsedVer = 0;
    if (std::sscanf(formattedBuffer, "System: %31s | Release: %d", parsedApp, &parsedVer) >= 1) {
        cout << "  - std::sscanf Parsed Substring: App = \"" << parsedApp << "\", Ver = " << parsedVer << "\n";
    }

    // =====================================================================================
    // 2. `<cstring>` - C STRING & RAW MEMORY MANIPULATION
    // =====================================================================================
    cout << "\n================ 2. `<cstring>` STRING & MEMORY UTILITIES ================\n";

    char destBuffer[64] = "Core Buffer: ";
    char srcChunk[] = "Active";

    // Safe Bounded Copy and Concatenation
    std::strncat(destBuffer, srcChunk, sizeof(destBuffer) - std::strlen(destBuffer) - 1);
    cout << "  - std::strncat Result         : \"" << destBuffer << "\"\n";

    // Raw Memory Block Operations (`memset` and `memcpy`)
    int rawNumbers[5] = {1, 2, 3, 4, 5};
    int copiedNumbers[5];

    // Copy raw byte block
    std::memcpy(copiedNumbers, rawNumbers, sizeof(rawNumbers));
    cout << "  - std::memcpy Output Array    : [ ";
    for (int num : copiedNumbers) {
        cout << num << " ";
    }
    cout << "]\n";

    // Zero-out raw byte block (`memset`)
    std::memset(copiedNumbers, 0, sizeof(copiedNumbers));
    cout << "  - std::memset Zero-out Array  : [ ";
    for (int num : copiedNumbers) {
        cout << num << " ";
    }
    cout << "]\n";

    // =====================================================================================
    // 3. `<cstdlib>` - UTILITIES, NUMERIC CONVERSIONS, SORT & SEARCH
    // =====================================================================================
    cout << "\n================ 3. `<cstdlib>` UTILITIES, qsort, & bsearch ================\n";

    // Numeric String Parsing (`strtol`, `strtod`)
    const char* numericStr = "1048576_bytes";
    char* endPtr = nullptr;
    long parsedLong = std::strtol(numericStr, &endPtr, 10);

    cout << "  - std::strtol Parsed Value    : " << parsedLong << " (Stopped at: \"" << endPtr << "\")\n";

    // Dynamic Allocation (Explicit C-style malloc/free in C++)
    int* dynamicArray = static_cast<int*>(std::malloc(4 * sizeof(int)));
    if (dynamicArray) {
        dynamicArray[0] = 95; dynamicArray[1] = 20; dynamicArray[2] = 75; dynamicArray[3] = 40;

        cout << "  - Dynamic Array before qsort  : [ 95, 20, 75, 40 ]\n";

        // C-style Quick Sort (`std::qsort`)
        std::qsort(dynamicArray, 4, sizeof(int), compareIntegers);

        cout << "  - Dynamic Array after qsort   : [ ";
        for (int i = 0; i < 4; ++i) {
            cout << dynamicArray[i] << " ";
        }
        cout << "]\n";

        // C-style Binary Search (`std::bsearch`)
        int searchKey = 75;
        void* foundPtr = std::bsearch(&searchKey, dynamicArray, 4, sizeof(int), compareIntegers);

        if (foundPtr) {
            cout << "  - std::bsearch Found Key " << searchKey << " at Address: " << foundPtr << "\n";
        }

        std::free(dynamicArray); // Clean up allocated C memory
    }

    // =====================================================================================
    // 4. `<cmath>` - MATHEMATICAL FUNCTIONS
    // =====================================================================================
    cout << "\n================ 4. `<cmath>` MATH FUNCTIONS ================\n";

    cout << "  - std::sqrt(" << mathInputValue << ")     : " << std::sqrt(mathInputValue) << "\n";
    cout << "  - std::pow(" << mathInputValue << ", 2)  : " << std::pow(mathInputValue, 2.0) << "\n";
    cout << "  - std::ceil(" << mathInputValue << ")    : " << std::ceil(mathInputValue) << "\n";
    cout << "  - std::floor(" << mathInputValue << ")   : " << std::floor(mathInputValue) << "\n";

    // =====================================================================================
    // 5. `<cctype>` - CHARACTER CLASSIFICATION & MUTATION
    // =====================================================================================
    cout << "\n================ 5. `<cctype>` CHARACTER OPERATIONS ================\n";

    char sampleChar = 'a';
    cout << "  - Character                   : '" << sampleChar << "'\n";
    cout << "  - std::isalpha('a')           : " << (std::isalpha(sampleChar) ? "True" : "False") << "\n";
    cout << "  - std::isdigit('a')           : " << (std::isdigit(sampleChar) ? "True" : "False") << "\n";
    cout << "  - std::toupper('a')           : '" << static_cast<char>(std::toupper(sampleChar)) << "'\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ C-STYLE LIBRARIES IN C++ SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| C Header Header       | Modern C++ Included Equivalent    | Primary Functions Provided        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| `<stdio.h>`           | `<cstdio>`                        | `snprintf`, `sscanf`, `printf`    |\n"
         << "| `<string.h>`          | `<cstring>`                       | `strlen`, `strncpy`, `memcpy`     |\n"
         << "| `<stdlib.h>`          | `<cstdlib>`                       | `malloc`, `free`, `qsort`, `strtol|\n"
         << "| `<math.h>`            | `<cmath>`                         | `sqrt`, `pow`, `ceil`, `floor`    |\n"
         << "| `<ctype.h>`           | `<cctype>`                        | `isalpha`, `isdigit`, `toupper`   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}