/*
 * =====================================================================================
 * CONCEPT        : Legacy Exception Specifications in Modern C++ (C++98 to C++20)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code detailing
 *                  the history, mechanics, deprecation, removal, and modern migration
 *                  of C++ Dynamic Exception Specifications (`throw(...)` and `throw()`):
 *
 *                  1. C++98 Dynamic Exception Specifications (`throw(T1, T2)`):
 *                     - Allowed functions to specify the exact list of exception types 
 *                       they were permitted to throw (e.g., `void foo() throw(int, std::runtime_error);`).
 *                     - Checked at RUNTIME, NOT at compile time!
 *                     - If a function threw an unlisted exception, the C++ runtime called `std::unexpected()`.
 *                     - Default `std::unexpected()` called `std::terminate()`, or converted the exception 
 *                       to `std::bad_exception` if `std::bad_exception` was listed in the specification.
 *
 *                  2. Why Dynamic Exception Specifications Failed:
 *                     - Runtime Overhead: Forced compilers to generate unseen try-catch wrappers around 
 *                       function bodies, increasing code size and slowing down execution.
 *                     - Incompatibility with Templates: Generic template functions could not know 
 *                       what exceptions underlying template arguments might throw.
 *                     - False Security: Did not prevent unlisted exceptions from being compiled; 
 *                       only punished them at runtime with crashes (`std::terminate`).
 *
 *                  3. Evolution Across Standards:
 *                     - C++98 : Introduced dynamic `throw(Type1, Type2)` and empty `throw()`.
 *                     - C++11 : Deprecated dynamic `throw(Type1, Type2)`. Introduced `noexcept`.
 *                     - C++17 : Removed dynamic `throw(Type1, Type2)`. Kept empty `throw()` as alias for `noexcept`.
 *                     - C++20 : Completely removed dynamic exception specifications and `throw()` syntax.
 *
 *                  4. Modern Replacements (C++11/17/20/23+):
 *                     - Replace `throw()` with `noexcept` or `noexcept(true)`.
 *                     - Replace `throw(T1, T2)` with standard exception handling, documentation,
 *                       or type-safe error wrappers like `std::expected` (C++23) / `std::optional`.
 *
 * TIME COMPLEXITY  : Legacy Dynamic Checking : O(Stack Unwind + Exception Search Overhead).
 *                    Modern `noexcept` Path  : O(1) Zero-cost exception dispatch setup.
 * SPACE COMPLEXITY : Modern Footprint       : 0 bytes extra runtime overhead.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>

using namespace std;

// =====================================================================================
// 1. LEGACY SYNTAX MIGRATION DEMONSTRATOR
// Demonstrates modern equivalents of legacy exception specification patterns.
// =====================================================================================
class LegacyExceptionSpecDemo {
public:
    // ---------------------------------------------------------------------------------
    // LEGACY PATTERN 1: Empty Exception Specification `throw()`
    // C++98 Syntax : void legacyNothrow() throw();
    // C++11/17+    : Marked `noexcept` or `noexcept(true)`
    // ---------------------------------------------------------------------------------
    static void modernNothrowFunction() noexcept {
        cout << "    [Modern Nothrow] Function executing safely under `noexcept` specifier.\n";
    }

    // ---------------------------------------------------------------------------------
    // LEGACY PATTERN 2: Dynamic Exception Specification `throw(std::out_of_range, std::invalid_argument)`
    // C++98 Syntax : void legacyRestrictedThrow(int value) throw(std::out_of_range, std::invalid_argument);
    // Modern C++   : Standard function declaration + try/catch at caller or internal error translation
    // ---------------------------------------------------------------------------------
    static void modernRestrictedThrowFunction(int scenarioCode) {
        if (scenarioCode == 1) {
            cout << "    [Restricted Throw] Throwing permitted `std::invalid_argument`...\n";
            throw std::invalid_argument("Invalid scenario parameter provided!");
        } else if (scenarioCode == 2) {
            cout << "    [Restricted Throw] Throwing permitted `std::out_of_range`...\n";
            throw std::out_of_range("Scenario parameter out of bounds!");
        } else if (scenarioCode == 3) {
            cout << "    [Restricted Throw] Throwing unlisted `std::runtime_error`...\n";
            throw std::runtime_error("Unexpected runtime operational error!");
        }
        cout << "    [Restricted Throw] Function executed without throwing.\n";
    }
};

// =====================================================================================
// 2. DEMONSTRATING `std::bad_exception` (REMNANT OF LEGACY EXCEPTION HANDLING)
// `std::bad_exception` was thrown when an unexpected exception occurred under dynamic throw specs.
// =====================================================================================
class BadExceptionDemo {
public:
    static void demonstrateBadExceptionUsage() {
        try {
            cout << "    [BadException Demo] Explicitly throwing `std::bad_exception`...\n";
            throw std::bad_exception();
        } catch (const std::bad_exception& ex) {
            cout << "    [CAUGHT `std::bad_exception`]: \"" << ex.what() << "\"\n";
            cout << "    (Historically thrown when std::unexpected handler translated unlisted exceptions)\n";
        }
    }
};

// =====================================================================================
// 3. MODERN ARCHITECTURAL PATTERN: TYPE-SAFE ERROR MIGRATION
// Replacing dynamic exception specifications with compile-time checked alternatives.
// =====================================================================================
template <typename T>
struct ExpectedResult {
    T value{};
    string errorMessage{};
    bool hasValue{false};

    static ExpectedResult success(T val) {
        return ExpectedResult{std::move(val), "", true};
    }

    static ExpectedResult failure(string err) {
        return ExpectedResult{T{}, std::move(err), false};
    }
};

class ModernErrorHandlingDemo {
public:
    // Type-safe replacement for `throw(std::invalid_argument)`
    static ExpectedResult<int> processInputTypeSafe(int inputVal) noexcept {
        if (inputVal <= 0) {
            return ExpectedResult<int>::failure("Input value must be strictly positive!");
        }
        return ExpectedResult<int>::success(inputVal * 2);
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Legacy Exception Specification analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. LEGACY `throw()` MIGRATION TO `noexcept`
    // =====================================================================================
    cout << "\n================ 1. LEGACY `throw()` vs MODERN `noexcept` ================\n";

    cout << "  - Calling modern equivalent of legacy `throw()`:\n";
    LegacyExceptionSpecDemo::modernNothrowFunction();
    cout << "  - `noexcept` compile-time evaluation: " 
         << (noexcept(LegacyExceptionSpecDemo::modernNothrowFunction()) ? "TRUE (guaranteed non-throwing)" : "FALSE") << "\n";

    // =====================================================================================
    // 2. MIGRATING DYNAMIC `throw(T1, T2)` TO STANDARD EXCEPTION DISPATCH
    // =====================================================================================
    cout << "\n================ 2. DYNAMIC SPECIFICATION MIGRATION ================\n";

    // Permitted Scenario 1
    try {
        cout << "  - Scenario 1 (Permitted invalid_argument):\n";
        LegacyExceptionSpecDemo::modernRestrictedThrowFunction(1);
    } catch (const std::invalid_argument& ex) {
        cout << "    * [CAUGHT EXPECTED EXCEPTION]: " << ex.what() << "\n";
    }

    // Permitted Scenario 2
    try {
        cout << "\n  - Scenario 2 (Permitted out_of_range):\n";
        LegacyExceptionSpecDemo::modernRestrictedThrowFunction(2);
    } catch (const std::out_of_range& ex) {
        cout << "    * [CAUGHT EXPECTED EXCEPTION]: " << ex.what() << "\n";
    }

    // Historical Unlisted Scenario 3
    try {
        cout << "\n  - Scenario 3 (Unlisted runtime_error - Historically invoked std::unexpected):\n";
        LegacyExceptionSpecDemo::modernRestrictedThrowFunction(3);
    } catch (const std::runtime_error& ex) {
        cout << "    * [CAUGHT UNLISTED EXCEPTION IN MODERN C++]: " << ex.what() << "\n";
        cout << "      (In C++98, if unlisted in throw(...), this would have invoked std::unexpected()!)\n";
    }

    // =====================================================================================
    // 3. HISTORICAL `std::bad_exception` EXPLOITATION
    // =====================================================================================
    cout << "\n================ 3. HISTORICAL `std::bad_exception` ================\n";

    BadExceptionDemo::demonstrateBadExceptionUsage();

    // =====================================================================================
    // 4. MODERN ALTERNATIVE: TYPE-SAFE COMPILE-TIME ERROR RETURN
    // =====================================================================================
    cout << "\n================ 4. MODERN ALTERNATIVE: TYPE-SAFE RETURN VALUES ================\n";

    cout << "  - Testing `processInputTypeSafe(" << userInputValue << ")`...\n";
    auto validRes = ModernErrorHandlingDemo::processInputTypeSafe(userInputValue);
    if (validRes.hasValue) {
        cout << "    * Success Result: " << validRes.value << "\n";
    }

    cout << "  - Testing `processInputTypeSafe(-50)`...\n";
    auto invalidRes = ModernErrorHandlingDemo::processInputTypeSafe(-50);
    if (!invalidRes.hasValue) {
        cout << "    * Error Handled Without Exceptions: \"" << invalidRes.errorMessage << "\"\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ LEGACY EXCEPTION SPECIFICATIONS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Feature / Syntax      | C++98 Standard Status | C++11/17/20 Status    | Modern Replacement & Behavior     |\n"
         << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n"
         << "| `throw(Type1, Type2)` | Standard (Runtime)    | Removed in C++17      | Standard Exception Handling / Docs|\n"
         << "| `throw()`             | Standard (Nothrow)    | Removed in C++20      | `noexcept` / `noexcept(true)`     |\n"
         << "| `std::unexpected()`   | Standard Handler      | Removed in C++17      | `std::terminate()` on `noexcept`  |\n"
         << "| `std::set_unexpected` | Standard Function     | Removed in C++17      | N/A (Use custom exception handlers|\n"
         << "| `std::bad_exception`  | Standard Exception    | Retained in `<ex>`    | Retained for backward compat      |\n"
         << "| Performance Impact    | Runtime Try-Catch Wrap| Zero Overhead (`noex`)| Optimizes register and frame layout|\n"
         << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n";

    return 0;
}