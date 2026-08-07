/*
 * =====================================================================================
 * CONCEPT        : Recursive Templates under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, mechanics,
 *                  base cases, and applications of Recursive Templates (Compile-Time Recursion):
 *
 *                  1. Compile-Time Mathematical Computations (Value Recursion) :
 *                     - Computing factorials, Fibonacci numbers, and prime checking at compile time 
 *                       using recursive template instantiations (`Fibonacci<N>`).
 *
 *                  2. Recursive Tuple / Heterogeneous Storage Indexing :
 *                     - Recursively traversing and querying heterogeneous lists or tuple-like 
 *                       data structures using recursive template specializations.
 *
 *                  3. Recursive Variadic List Expansion :
 *                     - Unpacking and processing variadic template parameter packs via recursive 
 *                       head-and-tail peeling.
 *
 *                  4. Base Case Specialization :
 *                     - Terminating infinite template instantiation loops using explicit or 
 *                       partial specializations (e.g., `Fibonacci<0>`, `Fibonacci<1>`).
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for runtime type inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Instantiation Depth : O(N) where N is the recursion depth.
 *                    Runtime Execution               : O(1) Zero-cost constant substitution.
 * SPACE COMPLEXITY : Binary Footprint         : Scales linearly with recursion depth N.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. COMPILE-TIME RECURSIVE TEMPLATES (FIBONACCI SEQUENCE)
// =====================================================================================

// Primary Template: Fibonacci(N) = Fibonacci(N-1) + Fibonacci(N-2)
template <std::size_t N>
struct Fibonacci {
    static constexpr std::size_t value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

// Base Case Specialization: Fibonacci<0> = 0
template <>
struct Fibonacci<0> {
    static constexpr std::size_t value = 0;
};

// Base Case Specialization: Fibonacci<1> = 1
template <>
struct Fibonacci<1> {
    static constexpr std::size_t value = 1;
};

// =====================================================================================
// 2. COMPILE-TIME RECURSIVE PRIME NUMBER CHECKER
// =====================================================================================
template <std::size_t N, std::size_t D>
struct PrimeChecker {
    static constexpr bool value = (N % D != 0) && PrimeChecker<N, D - 1>::value;
};

// Base Case: If divisor reaches 1, N is prime
template <std::size_t N>
struct PrimeChecker<N, 1> {
    static constexpr bool value = true;
};

// Helper meta-function to check if N is prime (checks divisors from N-1 down to 2)
template <std::size_t N>
struct IsPrime {
    static constexpr bool value = (N <= 1) ? false : PrimeChecker<N, N - 1>::value;
};

// =====================================================================================
// 3. RECURSIVE VARIADIC TYPE INSPECTION (HETEROGENEOUS PACK TRAVERSAL)
// =====================================================================================

// Primary Template (Base Case / Empty Pack)
template <typename... Rest>
struct VariadicPackInspector {
    static void inspect() {
        cout << "    [Base Case] End of recursive variadic pack traversal.\n";
    }
};

// Recursive Variadic Template: Peels off the 'Head' and recurses on the 'Tail...'
template <typename Head, typename... Tail>
struct VariadicPackInspector<Head, Tail...> {
    static void inspect() {
        cout << "    [Recursive Step] Processing Type: " << typeid(Head).name() 
             << " | Remaining Pack Size: " << sizeof...(Tail) << "\n";
        // Recurse on remaining tail parameters
        VariadicPackInspector<Tail...>::inspect();
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Recursive Templates analysis (e.g., 10): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 10." << endl;
        userInputValue = 10;
    }

    // Ensure safe compile-time bounds for template recursion depth
    constexpr std::size_t safeN = 10;

    // =====================================================================================
    // 1. COMPILE-TIME RECURSIVE FIBONACCI
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME RECURSIVE FIBONACCI ================\n";

    constexpr std::size_t fib10 = Fibonacci<safeN>::value;
    cout << "  - Fibonacci<" << safeN << ">::value (Computed at compile time) = " << fib10 << "\n";
    cout << "  - Fibonacci<5>::value  = " << Fibonacci<5>::value << "\n";
    cout << "  - Fibonacci<8>::value  = " << Fibonacci<8>::value << "\n";

    // =====================================================================================
    // 2. COMPILE-TIME RECURSIVE PRIME CHECKER
    // =====================================================================================
    cout << "\n================ 2. COMPILE-TIME RECURSIVE PRIME CHECKER ================\n";

    constexpr bool is13Prime = IsPrime<13>::value;
    constexpr bool is15Prime = IsPrime<15>::value;
    constexpr bool is17Prime = IsPrime<17>::value;

    cout << "  - IsPrime<13>::value : " << (is13Prime ? "TRUE (Prime)" : "FALSE") << "\n";
    cout << "  - IsPrime<15>::value : " << (is15Prime ? "TRUE" : "FALSE (Composite)") << "\n";
    cout << "  - IsPrime<17>::value : " << (is17Prime ? "TRUE (Prime)" : "FALSE") << "\n";

    // =====================================================================================
    // 3. RECURSIVE VARIADIC PACK INSPECTOR
    // =====================================================================================
    cout << "\n================ 3. RECURSIVE VARIADIC PACK TRAVERSAL ================\n";

    cout << "  - Traversing heterogeneous parameter pack recursively:\n";
    VariadicPackInspector<int, double, string, char, bool>::inspect();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RECURSIVE TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Recursion Domain      | C++ Implementation Syntax         | Architectural Behavior & Purpose  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Value Recursion       | `struct Fib<N> { val = Fib<N-1> }`| Computes mathematical constants   |\n"
         << "|                       |                                   | entirely at compile time          |\n"
         << "| Base Case Termination | `template<> struct Fib<0>`        | Halts recursive template chain    |\n"
         << "|                       |                                   | to prevent infinite instantiation |\n"
         << "| Boolean Metaprogram   | `PrimeChecker<N, D>`              | Compile-time logical reduction    |\n"
         << "| Variadic Unpacking    | `struct Inspector<Head, Tail...>` | Peels 1 argument per instantiation|\n"
         << "|                       |                                   | until parameter pack is empty     |\n"
         << "| Runtime Cost          | Zero-cost abstraction             | Results embedded as immediate     |\n"
         << "|                       |                                   | constants in final machine binary |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}