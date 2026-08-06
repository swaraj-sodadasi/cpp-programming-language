/*
 * =====================================================================================
 * CONCEPT        : Linkage in C++ (Internal, External, No Linkage, and Language Linkage)
 * DESCRIPTION    : Comprehensive program explaining symbol linkage types in C++:
 *                  1. No Linkage       : Local variables, block-scoped symbols, function 
 *                                        parameters. Accessible strictly within block scope.
 *                  2. Internal Linkage  : Symbols visible ONLY within the current Translation 
 *                                        Unit (TU). Achieved via `static` global scope, 
 *                                        anonymous namespaces `namespace { ... }`, or 
 *                                        file-scope `const`/`constexpr` variables.
 *                  3. External Linkage  : Symbols exported to the object file's symbol table, 
 *                                        accessible across multiple TUs during linking 
 *                                        (e.g., `extern` variables, regular global functions).
 *                  4. Language Linkage  : Directing the compiler on symbol name mangling rules 
 *                                        (e.g., `extern "C"` for C/C++ interop/ABI compliance).
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime; linkage resolution happens at compile/link time.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

// =====================================================================================
// 1. INTERNAL LINKAGE (FILE / TRANSLATION UNIT SCOPE PRIVACY)
// Symbols with internal linkage cannot be referenced or linked from other .cpp files.
// =====================================================================================

// A. Global static variable -> Internal Linkage (Local to this TU)
static int g_tuLocalCounter = 100;

// B. Anonymous/Unnamed Namespace -> Internal Linkage for all members
namespace {
    const string g_tuSecretKey = "TU_PRIVATE_AES_KEY_8832";

    void executeInternalRoutine() {
        cout << "  - [Anonymous Namespace] Executed private TU routine. Key: " 
             << g_tuSecretKey << endl;
    }
}

// C. File-scope const variable -> Defaults to Internal Linkage in C++ (unlike C!)
const double g_tuLocalConstant = 3.14159;

// Global static function -> Internal Linkage
static void incrementInternalCounter(int delta) {
    g_tuLocalCounter += delta;
    cout << "  - [Static Function] Internal counter updated to: " << g_tuLocalCounter << endl;
}

// =====================================================================================
// 2. EXTERNAL LINKAGE (CROSS-TRANSLATION UNIT SHARING)
// Symbols exported to symbol table; visible to other TUs via matching declarations.
// =====================================================================================

// Declaring an external variable (Definition provided below or in another TU)
extern int g_sharedProgramState; 

// Definition of global variable with External Linkage
int g_sharedProgramState = 500;

// Regular non-static global function -> External Linkage
void executeSharedGlobalFunction(int factor) {
    cout << "  - [External Function] Executed cross-TU function. State * Factor = " 
         << (g_sharedProgramState * factor) << endl;
}

// =====================================================================================
// 3. LANGUAGE LINKAGE (C vs C++ ABI & NAME MANGLING)
// Prevents C++ name mangling for C compatibility or DLL/so dynamic export symbols.
// =====================================================================================

extern "C" {
    // Suppresses C++ function name overloading/mangling; exports symbol cleanly as "c_compatible_api"
    void c_compatible_api(int id) {
        cout << "  - [extern \"C\"] Called unmangled C-compatible function with ID: " << id << endl;
    }
}

// =====================================================================================
// 4. NO LINKAGE (SCOPED / BLOCK LOCAL SYMBOLS)
// Local variables, parameter names, and local classes have NO linkage.
// =====================================================================================

void demonstrateNoLinkage(int paramValue) { // 'paramValue' has NO LINKAGE
    int localScopeVar = paramValue * 10;   // 'localScopeVar' has NO LINKAGE

    {
        int nestedBlockVar = localScopeVar + 5; // 'nestedBlockVar' has NO LINKAGE
        cout << "  - [No Linkage] Block local variable address: " << &nestedBlockVar 
             << " | Value: " << nestedBlockVar << endl;
    } // 'nestedBlockVar' goes out of scope here; unknown to rest of program
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int inputFactor = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a scaling factor for linkage demonstration (e.g., 4): " << flush;
    if (!(cin >> inputFactor) || inputFactor <= 0) {
        cout << "Invalid input factor. Defaulting to 4." << endl;
        inputFactor = 4;
    }

    // 1. NO LINKAGE DEMONSTRATION
    cout << "\n================ 1. NO LINKAGE (BLOCK SCOPE) ================" << endl;
    cout << "  - Symbols exist strictly within local block scopes. Inaccessible outside.\n";
    demonstrateNoLinkage(inputFactor);

    // 2. INTERNAL LINKAGE DEMONSTRATION
    cout << "\n================ 2. INTERNAL LINKAGE (TU PRIVACY) ================" << endl;
    cout << "  - Symbols restricted to THIS translation unit. Linker ignores them outside.\n";
    cout << "  - File-scope static variable : g_tuLocalCounter = " << g_tuLocalCounter << endl;
    incrementInternalCounter(inputFactor * 5);
    executeInternalRoutine();

    // 3. EXTERNAL LINKAGE DEMONSTRATION
    cout << "\n================ 3. EXTERNAL LINKAGE (CROSS-TU VISIBILITY) ================" << endl;
    cout << "  - Symbols exposed in the global symbol table for other object files.\n";
    cout << "  - Global variable            : g_sharedProgramState = " << g_sharedProgramState << endl;
    executeSharedGlobalFunction(inputFactor);

    // 4. LANGUAGE LINKAGE DEMONSTRATION
    cout << "\n================ 4. LANGUAGE LINKAGE (extern \"C\") ================" << endl;
    cout << "  - Prevents C++ name mangling to enable C language interop / C ABI compatibility.\n";
    c_compatible_api(1001 + inputFactor);

    // 5. SUMMARY COMPARISON
    cout << "\n================ LINKAGE TYPES SUMMARY ================" << endl;
    cout << "+-------------------+-----------------------------------+-----------------------------------+\n"
         << "| Linkage Type      | Scope / Lifetime                  | Mechanism / Syntax                |\n"
         << "+-------------------+-----------------------------------+-----------------------------------+\n"
         << "| No Linkage        | Block Scope / Stack Lifetime      | Local variables, function params  |\n"
         << "| Internal Linkage  | Translation Unit Scope / Static   | `static` globals, `namespace {}`  |\n"
         << "| External Linkage  | Whole Program Scope / Static      | Non-static globals, `extern`      |\n"
         << "| Language Linkage  | Whole Program Scope (Unmangled)   | `extern \"C\" { ... }`              |\n"
         << "+-------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}