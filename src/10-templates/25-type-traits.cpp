/*
 * =====================================================================================
 * CONCEPT        : Type Traits under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating standard and custom
 *                  type traits, type transformations, static assertions, and SFINAE
 *                  overload selection using <type_traits>:
 *
 *                  1. Type Inspection Traits :
 *                     - Querying compile-time properties (`std::is_integral_v`, 
 *                       `std::is_floating_point_v`, `std::is_pointer_v`, `std::is_class_v`).
 *
 *                  2. Type Transformation Traits :
 *                     - Modifying type qualifiers (`std::remove_reference_t`, `std::remove_const_t`, 
 *                       `std::decay_t`, `std::add_pointer_t`, `std::remove_pointer_t`).
 *
 *                  3. Custom Type Traits Implementation :
 *                     - Building type traits from scratch using template specialization and 
 *                       `std::integral_constant` (`std::true_type` / `std::false_type`).
 *
 *                  4. SFINAE & Overload Selection (`std::enable_if_t`) :
 *                     - Filtering function template overloads based on type traits properties.
 *                     - Fixed ternary operand compatibility issue by using `std::remove_pointer_t<T>{}`
 *                       instead of `T{}` when dereferencing pointers.
 *
 *                  5. Compile-Time Enforcements (`static_assert`) :
 *                     - Enforcing strict type constraints at compile time using traits.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <type_traits> : Standard type traits, SFINAE, std::integral_constant, std::remove_pointer_t
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for runtime type name inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <cstddef>     : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(1) trait evaluation per type instantiation.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint                   : Zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// 1. STANDARD TYPE INSPECTION TRAITS
// =====================================================================================
template <typename T>
class TypeInspector {
public:
    static void inspect(const string& label) {
        cout << "    [" << label << "] Type: " << typeid(T).name() << "\n"
             << "      - std::is_integral_v       : " << (std::is_integral_v<T> ? "TRUE" : "FALSE") << "\n"
             << "      - std::is_floating_point_v : " << (std::is_floating_point_v<T> ? "TRUE" : "FALSE") << "\n"
             << "      - std::is_pointer_v        : " << (std::is_pointer_v<T> ? "TRUE" : "FALSE") << "\n"
             << "      - std::is_class_v          : " << (std::is_class_v<T> ? "TRUE" : "FALSE") << "\n";
    }
};

// =====================================================================================
// 2. TYPE TRANSFORMATION TRAITS
// =====================================================================================
template <typename T>
class TransformationDemo {
public:
    static void demonstrate() {
        using RawType = std::remove_const_t<std::remove_reference_t<T>>;
        using DecayedType = std::decay_t<T>;
        using PointerType = std::add_pointer_t<RawType>;

        cout << "    [Transformation Input] Raw Expression Type: " << typeid(T).name() << "\n"
             << "      - After remove_const_t & remove_reference_t : " << typeid(RawType).name() << "\n"
             << "      - After std::decay_t                         : " << typeid(DecayedType).name() << "\n"
             << "      - After std::add_pointer_t                   : " << typeid(PointerType).name() << "\n";
    }
};

// =====================================================================================
// 3. CUSTOM TYPE TRAIT IMPLEMENTATION (FROM SCRATCH)
// Custom trait checking if a type T is a pointer type using specialization
// =====================================================================================
template <typename T>
struct CustomIsPointer : std::false_type {};

template <typename T>
struct CustomIsPointer<T*> : std::true_type {};

// Helper C++17 `_v` variable template for custom trait
template <typename T>
inline constexpr bool CustomIsPointer_v = CustomIsPointer<T>::value;

// Custom trait checking if two types are identical
template <typename T, typename U>
struct CustomIsSame : std::false_type {};

template <typename T>
struct CustomIsSame<T, T> : std::true_type {};

template <typename T, typename U>
inline constexpr bool CustomIsSame_v = CustomIsSame<T, U>::value;

// =====================================================================================
// 4. SFINAE & OVERLOAD SELECTION WITH std::enable_if_t
// =====================================================================================
class TraitOverloadProcessor {
public:
    // Overload 1: Enabled strictly for integral types
    template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    static void process(T val) {
        cout << "    [SFINAE Overload: Integral Type] Value: " << val 
             << " | Shifted Value (val << 1): " << (val << 1) << "\n";
    }

    // Overload 2: Enabled strictly for floating-point types
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    static void process(T val) {
        cout << "    [SFINAE Overload: Floating-Point Type] Value: " << val 
             << " | Half Value (val / 2.0): " << (val / 2.0) << "\n";
    }

    // Overload 3: Enabled for pointer types
    // RESOLVED: Replaced `T{}` with `std::remove_pointer_t<T>{}` in ternary operator 
    // to match element type of `*ptr` and prevent "Incompatible operand types ('int' and 'int *')"
    template <typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
    static void process(T ptr) {
        using ElementType = std::remove_pointer_t<T>;
        cout << "    [SFINAE Overload: Pointer Type] Address: " << static_cast<const void*>(ptr) 
             << " | Dereferenced Value: " << (ptr ? *ptr : ElementType{}) << "\n";
    }
};

// =====================================================================================
// 5. COMPILE-TIME ENFORCEMENT WITH static_assert AND TRAITS
// =====================================================================================
template <typename T>
class ConstrainedBuffer {
    // Compile-time check enforcing that T must be trivially copyable or std::string
    static_assert(std::is_trivially_copyable_v<T> || std::is_same_v<T, std::string>,
                  "ConstrainedBuffer Error: Type T must be trivially copyable or std::string!");

private:
    T element_{};

public:
    explicit ConstrainedBuffer(T val) : element_(val) {}

    void render() const {
        cout << "    [ConstrainedBuffer<" << typeid(T).name() << ">] Stored Element: " << element_ << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Type Traits analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. TYPE INSPECTION TRAITS
    // =====================================================================================
    cout << "\n================ 1. STANDARD TYPE INSPECTION TRAITS ================\n";
    TypeInspector<int>::inspect("Type: int");
    TypeInspector<double*>::inspect("Type: double*");
    TypeInspector<string>::inspect("Type: std::string");

    // =====================================================================================
    // 2. TYPE TRANSFORMATION TRAITS
    // =====================================================================================
    cout << "\n================ 2. TYPE TRANSFORMATION TRAITS ================\n";
    TransformationDemo<const int&>::demonstrate();
    TransformationDemo<const char[10]>::demonstrate();

    // =====================================================================================
    // 3. CUSTOM TYPE TRAITS IMPLEMENTATION
    // =====================================================================================
    cout << "\n================ 3. CUSTOM TYPE TRAITS IMPLEMENTATION ================\n";
    cout << "  - CustomIsPointer_v<int>    : " << (CustomIsPointer_v<int> ? "TRUE" : "FALSE") << "\n";
    cout << "  - CustomIsPointer_v<int*>   : " << (CustomIsPointer_v<int*> ? "TRUE" : "FALSE") << "\n";
    cout << "  - CustomIsSame_v<int, int>  : " << (CustomIsSame_v<int, int> ? "TRUE" : "FALSE") << "\n";
    cout << "  - CustomIsSame_v<int, double>: " << (CustomIsSame_v<int, double> ? "TRUE" : "FALSE") << "\n";

    // =====================================================================================
    // 4. SFINAE OVERLOAD SELECTION
    // =====================================================================================
    cout << "\n================ 4. SFINAE OVERLOAD SELECTION (std::enable_if_t) ================\n";
    
    int intVal = userInputValue;
    double dblVal = static_cast<double>(userInputValue) * 1.5;
    int* ptrVal = &intVal;

    TraitOverloadProcessor::process(intVal);  // Dispatches to Integral Overload
    TraitOverloadProcessor::process(dblVal);  // Dispatches to Floating-Point Overload
    TraitOverloadProcessor::process(ptrVal);  // Dispatches to Pointer Overload

    // =====================================================================================
    // 5. COMPILE-TIME ENFORCEMENT WITH static_assert
    // =====================================================================================
    cout << "\n================ 5. COMPILE-TIME ENFORCEMENT (static_assert) ================\n";
    ConstrainedBuffer<int> safeIntBuf(userInputValue);
    safeIntBuf.render();

    ConstrainedBuffer<string> safeStrBuf("Type_Trait_Safe_String");
    safeStrBuf.render();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TYPE TRAITS UNDER TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Trait Category        | C++ Implementation Example        | Architectural Purpose & Benefit   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Inspection Traits     | `std::is_integral_v<T>`           | Inspects type properties at       |\n"
         << "|                       | `std::is_pointer_v<T>`            | compile time without runtime cost |\n"
         << "| Transformation Traits | `std::remove_reference_t<T>`      | Modifies qualifiers (const, ref,  |\n"
         << "|                       | `std::decay_t<T>`                 | array bounds) to generate new type|\n"
         << "| Custom Trait Creation | `struct Trait : std::false_type`  | Extends meta-programming with     |\n"
         << "|                       | `struct Trait<T*>: std::true_type`| domain-specific type inspection   |\n"
         << "| SFINAE Overloading    | `std::enable_if_t<Condition, int>`| Conditionally includes or excludes|\n"
         << "|                       |                                   | function overloads from compile   |\n"
         << "| Static Enforcements   | `static_assert(trait_v<T>, msg)`  | Halts compilation early if type   |\n"
         << "|                       |                                   | invariants are violated           |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}