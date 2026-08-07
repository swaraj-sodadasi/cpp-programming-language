/*
 * =====================================================================================
 * CONCEPT        : std::enable_if under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, mechanics,
 *                  under-the-hood implementation, and applications of `std::enable_if`
 *                  and `std::enable_if_t` (C++14 alias) for SFINAE overload control:
 *
 *                  1. Custom Implementation of `enable_if` (Demystifying the Trait) :
 *                     - Rebuilding `my_enable_if` from scratch using primary templates 
 *                       and struct partial specialization to show how SFINAE occurs.
 *
 *                  2. Function Template SFINAE Strategies :
 *                     - Strategy A: Default Template Argument (`std::enable_if_t<Cond, int> = 0`).
 *                     - Strategy B: Return Type Constraint (`std::enable_if_t<Cond, ReturnType>`).
 *
 *                  3. Class Template Partial Specialization :
 *                     - Conditionally choosing class implementations based on traits using 
 *                       a dummy `typename Enable = void` template parameter.
 *
 *                  4. Constructor SFINAE & Perfect Forwarding Filtering :
 *                     - Restricting templated constructors to prevent hijacking copy/move 
 *                       constructors when passing custom types.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <type_traits> : std::enable_if, std::enable_if_t, std::is_integral_v, 
 *                                    std::is_floating_point_v, std::is_same_v, std::decay_t
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for runtime type name inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <cstddef>     : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Selection : O(1) SFINAE type substitution per overload candidate.
 *                    Runtime Execution     : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint      : Zero runtime memory or layout overhead.
 * =====================================================================================
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// 1. CUSTOM IMPLEMENTATION OF enable_if (UNDER THE HOOD)
// =====================================================================================

// Primary Template: When condition is FALSE, no nested ::type exists!
// Accessing ::type triggers substitution failure (SFINAE).
template <bool B, typename T = void>
struct CustomEnableIf {};

// Partial Specialization: When condition is TRUE, nested ::type is defined as T.
template <typename T>
struct CustomEnableIf<true, T> {
    using type = T;
};

// C++14 Style Helper Alias
template <bool B, typename T = void>
using CustomEnableIf_t = typename CustomEnableIf<B, T>::type;

// =====================================================================================
// 2. FUNCTION TEMPLATE SFINAE WITH std::enable_if_t
// =====================================================================================
class EnableIfFunctionDemo {
public:
    // Strategy A: Default Template Argument (Preferred for clarity and multiple overloads)
    template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    static void processValue(T val) {
        cout << "    [Strategy A: Integral Overload] Value: " << val 
             << " | Bitwise Left Shift (val << 1): " << (val << 1) << "\n";
    }

    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    static void processValue(T val) {
        cout << "    [Strategy A: Floating-Point Overload] Value: " << val 
             << " | Half Value (val / 2.0): " << (val / 2.0) << "\n";
    }

    // Strategy B: Return Type Constraint (Uses custom CustomEnableIf_t to demonstrate custom trait)
    template <typename T>
    static CustomEnableIf_t<std::is_same_v<T, std::string>, void> processValue(const T& val) {
        cout << "    [Strategy B: Return Type Custom enable_if (std::string)] String: \"" << val << "\"\n";
    }
};

// =====================================================================================
// 3. CLASS TEMPLATE PARTIAL SPECIALIZATION WITH std::enable_if_t
// =====================================================================================

// Primary Class Template declaration with default Enable = void parameter
template <typename T, typename Enable = void>
class SpecializedBuffer {
public:
    static void describe() {
        cout << "    [Primary Class Template] Fallback implementation for generic type: " 
             << typeid(T).name() << "\n";
    }
};

// Partial Specialization 1: Enabled for Integral Types
template <typename T>
class SpecializedBuffer<T, std::enable_if_t<std::is_integral_v<T>>> {
public:
    static void describe() {
        cout << "    [Specialized Class Template] Optimized for Integral Type: " 
             << typeid(T).name() << "\n";
    }
};

// Partial Specialization 2: Enabled for Floating-Point Types
template <typename T>
class SpecializedBuffer<T, std::enable_if_t<std::is_floating_point_v<T>>> {
public:
    static void describe() {
        cout << "    [Specialized Class Template] Optimized for Floating-Point Type: " 
             << typeid(T).name() << "\n";
    }
};

// =====================================================================================
// 4. CONSTRUCTOR SFINAE & PERFECT FORWARDING FILTERING
// Disables forwarding constructor when argument is of same class type (prevents hijacking copy ctor)
// =====================================================================================
class SmartWrapper {
private:
    string name_;

public:
    // Copy Constructor
    SmartWrapper(const SmartWrapper& other) : name_(other.name_) {
        cout << "    [SmartWrapper Copy Constructor Called]\n";
    }

    // Move Constructor
    SmartWrapper(SmartWrapper&& other) noexcept : name_(std::move(other.name_)) {
        cout << "    [SmartWrapper Move Constructor Called]\n";
    }

    // Forwarding Constructor constrained with std::enable_if_t
    // Disabled if decayed U is SmartWrapper, allowing copy/move constructors to handle same-type args!
    template <typename U, std::enable_if_t<!std::is_same_v<std::decay_t<U>, SmartWrapper>, int> = 0>
    explicit SmartWrapper(U&& arg) : name_(std::forward<U>(arg)) {
        cout << "    [SmartWrapper Templated Forwarding Constructor Called] Injected: " << name_ << "\n";
    }

    [[nodiscard]] const string& getName() const noexcept { return name_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for std::enable_if analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. FUNCTION TEMPLATE SFINAE WITH std::enable_if_t
    // =====================================================================================
    cout << "\n================ 1. FUNCTION TEMPLATE SFINAE ================\n";

    int intVal = userInputValue;
    double dblVal = static_cast<double>(userInputValue) * 1.5;
    string strVal = "Enable_If_Modern_Cpp";

    EnableIfFunctionDemo::processValue(intVal);  // Integral overload
    EnableIfFunctionDemo::processValue(dblVal);  // Floating-point overload
    EnableIfFunctionDemo::processValue(strVal);  // Custom enable_if_t return type overload

    // =====================================================================================
    // 2. CLASS TEMPLATE PARTIAL SPECIALIZATION
    // =====================================================================================
    cout << "\n================ 2. CLASS TEMPLATE SPECIALIZATION WITH std::enable_if_t ================\n";

    SpecializedBuffer<int>::describe();
    SpecializedBuffer<double>::describe();
    SpecializedBuffer<string>::describe(); // Uses fallback primary template

    // =====================================================================================
    // 3. CONSTRUCTOR SFINAE & PERFECT FORWARDING FILTERING
    // =====================================================================================
    cout << "\n================ 3. CONSTRUCTOR SFINAE (PERFECT FORWARDING FILTER) ================\n";

    cout << "  - Creating SmartWrapper from const char*:\n";
    SmartWrapper wrapper1("Initial_Node_Value");

    cout << "\n  - Creating SmartWrapper via Copy Constructor from wrapper1:\n";
    SmartWrapper wrapper2(wrapper1); // Correctly dispatches to Copy Ctor (not forwarding ctor!)

    cout << "\n  - Creating SmartWrapper via Move Constructor:\n";
    SmartWrapper wrapper3(std::move(wrapper1));

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ std::enable_if SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| enable_if Strategy    | C++ Syntax Example                | Architectural Purpose & Effect    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Default Template Arg  | `template <typename T,            | Cleanest strategy for function    |\n"
         << "|                       |  enable_if_t<Cond, int> = 0>`     | template overload selection       |\n"
         << "| Return Type Constraint| `enable_if_t<Cond, ReturnType>`   | Constrains function return type;  |\n"
         << "|                       | `func(T val)`                     | useful when return type is fixed  |\n"
         << "| Class Specialization  | `template <typename T>`           | Selects class layout/blueprint    |\n"
         << "|                       | `class C<T, enable_if_t<Cond>>`   | conditionally based on traits     |\n"
         << "| Constructor SFINAE    | `enable_if_t<!is_same_v<decay_t<U>| Prevents perfect forwarding ctors |\n"
         << "|                       |  Class>, int> = 0`                | from hijacking copy/move ctors    |\n"
         << "| C++14 Alias           | `std::enable_if_t<B, T>`          | Shorthand replacing verbose       |\n"
         << "|                       |                                   | `typename std::enable_if<B,T>::type`|\n"
         << "| Modern Replacement    | C++20 `requires` clauses          | Replaces enable_if with clean     |\n"
         << "| (C++20)               |                                   | language-level concepts           |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}