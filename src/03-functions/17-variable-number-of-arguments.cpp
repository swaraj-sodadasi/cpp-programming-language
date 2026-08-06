/*
 * =====================================================================================
 * CONCEPT        : Variable Number of Arguments in C++
 * DESCRIPTION    : Comprehensive implementation demonstrating variable argument techniques:
 *                  1. C-Style Variadic Functions (`<cstdarg>`) :
 *                     - Uses `va_list`, `va_start`, `va_arg`, and `va_end`.
 *                     - Historical C mechanism; type-unsafe and requires explicit count or format string.
 *                  2. C++11 Variadic Templates (Recursive Expansion) :
 *                     - Uses template parameter packs (`typename... Args`).
 *                     - Type-safe; uses function recursion and base cases to unpack arguments.
 *                  3. C++17 Fold Expressions :
 *                     - Modern C++ reduction syntax for parameter packs without manual recursion.
 *                  4. Homogeneous Alternative (`std::initializer_list`) :
 *                     - Safe, type-enforced variable arguments when all elements share the same type.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Traverses N variable arguments linearly.
 * SPACE COMPLEXITY : Best Case: O(1) for C-style/folds/initializer_list; O(N) recursive stack depth for un-inlined templates.
 * =====================================================================================
 */

#include <iostream>
#include <cstdarg>
#include <initializer_list>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. C-STYLE VARIADIC FUNCTIONS (<cstdarg>)
// =====================================================================================

/**
 * @brief Computes average of 'count' double arguments using C-style va_list.
 * @note Type-unsafe: requires exact count and matching caller types (promoted double).
 */
double calculateAverageCStyle(int count, ...) {
    if (count <= 0) return 0.0;

    va_list args;
    va_start(args, count); // Initialize args to point to arguments following 'count'

    double sum = 0.0;
    for (int i = 0; i < count; ++i) {
        // Retrieve next argument of type double
        sum += va_arg(args, double);
    }

    va_end(args); // Clean up va_list resources
    return sum / static_cast<double>(count);
}

// =====================================================================================
// 2. C++11 VARIADIC TEMPLATES (RECURSIVE PACK EXPANSION)
// =====================================================================================

// Base Case: Terminates recursive expansion when no arguments remain
void printArgsRecursive() {
    cout << endl;
}

// Recursive Case: Extracts the head parameter and unpacks the remaining tail pack
template <typename T, typename... Args>
void printArgsRecursive(T first, Args... rest) {
    cout << first << " ";
    printArgsRecursive(rest...); // Recursive call passing remaining pack
}

// =====================================================================================
// 3. C++17 FOLD EXPRESSIONS (MODERN VARIADIC TEMPLATES)
// =====================================================================================

/**
 * @brief Sums all variadic arguments using C++17 unary left fold `(... + args)`.
 */
template <typename... Args>
auto sumAllFold(Args... args) {
    return (... + args); // Fold expansion: ((arg1 + arg2) + arg3)...
}

/**
 * @brief Prints all arguments using C++17 binary fold expression over comma operator.
 */
template <typename... Args>
void printAllFold(Args... args) {
    cout << "  - [C++17 Fold Output] : ";
    ((cout << args << " | "), ...); // Fold expression over comma operator
    cout << endl;
}

// =====================================================================================
// 4. HOMOGENEOUS VARIABLE ARGUMENTS (std::initializer_list)
// =====================================================================================

/**
 * @brief Computes total sum of an arbitrary count of integer arguments of the SAME type.
 */
int sumInitializerList(initializer_list<int> values) {
    int total = 0;
    for (int val : values) {
        total += val;
    }
    return total;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int countInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a sample count for C-style average calculation (e.g., 3): " << flush;
    if (!(cin >> countInput) || countInput <= 0) {
        cout << "Invalid input. Defaulting count to 3." << endl;
        countInput = 3;
    }

    // 1. C-STYLE VARIADIC FUNCTIONS
    cout << "\n================ 1. C-STYLE VARIADIC FUNCTIONS (<cstdarg>) ================" << endl;
    cout << "  - Calling calculateAverageCStyle(3, 10.5, 20.5, 30.0):" << endl;
    double avgC = calculateAverageCStyle(3, 10.5, 20.5, 30.0);
    cout << "  - Computed C-Style Average = " << fixed << setprecision(2) << avgC << endl;
    cout << "  - [Warning] C-style variadics lack type-checking and require explicit counts/format strings." << endl;

    // 2. C++11 VARIADIC TEMPLATES (TYPE-SAFE RECURSION)
    cout << "\n================ 2. C++11 VARIADIC TEMPLATES (RECURSIVE) ================" << endl;
    cout << "  - Unpacking recursive arguments (int, double, string, char):" << endl;
    cout << "  - Output : ";
    printArgsRecursive(42, 3.14159, "Variadic", 'A');

    // 3. C++17 FOLD EXPRESSIONS
    cout << "\n================ 3. C++17 FOLD EXPRESSIONS ================" << endl;
    auto foldSum = sumAllFold(10, 20, 30, 40, 50);
    cout << "  - sumAllFold(10, 20, 30, 40, 50) = " << foldSum << endl;
    
    printAllFold("C++17", 100, 99.9, "Zero Boilerplate");

    // 4. HOMOGENEOUS VARIABLE ARGUMENTS (std::initializer_list)
    cout << "\n================ 4. HOMOGENEOUS std::initializer_list ================" << endl;
    int initListSum = sumInitializerList({1, 2, 3, 4, 5, 6, 7});
    cout << "  - sumInitializerList({1, 2, 3, 4, 5, 6, 7}) = " << initListSum << endl;

    cout << "\n================ VARIABLE ARGUMENTS SUMMARY ================" << endl;
    cout << "1. C-Style (`<cstdarg>`)     : Unsafe; avoid in modern C++ unless interfacing with legacy C APIs." << endl;
    cout << "2. Variadic Templates (C++11): Fully type-safe; handles mixed types via recursive pack expansion." << endl;
    cout << "3. Fold Expressions (C++17)  : Preferred approach for variadic templates; clean and concise syntax." << endl;
    cout << "4. `std::initializer_list`  : Best when passing variable numbers of elements of the SAME data type." << endl;

    return 0;
}