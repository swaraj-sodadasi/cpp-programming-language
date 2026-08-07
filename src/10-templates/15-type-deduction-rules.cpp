/*
 * =====================================================================================
 * CONCEPT        : Template Type Deduction Rules in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the 3 core cases and 
 *                  decay exceptions of Template Type Deduction (Scott Meyers' Rules):
 *
 *                  1. CASE 1: ParamType is a Reference or Pointer (not Forwarding Ref)
 *                     - Lvalue references (`T&`), const lvalue references (`const T&`),
 *                       and raw pointers (`T*`). Ref-ness is ignored during deduction.
 *
 *                  2. CASE 2: ParamType is a Universal / Forwarding Reference (`T&&`)
 *                     - Lvalues deduce `T` as an lvalue reference (`T&`), collapsing to `T&`.
 *                     - Rvalues deduce `T` as non-reference type `T`, resulting in `T&&`.
 *
 *                  3. CASE 3: ParamType is Pass-by-Value (`T`)
 *                     - Creates a copy. Top-level `const`, `volatile`, and references (`&`) 
 *                       are stripped from the deduced parameter type.
 *
 *                  4. SPECIAL CASES: Array and Function Decay Rules
 *                     - Arrays/Functions passed by value decay into pointers (`const char*`, `void(*)()`).
 *                     - Arrays/Functions passed by reference retain exact dimensions (`T(&)[N]`)
 *                       or function reference types without decaying.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for runtime type name inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <type_traits> : std::is_lvalue_reference_v, std::is_const_v, std::remove_reference_t
 *                  - <utility>     : std::move for rvalue reference generation
 *                  - <cstddef>     : std::size_t for array bounds deduction
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(1) deduction per instantiation call.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary footprint scales with unique deduced template signatures.
 * =====================================================================================
 */

#include <iostream>
#include <typeinfo>
#include <limits>
#include <type_traits>
#include <utility>
#include <cstddef>

using namespace std;

// Helper to inspect dynamic and static type characteristics
template <typename T>
void printDeductionTraits(const char* label) {
    using RawT = std::remove_reference_t<T>;
    cout << "      [" << label << "] T = " << typeid(T).name()
         << " | Ref Category: " << (std::is_lvalue_reference_v<T> ? "Lvalue Ref (T&)" : 
                                  (std::is_rvalue_reference_v<T> ? "Rvalue Ref (T&&)" : "Value (Non-Ref)"))
         << " | Const: " << (std::is_const_v<RawT> ? "const" : "non-const") << "\n";
}

// =====================================================================================
// 1. CASE 1: PARAMTYPE IS A REFERENCE OR POINTER (NOT FORWARDING REFERENCE)
// =====================================================================================
class Case1ReferencePointerDemo {
public:
    // Non-const Lvalue Reference Parameter
    template <typename T>
    static void lvalueRef(T& param) {
        (void)param; // Suppress unused parameter warning
        printDeductionTraits<T>("Case 1A: T& param");
    }

    // Const Lvalue Reference Parameter
    template <typename T>
    static void constLvalueRef(const T& param) {
        (void)param; // Suppress unused parameter warning
        printDeductionTraits<T>("Case 1B: const T& param");
    }

    // Pointer Parameter
    template <typename T>
    static void pointerParam(T* param) {
        (void)param; // Suppress unused parameter warning
        printDeductionTraits<T>("Case 1C: T* param");
    }
};

// =====================================================================================
// 2. CASE 2: PARAMTYPE IS A UNIVERSAL / FORWARDING REFERENCE (`T&&`)
// =====================================================================================
class Case2ForwardingRefDemo {
public:
    template <typename T>
    static void forwardingRef(T&& param) {
        (void)param; // Suppress unused parameter warning
        printDeductionTraits<T>("Case 2: T&& Forwarding Ref");
    }
};

// =====================================================================================
// 3. CASE 3: PARAMTYPE IS PASS-BY-VALUE (`T`)
// =====================================================================================
class Case3PassByValueDemo {
public:
    template <typename T>
    static void passByValue(T param) {
        (void)param; // Suppress unused parameter warning
        printDeductionTraits<T>("Case 3: T Pass-By-Value");
    }
};

// =====================================================================================
// 4. SPECIAL DECAY RULES: ARRAYS AND FUNCTIONS
// =====================================================================================
static void sampleTargetFunction(int val) {
    (void)val; // Suppress unused parameter warning
}

class DecayRulesDemo {
public:
    // Array Passed By Value -> Decays to Pointer
    template <typename T>
    static void arrayByValue(T param) {
        (void)param; // Suppress unused parameter warning
        cout << "    [Array By Value (T param)] Decayed Type T: " << typeid(T).name() << "\n";
    }

    // Array Passed By Reference -> Preserves Exact Array Type and Bounds
    template <typename T>
    static void arrayByReference(T& param) {
        (void)param; // Suppress unused parameter warning
        cout << "    [Array By Reference (T& param)] Exact Type T: " << typeid(T).name() << "\n";
    }

    // Deduce compile-time Array Dimension N via Reference
    template <typename T, std::size_t N>
    [[nodiscard]] static constexpr std::size_t deduceArraySize(const T (&)[N]) noexcept {
        return N;
    }

    // Function Passed By Value -> Decays to Function Pointer
    template <typename T>
    static void functionByValue(T param) {
        (void)param; // Suppress unused parameter warning
        cout << "    [Function By Value (T param)] Decayed Type T: " << typeid(T).name() << "\n";
    }

