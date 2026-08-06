/*
 * =====================================================================================
 * CONCEPT        : The 'return' Statement in C++ (Value Return, Reference Return, Void Exit, RVO)
 * DESCRIPTION    : Comprehensive implementation explaining C++ return mechanisms:
 *                  1. Value Return             : Returning prvalues by copy or move with 
 *                                                Return Value Optimization (RVO / NRVO).
 *                  2. Void Function Exit       : Using bare 'return;' to exit void functions early.
 *                  3. Reference Return (T&)    : Returning lvalue references to modify caller data 
 *                                                or support method chaining.
 *                  4. Const Reference Return   : Returning 'const T&' to provide read-only access 
 *                                                without incurring object copy overhead.
 *                  5. Multiple Values (tuple)  : Returning multiple values using std::tuple / std::pair 
 *                                                and unpacking via C++17 structured bindings.
 *                  6. Optional Return          : Returning std::optional<T> to signal missing or 
 *                                                invalid operational results safely.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct constant-time scalar returns and reference passing.
 * SPACE COMPLEXITY : Best Case: O(1) — Stack frame allocation managed via compiler optimizations (RVO).
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <optional>
#include <cstdlib>

using namespace std;

// 1. VOID FUNCTION EARLY RETURN
void validateAndProcessInput(int inputValue) {
    cout << "  Validating input parameter (" << inputValue << ")..." << endl;
    if (inputValue < 0) {
        cout << "  [Early Void Return] Negative value detected! Exiting function immediately." << endl;
        return; // Exits function early without returning a value
    }
    cout << "  [Success] Input value " << inputValue << " validated and processed successfully." << endl;
}

// 2. VALUE RETURN WITH RECURSION
int computeFactorial(int n) {
    if (n <= 1) {
        return 1; // Base case return
    }
    return n * computeFactorial(n - 1); // Recursive call value return
}

// 3. CONST REFERENCE RETURN (EFFICIENT READ-ONLY ACCESS)
const string& getLongerString(const string& str1, const string& str2) {
    // Returns a const reference to avoid creating an expensive heap string copy
    return (str1.length() >= str2.length()) ? str1 : str2;
}

// 4. NON-CONST LVALUE REFERENCE RETURN (MODIFIABLE REFERENCE)
int& getElementReference(vector<int>& vec, size_t index) {
    // Returns reference to element inside vector, allowing in-place modification
    return vec[index];
}

// 5. MULTIPLE VALUE RETURN VIA STD::TUPLE
tuple<int, double, string> computeSummary(int intVal, double doubleVal) {
    int doubledInt = intVal * 2;
    double halvedDouble = doubleVal / 2.0;
    string statusLabel = (intVal >= 0) ? "Positive/Zero" : "Negative";

    // Returns multiple distinct values packed into a single tuple
    return {doubledInt, halvedDouble, statusLabel};
}

// 6. OPTIONAL RETURN VALUE (SAFE ERROR HANDLING)
optional<double> safeDivide(double numerator, double denominator) {
    if (denominator == 0.0) {
        return nullopt; // Returns empty optional state on error / invalid input
    }
    return numerator / denominator; // Returns wrapped double value
}

int main() {
    int dynamicInt = 0;
    double dynamicDouble = 0.0;
    string textA, textB;

    // Dynamic input collection with stream flushing
    cout << "Enter an integer value (can be positive or negative): " << flush;
    if (!(cin >> dynamicInt)) {
        cout << "Invalid integer input. Program terminated." << endl;
        return EXIT_FAILURE;
    }

    cout << "Enter a floating-point divisor value: " << flush;
    if (!(cin >> dynamicDouble)) {
        cout << "Invalid floating-point input. Program terminated." << endl;
        return EXIT_FAILURE;
    }

    cout << "Enter first string word: " << flush;
    cin >> textA;

    cout << "Enter second string word: " << flush;
    cin >> textB;

    // 1. VOID EARLY RETURN DEMONSTRATION
    cout << "\n================ 1. VOID FUNCTION EARLY RETURN ================" << endl;
    validateAndProcessInput(dynamicInt);

    // 2. VALUE RETURN DEMONSTRATION
    cout << "\n================ 2. VALUE RETURN (COMPUTE FACTORIAL) ================" << endl;
    int absInt = (dynamicInt < 0) ? -dynamicInt : dynamicInt;
    int cappedInt = (absInt > 10) ? 10 : absInt; // Prevent integer overflow for demo
    cout << "Factorial of " << cappedInt << " = " << computeFactorial(cappedInt) << endl;

    // 3. CONST REFERENCE RETURN DEMONSTRATION
    cout << "\n================ 3. CONST REFERENCE RETURN ================" << endl;
    const string& longerStr = getLongerString(textA, textB);
    cout << "Longer String (Returned by Const Ref): \"" << longerStr << "\"" << endl;

    // 4. LVALUE REFERENCE RETURN DEMONSTRATION
    cout << "\n================ 4. LVALUE REFERENCE RETURN ================" << endl;
    vector<int> numbers = {10, 20, 30, 40, 50};
    cout << "Original vector element at index 2 : " << numbers[2] << endl;

    // Modify vector element directly through returned lvalue reference
    getElementReference(numbers, 2) = 999;
    cout << "Modified vector element at index 2 : " << numbers[2] << endl;

    // 5. MULTIPLE VALUE RETURN (C++17 STRUCTURED BINDING)
    cout << "\n================ 5. MULTIPLE RETURN VALUES (std::tuple) ================" << endl;
    auto [doubled, halved, label] = computeSummary(dynamicInt, dynamicDouble);
    cout << "Summary Tuple Results:" << endl;
    cout << "  |- Doubled Integer : " << doubled << endl;
    cout << "  |- Halved Double   : " << halved << endl;
    cout << "  |- Status Label    : " << label << endl;

    // 6. OPTIONAL RETURN DEMONSTRATION
    cout << "\n================ 6. OPTIONAL RETURN (std::optional) ================" << endl;
    auto divisionResult = safeDivide(100.0, dynamicDouble);
    if (divisionResult.has_value()) {
        cout << "Safe Division Result (100.0 / " << dynamicDouble << ") = " << divisionResult.value() << endl;
    } else {
        cout << "Safe Division Failed: Division by zero is undefined (returned nullopt)." << endl;
    }

    return EXIT_SUCCESS; // Standard explicit return from main indicating successful process completion
}