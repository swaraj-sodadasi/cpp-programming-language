/*
 * =====================================================================================
 * CONCEPT        : Function Templates & Template Argument Deduction in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating:
 *                  1. Function Template Syntax & Implicit Type Deduction
 *                  2. Deduction Ambiguity / Conflict Resolution (Explicit Specification)
 *                  3. Multi-Type Templates & Common Type Promotion (`std::common_type_t`)
 *                  4. Special Deduction Mechanics (Array Reference Dimensions & Pointers)
 *                  5. Template Overloading Resolution Precedence
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively invoked 
 *                  in this code are included:
 *                  - <iostream>    : Console I/O streams (cin, cout, endl, flush)
 *                  - <string>      : Type std::string and std::to_string
 *                  - <typeinfo>    : typeid operator for runtime type name inspection
 *                  - <limits>      : numeric_limits for stream clearing
 *                  - <type_traits> : std::common_type_t for unified type promotion
 *                  - <cstddef>     : std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N * M) instantiation cost.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary size footprint scales with unique instantiated type signatures.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <type_traits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. FUNDAMENTAL FUNCTION TEMPLATES & IMPLICIT DEDUCTION
// =====================================================================================
class FunctionTemplateBasics {
public:
    // Single-parameter template: Deduces T from function arguments
    template <typename T>
    [[nodiscard]] static T computeMax(T a, T b) {
        return (a > b) ? a : b;
    }

    // Multi-type template using std::common_type_t to handle mixed-type argument promotion
    template <typename T, typename U>
    [[nodiscard]] static std::common_type_t<T, U> computeMixedAdd(T a, U b) {
        return a + b;
    }
};

// =====================================================================================
// 2. SPECIAL DEDUCTION RULES (ARRAY REFERENCES & POINTERS)
// =====================================================================================
class SpecialDeductionRules {
public:
    // Deduces both element type T and compile-time array size N
    template <typename T, std::size_t N>
    [[nodiscard]] static constexpr std::size_t deduceArraySize(const T (&)[N]) noexcept {
        return N;
    }

    // Type inspection helpers
    template <typename T>
    static void inspectType(T val) {
        cout << "    [Value Inspection] Deducted type T: " << typeid(T).name() 
             << " | Passed value: " << val << "\n";
    }

    template <typename T>
    static void inspectPointer(T* ptr) {
        cout << "    [Pointer Inspection] Deducted type T (element): " << typeid(T).name() 
             << " | Pointed value: " << (ptr ? to_string(*ptr) : "null") << "\n";
    }
};

// =====================================================================================
// 3. FUNCTION TEMPLATE OVERLOADING & RESOLUTION ORDER
// =====================================================================================
class OverloadingResolutionDemo {
public:
    // Regular Non-Template Function
    static void processData(int val) {
        cout << "    [Overload Selection] Non-Template Function processData(int): " << val << "\n";
    }

    // Generic Primary Template Function
    template <typename T>
    static void processData(T val) {
        cout << "    [Overload Selection] Primary Template processData(T): " << val 
             << " (Type: " << typeid(T).name() << ")\n";
    }

    // Specialized Pointer Template Overload
    template <typename T>
    static void processData(T* ptr) {
        cout << "    [Overload Selection] Pointer Overload Template processData(T*): "
             << (ptr ? to_string(*ptr) : "null") << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Deduction analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. IMPLICIT VS EXPLICIT DEDUCTION
    // =====================================================================================
    cout << "\n================ 1. IMPLICIT VS EXPLICIT TEMPLATE DEDUCTION ================\n";

    // Case A: Implicit Deduction with matching types
    int intMax = FunctionTemplateBasics::computeMax(userInputValue, 250);
    cout << "  - Implicit Deduction (int, int)       : computeMax(" << userInputValue << ", 250) = " << intMax << "\n";

    double doubleMax = FunctionTemplateBasics::computeMax(3.14159, 2.71828);
    cout << "  - Implicit Deduction (double, double) : computeMax(3.14159, 2.71828) = " << doubleMax << "\n";

    // Case B: Explicit Template Argument Specification (Resolves ambiguous T mismatch)
    // Implicit call computeMax(userInputValue, 50.75) fails compile because T cannot be both int and double.
    double explicitMax = FunctionTemplateBasics::computeMax<double>(userInputValue, 50.75);
    cout << "  - Explicit Override <double>(int, double): computeMax(" << userInputValue << ", 50.75) = " << explicitMax << "\n";

    // Case C: Multi-Type Parameter with std::common_type_t
    auto mixedResult = FunctionTemplateBasics::computeMixedAdd(userInputValue, 15.85);
    cout << "  - Mixed Deduction (int + double)      : computeMixedAdd(" << userInputValue << ", 15.85) = " << mixedResult 
         << " (Result Type: " << typeid(decltype(mixedResult)).name() << ")\n";

    // =====================================================================================
    // 2. ARRAY SIZE DEDUCTION & TYPE INSPECTION
    // =====================================================================================
    cout << "\n================ 2. SPECIAL DEDUCTION RULES (ARRAYS & POINTERS) ================\n";

    int sampleArray[] = {userInputValue, userInputValue + 10, userInputValue + 20, 500, 600};
    constexpr std::size_t deducedSize = SpecialDeductionRules::deduceArraySize(sampleArray);
    cout << "  - Array Reference Deduction: Deduced array size N = " << deducedSize << " at compile-time!\n";

    int targetVal = userInputValue * 2;
    SpecialDeductionRules::inspectType(targetVal);        // Deduces T = int
    SpecialDeductionRules::inspectPointer(&targetVal);    // Deduces T = int (from int*)

    // =====================================================================================
    // 3. OVERLOADING & RESOLUTION PRECEDENCE
    // =====================================================================================
    cout << "\n================ 3. OVERLOADING RESOLUTION PRECEDENCE ================\n";

    cout << "  - Calling processData(userInputValue) [Exact Non-Template Match]:\n";
    OverloadingResolutionDemo::processData(userInputValue); // Matches non-template processData(int)

    cout << "\n  - Calling processData(" << userInputValue << ".5) [Primary Template Match]:\n";
    OverloadingResolutionDemo::processData(static_cast<double>(userInputValue) + 0.5); // Matches processData(T)

    cout << "\n  - Calling processData(&userInputValue) [Pointer Overload Match]:\n";
    OverloadingResolutionDemo::processData(&userInputValue); // Matches processData(T*)

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ FUNCTION TEMPLATES & DEDUCTION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Deduction Concept     | C++ Syntax / Strategy             | Resolution Behavior               |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Implicit Deduction    | `func(a, b)`                      | Infers T from function arguments  |\n"
         << "| Type Mismatch Ambiguity| `func(intVal, doubleVal)`        | Compilation Error (T ambiguous)   |\n"
         << "| Explicit Override     | `func<double>(intVal, doubleVal)` | Forces type coercion to T         |\n"
         << "| Multi-Type Common Type| `common_type_t<T, U> func(T, U)`  | Promotes to unified safe type     |\n"
         << "| Array Size Deduction  | `func(const T(&arr)[N])`          | Extracts array size N statically  |\n"
         << "| Overload Precedence   | Non-Template > Pointer > Primary  | Compiler chooses most specific    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}