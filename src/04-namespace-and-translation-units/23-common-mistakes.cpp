/*
 * =====================================================================================
 * CONCEPT        : Common Mistakes in Namespaces and Translation Units (TUs) in C++
 * DESCRIPTION    : Comprehensive program demonstrating common anti-patterns & solutions:
 *                  1. Mistake 1: `using namespace std;` in Headers / Global Scope
 *                     - Causes namespace pollution and silent symbol collision (e.g., `count`).
 *                  2. Mistake 2: Non-Inline Global Definitions in Headers (ODR Violation)
 *                     - Defining non-inline variables/functions in headers causes duplicate
 *                       symbol linker errors across multiple TUs. Fix: `inline` (C++17).
 *                  3. Mistake 3: Anonymous Namespaces in Header Files
 *                     - Anonymous namespaces in headers grant internal linkage, creating separate
 *                       isolated static instances in every TU that includes the header.
 *                  4. Mistake 4: Function Hijacking & ADL (Argument-Dependent Lookup) Surprises
 *                     - Placing generic helper functions in custom namespaces can inadvertently
 *                       hijack standard algorithm calls via ADL.
 *                  5. Mistake 5: Static Global Variables in Headers vs `inline`
 *                     - Using `static` in headers duplicates memory footprint per TU instead
 *                       of sharing a single global object.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime execution.
 * SPACE COMPLEXITY : Best Case: O(1) stack frame footprint.
 * =====================================================================================
 */

#include <iostream>
#include <algorithm>
#include <utility>

// =====================================================================================
// MISTAKE 1: NAMESPACE POLLUTION (`using namespace std;` IN HEADERS OR GLOBAL SCOPE)
// =====================================================================================

namespace BadPracticeExample {
    // DO NOT DO THIS IN HEADERS: using namespace std;
    // Supposing a header contained `using namespace std;`, local definitions clash!
    
    // Illustrating collision:
    int count = 42; // Clashes with std::count algorithm if `using namespace std;` is active!

    void demonstratePollutionFix() {
        // CORRECT: Explicit qualification avoids colliding with std::count
        std::cout << "  - [Mistake 1 Fix] Explicit qualification accesses local 'count': " 
                  << BadPracticeExample::count << "\n";
    }
}

// =====================================================================================
// MISTAKE 2: DEFINING NON-INLINE GLOBALS IN HEADERS (ODR VIOLATION)
// =====================================================================================

/*
 * ANTI-PATTERN IN A HEADER FILE ("Config.h"):
 * 
 * int g_appConfigValue = 100; // WRONG! Causes "multiple definition" Linker Error if included in 2+ TUs!
 * void printConfig() { ... }  // WRONG! Non-inline function causes Linker Error if included in 2+ TUs!
 */

// CORRECT FIX 1: Declaration with 'extern' in header, Definition in ONE .cpp file
extern int g_sharedAppConfig; 
int g_sharedAppConfig = 100; // Definition in single TU

// CORRECT FIX 2 (C++17): 'inline' variables and functions allow header definitions without ODR bugs
inline int g_inlineHeaderConfig = 200; 

inline void printInlineConfig() {
    std::cout << "  - [Mistake 2 Fix] Inline header function executed safely across TUs.\n";
}

// =====================================================================================
// MISTAKE 3: ANONYMOUS NAMESPACES IN HEADER FILES
// =====================================================================================

/*
 * ANTI-PATTERN IN A HEADER FILE ("HeaderWithAnon.h"):
 * 
 * namespace {
 *     int g_tuStateCounter = 0; // WRONG! Every .cpp file including this header gets a SEPARATE copy!
 * }                             // Modifying state in TU_A will NOT update state in TU_B!
 */

// CORRECT FIX: Anonymous namespaces belong EXCLUSIVELY inside source files (.cpp), never in headers!
namespace {
    // Perfectly valid inside this single .cpp file (Internal Linkage)
    int g_localSourceStateCounter = 0;

    void incrementLocalSourceState() {
        ++g_localSourceStateCounter;
    }
}

// =====================================================================================
// MISTAKE 4: ADL (ARGUMENT-DEPENDENT LOOKUP) HIJACKING & UNQUALIFIED CALLS
// =====================================================================================

namespace LegacySystem {

    struct CustomBuffer {
        int bufferId;
        std::size_t size;
    };

    // Helper swap function inside the type's namespace
    void swap(CustomBuffer& a, CustomBuffer& b) {
        std::cout << "  - [ADL Executed] LegacySystem::swap custom overload called!\n";
        std::swap(a.bufferId, b.bufferId);
        std::swap(a.size, b.size);
    }
}

