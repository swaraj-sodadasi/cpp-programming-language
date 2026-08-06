/*
 * =====================================================================================
 * CONCEPT        : Return Types in C++ (Void, Value, Reference, Tuple, Optional, Trailing)
 * DESCRIPTION    : Comprehensive implementation detailing C++ function return types:
 *                  1. Void Return Type       : Functions returning no value ('void'), using 'return;'
 *                                              for early exits.
 *                  2. Return by Value        : Returning prvalues (scalars, objects, std::vector)
 *                                              benefiting from Return Value Optimization (RVO / NRVO).
 *                  3. Reference Return Types : Returning lvalue references (T&) for in-place
 *                                              modification and 'const T&' for zero-copy read-only access.
 *                  4. Multiple Return Values : Returning compound structures (std::pair, std::tuple)
 *                                              unpacked via C++17 structured bindings.
 *                  5. Optional Return Type   : Using 'std::optional<T>' (C++17) to signal valid
 *                                              vs empty/failed results safely without pointers.
 *                  6. Auto & Trailing Syntax : Type deduction ('auto') and modern trailing return
 *                                              signatures ('auto func() -> Type').
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant-time scalar calculations and reference passing.
 * SPACE COMPLEXITY : Best Case: O(1) — Stack frame allocations optimized via RVO/NRVO.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <optional>
#include <iomanip>
#include <cmath>

using namespace std;

// =====================================================================================
// FUNCTION DECLARATIONS & DEMONSTRATION HELPERS
// =====================================================================================

/**
 * 1. VOID RETURN TYPE ('void')
 * Signals that the function performs side effects and returns no data value to the caller.
 */
void processVoidOperation(int inputValue) {
    cout << "  [void return] Validating input (" << inputValue << ")..." << endl;
    if (inputValue < 0) {
        cout << "  [void return] Negative input detected! Exiting early via 'return;'." << endl;
        return; // Early exit in void function
    }
    cout << "  [void return] Operation executed successfully for positive input." << endl;
}

/**
 * 2. RETURN BY VALUE (Primitives & Objects)
 * Returns a new object by value. Modern C++ compilers eliminate copy overhead 
 * via Named Return Value Optimization (NRVO).
 */
double computeCompoundInterest(double principal, double rate, int years) {
    double result = principal * std::pow(1.0 + rate, years);
    return result; // Value return (RVO/NRVO applies)
}

/**
 * 3A. CONST REFERENCE RETURN ('const T&')
 * Returns a read-only reference to an existing object, avoiding expensive copies.
 * (Target lifetime MUST outlive the function call!).
 */
const string& selectLongerString(const string& strA, const string& strB) {
    // Safe: Returns a reference to one of the input strings owned by caller
    return (strA.length() >= strB.length()) ? strA : strB;
}

/**
 * 3B. NON-CONST REFERENCE RETURN ('T&')
 * Returns a modifiable lvalue reference, allowing caller to mutate container elements directly.
 */
int& getElementReference(vector<int>& dataContainer, size_t index) {
    // Safe: Data container is owned by caller and outlives function call
    return dataContainer[index];
}

/**
 * 4. MULTIPLE RETURN VALUES (std::tuple / C++17 Structured Bindings)
 * Bundles multiple distinct return values into a single compound return type.
 */
tuple<int, double, string> GenerateDataSummary(int count, double multiplier) {
    int totalCount = count * 2;
    double scaledValue = multiplier * 3.14159;
    string status = (count > 0) ? "ACTIVE" : "INACTIVE";

    return {totalCount, scaledValue, status}; // Returns tuple
}

/**
 * 5. OPTIONAL RETURN TYPE ('std::optional<T>')
 * Represents a value that may or may not exist (replaces null pointers / error codes).
 */
optional<double> safeSquareRoot(double value) {
    if (value < 0.0) {
        return nullopt; // Returns empty optional state when operation is mathematically invalid
    }
    return std::sqrt(value); // Returns std::optional containing the double result
}

/**
 * 6A. TRAILING RETURN TYPE SYNTAX ('auto ... -> Type')
 * Modern signature syntax where return type appears after the parameter list.
 */
auto computeHypotenuse(double sideA, double sideB) -> double {
    return std::sqrt((sideA * sideA) + (sideB * sideB));
}

/**
 * 6B. AUTOMATIC RETURN TYPE DEDUCTION ('auto')
 * Compiler automatically deduces return type from the return expression type.
 */
auto generateSequenceVector(int size) {
    vector<int> sequence;
    sequence.reserve(static_cast<size_t>(size));
    for (int i = 1; i <= size; ++i) {
        sequence.push_back(i * 10);
    }
    return sequence; // Return type deduced as std::vector<int>
}

