/*
 * =====================================================================================
 * CONCEPT        : Reference Collapsing under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the rules, mechanics,
 *                  and practical applications of Reference Collapsing:
 *
 *                  1. The Four Reference Collapsing Rules :
 *                     - In C++, creating a reference to a reference directly is illegal, 
 *                       but it can occur during template deduction, type aliases (`using`), 
 *                       or `decltype`.
 *                     - Rules:
 *                       * `T&  &`  --> `T&`  (Lvalue  + Lvalue  = Lvalue)
 *                       * `T&  &&` --> `T&`  (Lvalue  + Rvalue  = Lvalue)
 *                       * `T&& &`  --> `T&`  (Rvalue  + Lvalue  = Lvalue)
 *                       * `T&& &&` --> `T&&` (Rvalue  + Rvalue  = Rvalue)
 *                     - Mnemonic: An Lvalue reference (`&`) ALWAYS wins! Only `&& + &&` yields `&&`.
 *
 *                  2. Universal / Forwarding References (`T&&`) :
 *                     - When an lvalue of type `X` is passed to `template <typename T> void f(T&& arg)`:
 *                       `T` is deduced as `X&`. `T&&` becomes `X& &&` which collapses to `X&`.
 *                     - When an rvalue of type `X` is passed:
 *                       `T` is deduced as `X`. `T&&` becomes `X&&`.
 *
 *                  3. Mechanics of `std::forward` :
 *                     - How `std::forward<T>(param)` utilizes reference collapsing (`static_cast<T&&>(param)`)
 *                       to achieve Perfect Forwarding.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for runtime type name inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <type_traits> : std::is_lvalue_reference_v, std::is_rvalue_reference_v,
 *                                    std::remove_reference_t, std::is_same_v
 *                  - <utility>     : std::move for rvalue generation
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(1) deduction and collapsing per call.
 *                    Runtime Execution            : O(1) Zero-cost abstraction (no extra moves/copies).
 * SPACE COMPLEXITY : Binary footprint scales with unique deduced template signatures.
 * =====================================================================================
 */

#include <iostream>
#include <typeinfo>
#include <limits>
#include <type_traits>
#include <utility>

using namespace std;

// =====================================================================================
// HELPER FUNCTION: TYPE INSPECTION
// =====================================================================================
template <typename T>
void printTypeTraits(const char* label) {
    using RawT = std::remove_reference_t<T>;
    cout << "      [" << label << "] Deducted T = " << typeid(RawT).name()
         << " | Category: ";
    if constexpr (std::is_lvalue_reference_v<T>) {
        cout << "Lvalue Reference (T&)";
    } else if constexpr (std::is_rvalue_reference_v<T>) {
        cout << "Rvalue Reference (T&&)";
    } else {
        cout << "Value Type (Non-Ref T)";
    }
    cout << "\n";
}

// =====================================================================================
// 1. REFERENCE COLLAPSING VIA TYPE ALIASES (`using`)
// Direct demonstration of the 4 collapsing combinations.
// =====================================================================================
class TypeAliasCollapsingDemo {
public:
    static void demonstrateRules() {
        using BaseType = int;

        using LRef = BaseType&;   // int&
        using RRef = BaseType&&;  // int&&

        // Rule 1: Lvalue & + Lvalue & = Lvalue &
        using Collapse1 = LRef&;
        static_assert(std::is_same_v<Collapse1, int&>, "Rule 1 Failed: & + & should be &");

        // Rule 2: Lvalue & + Rvalue && = Lvalue &
        using Collapse2 = LRef&&;
        static_assert(std::is_same_v<Collapse2, int&>, "Rule 2 Failed: & + && should be &");

        // Rule 3: Rvalue && + Lvalue & = Lvalue &
        using Collapse3 = RRef&;
        static_assert(std::is_same_v<Collapse3, int&>, "Rule 3 Failed: && + & should be &");

        // Rule 4: Rvalue && + Rvalue && = Rvalue &&
        using Collapse4 = RRef&&;
        static_assert(std::is_same_v<Collapse4, int&&>, "Rule 4 Failed: && + && should be &&");

        cout << "    [Static Assertions Passed] All 4 reference collapsing rules verified:\n"
             << "      1. (int&)&   --> int&\n"
             << "      2. (int&)&&  --> int&\n"
             << "      3. (int&&)&  --> int&\n"
             << "      4. (int&&)&& --> int&&\n";
    }
};