void demonstrateAdlMechanism() {
    LegacySystem::CustomBuffer bufA{101, 2048};
    LegacySystem::CustomBuffer bufB{102, 4096};

    // ADL Best Practice: Two-step swap pattern
    using std::swap; // Fallback to std::swap for primitive/standard types
    swap(bufA, bufB); // Calls LegacySystem::swap via ADL automatically!
}

// =====================================================================================
// MISTAKE 5: `static` GLOBALS IN HEADERS VS `inline` (MEMORY WASTAGE)
// =====================================================================================

/*
 * ANTI-PATTERN IN A HEADER FILE ("Constants.h"):
 * 
 * static const std::string G_APP_NAME = "MyEnterpriseApp"; // WRONG!
 * // Creates a duplicate copy of G_APP_NAME string object in every single .cpp file!
 */

// CORRECT FIX (C++17): Use 'inline constexpr' for shared zero-overhead header constants
inline constexpr std::string_view G_CORRECT_APP_NAME = "MyEnterpriseApp_v2026";

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userConfigInput = 0;

    // Dynamic input collection with stream flushing
    std::cout << "Enter a custom application configuration value (e.g., 350): " << std::flush;
    if (!(std::cin >> userConfigInput) || userConfigInput <= 0) {
        std::cout << "Invalid input. Defaulting config value to 350." << std::endl;
        userConfigInput = 350;
    }

    g_sharedAppConfig = userConfigInput;

    // 1. MISTAKE 1: NAMESPACE POLLUTION
    std::cout << "\n================ 1. NAMESPACE POLLUTION & SYMBOL CLASHES ================\n";
    BadPracticeExample::demonstratePollutionFix();
    std::cout << "  - Lesson: Never place 'using namespace std;' in headers or global scope.\n";

    // 2. MISTAKE 2: ODR VIOLATIONS IN HEADERS
    std::cout << "\n================ 2. ODR VIOLATIONS & HEADER DEFINITIONS ================\n";
    std::cout << "  - External Shared Config Value : " << g_sharedAppConfig << "\n";
    std::cout << "  - C++17 Inline Header Variable : " << g_inlineHeaderConfig << "\n";
    printInlineConfig();
    std::cout << "  - Lesson: Use 'extern' for variables or 'inline' (C++17) for header definitions.\n";

    // 3. MISTAKE 3: ANONYMOUS NAMESPACES IN HEADERS
    std::cout << "\n================ 3. ANONYMOUS NAMESPACE SCOPING ================\n";
    incrementLocalSourceState();
    std::cout << "  - Local TU State Counter : " << g_localSourceStateCounter << "\n";
    std::cout << "  - Lesson: Anonymous namespaces belong ONLY in .cpp files, never in headers.\n";

    // 4. MISTAKE 4: ADL & SWAP PATTERN
    std::cout << "\n================ 4. ADL & FUNCTION HIJACKING ================\n";
    demonstrateAdlMechanism();
    std::cout << "  - Lesson: Keep custom type operators/helpers in the type's namespace for safe ADL.\n";

    // 5. MISTAKE 5: HEADER MEMORY BLOAT
    std::cout << "\n================ 5. HEADER CONSTANTS & MEMORY BLOAT ================\n";
    std::cout << "  - C++17 Inline Constant View : " << G_CORRECT_APP_NAME << "\n";
    std::cout << "  - Lesson: Prefer 'inline constexpr' over 'static' for header constants.\n";

    // SUMMARY TABLE
    std::cout << "\n================ COMMON MISTAKES QUICK REFERENCE TABLE ================\n";
    std::cout << "+---------------------------------+-----------------------------------+-----------------------------------+\n"
              << "| Bad Practice / Anti-Pattern     | Fatal Bug / Negative Consequence  | Modern Corrective Solution        |\n"
              << "+---------------------------------+-----------------------------------+-----------------------------------+\n"
              << "| `using namespace std;` in .h    | Global symbol collisions          | Fully qualify `std::` explicitly  |\n"
              << "| Non-inline global var in .h     | ODR Linker Error (Duplicate Sym)  | Use `extern` or `inline` (C++17)  |\n"
              << "| Anonymous namespace in .h       | Creates split instances per TU    | Move anonymous namespace to .cpp  |\n"
              << "| `static` constant obj in .h     | Memory bloat (Duplicates per TU)  | Use `inline constexpr` (C++17)    |\n"
              << "| Hidden helper in global scope   | Accidental ADL function hijacking | Enclose helpers in custom sub-ns  |\n"
              << "+---------------------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}