// =====================================================================================
// MAIN FUNCTION (Entry point executing return type demonstrations)
// =====================================================================================
int main() {
    int userInt = 0;
    double userDouble = 0.0;
    string text1, text2;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter an integer value (positive or negative): " << flush;
    if (!(cin >> userInt)) {
        cout << "Invalid integer input. Defaulting to 5." << endl;
        userInt = 5;
    }

    cout << "Enter a floating-point value for square root calculation: " << flush;
    if (!(cin >> userDouble)) {
        cout << "Invalid double input. Defaulting to 16.0." << endl;
        userDouble = 16.0;
    }

    cout << "Enter first comparison word: " << flush;
    cin >> text1;

    cout << "Enter second comparison word: " << flush;
    cin >> text2;

    // 2. DEMONSTRATING VOID RETURN TYPE
    cout << "\n================ 1. VOID RETURN TYPE ================" << endl;
    processVoidOperation(userInt);

    // 3. DEMONSTRATING RETURN BY VALUE
    cout << "\n================ 2. RETURN BY VALUE ================" << endl;
    double interestResult = computeCompoundInterest(1000.0, 0.05, 3);
    cout << "  Compound Interest Result ($1000 @ 5% for 3 yrs) = $" 
         << fixed << setprecision(2) << interestResult << endl;

    // 4. DEMONSTRATING REFERENCE RETURN TYPES
    cout << "\n================ 3. REFERENCE RETURN TYPES ================" << endl;
    
    // Const reference return (zero-copy string access)
    const string& longerWord = selectLongerString(text1, text2);
    cout << "  Longer word (via const string&) : \"" << longerWord << "\"" << endl;

    // Non-const reference return (in-place modification)
    vector<int> sampleVector = {100, 200, 300, 400};
    cout << "  Vector element at index 1 BEFORE modification : " << sampleVector[1] << endl;
    
    getElementReference(sampleVector, 1) = 999; // Modifies vector element directly!
    cout << "  Vector element at index 1 AFTER modification  : " << sampleVector[1] << endl;

    // 5. DEMONSTRATING MULTIPLE RETURNS (C++17 STRUCTURED BINDING)
    cout << "\n================ 4. MULTIPLE RETURNS (std::tuple) ================" << endl;
    
    // Unpacking returned tuple into distinct local variables using C++17 structured bindings
    auto [summaryCount, summaryScaled, summaryStatus] = GenerateDataSummary(userInt, userDouble);
    cout << "  Unpacked Tuple Summary:" << endl;
    cout << "    |- Doubled Count : " << summaryCount << endl;
    cout << "    |- Scaled Value  : " << summaryScaled << endl;
    cout << "    |- System Status : " << summaryStatus << endl;

    // 6. DEMONSTRATING OPTIONAL RETURN TYPE
    cout << "\n================ 5. OPTIONAL RETURN TYPE (std::optional) ================" << endl;
    
    auto sqrtResult = safeSquareRoot(userDouble);
    if (sqrtResult.has_value()) {
        cout << "  Safe Square Root (" << userDouble << ") = " << sqrtResult.value() << endl;
    } else {
        cout << "  Safe Square Root (" << userDouble << ") = INVALID (Returned nullopt state)" << endl;
    }

    // 7. DEMONSTRATING MODERN RETURN SYNTAX (TRAILING & AUTO)
    cout << "\n================ 6. TRAILING & AUTO RETURN TYPES ================" << endl;
    
    double hypotenuse = computeHypotenuse(3.0, 4.0);
    cout << "  Trailing return hypotenuse(3.0, 4.0) -> " << hypotenuse << endl;

    auto deducedVector = generateSequenceVector(4);
    cout << "  Deduced auto return vector sequence : ";
    for (int item : deducedVector) {
        cout << item << " ";
    }
    cout << endl;

    cout << "\n================ C++ RETURN TYPES SUMMARY ================" << endl;
    cout << "1. 'void'         : Used when function performs actions without returning data." << endl;
    cout << "2. Value (T)      : Safe for primitives and local objects; optimized via RVO/NRVO." << endl;
    cout << "3. Reference (T&) : Fast zero-copy access; NEVER return references to local variables!" << endl;
    cout << "4. Tuple / Pair   : Clean mechanism for returning multiple values cleanly." << endl;
    cout << "5. std::optional  : Safe wrapper for functions that may fail or return no result." << endl;
    cout << "6. Trailing / Auto: Modern C++ syntax simplifying complex return signatures." << endl;

    return 0;
}