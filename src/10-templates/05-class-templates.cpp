/*
 * =====================================================================================
 * CONCEPT        : Class Templates under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the core syntax,
 *                  architecture, and mechanics of Class Templates in C++:
 *
 *                  1. Class Template Definition & Instantiation :
 *                     - Parameterizing class types (`template <typename T> class Container`).
 *                     - Instantiating type-specific object instances (`Container<int>`, `Container<std::string>`).
 *
 *                  2. Member Functions Defined Inside vs. Outside Class Body :
 *                     - Inline member method definitions vs. out-of-line definitions 
 *                       requiring the full `template <typename T, ...>` scope prefix.
 *
 *                  3. Default Template Arguments & Non-Type Parameters (NTTP) :
 *                     - Defaulting fallback types (`typename T = int`) and compile-time constants 
 *                       (`std::size_t Capacity = 5`).
 *
 *                  4. Multiple Template Parameters & Nested Class Templates :
 *                     - Heterogeneous key-value pairs (`KeyValuePair<KeyType, ValueType>`).
 *
 *                  5. Friend Functions / Operators inside Class Templates :
 *                     - Overloading operator `<<` as a friend function inside a template class.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>  : Console I/O streams (cin, cout, endl, flush, ostream)
 *                  - <string>    : Type std::string
 *                  - <typeinfo>  : RTTI typeid operator for type inspection
 *                  - <limits>    : Stream clearing via std::numeric_limits
 *                  - <cstddef>   : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N) distinct class template instantiations.
 *                    Runtime Execution            : O(1) Zero-cost abstraction for member access.
 * SPACE COMPLEXITY : Binary footprint scales with each unique instantiated class template type.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. FIXED-CAPACITY STACK CLASS TEMPLATE (WITH NTTP & DEFAULT TEMPLATE ARGUMENTS)
// =====================================================================================
template <typename T = int, std::size_t Capacity = 5>
class CustomStack {
private:
    T elements_[Capacity]{};
    std::size_t count_{0};

public:
    CustomStack() = default;

    // Inline Member Function: Push element onto stack
    bool push(const T& val) {
        if (count_ >= Capacity) {
            return false;
        }
        elements_[count_++] = val;
        return true;
    }

    // Member Function Declared Inline, Defined Outside Class Body
    bool pop();

    [[nodiscard]] const T& top() const {
        if (count_ == 0) {
            throw std::out_of_range("CustomStack::top() called on an empty stack!");
        }
        return elements_[count_ - 1];
    }

    [[nodiscard]] std::size_t size() const noexcept { return count_; }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }
    [[nodiscard]] bool empty() const noexcept { return count_ == 0; }

    // Friend Operator Overload for Stream Output inside Class Template
    friend std::ostream& operator<<(std::ostream& os, const CustomStack& stack) {
        os << "CustomStack<Type: " << typeid(T).name() << ", Cap: " << Capacity
           << "> [Size: " << stack.count_ << "/" << Capacity << "] -> { ";
        for (std::size_t i = 0; i < stack.count_; ++i) {
            os << stack.elements_[i] << (i + 1 < stack.count_ ? ", " : " ");
        }
        os << "}";
        return os;
    }
};

// Out-of-line Member Function Definition syntax for Class Templates
template <typename T, std::size_t Capacity>
bool CustomStack<T, Capacity>::pop() {
    if (count_ == 0) {
        return false;
    }
    --count_;
    return true;
}

// =====================================================================================
// 2. MULTI-TYPE CLASS TEMPLATE (HETEROGENEOUS PAIR)
// =====================================================================================
template <typename KeyType, typename ValueType>
class KeyValuePair {
private:
    KeyType key_;
    ValueType value_;

public:
    KeyValuePair(KeyType key, ValueType value)
        : key_(key), value_(value) {}

    [[nodiscard]] KeyType getKey() const { return key_; }
    [[nodiscard]] ValueType getValue() const { return value_; }

    void print() const {
        cout << "    [KeyValuePair] Key (" << typeid(KeyType).name() << "): " << key_
             << " | Value (" << typeid(ValueType).name() << "): " << value_ << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Class Templates analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CLASS TEMPLATE INSTANTIATION WITH DEFAULTS & EXPLICIT NTTP
    // =====================================================================================
    cout << "\n================ 1. CLASS TEMPLATE INSTANTIATION & OPERATIONS ================\n";

    // Instance A: Using default template arguments (Type = int, Capacity = 5)
    CustomStack<> defaultStack;
    defaultStack.push(userInputValue);
    defaultStack.push(userInputValue + 10);
    defaultStack.push(userInputValue + 20);
    cout << "  - Default Stack Instance: " << defaultStack << "\n";

    defaultStack.pop();
    cout << "  - After pop(): " << defaultStack << " | New Top: " << defaultStack.top() << "\n";

    // Instance B: Explicit Type (std::string) and custom NTTP Capacity (Capacity = 3)
    cout << "\n  - Creating `CustomStack<std::string, 3>`:\n";
    CustomStack<string, 3> stringStack;
    stringStack.push("Modern");
    stringStack.push("Cpp");
    stringStack.push("ClassTemplates");

    bool pushOverflow = stringStack.push("OverflowItem");
    cout << "  - String Stack Instance: " << stringStack << "\n";
    cout << "  - Attempted push on full stack result: " << (pushOverflow ? "Success" : "Failed (Buffer Full)") << "\n";

    // =====================================================================================
    // 2. MULTI-PARAMETER CLASS TEMPLATE (HETEROGENEOUS PAIRS)
    // =====================================================================================
    cout << "\n================ 2. MULTI-PARAMETER CLASS TEMPLATES ================\n";

    KeyValuePair<int, string> pair1(userInputValue, "Cluster_Node_Alpha");
    KeyValuePair<string, double> pair2("CpuFrequencyGHz", static_cast<double>(userInputValue) * 0.035);

    pair1.print();
    pair2.print();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CLASS TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Class Template Feature| C++ Syntax / Strategy             | Architectural Effect & Rules      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Basic Declaration     | `template <typename T> class C`   | Generates generic blueprint       |\n"
         << "| Object Instantiation  | `C<int> obj;`                     | Compiler creates concrete class   |\n"
         << "| Default Template Args | `template <typename T = int>`     | Fallback type if omitted by user  |\n"
         << "| NTTP (Non-Type)       | `template <..., size_t Cap>`      | Embeds compile-time sizing/values |\n"
         << "| Out-of-line Methods   | `template <...> Return C<...>::fn`| Requires full template prefix     |\n"
         << "| Multi-Type Templates  | `template <typename K, typename V>`| Encapsulates mixed domain types   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}