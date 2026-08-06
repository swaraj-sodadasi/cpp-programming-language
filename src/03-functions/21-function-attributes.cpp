/*
 * =====================================================================================
 * CONCEPT        : Standard Function Attributes in C++ ([[nodiscard]], [[deprecated]], etc.)
 * DESCRIPTION    : Comprehensive implementation showcasing standard C++ function attributes:
 *                  1. [[nodiscard]] / [[nodiscard("reason")]] (C++17/C++20) :
 *                     - Issues compiler warnings if function return values are ignored.
 *                  2. [[deprecated]] / [[deprecated("reason")]] (C++14) :
 *                     - Flags outdated functions and guides developers to modern alternatives.
 *                  3. [[maybe_unused]] (C++17) :
 *                     - Suppresses compiler warnings for intentionally unused function parameters.
 *                  4. [[noreturn]] (C++11) :
 *                     - Informs compiler/optimizer that control flow never returns from this function.
 *                  5. [[likely]] / [[unlikely]] (C++20) :
 *                     - Branch prediction hints to assist compiler code layout optimization.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Function attributes are compile-time directives.
 * SPACE COMPLEXITY : Best Case: O(1) — Attributes introduce zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. [[nodiscard]] ATTRIBUTE (C++17 / C++20)
// Prevents caller from accidentally ignoring important return values (e.g., error codes, handles).
// =====================================================================================

// Standard C++17 nodiscard
[[nodiscard]] bool connectToServer(const string& endpoint) {
    if (endpoint.empty()) return false;
    return true; // Connection successful
}

// C++20 nodiscard with reason (with fallback for C++17 compilers)
#if defined(__has_cpp_attribute) && __has_cpp_attribute(nodiscard) >= 201904L
[[nodiscard("Ignoring error code can lead to silent transaction failures")]]
#else
[[nodiscard]]
#endif
int executeTransaction(double amount) {
    if (amount <= 0.0) return -1; // Invalid amount error code
    return 0; // Success
}

// =====================================================================================
// 2. [[deprecated]] ATTRIBUTE (C++14)
// Warns when calling legacy functions and suggests replacement API.
// =====================================================================================

[[deprecated("Use calculateTaxModern(double, double) instead for regional tax support.")]]
double calculateTaxLegacy(double amount) {
    return amount * 0.05; // Fixed 5% tax rate
}

double calculateTaxModern(double amount, double taxRate) {
    return amount * taxRate;
}

// =====================================================================================
// 3. [[maybe_unused]] ATTRIBUTE (C++17)
// Suppresses 'unused parameter' warnings when a parameter is needed for API signatures or debugging.
// =====================================================================================

void processDataLog(int recordId, [[maybe_unused]] bool debugVerboseMode) {
    // 'debugVerboseMode' is intentionally unused in release build; no compiler warning triggered
    cout << "  - [Log System] Processed record ID: " << recordId << endl;
}

// =====================================================================================
// 4. [[noreturn]] ATTRIBUTE (C++11)
// Informs compiler that function terminates execution via exception or std::exit/abort.
// =====================================================================================

[[noreturn]] void handleFatalError(const string& errorMessage) {
    cout << "  - [CRITICAL ERROR] " << errorMessage << endl;
    throw std::runtime_error("Fatal Error: " + errorMessage);
}

// =====================================================================================
// 5. [[likely]] / [[unlikely]] ATTRIBUTES (C++20)
// Optimization hints for branch prediction layout.
// =====================================================================================

bool validateInput(int value) {
    // Assume input is overwhelmingly valid in normal execution paths
    if (value > 0) 
#if defined(__has_cpp_attribute) && __has_cpp_attribute(likely)
    [[likely]]
#endif
    {
        return true;
    } 
    else 
#if defined(__has_cpp_attribute) && __has_cpp_attribute(unlikely)
    [[unlikely]]
#endif
    {
        return false;
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double userAmount = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter transaction amount (e.g., 150.00): " << flush;
    if (!(cin >> userAmount) || userAmount <= 0.0) {
        cout << "Invalid input amount. Defaulting to 150.00." << endl;
        userAmount = 150.00;
    }

    // 1. DEMONSTRATING [[nodiscard]]
    cout << "\n================ 1. [[nodiscard]] ATTRIBUTE ================" << endl;
    
    // Proper usage (capturing and checking return value):
    int status = executeTransaction(userAmount);
    if (status == 0) {
        cout << "  - Transaction of $" << fixed << setprecision(2) << userAmount << " succeeded." << endl;
    } else {
        cout << "  - Transaction failed with status code: " << status << endl;
    }

    bool connStatus = connectToServer("https://api.example.com");
    cout << "  - Server Connection Status: " << (connStatus ? "CONNECTED" : "FAILED") << endl;

    // NOTE: Calling `executeTransaction(userAmount);` without capturing the return value
    // causes the compiler to emit a warning: "ignoring return value of function declared with 'nodiscard'".

    // 2. DEMONSTRATING [[deprecated]]
    cout << "\n================ 2. [[deprecated]] ATTRIBUTE ================" << endl;
    
    // Calling modern non-deprecated API:
    double modernTax = calculateTaxModern(userAmount, 0.08);
    cout << "  - Modern Tax Calculation (8%): $" << modernTax << endl;

    // Calling legacy deprecated API (Triggers compiler warning during compilation):
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    double legacyTax = calculateTaxLegacy(userAmount);
    #pragma GCC diagnostic pop
    cout << "  - Legacy Tax Calculation (5%): $" << legacyTax << endl;

    // 3. DEMONSTRATING [[maybe_unused]]
    cout << "\n================ 3. [[maybe_unused]] ATTRIBUTE ================" << endl;
    processDataLog(1001, true); // Second argument is marked [[maybe_unused]] in parameter list

    // 4. DEMONSTRATING [[likely]] / [[unlikely]]
    cout << "\n================ 4. [[likely]] / [[unlikely]] ATTRIBUTES ================" << endl;
    bool isValid = validateInput(static_cast<int>(userAmount));
    cout << "  - Branch evaluation for amount " << userAmount << " -> Valid: " 
         << (isValid ? "YES" : "NO") << endl;

    // 5. DEMONSTRATING [[noreturn]]
    cout << "\n================ 5. [[noreturn]] ATTRIBUTE ================" << endl;
    cout << "  - Executing [[noreturn]] error handler inside try-catch block:" << endl;
    try {
        if (userAmount < 0) {
            handleFatalError("Negative transaction value encountered.");
        } else {
            cout << "  - No fatal error triggered. All attributes demonstrated successfully." << endl;
        }
    } catch (const std::exception& ex) {
        cout << "  - Caught exception from [[noreturn]] function: " << ex.what() << endl;
    }

    cout << "\n================ FUNCTION ATTRIBUTES SUMMARY ================" << endl;
    cout << "1. [[nodiscard]]    : Warns if return value is discarded (C++17/C++20)." << endl;
    cout << "2. [[deprecated]]   : Warns when using outdated APIs and suggests alternatives (C++14)." << endl;
    cout << "3. [[maybe_unused]] : Suppresses warnings for unused variables/parameters (C++17)." << endl;
    cout << "4. [[noreturn]]     : Informs compiler that function control never returns to caller (C++11)." << endl;
    cout << "5. [[likely/unlikely]]: Branch prediction hints for performance optimizations (C++20)." << endl;

    return 0;
}