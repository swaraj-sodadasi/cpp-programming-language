/*
 * =====================================================================================
 * CONCEPT        : Explicit Template Arguments under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating when, why, and how to
 *                  use Explicit Template Arguments in C++:
 *
 *                  1. Un-deducible Template Parameters (e.g., Return Type Templates):
 *                     - When a template parameter appears ONLY in the return type or is not
 *                       present in function parameters, explicit argument specification is REQUIRED.
 *
 *                  2. Resolving Type Deduction Ambiguities:
 *                     - Forcing a unified template type `T` when conflicting argument types are passed
 *                       (e.g., passing `int` and `double` to `template <typename T> T max(T, T)`).
 *
 *                  3. Partial Explicit Specification:
 *                     - Explicitly specifying the first N template parameters while letting the compiler
 *                       implicitly deduce the remaining trailing parameters.
 *
 *                  4. Explicit Non-Type Template Arguments (NTTP):
 *                     - Passing explicit compile-time constants (sizes, scale factors) alongside types.
 *
 *                  5. Overriding Non-Template Preference:
 *                     - Forcing the invocation of a function template over a matching non-template function.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O (cin, cout, flush, endl)
 *                  - <string>   : std::string
 *                  - <typeinfo> : RTTI typeid operator
 *                  - <limits>   : std::numeric_limits for input stream clearing
 *                  - <cstddef>  : std::size_t for non-type template parameters
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N) explicit template instantiations.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary footprint scales with unique explicitly instantiated type signatures.
 * =====================================================================================
 */

#include <iostream>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. UN-DEDUCIBLE TEMPLATE PARAMETERS (RETURN TYPE TEMPLATES)
// TargetType appears ONLY in the return type, so compiler CANNOT deduce it from parameters.
// =====================================================================================
class UnDeducibleReturnDemo {
public:
    template <typename TargetType, typename SourceType>
    [[nodiscard]] static TargetType convertValue(SourceType val) {
        cout << "    [convertValue] Converting from " << typeid(SourceType).name() 
             << " to " << typeid(TargetType).name() << "\n";
        return static_cast<TargetType>(val);
    }
};

// =====================================================================================
// 2. RESOLVING TYPE DEDUCTION AMBIGUITIES
// T cannot be deduced automatically when conflicting types (int and double) are passed.
// =====================================================================================
class AmbiguityResolutionDemo {
public:
    template <typename T>
    [[nodiscard]] static T computeMaximum(T a, T b) {
        return (a > b) ? a : b;
    }
};

// =====================================================================================
// 3. PARTIAL EXPLICIT SPECIFICATION
// Explicitly specifying TargetType while Source1 and Source2 are implicitly deduced.
// =====================================================================================
class PartialSpecificationDemo {
public:
    template <typename ResultType, typename InputA, typename InputB>
    [[nodiscard]] static ResultType formatAndAdd(InputA a, InputB b) {
        cout << "    [formatAndAdd] Explicit ResultType: " << typeid(ResultType).name()
             << " | Deduced InputA: " << typeid(InputA).name()
             << " | Deduced InputB: " << typeid(InputB).name() << "\n";
        return static_cast<ResultType>(a + b);
    }
};

// =====================================================================================
// 4. EXPLICIT NON-TYPE TEMPLATE ARGUMENTS (NTTP) & OVERRIDE DEMO
// Specifying explicit compile-time constants and overriding non-template preference.
// =====================================================================================
class NonTypeAndOverrideDemo {
public:
    // Non-template function
    static void process(int val) {
        cout << "    [SELECTED]: Non-Template process(int): " << val << "\n";
    }

    // Primary function template
    template <typename T>
    static void process(T val) {
        cout << "    [SELECTED]: Template process<T>(T): " << val 
             << " [Type: " << typeid(T).name() << "]\n";
    }

