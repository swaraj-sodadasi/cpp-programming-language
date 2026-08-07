/*
 * =====================================================================================
 * CONCEPT        : Partial Specialisation under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the rules, syntax,
 *                  mechanics, and applications of Partial Template Specialization:
 *
 *                  1. Class Template Partial Specialization :
 *                     - Customizing class templates for a subset of template parameters 
 *                       or specific parameter patterns (e.g., matching types, pointers).
 *                     - Note: Standard C++ allows Partial Specialization for CLASS templates, 
 *                       but NOT for function templates (use overloading/SFINAE/Concepts instead).
 *
 *                  2. Pattern-Based Partial Specialization :
 *                     - Specializing for type categories such as pointer types (`T*`), 
 *                       const types (`const T`), or reference types (`T&`).
 *
 *                  3. Multi-Parameter Partial Specialization :
 *                     - Specializing multi-type parameters (e.g., `Container<T, T>` vs `Container<T1, T2>`)
 *                       or fixing specific arguments (`Container<T, int>`).
 *
 *                  4. Ambiguity Resolution via Explicit Specialization :
 *                     - Resolving overlap conflicts between competing partial specializations
 *                       using explicit (full) specialization.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, endl, flush)
 *                  - <string>   : Type std::string and std::to_string
 *                  - <typeinfo> : RTTI typeid operator for type inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Selection : O(1) exact pattern matching by compiler.
 *                    Runtime Execution     : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint      : Generates separate binary code per specialized pattern.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. MULTI-PARAMETER CLASS TEMPLATE PARTIAL SPECIALIZATION
// =====================================================================================

// Primary Class Template (Least Specialized)
template <typename T1, typename T2>
class MultiTypeContainer {
public:
    void inspect() const {
        cout << "    [Primary Template<T1, T2>] Heterogeneous Types: ("
             << typeid(T1).name() << ", " << typeid(T2).name() << ")\n";
    }
};

// Partial Specialization 1: Homogeneous Types (T1 == T2)
template <typename T>
class MultiTypeContainer<T, T> {
public:
    void inspect() const {
        cout << "    [Partial Specialization <T, T>] Homogeneous Type: ("
             << typeid(T).name() << ")\n";
    }
};

// Partial Specialization 2: Fixed Second Parameter (T2 == int)
template <typename T1>
class MultiTypeContainer<T1, int> {
public:
    void inspect() const {
        cout << "    [Partial Specialization <T1, int>] Fixed Second Type: ("
             << typeid(T1).name() << ", int)\n";
    }
};

// Explicit Specialization: Resolves ambiguity when BOTH T1==T2 AND T2==int (MultiTypeContainer<int, int>)
template <>
class MultiTypeContainer<int, int> {
public:
    void inspect() const {
        cout << "    [Explicit Specialization <int, int>] Ambiguity Tie-Breaker for (int, int)\n";
    }
};

// =====================================================================================
// 2. TYPE CATEGORY PARTIAL SPECIALIZATION (POINTERS & CONST TYPES)
// =====================================================================================

// Primary Class Template
template <typename T>
class TypeInspector {
public:
    static void describe(const T& val) {
        cout << "    [Primary Template<T>] Standard Value Type (" << typeid(T).name()
             << ") | Value: " << val << "\n";
    }
};

// Partial Specialization for Pointer Types (T*)
template <typename T>
class TypeInspector<T*> {
public:
    static void describe(T* ptr) {
        cout << "    [Partial Specialization <T*>] Pointer to (" << typeid(T).name()
             << ") | Pointed Value: " << (ptr ? to_string(*ptr) : "nullptr") << "\n";
    }
};

// Partial Specialization for Const Types (const T)
template <typename T>
class TypeInspector<const T> {
public:
    static void describe(const T& val) {
        cout << "    [Partial Specialization <const T>] Read-Only Const Type (" 
             << typeid(T).name() << ") | Value: " << val << "\n";
    }
};

// =====================================================================================
// 3. NON-TYPE TEMPLATE PARAMETER PARTIAL SPECIALIZATION (ARRAY BOUNDS)
// =====================================================================================

// Primary Template with Non-Type Template Parameter (NTTP)
template <typename T, std::size_t Dimension>
class FixedArrayWrapper {
public:
    void render() const {
        cout << "    [Primary Template<T, Dimension>] Stack Array of " << Dimension 
             << " elements of type " << typeid(T).name() << "\n";
    }
};

// Partial Specialization for Single-Element Dimension (Dimension == 1)
template <typename T>
class FixedArrayWrapper<T, 1> {
public:
    void render() const {
        cout << "    [Partial Specialization <T, 1>] Optimized Scalar Storage for Type " 
             << typeid(T).name() << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Partial Specialisation analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MULTI-PARAMETER PARTIAL SPECIALIZATION
    // =====================================================================================
    cout << "\n================ 1. MULTI-PARAMETER PARTIAL SPECIALIZATION ================\n";

    // Scenario A: Heterogeneous types -> Primary Template
    cout << "  - Instantiating MultiTypeContainer<double, string>:\n";
    MultiTypeContainer<double, string> containerPrimary;
    containerPrimary.inspect();

    // Scenario B: Homogeneous types -> Partial Specialization <T, T>
    cout << "\n  - Instantiating MultiTypeContainer<double, double>:\n";
    MultiTypeContainer<double, double> containerHomogeneous;
    containerHomogeneous.inspect();

    // Scenario C: Fixed second parameter -> Partial Specialization <T1, int>
    cout << "\n  - Instantiating MultiTypeContainer<string, int>:\n";
    MultiTypeContainer<string, int> containerFixedInt;
    containerFixedInt.inspect();

    // Scenario D: Ambiguity tie-breaker -> Explicit Specialization <int, int>
    cout << "\n  - Instantiating MultiTypeContainer<int, int>:\n";
    MultiTypeContainer<int, int> containerAmbiguityResolved;
    containerAmbiguityResolved.inspect();

    // =====================================================================================
    // 2. TYPE CATEGORY PARTIAL SPECIALIZATION (POINTERS & CONST)
    // =====================================================================================
    cout << "\n================ 2. TYPE CATEGORY PARTIAL SPECIALIZATION ================\n";

    int scalarVal = userInputValue;
    const int constVal = userInputValue + 50;

    cout << "  - Calling TypeInspector<int>::describe(scalarVal) [Primary Template]:\n";
    TypeInspector<int>::describe(scalarVal);

    cout << "\n  - Calling TypeInspector<int*>::describe(&scalarVal) [Pointer Specialization]:\n";
    TypeInspector<int*>::describe(&scalarVal);

    cout << "\n  - Calling TypeInspector<const int>::describe(constVal) [Const Specialization]:\n";
    TypeInspector<const int>::describe(constVal);

    // =====================================================================================
    // 3. NTTP PARTIAL SPECIALIZATION
    // =====================================================================================
    cout << "\n================ 3. NTTP PARTIAL SPECIALIZATION ================\n";

    cout << "  - Instantiating FixedArrayWrapper<int, 5> [Primary NTTP Template]:\n";
    FixedArrayWrapper<int, 5> array5;
    array5.render();

    cout << "\n  - Instantiating FixedArrayWrapper<int, 1> [Specialized Dimension=1 Template]:\n";
    FixedArrayWrapper<int, 1> scalarArray;
    scalarArray.render();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ PARTIAL SPECIALISATION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Specialization Pattern| Syntax Example                    | Key Rule / Architectural Effect   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Homogeneous Types     | `template<T> class C<T, T>`       | Matches when both types are equal |\n"
         << "| Fixed Parameter       | `template<T1> class C<T1, int>`   | Fixes one parameter to specific type|\n"
         << "| Pointer Category      | `template<T> class C<T*>`         | Matches any pointer type argument |\n"
         << "| Const Category        | `template<T> class C<const T>`    | Matches read-only const types     |\n"
         << "| NTTP Specialization   | `template<T> class C<T, 1>`       | Specializes for specific constant |\n"
         << "| Function Restriction  | Class Templates ONLY              | Function templates use overloading|\n"
         << "| Ambiguity Resolution  | `template<> class C<int, int>`    | Full specialization resolves ties |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}