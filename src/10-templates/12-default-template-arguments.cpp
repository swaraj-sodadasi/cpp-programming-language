/*
 * =====================================================================================
 * CONCEPT        : Default Template Arguments under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, rules,
 *                  mechanics, and applications of Default Template Arguments in C++:
 *
 *                  1. Default Type Parameters (`typename T = int`) :
 *                     - Providing default fallback data types for class and function templates.
 *
 *                  2. Default Non-Type Template Parameters (NTTP) (`size_t Capacity = 5`) :
 *                     - Providing default compile-time constant values.
 *
 *                  3. Default Template Arguments in Function Templates :
 *                     - Combining type deduction with default type arguments for un-deduced 
 *                       template parameters (e.g., return types).
 *
 *                  4. Accumulation across Forward Declarations :
 *                     - Accumulating default template arguments across multiple forward declarations 
 *                       within the same scope.
 *
 *                  5. Right-to-Left Ordering Rules :
 *                     - Default template arguments in class templates must follow right-to-left 
 *                       placement rules unless trailing parameters can be deduced (as in function templates).
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for type inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N) distinct class/function instantiations.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint      : Scales with each unique instantiated type combination.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. CLASS TEMPLATES WITH DEFAULT TYPE AND NON-TYPE PARAMETERS
// Demonstrates defaulting both type (T = int) and NTTP (Capacity = 5).
// =====================================================================================
template <typename T = int, std::size_t Capacity = 5>
class DefaultBuffer {
private:
    T storage_[Capacity]{};
    std::size_t size_{0};

public:
    DefaultBuffer() = default;

    bool add(const T& item) {
        if (size_ >= Capacity) {
            return false;
        }
        storage_[size_++] = item;
        return true;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }

    void printBuffer(const string& label) const {
        cout << "    [" << label << "] Buffer<Type: " << typeid(T).name()
             << ", Capacity: " << Capacity << "> (Size: " << size_ << "/" << Capacity << ") -> { ";
        for (std::size_t i = 0; i < size_; ++i) {
            cout << storage_[i] << (i + 1 < size_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// =====================================================================================
// 2. FUNCTION TEMPLATES WITH DEFAULT TEMPLATE ARGUMENTS
// Useful when a return type cannot be deduced from function arguments.
// =====================================================================================
class FunctionDefaultDemo {
public:
    // ResultType defaults to double if not explicitly specified by the caller
    template <typename ResultType = double, typename InputType = int>
    [[nodiscard]] static ResultType computeScaledValue(InputType val, double factor = 1.5) {
        cout << "    [computeScaledValue] ResultType: " << typeid(ResultType).name()
             << " | InputType: " << typeid(InputType).name() << "\n";
        return static_cast<ResultType>(val * factor);
    }
};

// =====================================================================================
// 3. ACCUMULATION OF DEFAULT TEMPLATE ARGUMENTS ACROSS FORWARD DECLARATIONS
// Default arguments can be accumulated across declarations in the same scope.
// =====================================================================================

// Forward declaration 1: Specifies default for the second parameter (U = std::string)
template <typename T, typename U = std::string>
class PairConfig;

// Forward declaration 2 / Definition: Specifies default for the first parameter (T = int)
template <typename T = int, typename U>
class PairConfig {
private:
    T first_{};
    U second_{};

public:
    PairConfig() = default;
    PairConfig(T f, U s) : first_(f), second_(s) {}

    void display(const string& label) const {
        cout << "    [" << label << "] PairConfig<" << typeid(T).name()
             << ", " << typeid(U).name() << "> -> First: " << first_
             << " | Second: \"" << second_ << "\"\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Default Template Arguments analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CLASS TEMPLATE DEFAULT ARGUMENTS
    // =====================================================================================
    cout << "\n================ 1. CLASS TEMPLATES WITH DEFAULT ARGUMENTS ================\n";

    // Scenario A: Omit all template arguments (Uses T = int, Capacity = 5)
    DefaultBuffer<> defaultObj;
    defaultObj.add(userInputValue);
    defaultObj.add(userInputValue + 10);
    defaultObj.printBuffer("Default Buffer <>");

    // Scenario B: Override Type parameter (T = std::string), keep default NTTP (Capacity = 5)
    DefaultBuffer<string> stringObj;
    stringObj.add("Modern");
    stringObj.add("Cpp");
    stringObj.add("Templates");
    stringObj.printBuffer("Override Type <string>");

    // Scenario C: Override both Type and NTTP parameters (T = double, Capacity = 3)
    DefaultBuffer<double, 3> customObj;
    customObj.add(static_cast<double>(userInputValue) * 1.5);
    customObj.add(88.75);
    customObj.printBuffer("Override Both <double, 3>");

    // =====================================================================================
    // 2. FUNCTION TEMPLATE DEFAULT ARGUMENTS
    // =====================================================================================
    cout << "\n================ 2. FUNCTION TEMPLATES WITH DEFAULT ARGUMENTS ================\n";

    // Scenario A: Omit explicit return type (ResultType defaults to double)
    auto defaultResult = FunctionDefaultDemo::computeScaledValue(userInputValue);
    cout << "  - Default ResultType (double): " << defaultResult << "\n";

    // Scenario B: Explicitly specify ResultType = int (Overrides default double)
    cout << "\n  - Override ResultType <int>:\n";
    auto intResult = FunctionDefaultDemo::computeScaledValue<int>(userInputValue, 2.75);
    cout << "  - Explicit ResultType (int): " << intResult << "\n";

    // =====================================================================================
    // 3. ACCUMULATED DEFAULT ARGUMENTS ACROSS DECLARATIONS
    // =====================================================================================
    cout << "\n================ 3. ACCUMULATED DEFAULT ARGUMENTS ================\n";

    // Uses T = int (from definition) and U = std::string (from forward declaration)
    PairConfig<> accumulatedObj;
    accumulatedObj.display("Accumulated Defaults <>");

    PairConfig<double, int> explicitPair(3.14159, userInputValue);
    explicitPair.display("Explicit Pair <double, int>");

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DEFAULT TEMPLATE ARGUMENTS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Concept     | C++ Syntax Example                | Architectural Behavior & Rules    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Default Type          | `template <typename T = int>`     | Fallback type if omitted by user  |\n"
         << "| Default NTTP          | `template <size_t Cap = 5>`       | Fallback compile-time constant    |\n"
         << "| Class Instantiation   | `Buffer<> obj;`                   | Empty angle brackets use defaults |\n"
         << "| Partial Override      | `Buffer<string> obj;`             | Overrides first, defaults trailing|\n"
         << "| Function Default      | `ResultType compute(InputVal)`    | Provides return type defaults     |\n"
         << "| Accumulation Rule     | Split across declarations         | Defaults accumulate in same scope |\n"
         << "| Right-to-Left Rule    | `template <T = int, U>` (ERROR)   | Defaults must be trailing in class|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}