/*
 * =====================================================================================
 * CONCEPT        : `#pragma once` Preprocessor Directive in C++
 * DESCRIPTION    : Comprehensive implementation explaining `#pragma once`:
 *                  1. Purpose                  : A single-line preprocessor directive that guarantees
 *                                                a header file is included only once per translation unit.
 *                  2. Solves Double-Inclusion  : Prevents One Definition Rule (ODR) violations and
 *                                                compiler redefinition errors (e.g., duplicate structs/classes).
 *                  3. `#pragma once` vs Guards :
 *                     - Traditional Guards : `#ifndef HEADER_H` / `#define HEADER_H` ... `#endif`
 *                       (Requires unique macro names; prone to macro collisions or copy-paste typos).
 *                     - `#pragma once`     : Non-standard but supported by all modern compilers
 *                       (GCC, Clang, MSVC, ICC). Simpler, cleaner, and reduces preprocessor parsing overhead.
 *                  4. How Compilers Handle It : The compiler tracks file identity on disk; if a file
 *                                                marked with `#pragma once` is seen again, it is skipped.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime; compile-time preprocessing optimization.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

// =====================================================================================
// SIMULATION OF HEADER FILE 1: "EngineConfig.h" USING `#pragma once`
// In an actual multi-file project, the `#pragma once` directive is placed at the top line
// of the header file.
// =====================================================================================

// #pragma once  <-- Placed at line 1 of "EngineConfig.h"

// Struct definition that would cause a redefinition compiler error if included twice
struct EngineConfig {
    string engineName;
    int maxRpm;
    double horsepower;

    void displayDetails() const {
        cout << "  - Engine Name : " << engineName << "\n"
             << "  - Max RPM     : " << maxRpm << " RPM\n"
             << "  - Horsepower  : " << horsepower << " HP\n";
    }
};

// =====================================================================================
// SIMULATION OF SECOND INCLUSION (How `#pragma once` prevents redefinition)
// If `#pragma once` is present, the preprocessor ignores subsequent inclusions of "EngineConfig.h".
// Below, we simulate what WITHOUT `#pragma once` would cause:
// =====================================================================================

/*
 * UNGUARDED DUPLICATE INCLUSION SIMULATION:
 *
 * struct EngineConfig { ... }; // COMPILER FATAL ERROR: redefinition of 'struct EngineConfig'
 *
 * Because `#pragma once` tells the compiler: "Only process the tokens in this file ONCE per TU,"
 * the second include statement is safely turned into a no-op by the build system.
 */

// =====================================================================================
// COMPARISON: TRADITIONAL HEADER GUARDS VS `#pragma once`
// =====================================================================================

// Traditional Guard Syntax (Requires 3 lines + unique macro name):
#ifndef ENGINE_CONFIG_GUARD_H
#define ENGINE_CONFIG_GUARD_H

struct AlternativeConfig {
    int maxThreads;
};

#endif // ENGINE_CONFIG_GUARD_H

// Modern `#pragma once` Syntax:
// Just put `#pragma once` at the very top of your file. Zero macro boilerplate needed.

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string nameInput;
    int rpmInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter Engine Name (e.g., V8_TwinTurbo): " << flush;
    if (!(cin >> nameInput) || nameInput.empty()) {
        nameInput = "V8_TwinTurbo";
    }

    cout << "Enter Max RPM (e.g., 7200): " << flush;
    if (!(cin >> rpmInput) || rpmInput <= 0) {
        cout << "Invalid RPM input. Defaulting to 7200." << endl;
        rpmInput = 7200;
    }

    // 1. DEMONSTRATING GUARDED STRUCT EXECUTION
    cout << "\n================ 1. `#pragma once` PROTECTED STRUCT DEMO ================" << endl;
    
    EngineConfig v8Engine{nameInput, rpmInput, 650.5};
    v8Engine.displayDetails();

    cout << "  - Status: Struct successfully defined and instantiated without redefinition errors!\n";

    // 2. DETAILED COMPARISON TABLE
    cout << "\n================ 2. `#pragma once` VS TRADITIONAL GUARDS ================" << endl;
    cout << "+------------------------+-------------------------------+-------------------------------+\n"
         << "| Feature                | `#pragma once`                | Traditional `#ifndef` Guards  |\n"
         << "+------------------------+-------------------------------+-------------------------------+\n"
         << "| Lines of Code Required | 1 line at top of file         | 3 lines (#ifndef,#define,#endif)|\n"
         << "| Macro Name Collisions  | IMPOSSIBLE (No macros used)   | POSSIBLE if macro names match |\n"
         << "| Copy-Paste Bugs        | IMPOSSIBLE                    | LIKELY if developer forgets   |\n"
         << "                         |                               | to rename guard macro         |\n"
         << "| C++ Standard Status    | Non-standard (Vendor extension)| Official C++ Standard         |\n"
         << "| Compiler Support       | Supported by ALL major        | 100% universal across all     |\n"
         << "|                        | compilers (GCC, Clang, MSVC)  | compilers                     |\n"
         << "| Build Performance      | Faster (Compiler can skip     | Slightly slower (Preprocessor |\n"
         << "|                        | opening file on 2nd include)  | must re-open and parse macros)|\n"
         << "+------------------------+-------------------------------+-------------------------------+\n";

    // 3. KEY HIGHLIGHTS & EDGE CASES
    cout << "\n================ 3. `#pragma once` MECHANICS & EDGE CASES ================" << endl;
    cout << "  1. File-System Based : Compilers identify files using disk paths/inodes.\n";
    cout << "  2. Symlink Edge Case : If the exact same header is accessed via two different\n"
         << "                         symlinks, older compilers might treat them as two files\n"
         << "                         (Traditional `#ifndef` guards handle symlinks better).\n";
    cout << "  3. Industry Standard : Despite being non-standard, `#pragma once` is used in\n"
         << "                         most modern commercial C++ codebases due to safety & speed.\n";

    cout << "\n================ `#pragma once` SUMMARY ================" << endl;
    cout << "1. One-Line Guard    : Replaces verbose `#ifndef / #define / #endif` boilerplate." << endl;
    cout << "2. Prevents Redef     : Protects classes, structs, and enums from double-inclusion ODR bugs." << endl;
    cout << "3. Zero Macro Risk   : Eliminates bugs caused by accidental duplicate macro guard names." << endl;
    cout << "4. Modern Default    : Preferred for new C++ header files on standard modern toolchains." << endl;

    return 0;
}