/*
 * =====================================================================================
 * CONCEPT        : Modern Function Features in C++ (C++11 through C++20/C++23)
 * DESCRIPTION    : Comprehensive, multi-standard compliant modern C++ code showcasing:
 *                  1. Trailing Return Types & Auto Deduction (C++11/C++14).
 *                  2. Compile-Time Functions (`constexpr` & C++20 `consteval` with fallbacks).
 *                  3. Explicit Function Control (`= delete` - C++11).
 *                  4. Compile-Time Conditional Branching (`if constexpr` - C++17).
 *                  5. Constrained Functions via Concepts (C++20 with C++17 SFINAE fallback).
 *                  6. Structured Bindings with Multiple Return Values (C++17).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Compile-time evaluations eliminate runtime overhead.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <cstdint> // Fixes: Unknown type name 'uint64_t'

// =====================================================================================
// FEATURE-DETECTION MACROS (Ensures compatibility across C++14 / C++17 / C++20)
// =====================================================================================

// Check for C++20 Concepts support
#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
    #include <concepts>
    #define HAS_CPP20_CONCEPTS 1
#else
    #define HAS_CPP20_CONCEPTS 0
#endif

// Check for C++20 Consteval support
#if defined(__cpp_consteval) && __cpp_consteval >= 201811L
    #define HAS_CPP20_CONSTEVAL 1
#else
    #define HAS_CPP20_CONSTEVAL 0
#endif

using namespace std;

// =====================================================================================
// 1. TRAILING RETURN TYPES & RETURN TYPE DEDUCTION (C++11 / C++14)
// =====================================================================================

// C++11 Trailing Return Type
template <typename T, typename U>
auto multiplyTrailing(T a, U b) -> decltype(a * b) {
    return a * b;
}

// C++14 Return Type Deduction
template <typename T, typename U>
auto multiplyDeduced(T a, U b) {
    return a * b; // Return type automatically deduced by compiler
}

// =====================================================================================
// 2. COMPILE-TIME FUNCTIONS: constexpr (C++11) & consteval (C++20)
// =====================================================================================

// constexpr: Evaluated at compile-time if inputs are constants; otherwise at runtime
constexpr uint64_t constexprFactorial(unsigned int n) {
    return (n <= 1) ? 1 : (n * constexprFactorial(n - 1));
}

// C++20 consteval (Immediate Function) with constexpr fallback for pre-C++20 compilers
#if HAS_CPP20_CONSTEVAL
consteval uint64_t immediateSquare(uint64_t x) {
    return x * x;
}
#else
constexpr uint64_t immediateSquare(uint64_t x) {
    return x * x;
}
#endif

// =====================================================================================
// 3. EXPLICIT FUNCTION CONTROL: = delete (C++11)
// =====================================================================================

// Function accepting strictly integer types
void processIntegerOnly(int val) {
    cout << "  - [Int Only] Processing integer: " << val << endl;
}

// Deleted overload preventing implicit double-to-int conversions
void processIntegerOnly(double) = delete;

// =====================================================================================
// 4. COMPILE-TIME CONDITIONAL BRANCHING: if constexpr (C++17)
// =====================================================================================

template <typename T>
auto formatValue(T val) {
    if constexpr (is_same_v<T, string>) {
        return "String Literal: \"" + val + "\"";
    } else if constexpr (is_floating_point_v<T>) {
        return "Floating Point: " + to_string(val);
    } else {
        return "Integral Value: " + to_string(val);
    }
}

// =====================================================================================
// 5. CONSTRAINED FUNCTIONS (C++20 CONCEPTS / C++17 SFINAE FALLBACK)
// =====================================================================================

#if HAS_CPP20_CONCEPTS
// C++20 Concept Definition
template <typename T>
concept Arithmetic = is_arithmetic_v<T>;

// C++20 Abbreviated Function Template syntax
auto constrainedAdd(Arithmetic auto a, Arithmetic auto b) {
    return a + b;
}
#else
// Pre-C++20 Fallback using SFINAE (std::enable_if_t)
template <typename T, typename U, 
          typename = std::enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>>>
auto constrainedAdd(T a, U b) {
    return a + b;
}
#endif

// =====================================================================================
// 6. MULTIPLE RETURNS VIA STRUCTURED BINDINGS (C++17)
// =====================================================================================

auto getSystemStatus(int inputCode) -> tuple<int, string, bool> {
    if (inputCode >= 0) {
        return {200, "OK_SUCCESS", true};
    }
    return {500, "ERROR_NEGATIVE_CODE", false};
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userNum = 0;
    double userDouble = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter a small integer for compile-time factorial (e.g., 5): " << flush;
    if (!(cin >> userNum) || userNum < 0) {
        cout << "Invalid integer input. Defaulting to 5." << endl;
        userNum = 5;
    }

    cout << "Enter a floating-point number (e.g., 3.14): " << flush;
    if (!(cin >> userDouble)) {
        cout << "Invalid double input. Defaulting to 3.14." << endl;
        userDouble = 3.14;
    }

    // 1. TRAILING RETURN & DEDUCTION DEMO
    cout << "\n================ 1. TRAILING RETURN & AUTO DEDUCTION ================" << endl;
    auto res1 = multiplyTrailing(userNum, userDouble);
    auto res2 = multiplyDeduced(userNum, 10);
    cout << "  - multiplyTrailing(" << userNum << ", " << userDouble << ") = " << res1 << endl;
    cout << "  - multiplyDeduced(" << userNum << ", 10)       = " << res2 << endl;

    // 2. COMPILE-TIME EVALUATION DEMO
    cout << "\n================ 2. COMPILE-TIME EVALUATION (constexpr / consteval) ================" << endl;
    constexpr uint64_t compileTimeFact = constexprFactorial(5);
    cout << "  - Compile-Time constexprFactorial(5) = " << compileTimeFact << endl;

    uint64_t runtimeFact = constexprFactorial(static_cast<unsigned int>(userNum));
    cout << "  - Runtime constexprFactorial(" << userNum << ")     = " << runtimeFact << endl;

    constexpr uint64_t compileTimeSquare = immediateSquare(12);
    cout << "  - Immediate Square (12 * 12)          = " << compileTimeSquare << endl;

    // 3. DELETED FUNCTIONS DEMO
    cout << "\n================ 3. DELETED FUNCTIONS (= delete) ================" << endl;
    processIntegerOnly(userNum);
    // processIntegerOnly(3.14); // COMPILER ERROR: Attempting to call deleted function!
    cout << "  - processIntegerOnly(double) is deleted; prevents unintended implicit double conversions." << endl;

    // 4. IF CONSTEXPR DEMO
    cout << "\n================ 4. COMPILE-TIME BRANCHING (if constexpr) ================" << endl;
    cout << "  - " << formatValue(string("Modern C++")) << endl;
    cout << "  - " << formatValue(userDouble) << endl;
    cout << "  - " << formatValue(userNum) << endl;

    // 5. CONSTRAINED FUNCTIONS DEMO
    cout << "\n================ 5. CONSTRAINED FUNCTIONS (CONCEPTS / SFINAE) ================" << endl;
    auto conceptSum = constrainedAdd(userNum, 42);
    cout << "  - constrainedAdd(" << userNum << ", 42) = " << conceptSum << endl;

    // 6. STRUCTURED BINDINGS DEMO
    cout << "\n================ 6. STRUCTURED BINDINGS (C++17) ================" << endl;
    auto [statusCode, statusMsg, isSuccess] = getSystemStatus(userNum);
    cout << "  - Unpacked Tuple -> Code: " << statusCode 
         << " | Status: " << statusMsg 
         << " | Success: " << (isSuccess ? "YES" : "NO") << endl;

    cout << "\n================ MODERN FUNCTION FEATURES SUMMARY ================" << endl;
    cout << "1. Auto & Trailing : `auto func() -> Type` enables clear, dynamic return typing." << endl;
    cout << "2. Consteval/expr   : Shifts heavy computation from runtime to compile time." << endl;
    cout << "3. Deleted Functions: `= delete` strictly prevents unsafe type conversions." << endl;
    cout << "4. if constexpr     : Discards unused template branches at compile time." << endl;
    cout << "5. Concepts / SFINAE: Restricts template types cleanly without cryptic compiler errors." << endl;
    cout << "6. Structured Bind  : Cleanly unpacks multi-value returns without tuple boilerplate." << endl;

    return 0;
}