// =====================================================================================
// 2. REFERENCE COLLAPSING IN UNIVERSAL / FORWARDING REFERENCES (`T&&`)
// =====================================================================================
class ForwardingReferenceDemo {
public:
    template <typename T>
    static void process(T&& param) {
        printTypeTraits<T>("Forwarding Ref T");
        printTypeTraits<decltype(param)>("Param Expression decltype(param)");
    }
};

// =====================================================================================
// 3. MECHANICS OF CUSTOM PERFECT FORWARDING USING REFERENCE COLLAPSING
// Custom implementation of std::forward to illustrate static_cast<T&&>(val)
// =====================================================================================
template <typename T>
[[nodiscard]] constexpr T&& customForward(std::remove_reference_t<T>& param) noexcept {
    // If T is an lvalue reference (e.g., int&), T&& collapses to int& && -> int&.
    // If T is a non-reference (e.g., int), T&& remains int&&.
    return static_cast<T&&>(param);
}

class CustomForwardingDemo {
public:
    static void targetFunction(int& lval) {
        cout << "      -> Called targetFunction(int& LVALUE ref): " << lval << "\n";
    }

    static void targetFunction(int&& rval) {
        cout << "      -> Called targetFunction(int&& RVALUE ref): " << rval << "\n";
    }

    template <typename T>
    static void passThrough(T&& param) {
        cout << "    [passThrough] Forwarding argument using customForward<T>...\n";
        targetFunction(customForward<T>(param));
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Reference Collapsing analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. TYPE ALIAS REFERENCE COLLAPSING
    // =====================================================================================
    cout << "\n================ 1. REFERENCE COLLAPSING RULES VIA TYPE ALIASES ================\n";
    TypeAliasCollapsingDemo::demonstrateRules();

    // =====================================================================================
    // 2. UNIVERSAL REFERENCES & DEDUCTION COLLAPSING
    // =====================================================================================
    cout << "\n================ 2. UNIVERSAL / FORWARDING REFERENCE DEDUCTION ================\n";

    int lvalueVar = userInputValue;

    cout << "  - Passing Lvalue variable (`int lvalueVar`):\n";
    // Deduced T = int&. T&& -> int& && -> int&
    ForwardingReferenceDemo::process(lvalueVar);

    cout << "\n  - Passing Rvalue expression (`userInputValue + 50`):\n";
    // Deduced T = int. T&& -> int&&
    ForwardingReferenceDemo::process(userInputValue + 50);

    cout << "\n  - Passing Rvalue via std::move (`std::move(lvalueVar)`):\n";
    // Deduced T = int. T&& -> int&&
    ForwardingReferenceDemo::process(std::move(lvalueVar));

    // =====================================================================================
    // 3. PERFECT FORWARDING MECHANICS
    // =====================================================================================
    cout << "\n================ 3. PERFECT FORWARDING MECHANICS (`customForward<T>`) ================\n";

    int targetLvalue = userInputValue * 2;

    cout << "  - Forwarding Lvalue argument:\n";
    CustomForwardingDemo::passThrough(targetLvalue);

    cout << "\n  - Forwarding Rvalue argument:\n";
    CustomForwardingDemo::passThrough(userInputValue + 500);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ REFERENCE COLLAPSING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Type Combination      | Collapsed Reference   | Key Mnemonic / Rule               |\n"
         << "+-----------------------+-----------------------+-----------------------------------+\n"
         << "| `T&  &`               | `T&`                  | Lvalue + Lvalue = Lvalue          |\n"
         << "| `T&  &&`              | `T&`                  | Lvalue + Rvalue = Lvalue (& wins) |\n"
         << "| `T&& &`               | `T&`                  | Rvalue + Lvalue = Lvalue (& wins) |\n"
         << "| `T&& &&`              | `T&&`                 | Rvalue + Rvalue = Rvalue (Only 1) |\n"
         << "| Deduced Lvalue Arg    | T = `X&`  -> `X&`     | Universal Ref collapses to Lvalue |\n"
         << "| Deduced Rvalue Arg    | T = `X`   -> `X&&`    | Universal Ref remains Rvalue      |\n"
         << "| `std::forward<T>(v)`  | `static_cast<T&&>(v)` | Uses collapsing for perfect fwd   |\n"
         << "+-----------------------+-----------------------+-----------------------------------+\n";

    return 0;
}