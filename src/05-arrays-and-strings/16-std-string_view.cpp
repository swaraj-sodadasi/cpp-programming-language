/*
 * =====================================================================================
 * CONCEPT        : `std::string_view` in C++17 (Non-Owning Character Range View)
 * DESCRIPTION    : Comprehensive guide and implementation of `std::string_view`:
 *                  1. What is std::string_view?: A lightweight (pointer + length pair) non-owning 
 *                     reference to a contiguous sequence of characters.
 *                  2. Zero Allocation Slicing   : O(1) substring operations (`substr`, `remove_prefix`, 
 *                     `remove_suffix`) without heap allocation or copying.
 *                  3. Optimal Function Parameter: Replaces `const std::string&` and `const char*` 
 *                     to accept any string representation without implicit conversion overhead.
 *                  4. String View Literals      : Modern literal suffix `""sv` (`std::string_view_literals`).
 *                  5. Critical Lifetime Traps   : Non-owning nature means viewing a destroyed 
 *                     temporary string creates a dangling reference (Undefined Behavior).
 *                  6. Null-Termination Caveat   : `std::string_view` is NOT guaranteed to be null-terminated 
 *                     (especially after slicing operations).
 *
 * TIME COMPLEXITY  : Construction: O(1) for std::string / string_view, O(N) for const char*
 *                    Substr / Trimming / Access: O(1)
 * SPACE COMPLEXITY : 16 bytes overhead (2 words: 1 pointer + 1 size_t) on 64-bit systems.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <string_view>
#include <iomanip>

using namespace std;
using namespace std::string_view_literals; // Enables ""sv string_view literal suffix

// =====================================================================================
// 1. EFFICIENT FUNCTION PARAMETERS WITHOUT ALLOCATION
// Accepting std::string_view works seamlessly with std::string, string literals, 
// and char arrays without generating temporary std::string heap allocations.
// =====================================================================================

void processStringView(std::string_view sv, std::string_view label) {
    cout << "  - [" << left << setw(22) << label << "] View Size: " << setw(2) << sv.size() 
         << " | Content: \"" << sv << "\"\n";
}

// Demonstrating string parameter overhead comparison
void inspectViewAddress(std::string_view sv) {
    cout << "    View Address (Pointer): " << static_cast<const void*>(sv.data()) 
         << " | View Length: " << sv.length() << " chars\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userInput;

    // Dynamic input collection with stream flushing
    cout << "Enter a sample text string for std::string_view parsing (e.g.,   [HTTP/2.0 OK]   ): " << flush;
    getline(cin, userInput);

    if (userInput.empty()) {
        userInput = "   [HTTP/2.0 200 OK]   ";
        cout << "Empty input detected. Defaulting to: \"" << userInput << "\"\n";
    }

    // =====================================================================================
    // 1. CONSTRUCTION & ZERO-COST FLEXIBILITY
    // =====================================================================================
    cout << "\n================ 1. CONSTRUCTION & PARAMETER FLEXIBILITY ================\n";

    // Standard string source
    std::string_view viewFromStdString(userInput);

    // C-style literal source (No heap allocation)
    std::string_view viewFromLiteral("Embedded Literal Data");

    // Modern C++17 string_view literal suffix (""sv)
    auto viewFromSuffix = "Direct Suffix View"sv;

    processStringView(viewFromStdString, "From std::string");
    processStringView(viewFromLiteral, "From C-String Literal");
    processStringView(viewFromSuffix, "From ""sv Literal");

    // =====================================================================================
    // 2. O(1) ZERO-ALLOCATION SLICING (`remove_prefix`, `remove_suffix`, `substr`)
    // =====================================================================================
    cout << "\n================ 2. O(1) ZERO-ALLOCATION TRIMMING & SLICING ================\n";

    std::string_view sliceable = userInput;
    cout << "  - Original View Content   : \"" << sliceable << "\"\n";

    // Trimming leading whitespaces / characters in O(1) time
    while (!sliceable.empty() && (sliceable.front() == ' ' || sliceable.front() == '[')) {
        sliceable.remove_prefix(1); // Advances underlying pointer by 1 byte
    }
    cout << "  - After remove_prefix     : \"" << sliceable << "\"\n";

    // Trimming trailing whitespaces / characters in O(1) time
    while (!sliceable.empty() && (sliceable.back() == ' ' || sliceable.back() == ']')) {
        sliceable.remove_suffix(1); // Decrements internal length tracker by 1
    }
    cout << "  - After remove_suffix     : \"" << sliceable << "\"\n";

    // O(1) Substring creation (No string copy/heap allocation created!)
    std::string_view protocolSubstr = sliceable.substr(0, 6); // First 6 characters
    cout << "  - O(1) .substr(0, 6) View : \"" << protocolSubstr << "\"\n";

    // =====================================================================================
    // 3. POINTER INSPECTION & NULL-TERMINATION WARNING
    // =====================================================================================
    cout << "\n================ 3. POINTER INSPECTION & NULL-TERMINATION CAVEAT ================\n";

    std::string_view originalView(userInput);
    std::string_view slicedView = originalView.substr(2, 5);

    cout << "  - Original String Address : " << static_cast<const void*>(userInput.data()) << "\n";
    cout << "  - Sliced View Pointer Addr: " << static_cast<const void*>(slicedView.data()) << "\n";
    cout << "  - Proof of Non-Owning     : Sliced pointer is offset into original string memory (+2 bytes)!\n";
    cout << "  - [WARNING]: Sliced string_view `.data()` is NOT null-terminated! Do NOT pass sliced `.data()` directly to C-APIs expecting `const char*`.\n";

    // =====================================================================================
    // 4. LIFETIME DANGERS & DANGLING VIEWS
    // =====================================================================================
    cout << "\n================ 4. LIFETIME MANAGEMENT & DANGLING VIEWS ================\n";

    std::string_view danglingView;
    {
        std::string tempString = "Temporary Heap Data";
        danglingView = tempString; // Dangerous: danglingView views memory owned by tempString
    } // tempString destroyed HERE! Memory freed.

    cout << "  - [CRITICAL WARNING] Dangling View Risk: 'danglingView' points to deallocated memory!\n";
    cout << "  - Rule: Never return a `std::string_view` bound to a local temporary or function-stack variable.\n";

    // Converting string_view safely back to an owning std::string
    std::string safeOwningCopy(sliceable);
    cout << "  - Safe Conversion to Owning std::string: \"" << safeOwningCopy << "\"\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ STRING REPRESENTATIONS COMPARISON SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-------------------+-------------------+\n"
         << "| Type                  | Memory Ownership  | Parameter Cost    | Resizable?        | Null-Terminated?  |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+-------------------+\n"
         << "| `const char*`         | Non-Owning Pointer| O(1) Pointer Pass | NO                | YES (Requires '\\0')|\n"
         << "| `std::string`         | Owning Heap/SSO   | O(N) Copy         | YES               | YES               |\n"
         << "| `const std::string&`  | Non-Owning Ref    | O(1) Ref / O(N)*  | NO (Const)        | YES               |\n"
         << "  (*Forces temp heap alloc if passed C-String literal)\n"
         << "| `std::string_view`    | Non-Owning View   | O(1) Always       | NO (Can Trim View)| NOT GUARANTEED    |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+-------------------+\n";

    return 0;
}