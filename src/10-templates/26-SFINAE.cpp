/*
 * =====================================================================================
 * CONCEPT        : SFINAE (Substitution Failure Is Not An Error) under Templates
 * DESCRIPTION    : Production-grade executable code demonstrating the core principles,
 *                  mechanics, idioms, and applications of SFINAE in C++:
 *
 *                  1. SFINAE Core Rule :
 *                     - If an invalid type or expression is created during template 
 *                       argument substitution, the compiler discards that overload candidate 
 *                       from the overload resolution set instead of issuing a compile error.
 *
 *                  2. Function Template SFINAE (`std::enable_if_t`) :
 *                     - Conditionally enabling/disabling function template overloads based on
 *                       compile-time type conditions (e.g., integral vs floating-point vs non-arithmetic).
 *
 *                  3. Expression SFINAE & Detection Idiom (`std::void_t` / `std::declval`) :
 *                     - Checking for the existence of specific member functions, nested types, or 
 *                       valid expressions (`toString()`) at compile time.
 *
 *                  4. Class Template SFINAE (Partial Specialization with `std::enable_if_t`) :
 *                     - Conditionally choosing class template blueprint implementations based on type traits.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <type_traits> : std::enable_if_t, std::is_integral_v, std::is_floating_point_v, 
 *                                    std::void_t, std::true_type, std::false_type
 *                  - <utility>     : std::declval for expression inspection without instantiation
 *                  - <string>      : Type std::string
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <cstddef>     : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Overload Resolution : O(N) where N is overload candidates.
 *                    Runtime Execution               : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint                 : Zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>
#include <type_traits>
#include <utility>
#include <string>
#include <limits>

using namespace std;

// =====================================================================================
// 1. FUNCTION TEMPLATE SFINAE WITH std::enable_if_t
// Overload filtering based on type properties.
// =====================================================================================
class SfinaeFunctionOverloads {
public:
    // Overload 1: Enabled strictly for integral types
    template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    static void processData(T val) {
        cout << "    [SFINAE Overload 1 (Integral)] Value: " << val 
             << " | Bitwise Shift (val << 1): " << (val << 1) << "\n";
    }

    // Overload 2: Enabled strictly for floating-point types
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    static void processData(T val) {
        cout << "    [SFINAE Overload 2 (Floating-Point)] Value: " << val 
             << " | Half Value (val / 2.0): " << (val / 2.0) << "\n";
    }

    // Overload 3: Enabled for non-arithmetic types (e.g., std::string)
    template <typename T, std::enable_if_t<!std::is_arithmetic_v<T>, int> = 0>
    static void processData(const T& val) {
        cout << "    [SFINAE Overload 3 (Non-Arithmetic)] Value: \"" << val << "\"\n";
    }
};

// =====================================================================================
// 2. EXPRESSION SFINAE & DETECTION IDIOM (std::void_t + std::declval)
// =====================================================================================

// Helper struct to test if T has a `.toString()` member function
template <typename T, typename = void>
struct HasToString : std::false_type {};

// Specialization triggers SFINAE: Substitution fails if T does not have .toString()
template <typename T>
struct HasToString<T, std::void_t<decltype(std::declval<T>().toString())>> : std::true_type {};

template <typename T>
inline constexpr bool HasToString_v = HasToString<T>::value;

// Types used to demonstrate Expression SFINAE
struct CustomPrintable {
    [[nodiscard]] string toString() const {
        return "CustomPrintable_Object_State_OK";
    }
};

struct NonPrintable {};

// Function template leveraging Expression SFINAE via SFINAE overload selection
class StringConverter {
public:
    // Selected if T has .toString() method
    template <typename T, std::enable_if_t<HasToString_v<T>, int> = 0>
    static void convertAndPrint(const T& obj) {
        cout << "    [Expression SFINAE Match] Invoked .toString(): " << obj.toString() << "\n";
    }

    // Fallback selected if T does NOT have .toString() method
    template <typename T, std::enable_if_t<!HasToString_v<T>, int> = 0>
    static void convertAndPrint(const T&) {
        cout << "    [Expression SFINAE Fallback] Type lacks .toString() method! Default handled.\n";
    }
};

// =====================================================================================
// 3. CLASS TEMPLATE SFINAE VIA PARTIAL SPECIALIZATION
// =====================================================================================

// Primary Class Template Blueprint
template <typename T, typename Enable = void>
class DataSerializer {
public:
    static void serialize(const T& val) {
        (void)val; // Suppress unused parameter warning
        cout << "    [Primary Class Serializer] Generic fall-back serialization for non-integral data.\n";
    }
};

// SFINAE Class Partial Specialization for Integral Types
template <typename T>
class DataSerializer<T, std::enable_if_t<std::is_integral_v<T>>> {
public:
    static void serialize(const T& val) {
        cout << "    [Specialized Class Serializer (Integral)] Fast byte-dump serialization for: " 
             << val << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for SFINAE analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. FUNCTION TEMPLATE SFINAE
    // =====================================================================================
    cout << "\n================ 1. FUNCTION TEMPLATE SFINAE (std::enable_if_t) ================\n";

    int intVal = userInputValue;
    double dblVal = static_cast<double>(userInputValue) * 1.5;
    string strVal = "SFINAE_Overload_Selection";

    SfinaeFunctionOverloads::processData(intVal);  // Triggers Integral Overload
    SfinaeFunctionOverloads::processData(dblVal);  // Triggers Floating-Point Overload
    SfinaeFunctionOverloads::processData(strVal);  // Triggers Non-Arithmetic Overload

    // =====================================================================================
    // 2. EXPRESSION SFINAE & DETECTION IDIOM
    // =====================================================================================
    cout << "\n================ 2. EXPRESSION SFINAE (DETECTION IDIOM) ================\n";

    CustomPrintable printableObj;
    NonPrintable nonPrintableObj;

    cout << "  - Testing CustomPrintable struct:\n";
    StringConverter::convertAndPrint(printableObj);

    cout << "\n  - Testing NonPrintable struct:\n";
    StringConverter::convertAndPrint(nonPrintableObj);

    // =====================================================================================
    // 3. CLASS TEMPLATE PARTIAL SPECIALIZATION SFINAE
    // =====================================================================================
    cout << "\n================ 3. CLASS TEMPLATE PARTIAL SPECIALIZATION SFINAE ================\n";

    cout << "  - Serializing int (Triggers Specialized Integral Class):\n";
    DataSerializer<int>::serialize(userInputValue);

    cout << "\n  - Serializing std::string (Triggers Primary Fallback Class):\n";
    DataSerializer<string>::serialize(string("Generic_Buffer"));

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ SFINAE UNDER TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| SFINAE Technique      | C++ Implementation Syntax         | Architectural Behavior & Purpose  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Function Overloading  | `std::enable_if_t<Cond, int> = 0` | Filters function overloads out of |\n"
         << "|                       |                                   | candidate set if condition fails  |\n"
         << "| Expression SFINAE     | `std::void_t<decltype(expr)>`     | Checks if methods/expressions are |\n"
         << "|                       |                                   | valid without runtime errors      |\n"
         << "| Detection Idiom       | `std::declval<T>().method()`      | Evaluates method existence at     |\n"
         << "|                       |                                   | compile time                      |\n"
         << "| Class Specialization  | `template<T> class C<T, enable>`  | Conditionally selects class layout|\n"
         << "|                       |                                   | or implementation blueprints      |\n"
         << "| Modern C++20 Replacement| `requires` clauses & Concepts    | Replaces verbose SFINAE with      |\n"
         << "|                       |                                   | readable compiler constraints     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}