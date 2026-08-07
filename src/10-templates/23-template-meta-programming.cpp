/*
 * =====================================================================================
 * CONCEPT        : Introduction to Template Meta-Programming (TMP) under Templates
 * DESCRIPTION    : Production-grade executable code demonstrating the foundation,
 *                  mechanics, idioms, and core techniques of Template Metaprogramming:
 *
 *                  1. Compile-Time Value Computation (Classical Struct Recurrence) :
 *                     - Performing mathematical computations entirely at compile time 
 *                       using recursive template instantiations and base-case specializations 
 *                       (e.g., `Factorial<N>` and `Power<Base, Exp>`).
 *
 *                  2. Compile-Time Type Inspection & Manipulation (Custom Type Traits) :
 *                     - Introspecting and transforming types at compile time using template 
 *                       specialization (e.g., custom `IsSameType<T, U>` and `RemovePointer<T>`).
 *
 *                  3. Compile-Time Selection (Type Conditionals) :
 *                     - Choosing types based on compile-time boolean flags using 
 *                       `TypeSelect<Condition, TrueType, FalseType>`.
 *
 *                  4. Static Assertions (`static_assert`) & Zero Runtime Cost :
 *                     - Validating compile-time metaprogramming results before runtime execution.
 *                     - Demonstrating that TMP results reside directly in read-only memory or 
 *                       immediate machine instructions with zero runtime computation cost.
 *
 *                  5. Classical TMP vs. Modern C++ `constexpr` Functions :
 *                     - Contrasting template struct metaprogramming with modern C++ `constexpr`
 *                       functions for compile-time execution.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for runtime type name inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Execution : O(N) template instantiation depth.
 *                    Runtime Execution     : O(1) Zero runtime cost (immediate constant substitution).
 * SPACE COMPLEXITY : Binary Footprint      : Embedded compile-time values in read-only memory section.
 * =====================================================================================
 */

#include <iostream>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. COMPILE-TIME VALUE COMPUTATION (CLASSICAL TMP METAPROGRAMS)
// =====================================================================================

// Recursive Template Meta-Program for Factorial Computation
template <std::size_t N>
struct Factorial {
    static constexpr std::size_t value = N * Factorial<N - 1>::value;
};

// Base Case Specialization: 0! = 1
template <>
struct Factorial<0> {
    static constexpr std::size_t value = 1;
};

// Recursive Template Meta-Program for Exponentiation (Base^Exp)
template <std::size_t Base, std::size_t Exp>
struct Power {
    static constexpr std::size_t value = Base * Power<Base, Exp - 1>::value;
};

// Base Case Specialization: Base^0 = 1
template <std::size_t Base>
struct Power<Base, 0> {
    static constexpr std::size_t value = 1;
};

// =====================================================================================
// 2. COMPILE-TIME TYPE INSPECTION & MANIPULATION (CUSTOM TYPE TRAITS)
// =====================================================================================

// Primary Template: Assumes two types T and U are different
template <typename T, typename U>
struct IsSameType {
    static constexpr bool value = false;
};

// Partial Specialization: Matches when both types are identical (T and T)
template <typename T>
struct IsSameType<T, T> {
    static constexpr bool value = true;
};

// Custom Type Transformation Meta-Program: Removes pointer qualifier from T*
template <typename T>
struct RemovePointer {
    using type = T;
};

template <typename T>
struct RemovePointer<T*> {
    using type = T;
};

// Helper Alias for RemovePointer
template <typename T>
using RemovePointer_t = typename RemovePointer<T>::type;

// =====================================================================================
// 3. COMPILE-TIME CONDITIONAL TYPE SELECTION
// =====================================================================================

// Primary Template: If Condition is true, select TrueType
template <bool Condition, typename TrueType, typename FalseType>
struct TypeSelect {
    using type = TrueType;
};

// Partial Specialization: If Condition is false, select FalseType
template <typename TrueType, typename FalseType>
struct TypeSelect<false, TrueType, FalseType> {
    using type = FalseType;
};

template <bool Condition, typename TrueType, typename FalseType>
using TypeSelect_t = typename TypeSelect<Condition, TrueType, FalseType>::type;

