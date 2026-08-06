/*
 * =====================================================================================
 * CONCEPT        : Translation Unit (TU) in C++
 * DESCRIPTION    : Comprehensive code and commentary detailing C++ Translation Units:
 *                  1. What is a Translation Unit? : A single source file (.cpp) combined 
 *                                                   with all headers (#include) after 
 *                                                   preprocessor expansion.
 *                  2. Preprocessing Stage         : Macro expansion, conditional compilation 
 *                                                   (#ifdef), and header insertion.
 *                  3. Internal Linkage            : Symbols isolated strictly inside the 
 *                                                   current TU (`static`, anonymous `namespace`).
 *                  4. External Linkage            : Symbols accessible across multiple TUs 
 *                                                   (`extern` declarations, non-static functions).
 *                  5. One Definition Rule (ODR)    : Guidelines for defining types, variables, 
 *                                                   and functions across single vs multiple TUs.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Symbol linkage and TU scopes are resolved at compile/link time.
 * SPACE COMPLEXITY : Best Case: O(1) — Zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

// =====================================================================================
// 1. PREPROCESSOR STAGE (INCLUDED IN THIS TRANSLATION UNIT)
// Before compilation, preprocessor macros are substituted directly into this TU.
// =====================================================================================

#define CURRENT_TU_NAME "TranslationUnit_Main.cpp"
#define COMPILER_BUILD_VERSION 2026

// =====================================================================================
// 2. INTERNAL LINKAGE (TRANSLATION UNIT PRIVACY)
// Symbols defined with `static` or inside an anonymous `namespace` have Internal Linkage.
// They are private to THIS Translation Unit and invisible to other .cpp files during linking.
// =====================================================================================

namespace {
    // Visible ONLY within this translation unit
    const string g_tuSecretKey = "TU_PRIVATE_AES_KEY_9981";

    void executeTULocalRoutine() {
        cout << "  - [Internal Linkage] Executing anonymous namespace routine inside " 
             << CURRENT_TU_NAME << endl;
    }
}

// Global static variable (Internal linkage - local copy per TU if defined in header)
static int g_tuLocalCounter = 100;

static void incrementTULocalCounter(int step) {
    g_tuLocalCounter += step;
    cout << "  - [Static Function] Local counter inside this TU updated to: " 
         << g_tuLocalCounter << endl;
}

// =====================================================================================
// 3. EXTERNAL LINKAGE (CROSS-TRANSLATION UNIT SHARING)
// Symbols defined without `static` or `namespace {}` have External Linkage.
// The linker can resolve references to these symbols from OTHER translation units.
// =====================================================================================

// Simulated external declaration (as if declared in another TU and linked here via 'extern')
extern int g_externalGlobalVariable; 
int g_externalGlobalVariable = 500; // Definition with External Linkage

void globalSharedFunction(int multiplier) {
    cout << "  - [External Linkage] Function accessible by other TUs during linking. Result: " 
         << (g_externalGlobalVariable * multiplier) << endl;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userMultiplier = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a multiplier for external linkage demonstration (e.g., 3): " << flush;
    if (!(cin >> userMultiplier) || userMultiplier <= 0) {
        cout << "Invalid input. Defaulting multiplier to 3." << endl;
        userMultiplier = 3;
    }

    // 1. TRANSLATION UNIT PREPROCESSING STAGE
    cout << "\n================ 1. PREPROCESSING & TU COMPILATION ================" << endl;
    cout << "  - Current Translation Unit Identifier : " << CURRENT_TU_NAME << endl;
    cout << "  - Compiler Target Build Version      : " << COMPILER_BUILD_VERSION << endl;
    cout << "  - Explanation: A Translation Unit (TU) is the complete text stream resulting\n"
         << "                 from processing a single source file (.cpp) plus all included\n"
         << "                 headers (#include) after macro substitution and conditional checks.\n";

    // 2. INTERNAL LINKAGE DEMONSTRATION
    cout << "\n================ 2. INTERNAL LINKAGE (TU PRIVACY) ================" << endl;
    cout << "  - Anonymous Namespace Secret Key      : " << g_tuSecretKey << endl;
    executeTULocalRoutine();

    cout << "  - Initial TU-static counter value     : " << g_tuLocalCounter << endl;
    incrementTULocalCounter(userMultiplier * 10);

    cout << "  - Insight: Symbols with Internal Linkage cannot cause duplicate symbol\n"
         << "             linker errors (ODR violations) even if defined in other TUs.\n";

    // 3. EXTERNAL LINKAGE DEMONSTRATION
    cout << "\n================ 3. EXTERNAL LINKAGE (CROSS-TU LINKING) ================" << endl;
    cout << "  - External Variable Initial Value     : " << g_externalGlobalVariable << endl;
    globalSharedFunction(userMultiplier);

    cout << "  - Insight: Symbols with External Linkage are exported to the symbol table.\n"
         << "             The Linker connects calls across TUs using these symbol names.\n";

    // 4. ONE DEFINITION RULE (ODR) PRINCIPLES
    cout << "\n================ 4. ONE DEFINITION RULE (ODR) IN TUs ================" << endl;
    cout << "  - Rule 1 (Within a Single TU): A variable, function, class, or enum can have\n"
         << "                                 at most ONE definition per Translation Unit.\n";
    cout << "  - Rule 2 (Across Entire Program): Non-inline functions and global variables\n"
         << "                                    must have EXACTLY ONE definition across all TUs.\n";
    cout << "  - Rule 3 (Inline Functions/Types): Classes, inline functions, and templates can\n"
         << "                                    be defined in multiple TUs provided definitions\n"
         << "                                    are identical token-for-token.\n";

    cout << "\n================ TRANSLATION UNIT SUMMARY ================" << endl;
    cout << "1. Source + Headers : TU = Source file (.cpp) + #included headers post-preprocessing." << endl;
    cout << "2. Independent Build: The C++ compiler compiles each TU independently into object files (.o/.obj)." << endl;
    cout << "3. Linker Phase     : The Linker joins object files together, matching external linkage symbols." << endl;
    cout << "4. Internal Linkage : `static` and `namespace {}` restrict symbol visibility exclusively to one TU." << endl;

    return 0;
}