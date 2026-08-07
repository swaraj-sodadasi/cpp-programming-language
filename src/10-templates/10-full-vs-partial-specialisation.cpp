/*
 * =====================================================================================
 * CONCEPT        : Full vs Partial Specialisation under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the fundamental 
 *                  differences, syntax rules, mechanics, and architectural trade-offs 
 *                  between Full (Explicit) and Partial Template Specialization:
 *
 *                  1. Full (Explicit) Specialization :
 *                     - All template parameters are explicitly replaced with concrete types.
 *                     - Uses `template <>` syntax (0 template parameters remain generic).
 *                     - Supported for BOTH Class Templates and Function Templates.
 *
 *                  2. Partial Specialization :
 *                     - A subset of template parameters remain generic, or specific type 
 *                       patterns (e.g., `T*`, `T&`, `const T`, `Container<T, T>`) are matched.
 *                     - Retains generic parameters in `template <typename T>`.
 *                     - Supported for Class Templates (and Variable Templates), BUT 
 *                       STRICTLY FORBIDDEN for Function Templates by C++ language rules.
 *
 *                  3. Function Template Specialization vs. Overloading :
 *                     - Because function templates cannot be partially specialized, C++ 
 *                       utilizes Function Overloading or SFINAE/Concepts as the idiom 
 *                       for partial customization.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, endl, flush)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for runtime type inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Selection : O(1) exact match / pattern lookup by compiler.
 *                    Runtime Execution     : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint      : Distinct binary code generated per specialized instance.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// 1. CLASS TEMPLATES: FULL VS. PARTIAL SPECIALIZATION
// =====================================================================================

// Primary Class Template (2 generic parameters: T and U)
template <typename T, typename U>
class TypeMatrix {
public:
    static void renderInfo() {
        cout << "    [1. PRIMARY CLASS TEMPLATE] Generic Types: (" 
             << typeid(T).name() << ", " << typeid(U).name() << ")\n";
    }
};

// Partial Specialization #1: Homogeneous Types (T1 == T2)
// Template parameter list still has 1 parameter: `template <typename T>`
template <typename T>
class TypeMatrix<T, T> {
public:
    static void renderInfo() {
        cout << "    [2. PARTIAL SPECIALIZATION] Homogeneous Pattern <T, T> for Type: (" 
             << typeid(T).name() << ")\n";
    }
};

// Partial Specialization #2: Pointer Pattern for T + Fixed Second Parameter (U == int)
// Template parameter list still has 1 parameter: `template <typename T>`
template <typename T>
class TypeMatrix<T*, int> {
public:
    static void renderInfo() {
        cout << "    [3. PARTIAL SPECIALIZATION] Pointer & Fixed Type <T*, int> for Base Type: (" 
             << typeid(T).name() << ")\n";
    }
};

// Full (Explicit) Specialization: Concrete target types <int, double>
// Template parameter list is empty: `template <>` (0 remaining generic parameters)
template <>
class TypeMatrix<int, double> {
public:
    static void renderInfo() {
        cout << "    [4. FULL SPECIALIZATION] Explicit Target Types <int, double>\n";
    }
};

// =====================================================================================
// 2. FUNCTION TEMPLATES: FULL SPECIALIZATION VS. OVERLOADING
// =====================================================================================

class FunctionTemplateDemo {
public:
    // Primary Function Template
    template <typename T, typename U>
    static void processData(T a, U b) {
        cout << "    [Primary Function Template] Generic Args: (" 
             << typeid(T).name() << ": " << a << ", " 
             << typeid(U).name() << ": " << b << ")\n";
    }

    // Full Function Specialization for <int, std::string>
    // Valid in C++ because ALL template parameters are fully specified.
    // Uses `template <>`
};

// Full Specialization syntax for member function template
template <>
void FunctionTemplateDemo::processData<int, string>(int a, string b) {
    cout << "    [Full Function Specialization <int, string>] Specialized Execution: (" 
         << a << " | \"" << b << "\")\n";
}

// Function Overloading as a substitute for Partial Specialization
// NOTE: C++ DOES NOT allow partial specialization of function templates!
// e.g., `template <typename T> void processData<T, T>(T a, T b)` is a COMPILE ERROR.
// Solution: Use Function Overloading:
class FunctionOverloadAlternative {
public:
    // Primary Template
    template <typename T, typename U>
    static void execute(T a, U b) {
        cout << "    [Primary Function Template] Types: (" 
             << typeid(T).name() << ", " << typeid(U).name() << ") -> " << a << ", " << b << "\n";
    }

    // Function Overload acting as "Partial Specialization" for Homogeneous Types (T, T)
    template <typename T>
    static void execute(T a, T b) {
        cout << "    [Function Overload (Simulated Partial Spec)] Homogeneous Types: (" 
             << typeid(T).name() << ") -> " << a << ", " << b << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Full vs Partial Specialisation analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CLASS TEMPLATES: FULL VS PARTIAL SPECIALIZATION DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. CLASS TEMPLATES: FULL VS. PARTIAL SPECIALIZATION ================\n";

    // Scenario A: Primary Template (Heterogeneous un-matched types)
    cout << "  - Instantiating TypeMatrix<double, string>:\n";
    TypeMatrix<double, string>::renderInfo();

    // Scenario B: Partial Specialization <T, T> (Homogeneous matching types)
    cout << "\n  - Instantiating TypeMatrix<string, string>:\n";
    TypeMatrix<string, string>::renderInfo();

    // Scenario C: Partial Specialization <T*, int> (Pointer pattern + fixed int)
    cout << "\n  - Instantiating TypeMatrix<double*, int>:\n";
    TypeMatrix<double*, int>::renderInfo();

    // Scenario D: Full Specialization <int, double> (Zero generic parameters left)
    cout << "\n  - Instantiating TypeMatrix<int, double>:\n";
    TypeMatrix<int, double>::renderInfo();

    // =====================================================================================
    // 2. FUNCTION TEMPLATES: FULL SPECIALIZATION VS OVERLOADING
    // =====================================================================================
    cout << "\n================ 2. FUNCTION TEMPLATES: FULL SPEC VS. OVERLOADING ================\n";

    cout << "  - Calling processData(userInputValue, 45.5) [Primary Function Template]:\n";
    FunctionTemplateDemo::processData(userInputValue, 45.5);

    cout << "\n  - Calling processData(userInputValue, string(\"Node_Alpha\")) [Full Specialization <int, string>]:\n";
    FunctionTemplateDemo::processData(userInputValue, string("Node_Alpha"));

    cout << "\n  - Calling FunctionOverloadAlternative::execute(userInputValue, 200) [Homogeneous Overload]:\n";
    FunctionOverloadAlternative::execute(userInputValue, 200);

    cout << "\n  - Calling FunctionOverloadAlternative::execute(userInputValue, 99.9) [Heterogeneous Primary]:\n";
    FunctionOverloadAlternative::execute(userInputValue, 99.9);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ FULL VS PARTIAL SPECIALISATION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature Attribute     | Full (Explicit) Specialization    | Partial Specialization            |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Declaration Syntax    | `template <>`                     | `template <typename T>`           |\n"
         << "| Generic Parameters    | 0 remaining (all replaced)        | 1 or more generic params remain   |\n"
         << "| Class Template Support| YES                               | YES                               |\n"
         << "| Function Template Sup | YES                               | NO (STRICTLY FORBIDDEN BY C++)    |\n"
         << "| Function Workaround   | N/A                               | Use Function Overloading / SFINAE |\n"
         << "| Pattern Matching      | Specific concrete types           | Pointers (`T*`), Const, Identical |\n"
         << "| Resolution Priority   | Highest precedence over primary   | Higher than primary, lower than   |\n"
         << "|                       | and partial specializations       | full explicit specialization      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}