// =====================================================================================
// 4. MODERN C++ CONSTEXPR COMPARISON
// =====================================================================================
constexpr std::size_t constexprFactorial(std::size_t n) noexcept {
    return (n <= 1) ? 1 : n * constexprFactorial(n - 1);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Template Meta-Programming analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. COMPILE-TIME METAPROGRAMMING VALUE COMPUTATION
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME METAPROGRAM VALUE COMPUTATION ================\n";

    // Values evaluated 100% during compilation!
    constexpr std::size_t fact5 = Factorial<5>::value;
    constexpr std::size_t fact7 = Factorial<7>::value;
    constexpr std::size_t pow2_10 = Power<2, 10>::value;

    // Static assertions guarantee zero runtime computation errors
    static_assert(Factorial<5>::value == 120, "Compile-time calculation error: 5! != 120");
    static_assert(Power<2, 10>::value == 1024, "Compile-time calculation error: 2^10 != 1024");

    cout << "  - Factorial<5>::value  : " << fact5 << " (Verified via static_assert)\n";
    cout << "  - Factorial<7>::value  : " << fact7 << "\n";
    cout << "  - Power<2, 10>::value  : " << pow2_10 << "\n";

    // Incorporating user input into runtime comparison with compile-time computed value
    std::size_t scaledRuntimeValue = userInputValue * Factorial<5>::value;
    cout << "  - Runtime Scaling (" << userInputValue << " * Factorial<5>::value) = " << scaledRuntimeValue << "\n";

    // =====================================================================================
    // 2. TYPE TRAITS & TYPE INSPECTION METAPROGRAMMING
    // =====================================================================================
    cout << "\n================ 2. COMPILE-TIME TYPE TRAITS & MANIPULATION ================\n";

    constexpr bool sameCheck1 = IsSameType<int, int>::value;
    constexpr bool sameCheck2 = IsSameType<int, double>::value;

    cout << "  - IsSameType<int, int>::value    : " << (sameCheck1 ? "TRUE" : "FALSE") << "\n";
    cout << "  - IsSameType<int, double>::value : " << (sameCheck2 ? "TRUE" : "FALSE") << "\n";

    using RawType = RemovePointer_t<int*>;
    cout << "  - RemovePointer_t<int*> Result   : " << typeid(RawType).name() << " (Pointers stripped)\n";

    // =====================================================================================
    // 3. COMPILE-TIME CONDITIONAL TYPE SELECTION
    // =====================================================================================
    cout << "\n================ 3. COMPILE-TIME TYPE SELECTION ================\n";

    // Selects double if size > 4 bytes, otherwise float
    using OptimalFloat = TypeSelect_t<(sizeof(userInputValue) > 4), double, float>;
    cout << "  - OptimalFloat Type chosen based on system word size: " << typeid(OptimalFloat).name() << "\n";

    // =====================================================================================
    // 4. CLASSICAL TMP VS MODERN CONSTEXPR
    // =====================================================================================
    cout << "\n================ 4. CLASSICAL TMP VS. MODERN CONSTEXPR ================\n";

    constexpr std::size_t modernFact6 = constexprFactorial(6);
    cout << "  - Classical Metaprogram Factorial<6>::value = " << Factorial<6>::value << "\n";
    cout << "  - Modern C++ constexprFactorial(6)          = " << modernFact6 << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TEMPLATE META-PROGRAMMING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| TMP Pillar / Mechanism| C++ Implementation Syntax         | Architectural Behavior & Effect   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Value Metaprograms    | `struct Fact<N> { static ... };`  | Recursive type lookup at compile  |\n"
         << "| Base Case Stopping    | `template<> struct Fact<0>`       | Specialization halts recursion    |\n"
         << "| Type Traits Introspect| `template<T, U> struct IsSame`    | Pattern matching via partial spec |\n"
         << "| Type Transformations  | `using RemovePtr_t = typename ...`| Modifies qualifiers at compile    |\n"
         << "| Conditional Selection | `TypeSelect_t<Condition, T, F>`   | Type-level `if-else` branch       |\n"
         << "| Static Validation     | `static_assert(Condition, msg);`  | Enforces rules before compilation |\n"
         << "| Modern Alternative    | `constexpr` / `consteval` functions| Imperative compile-time execution |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}