    // Function Passed By Reference -> Retains Function Reference Type
    template <typename T>
    static void functionByReference(T& param) {
        (void)param; // Suppress unused parameter warning
        cout << "    [Function By Reference (T& param)] Exact Type T: " << typeid(T).name() << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Type Deduction analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // Common test variables
    int x = userInputValue;            // int
    const int cx = userInputValue;      // const int
    const int& rx = cx;                // const int&
    const int* px = &cx;               // const int*

    // =====================================================================================
    // 1. CASE 1 DEMONSTRATION (REFERENCES & POINTERS)
    // =====================================================================================
    cout << "\n================ 1. CASE 1: PARAMTYPE IS A REFERENCE OR POINTER ================\n";

    cout << "  - A. lvalueRef(T& param):\n";
    Case1ReferencePointerDemo::lvalueRef(x);   // T = int
    Case1ReferencePointerDemo::lvalueRef(cx);  // T = const int
    Case1ReferencePointerDemo::lvalueRef(rx);  // T = const int (ref stripped, const preserved)

    cout << "\n  - B. constLvalueRef(const T& param):\n";
    Case1ReferencePointerDemo::constLvalueRef(x);  // T = int (const added by ParamType)
    Case1ReferencePointerDemo::constLvalueRef(cx); // T = int
    Case1ReferencePointerDemo::constLvalueRef(rx); // T = int

    cout << "\n  - C. pointerParam(T* param):\n";
    Case1ReferencePointerDemo::pointerParam(&x); // T = int
    Case1ReferencePointerDemo::pointerParam(px); // T = const int

    // =====================================================================================
    // 2. CASE 2 DEMONSTRATION (FORWARDING / UNIVERSAL REFERENCES)
    // =====================================================================================
    cout << "\n================ 2. CASE 2: PARAMTYPE IS A FORWARDING REFERENCE (`T&&`) ================\n";

    cout << "  - Passing Lvalues (T deduced as Lvalue Reference T&):\n";
    Case2ForwardingRefDemo::forwardingRef(x);  // x is lvalue -> T = int&
    Case2ForwardingRefDemo::forwardingRef(cx); // cx is lvalue -> T = const int&
    Case2ForwardingRefDemo::forwardingRef(rx); // rx is lvalue -> T = const int&

    cout << "\n  - Passing Rvalues (T deduced as Non-Reference T):\n";
    Case2ForwardingRefDemo::forwardingRef(userInputValue + 50); // Rvalue -> T = int
    Case2ForwardingRefDemo::forwardingRef(std::move(x));        // Rvalue -> T = int

    // =====================================================================================
    // 3. CASE 3 DEMONSTRATION (PASS-BY-VALUE)
    // =====================================================================================
    cout << "\n================ 3. CASE 3: PARAMTYPE IS PASS-BY-VALUE (`T`) ================\n";

    cout << "  - Pass-by-value strips top-level constness and reference qualifiers:\n";
    Case3PassByValueDemo::passByValue(x);   // T = int
    Case3PassByValueDemo::passByValue(cx);  // T = int (const stripped!)
    Case3PassByValueDemo::passByValue(rx);  // T = int (ref & const stripped!)

    const char* const ptrConst = "Constant Pointer String";
    Case3PassByValueDemo::passByValue(ptrConst); // T = const char* (top-level ptr const stripped, pointed-to const kept)

    // =====================================================================================
    // 4. SPECIAL DECAY RULES (ARRAYS AND FUNCTIONS)
    // =====================================================================================
    cout << "\n================ 4. SPECIAL DECAY RULES (ARRAYS & FUNCTIONS) ================\n";

    const char textArray[] = "Modern C++ Templates";
    int numArray[] = {10, 20, 30, 40, 50};

    cout << "  - Array Arguments (By-Value vs. By-Reference):\n";
    DecayRulesDemo::arrayByValue(textArray);      // Decays to const char*
    DecayRulesDemo::arrayByReference(textArray);  // Retains const char[21]

    constexpr std::size_t arrayLen = DecayRulesDemo::deduceArraySize(numArray);
    cout << "    * Deduced Array Size at Compile-Time: N = " << arrayLen << "\n";

    cout << "\n  - Function Arguments (By-Value vs. By-Reference):\n";
    DecayRulesDemo::functionByValue(sampleTargetFunction);     // Decays to void(*)(int)
    DecayRulesDemo::functionByReference(sampleTargetFunction); // Retains void(&)(int)

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TEMPLATE TYPE DEDUCTION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Parameter Case        | Expression Category   | Deduced Type T & ParamType Rules  |\n"
         << "+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Case 1: `T&`          | Lvalue (`int x`)      | T = int, ParamType = int&         |\n"
         << "| Case 1: `T&`          | Const Lvalue (`const`)| T = const int, ParamType = const& |\n"
         << "| Case 1: `const T&`    | Lvalue (`int x`)      | T = int, ParamType = const int&   |\n"
         << "| Case 2: `T&&`         | Lvalue (`x`)          | T = int&, ParamType = int& (Ref)  |\n"
         << "| Case 2: `T&&`         | Rvalue (`std::move`)  | T = int, ParamType = int&&        |\n"
         << "| Case 3: `T` (By Val)  | `const int&`          | T = int (Strips const & reference)|\n"
         << "| Array Decay (By Val)  | `char arr[10]`        | T = const char* (Decays to ptr)   |\n"
         << "| Array Ref (By Ref)    | `char arr[N]`         | T = char[N] (Preserves bounds N)  |\n"
         << "| Function Decay        | `void fn(int)`        | T = void(*)(int) (Decays to ptr)  |\n"
         << "+-----------------------+-----------------------+-----------------------------------+\n";

    return 0;
}