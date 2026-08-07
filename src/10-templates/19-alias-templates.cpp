/*
 * =====================================================================================
 * CONCEPT        : Alias Templates under Templates in Modern C++ (C++11/C++14/C++17)
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, mechanics,
 *                  and architectural benefits of Alias Templates (`using` templates):
 *
 *                  1. Basic Type Alias Templates :
 *                     - Creating clean, parameterized type aliases using `template <...> using AliasName = ...;`.
 *                     - Comparison against legacy `typedef` inside helper struct idioms.
 *
 *                  2. Partial Parameter Binding / Binding Defaults :
 *                     - Fixing specific template parameters while leaving others generic
 *                       (e.g., binding the key of a map/pair template to `std::string`).
 *
 *                  3. Non-Type Template Parameter (NTTP) Alias Templates :
 *                     - Creating aliases with fixed sizes or dimensions (e.g., `SmallBuffer<T>`
 *                       or `IntBuffer<N>`).
 *
 *                  4. The Metaprogramming Standard `_t` Suffix Idiom :
 *                     - Implementing standard-library-style type trait aliases (e.g., `RemoveRef_t<T>`
 *                       replacing `typename std::remove_reference<T>::type`).
 *
 *                  5. Nested Alias Templates inside Class Templates :
 *                     - Encapsulating type abstractions directly within generic class blueprints.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for type inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <type_traits> : std::remove_reference, std::is_same_v
 *                  - <cstddef>     : std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Type Alias Resolution : O(1) evaluated entirely at compile time.
 *                    Runtime Execution                 : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint                   : Zero runtime memory or binary overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <type_traits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. BASIC TYPE ALIAS TEMPLATE & PARTIAL PARAMETER BINDING
// =====================================================================================

// Primary class template with two type parameters
template <typename Key, typename Value>
struct KeyValuePair {
    Key key;
    Value value;

    void print(const string& label) const {
        cout << "    [" << label << "] Pair<Key: " << typeid(Key).name() 
             << ", Value: " << typeid(Value).name() << "> -> Key: " << key 
             << " | Value: " << value << "\n";
    }
};

// Alias Template: Partially binds Key type to std::string while leaving Value type generic
template <typename Value>
using StringKeyMap = KeyValuePair<string, Value>;

// Alias Template: Binds Value type to double while leaving Key type generic
template <typename Key>
using MetricEntry = KeyValuePair<Key, double>;

// =====================================================================================
// 2. NON-TYPE TEMPLATE PARAMETER (NTTP) ALIAS TEMPLATES
// =====================================================================================

template <typename T, std::size_t Capacity>
class FixedCapacityBuffer {
private:
    T storage_[Capacity]{};
    std::size_t count_{0};

public:
    FixedCapacityBuffer() = default;

    bool push(const T& item) {
        if (count_ >= Capacity) return false;
        storage_[count_++] = item;
        return true;
    }

    [[nodiscard]] std::size_t size() const noexcept { return count_; }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }

    void print(const string& bufferName) const {
        cout << "    [" << bufferName << "] Buffer<Type: " << typeid(T).name() 
             << ", Capacity: " << Capacity << "> (Size: " << count_ << "/" << Capacity << ") -> { ";
        for (std::size_t i = 0; i < count_; ++i) {
            cout << storage_[i] << (i + 1 < count_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// Alias Template 1: Fixes Capacity to 5 elements (Type remains generic)
template <typename T>
using SmallBuffer = FixedCapacityBuffer<T, 5>;

// Alias Template 2: Fixes Type to int (Capacity remains generic)
template <std::size_t Capacity>
using IntBuffer = FixedCapacityBuffer<int, Capacity>;

// =====================================================================================
// 3. TYPE TRAITS META-PROGRAMMING ALIAS (`_t` IDIOM) & POINTER ALIASES
// =====================================================================================

// Recreating the standard C++14 `_t` type trait alias pattern
template <typename T>
using CustomRemoveRef_t = typename std::remove_reference<T>::type;

// Alias template transforming any type T into a pointer type T*
template <typename T>
using PointerTo_t = T*;

// Helper demonstrating type trait alias verification
template <typename T>
void verifyTypeTraitAlias(T&& arg) {
    using CleanType = CustomRemoveRef_t<decltype(arg)>;
    static_assert(!std::is_same_v<CleanType, void>, "Type must not be void");

    cout << "    [Type Trait Alias] Deduced CleanType: " << typeid(CleanType).name() 
         << " (Removed reference qualifiers successfully)\n";
}

// =====================================================================================
// 4. NESTED ALIAS TEMPLATES INSIDE CLASS TEMPLATES
// =====================================================================================
template <typename ElementType>
class StorageAdapter {
public:
    // Alias template defined inside a class template scope
    template <std::size_t Size>
    using BufferType = FixedCapacityBuffer<ElementType, Size>;

    // Member pointer alias template usage
    using PointerType = PointerTo_t<ElementType>;

    static void executeDemo(ElementType value) {
        PointerType ptr = &value;
        cout << "    [Nested Alias Demo] ElementType: " << typeid(ElementType).name() 
             << " | Value via PointerType: " << *ptr << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Alias Templates analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC TYPE ALIAS TEMPLATES & PARTIAL BINDING
    // =====================================================================================
    cout << "\n================ 1. BASIC ALIAS TEMPLATES & PARTIAL BINDING ================\n";

    // StringKeyMap<int> expands to KeyValuePair<std::string, int>
    StringKeyMap<int> configPair{"MaxConnections", userInputValue};
    configPair.print("StringKeyMap<int>");

    // MetricEntry<int> expands to KeyValuePair<int, double>
    MetricEntry<int> metricPair{userInputValue, static_cast<double>(userInputValue) * 1.05};
    metricPair.print("MetricEntry<int>");

    // =====================================================================================
    // 2. NTTP ALIAS TEMPLATES
    // =====================================================================================
    cout << "\n================ 2. NON-TYPE TEMPLATE PARAMETER ALIASES ================\n";

    // SmallBuffer<string> expands to FixedCapacityBuffer<std::string, 5>
    SmallBuffer<string> stringBuf;
    stringBuf.push("Modern");
    stringBuf.push("Cpp");
    stringBuf.push("AliasTemplates");
    stringBuf.print("SmallBuffer<string>");

    // IntBuffer<3> expands to FixedCapacityBuffer<int, 3>
    IntBuffer<3> intBuf;
    intBuf.push(userInputValue);
    intBuf.push(userInputValue + 10);
    intBuf.push(userInputValue + 20);
    intBuf.print("IntBuffer<3>");

    // =====================================================================================
    // 3. TYPE TRAIT `_t` ALIAS IDIOM
    // =====================================================================================
    cout << "\n================ 3. TYPE TRAIT `_t` ALIAS IDIOM ================\n";

    int lvalueVar = userInputValue;
    verifyTypeTraitAlias(lvalueVar);                  // Passes lvalue reference int&
    verifyTypeTraitAlias(userInputValue + 50);        // Passes rvalue int

    // =====================================================================================
    // 4. NESTED ALIAS TEMPLATES
    // =====================================================================================
    cout << "\n================ 4. NESTED ALIAS TEMPLATES INSIDE CLASS ================\n";

    StorageAdapter<double>::executeDemo(static_cast<double>(userInputValue) * 3.14159);

    // Using nested BufferType alias template directly
    StorageAdapter<int>::BufferType<4> nestedBuffer;
    nestedBuffer.push(userInputValue);
    nestedBuffer.push(userInputValue * 2);
    nestedBuffer.print("StorageAdapter<int>::BufferType<4>");

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ ALIAS TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Concept     | C++ Syntax Example                | Architectural Benefit             |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Basic Type Alias      | `template<T> using Vec = vector<T>`| Cleaner alternative to typedef    |\n"
         << "| Partial Parameter     | `template<V> using Map = pair<str,V>`| Fixes specific template parameters|\n"
         << "| NTTP Parameter        | `template<T> using Buf5 = Buf<T,5>`| Fixes compile-time dimensions/size|\n"
         << "| Standard `_t` Trait   | `using RemoveRef_t = typename ...`| Eliminates verbose `typename ...::type`|\n"
         << "| Nested Class Alias    | `template<N> using Buf = Buffer<N>`| Parameterized type aliases in class|\n"
         << "| Zero Overhead         | Pure compile-time alias mapping   | No binary bloat, no runtime cost  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}