    // Explicit NTTP template function
    template <typename T, std::size_t Multiplier>
    [[nodiscard]] static T scaleValue(T val) {
        return val * static_cast<T>(Multiplier);
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Explicit Template Arguments analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MANDATORY EXPLICIT TEMPLATE ARGUMENTS (RETURN TYPES)
    // =====================================================================================
    cout << "\n================ 1. MANDATORY EXPLICIT ARGUMENTS (RETURN TYPE) ================\n";

    // convertValue(userInputValue); // COMPILE ERROR: Cannot deduce TargetType from argument!
    double convertedDouble = UnDeducibleReturnDemo::convertValue<double>(userInputValue);
    cout << "  - Explicit Return Type <double>: " << convertedDouble << "\n";

    float convertedFloat = UnDeducibleReturnDemo::convertValue<float>(userInputValue + 50);
    cout << "  - Explicit Return Type <float>: " << convertedFloat << "\n";

    // =====================================================================================
    // 2. RESOLVING TYPE DEDUCTION AMBIGUITIES
    // =====================================================================================
    cout << "\n================ 2. AMBIGUITY RESOLUTION VIA EXPLICIT ARGUMENTS ================\n";

    // computeMaximum(userInputValue, 45.85); // COMPILE ERROR: Deduced T as both int and double!
    double maxResult = AmbiguityResolutionDemo::computeMaximum<double>(userInputValue, 45.85);
    cout << "  - Resolving (int, double) ambiguity via <double>: max = " << maxResult << "\n";

    int maxIntResult = AmbiguityResolutionDemo::computeMaximum<int>(userInputValue, static_cast<int>(45.85));
    cout << "  - Resolving via <int>: max = " << maxIntResult << "\n";

    // =====================================================================================
    // 3. PARTIAL EXPLICIT SPECIFICATION
    // =====================================================================================
    cout << "\n================ 3. PARTIAL EXPLICIT SPECIFICATION ================\n";

    // Only ResultType is explicitly provided (<double>). InputA and InputB are deduced.
    double partialResult = PartialSpecificationDemo::formatAndAdd<double>(userInputValue, 25.5f);
    cout << "  - Partial Specification <double>(int, float) -> Result: " << partialResult << "\n";

    // =====================================================================================
    // 4. EXPLICIT NON-TYPE TEMPLATE ARGUMENTS & OVERRIDE PRECEDENCE
    // =====================================================================================
    cout << "\n================ 4. EXPLICIT NTTP & OVERRIDING NON-TEMPLATE PRECEDENCE ================\n";

    // Calling scaleValue with explicitly passed NTTP Multiplier = 5
    int scaledVal = NonTypeAndOverrideDemo::scaleValue<int, 5>(userInputValue);
    cout << "  - Explicit NTTP <int, 5>: scaleValue(" << userInputValue << ") = " << scaledVal << "\n";

    // Overriding non-template function preference using explicit template qualification <>
    cout << "\n  - Calling process(userInputValue) [Selects Non-Template]:\n";
    NonTypeAndOverrideDemo::process(userInputValue);

    cout << "  - Calling process<int>(userInputValue) [Bypasses Non-Template via explicit <int>]:\n";
    NonTypeAndOverrideDemo::process<int>(userInputValue);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXPLICIT TEMPLATE ARGUMENTS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Scenario / Use Case   | Syntax Example                    | Reason / Architectural Requirement|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Return Type Template  | `convertValue<double>(val)`       | TargetType cannot be deduced      |\n"
         << "| Ambiguity Resolution  | `computeMaximum<double>(int, dbl)`| Unifies conflicting argument types|\n"
         << "| Partial Specification | `formatAndAdd<double>(int, float)` | Explicit 1st type, deduce rest    |\n"
         << "| Explicit NTTP         | `scaleValue<int, 5>(val)`         | Non-type constant passed explicitly|\n"
         << "| Override Non-Template | `process<int>(val)`               | Forces template over non-template |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}