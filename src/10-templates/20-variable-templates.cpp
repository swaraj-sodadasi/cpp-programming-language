/*
 * =====================================================================================
 * CONCEPT        : Variable Templates under Templates in Modern C++ (C++14 / C++17)
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, rules,
 *                  specializations, and idioms of Variable Templates in C++:
 *
 *                  1. Parameterized Constants (Mathematical & Physical) :
 *                     - Defining compile-time constants parameterized by type 
 *                       (`template <typename T> constexpr T pi = T(3.1415926535897932385L)`).
 *
 *                  2. Full & Partial Specialization of Variable Templates :
 *                     - Full specialization for specific target types (`int`, `double`, `std::string`).
 *                     - Partial specialization for type categories (e.g., pointer types `T*`).
 *
 *                  3. Type Traits `_v` Suffix Idiom (C++17) :
 *                     - Simplifying meta-programming query expressions by wrapping `::value`
 *                       trait struct lookups with variable templates.
 *
 *                  4. Non-Type Template Parameter (NTTP) Variable Templates :
 *                     - Compile-time recursive computations using NTTP variable templates 
 *                       (e.g., compile-time factorial calculation).
 *
 *                  5. Nested Member Variable Templates :
 *                     - Static member variable templates declared within class templates.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for runtime type inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <type_traits> : std::is_integral, std::is_floating_point
 *                  - <cstddef>     : std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(1) evaluated entirely at compile time.
 *                    Runtime Execution             : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint       : Zero runtime memory allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <limits>
#include <type_traits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. BASIC PARAMETERIZED MATHEMATICAL CONSTANTS
// =====================================================================================

// Parameterized Constant Pi
template <typename T>
constexpr T pi_v = T(3.141592653589793238462643383279502884L);

// Parameterized Constant Euler's Number e
template <typename T>
constexpr T e_v = T(2.718281828459045235360287471352662497L);

// =====================================================================================
// 2. FULL AND PARTIAL SPECIALIZATION OF VARIABLE TEMPLATES
// =====================================================================================

// Primary Variable Template (Generic fallback)
template <typename T>
constexpr const char* type_description_v = "Generic / Unspecialized Type";

// Full Specialization for `int`
template <>
constexpr const char* type_description_v<int> = "32-bit Signed Integer (int)";

// Full Specialization for `double`
template <>
constexpr const char* type_description_v<double> = "64-bit IEEE-754 Floating Point (double)";

// Full Specialization for `std::string`
template <>
constexpr const char* type_description_v<string> = "Standard Character Sequence (std::string)";

// Partial Specialization for Pointer Types `T*`
template <typename T>
constexpr const char* type_description_v<T*> = "Pointer to Type (T*)";

// =====================================================================================
// 3. TYPE TRAITS `_v` SUFFIX IDIOM (SIMULATING C++17 TRAIT ALIASES)
// =====================================================================================

// Variable template wrapping std::is_integral<T>::value
template <typename T>
constexpr bool is_integral_custom_v = std::is_integral<T>::value;

// Variable template combining multiple type traits (Numeric Check)
template <typename T>
constexpr bool is_numeric_v = std::is_integral<T>::value || std::is_floating_point<T>::value;

// =====================================================================================
// 4. NON-TYPE TEMPLATE PARAMETER (NTTP) & RECURSIVE VARIABLE TEMPLATES
// =====================================================================================

// Primary Recursive Variable Template for Compile-Time Factorial
template <std::size_t N>
constexpr std::size_t factorial_v = N * factorial_v<N - 1>;

// Specialization Base Case (0! = 1)
template <>
constexpr std::size_t factorial_v<0> = 1;

// =====================================================================================
// 5. MEMBER VARIABLE TEMPLATES INSIDE CLASS BLUEPRINTS
// =====================================================================================
template <typename UnitSystemTag>
struct PhysicalConstants {
    // Nested static member variable template
    template <typename T>
    static constexpr T speed_of_light = T(299792458.0L); // m/s

    template <typename T>
    static constexpr T standard_gravity = T(9.80665L);   // m/s^2
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Variable Templates analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. PARAMETERIZED MATHEMATICAL CONSTANTS
    // =====================================================================================
    cout << "\n================ 1. PARAMETERIZED MATHEMATICAL CONSTANTS ================\n";

    cout << "  - pi_v<float>       : " << pi_v<float> << " (Type: float)\n";
    cout << "  - pi_v<double>      : " << pi_v<double> << " (Type: double)\n";
    cout << "  - pi_v<long double> : " << pi_v<long double> << " (Type: long double)\n";
    cout << "  - e_v<double>       : " << e_v<double> << "\n";

    // Dynamic calculation using variable templates
    double scaledCircleArea = pi_v<double> * userInputValue * userInputValue;
    cout << "  - Circle Area (radius = " << userInputValue << ") = pi_v<double> * r^2 = " << scaledCircleArea << "\n";

    // =====================================================================================
    // 2. FULL AND PARTIAL SPECIALIZATION
    // =====================================================================================
    cout << "\n================ 2. FULL & PARTIAL SPECIALIZATION OF VARIABLE TEMPLATES ================\n";

    cout << "  - type_description_v<int>         : " << type_description_v<int> << "\n";
    cout << "  - type_description_v<double>      : " << type_description_v<double> << "\n";
    cout << "  - type_description_v<std::string> : " << type_description_v<string> << "\n";
    cout << "  - type_description_v<int*>        : " << type_description_v<int*> << " [Partial Spec]\n";
    cout << "  - type_description_v<char>        : " << type_description_v<char> << " [Primary Fallback]\n";

    // =====================================================================================
    // 3. TYPE TRAITS `_v` IDIOM
    // =====================================================================================
    cout << "\n================ 3. TYPE TRAITS `_v` IDIOM ================\n";

    cout << "  - is_integral_custom_v<int>       : " << (is_integral_custom_v<int> ? "TRUE" : "FALSE") << "\n";
    cout << "  - is_integral_custom_v<double>    : " << (is_integral_custom_v<double> ? "TRUE" : "FALSE") << "\n";
    cout << "  - is_numeric_v<double>            : " << (is_numeric_v<double> ? "TRUE" : "FALSE") << "\n";
    cout << "  - is_numeric_v<std::string>       : " << (is_numeric_v<string> ? "TRUE" : "FALSE") << "\n";

    // =====================================================================================
    // 4. NTTP RECURSIVE VARIABLE TEMPLATES
    // =====================================================================================
    cout << "\n================ 4. NTTP RECURSIVE VARIABLE TEMPLATES ================\n";

    cout << "  - Compile-Time Factorial factorial_v<5>  : " << factorial_v<5> << "\n";
    cout << "  - Compile-Time Factorial factorial_v<10> : " << factorial_v<10> << "\n";

    // =====================================================================================
    // 5. MEMBER VARIABLE TEMPLATES
    // =====================================================================================
    cout << "\n================ 5. MEMBER VARIABLE TEMPLATES ================\n";

    struct StandardMetricTag {};
    cout << "  - Speed of Light (float) : " 
         << PhysicalConstants<StandardMetricTag>::speed_of_light<float> << " m/s\n";
    cout << "  - Gravity (double)       : " 
         << PhysicalConstants<StandardMetricTag>::standard_gravity<double> << " m/s^2\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ VARIABLE TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Variable Template Form| C++ Implementation Syntax         | Purpose / Architectural Benefit   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Parameterized Constant| `template<typename T> constexpr T`| Typed precision for constants     |\n"
         << "| Full Specialization   | `template<> constexpr T var<int>` | Type-specific constant overrides  |\n"
         << "| Partial Specialization| `template<T> constexpr T var<T*>` | Category-based value customization|\n"
         << "| Type Trait `_v` Idiom | `constexpr bool trait_v = ...`    | Eliminates verbose `::value` lookup|\n"
         << "| NTTP Recursion        | `factorial_v<N> = N * ...`        | Pure compile-time math evaluation |\n"
         << "| Member Variable Temp  | `static constexpr T var = ...`    | Parameterized class constants     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}