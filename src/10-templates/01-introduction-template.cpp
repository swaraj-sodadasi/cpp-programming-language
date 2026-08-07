/*
 * =====================================================================================
 * CONCEPT        : Introduction to Templates in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the foundational mechanics, syntax, and capabilities of Templates in C++:
 *
 *                  1. Function Templates :
 *                     - Generic functions operating on arbitrary types without code duplication.
 *                     - Implicit type deduction vs. explicit template argument specification.
 *                     - Multi-type template parameters (`template <typename T1, typename T2>`).
 *
 *                  2. Class Templates :
 *                     - Parameterized types for building generic containers and wrappers.
 *                     - Member function definitions inside and outside the class body.
 *
 *                  3. Non-Type Template Parameters (NTTP) :
 *                     - Passing compile-time values (integers, sizes) directly to template parameters.
 *
 *                  4. Default Template Arguments :
 *                     - Providing default fallback types and non-type constants.
 *
 *                  5. Template Instantiation Mechanics :
 *                     - How the compiler generates concrete, type-specific code at compile time
 *                       (Zero-runtime-overhead abstraction).
 *
 * TIME COMPLEXITY  : Compilation Phase : O(N * M) where N is template size and M is distinct types.
 *                    Runtime Phase     : O(1) Zero-cost abstraction; identical to hand-written code.
 * SPACE COMPLEXITY : Binary Footprint  : Monomorphization generates type-specific binary instances.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. CUSTOM TYPE FOR TEMPLATE COMPATIBILITY DEMONSTRATION
// Demonstrates that templates work with user-defined types that implement required operators.
// =====================================================================================
struct CustomPoint {
    int x{0};
    int y{0};

    // Overloading '>' operator required by getMaximum() template function
    bool operator>(const CustomPoint& other) const noexcept {
        return (x * x + y * y) > (other.x * other.x + other.y * other.y);
    }

    // Overloading '<<' operator required by console stream output
    friend std::ostream& operator<<(std::ostream& os, const CustomPoint& pt) {
        return os << "Point(" << pt.x << ", " << pt.y << ")";
    }
};

// =====================================================================================
// 2. FUNCTION TEMPLATES
// Generic functions that work across multiple data types without code redundancy.
// =====================================================================================
class FunctionTemplatesDemo {
public:
    // Single-type Function Template: Computes maximum of two generic values
    template <typename T>
    [[nodiscard]] static T getMaximum(T a, T b) {
        return (a > b) ? a : b;
    }

    // Multi-type Function Template: Prints a key-value pair of distinct generic types
    template <typename T1, typename T2>
    static void printKeyValuePair(const string& label, const T1& key, const T2& value) {
        cout << "    [" << label << "] Key (" << typeid(T1).name() << "): " << key 
             << " | Value (" << typeid(T2).name() << "): " << value << "\n";
    }

    // Swapping values using generic reference parameters
    template <typename T>
    static void genericSwap(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> && 
                                                  std::is_nothrow_move_assignable_v<T>) {
        T temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }
};

// =====================================================================================
// 3. CLASS TEMPLATES WITH NTTP & DEFAULT TEMPLATE ARGUMENTS
// Generic container encapsulation with compile-time configuration parameters.
// =====================================================================================

// Template parameters: T (type defaulted to int), Capacity (non-type defaulted to 5)
template <typename T = int, std::size_t Capacity = 5>
class FixedCapacityBuffer {
private:
    T storage_[Capacity]{};
    std::size_t currentSize_{0};

public:
    FixedCapacityBuffer() = default;

    // Push an item into the generic buffer
    bool push(const T& element) {
        if (currentSize_ >= Capacity) {
            return false;
        }
        storage_[currentSize_++] = element;
        return true;
    }

    [[nodiscard]] std::size_t size() const noexcept { return currentSize_; }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }

    // Const indexing operator
    const T& operator[](std::size_t index) const {
        return storage_[index];
    }

    // Print contents of the template container
    void printBuffer(const string& bufferName) const {
        cout << "    [" << bufferName << "] Container<Type: " << typeid(T).name() 
             << ", Capacity: " << Capacity << "> (Size: " << currentSize_ << "/" << Capacity << ") -> { ";
        for (std::size_t i = 0; i < currentSize_; ++i) {
            cout << storage_[i] << (i + 1 < currentSize_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// Member function defined outside class body syntax demonstration
template <typename T, std::size_t Capacity>
class OutOfLineTemplateDemo {
private:
    T value_;

public:
    explicit OutOfLineTemplateDemo(T val);
    T getValue() const;
};

// Definition outside class body requires full template declaration prefix
template <typename T, std::size_t Capacity>
OutOfLineTemplateDemo<T, Capacity>::OutOfLineTemplateDemo(T val) : value_(val) {}

template <typename T, std::size_t Capacity>
T OutOfLineTemplateDemo<T, Capacity>::getValue() const {
    return value_;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Templates analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. FUNCTION TEMPLATES & TYPE DEDUCTION
    // =====================================================================================
    cout << "\n================ 1. FUNCTION TEMPLATES & TYPE DEDUCTION ================\n";

    // Scenario A: Implicit Type Deduction (Compiler infers T = int)
    int intA = userInputValue;
    int intB = userInputValue + 50;
    int maxInt = FunctionTemplatesDemo::getMaximum(intA, intB);
    cout << "  - Implicit Deduction (int): max(" << intA << ", " << intB << ") = " << maxInt << "\n";

    // Scenario B: Implicit Type Deduction (Compiler infers T = double)
    double doubleA = static_cast<double>(userInputValue) * 1.5;
    double doubleB = 88.75;
    double maxDouble = FunctionTemplatesDemo::getMaximum(doubleA, doubleB);
    cout << "  - Implicit Deduction (double): max(" << doubleA << ", " << doubleB << ") = " << maxDouble << "\n";

    // Scenario C: Explicit Template Argument Specification (Overrides implicit ambiguity)
    double explicitMax = FunctionTemplatesDemo::getMaximum<double>(userInputValue, 75.5);
    cout << "  - Explicit Specification <double>(int, double): max = " << explicitMax << "\n";

    // Scenario D: Custom User-Defined Type
    CustomPoint pt1{3, 4};  // Magnitude squared = 25
    CustomPoint pt2{1, 10}; // Magnitude squared = 101
    CustomPoint maxPoint = FunctionTemplatesDemo::getMaximum(pt1, pt2);
    cout << "  - Custom Point Deduction: max(" << pt1 << ", " << pt2 << ") = " << maxPoint << "\n";

    // Scenario E: Multi-Type Function Templates
    cout << "\n  - Multi-Type Function Templates:\n";
    FunctionTemplatesDemo::printKeyValuePair("Config 1", "MaxConnections", userInputValue);
    FunctionTemplatesDemo::printKeyValuePair("Config 2", 101, "ServerCluster_Alpha");

    // =====================================================================================
    // 2. CLASS TEMPLATES & NON-TYPE TEMPLATE PARAMETERS (NTTP)
    // =====================================================================================
    cout << "\n================ 2. CLASS TEMPLATES & NON-TYPE PARAMETERS (NTTP) ================\n";

    // Instance 1: Default Template Arguments (Type = int, Capacity = 5)
    FixedCapacityBuffer<> defaultBuffer;
    defaultBuffer.push(userInputValue);
    defaultBuffer.push(userInputValue + 10);
    defaultBuffer.push(userInputValue + 20);
    defaultBuffer.printBuffer("DefaultBuffer");

    // Instance 2: Explicit Type (std::string) & Custom NTTP Capacity (Size = 3)
    FixedCapacityBuffer<string, 3> stringBuffer;
    stringBuffer.push("Modern");
    stringBuffer.push("Cpp");
    stringBuffer.push("Templates");
    stringBuffer.printBuffer("StringBuffer");

    // Instance 3: Custom Type with NTTP Capacity
    FixedCapacityBuffer<CustomPoint, 2> pointBuffer;
    pointBuffer.push(pt1);
    pointBuffer.push(pt2);
    pointBuffer.printBuffer("PointBuffer");

    // Out-of-Line Member Definition Verification
    OutOfLineTemplateDemo<double, 10> outOfLineObj(3.14159);
    cout << "  - Out-of-line template method return value: " << outOfLineObj.getValue() << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ INTRODUCTION TO TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Template Feature      | C++ Implementation Syntax         | Core Benefit & Mechanics          |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Function Template     | `template <typename T> T fn(T)`   | Eliminates function overloading   |\n"
         << "| Type Deduction        | `fn(val1, val2)`                  | Compiler automatically infers T   |\n"
         << "| Explicit Specification| `fn<double>(val1, val2)`          | Forces conversion to specified type|\n"
         << "| Class Template        | `template <typename T> class C`   | Generic container/wrapper creation|\n"
         << "| NTTP (Non-Type)       | `template <typename T, size_t N>` | Compile-time constant parameters  |\n"
         << "| Default Arguments     | `template <typename T = int>`     | Fallback types if omitted by user |\n"
         << "| Instantiation Model   | Compile-time code generation      | Zero runtime cost / High speed    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}