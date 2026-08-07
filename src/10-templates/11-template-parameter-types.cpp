/*
 * =====================================================================================
 * CONCEPT        : Template Parameter Types in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating all four primary
 *                  categories of template parameters supported by Modern C++:
 *
 *                  1. Type Template Parameters (`typename T` / `class T`) :
 *                     - Accepts standard data types, user-defined classes, or structs.
 *
 *                  2. Non-Type Template Parameters (NTTP) :
 *                     - Accepts compile-time constant values (integrals, enums, 
 *                       function pointers, and C++17 `auto` deduced constants).
 *
 *                  3. Template Template Parameters (`template <typename...> class Container`) :
 *                     - Accepts class templates as arguments rather than concrete types,
 *                       enabling higher-order template abstractions.
 *
 *                  4. Variadic Template Parameter Packs (`typename... Args`) :
 *                     - Accepts arbitrary numbers of type parameters for variadic templates.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>  : Console I/O streams (cin, cout, flush)
 *                  - <string>    : Type std::string
 *                  - <vector>    : Container std::vector used for template template parameter demo
 *                  - <typeinfo>  : RTTI typeid operator for type inspection
 *                  - <limits>    : Stream clearing via std::numeric_limits
 *                  - <cstddef>   : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N) distinct instantiations.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary footprint scales with each unique parameter instantiation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. TYPE TEMPLATE PARAMETERS (`typename T` / `class T`)
// =====================================================================================
template <typename T>
class TypeParameterWrapper {
private:
    T value_;

public:
    explicit TypeParameterWrapper(T val) : value_(val) {}

    void display() const {
        cout << "    [Type Parameter] Value: " << value_ 
             << " | Deducted Type: " << typeid(T).name() << "\n";
    }
};

// =====================================================================================
// 2. NON-TYPE TEMPLATE PARAMETERS (NTTP: Integrals, Enums, Pointers, `auto`)
// =====================================================================================

// Enum for NTTP demonstration
enum class OperatingMode { Standard, HighPerformance, Debug };

// Function pointer type for NTTP demonstration
using TransformFunction = int (*)(int);

// Helper function to serve as an NTTP function pointer
static int squareInput(int x) {
    return x * x;
}

template <
    std::size_t BufferSize,        // Integral NTTP
    OperatingMode Mode,            // Enum NTTP
    TransformFunction Transformer, // Function Pointer NTTP
    auto DynamicConstant           // C++17 `auto` NTTP
>
class NonTypeParametersDemo {
public:
    static void execute(int input) {
        int transformedValue = Transformer ? Transformer(input) : input;
        cout << "    [NTTP Demo Configuration]:\n"
             << "      - Integral BufferSize (std::size_t): " << BufferSize << "\n"
             << "      - Enum OperatingMode               : " 
             << (Mode == OperatingMode::HighPerformance ? "HighPerformance" : "Other") << "\n"
             << "      - Function Pointer Output (" << input << "^2) : " << transformedValue << "\n"
             << "      - Deduced `auto` NTTP Value        : " << DynamicConstant 
             << " (Type: " << typeid(decltype(DynamicConstant)).name() << ")\n";
    }
};

// =====================================================================================
// 3. TEMPLATE TEMPLATE PARAMETERS (`template <typename...> class Container`)
// Accepts a template class as a parameter rather than a concrete type.
// =====================================================================================
template <typename ElementType, template <typename...> class ContainerType>
class HigherOrderAdapter {
private:
    ContainerType<ElementType> storage_;

public:
    void push(const ElementType& item) {
        storage_.push_back(item);
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return storage_.size();
    }

    void printSummary(const string& adapterName) const {
        cout << "    [" << adapterName << "] Higher-Order Container holding " 
             << storage_.size() << " elements of type " << typeid(ElementType).name() << "\n";
    }
};

// =====================================================================================
// 4. VARIADIC TEMPLATE PARAMETER PACKS (`typename... Args`)
// =====================================================================================
template <typename... Args>
class ParameterPackInspector {
public:
    static constexpr std::size_t parameterCount = sizeof...(Args);

    static void printDetails() {
        cout << "    [Variadic Parameter Pack] Number of Type Parameters in Pack: " 
             << parameterCount << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Template Parameter Types analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. TYPE TEMPLATE PARAMETERS
    // =====================================================================================
    cout << "\n================ 1. TYPE TEMPLATE PARAMETERS ================\n";

    TypeParameterWrapper<int> intWrapper(userInputValue);
    intWrapper.display();

    TypeParameterWrapper<string> stringWrapper("Modern_Cpp_Templates");
    stringWrapper.display();

    // =====================================================================================
    // 2. NON-TYPE TEMPLATE PARAMETERS (NTTP)
    // =====================================================================================
    cout << "\n================ 2. NON-TYPE TEMPLATE PARAMETERS (NTTP) ================\n";

    // Passing compile-time constant size 512, Enum Mode, Function Pointer, and auto character 'X'
    NonTypeParametersDemo<512, OperatingMode::HighPerformance, &squareInput, 'X'>::execute(userInputValue);

    // =====================================================================================
    // 3. TEMPLATE TEMPLATE PARAMETERS
    // =====================================================================================
    cout << "\n================ 3. TEMPLATE TEMPLATE PARAMETERS ================\n";

    // Passing `int` as ElementType and `std::vector` as ContainerType
    HigherOrderAdapter<int, std::vector> vectorAdapter;
    vectorAdapter.push(userInputValue);
    vectorAdapter.push(userInputValue + 10);
    vectorAdapter.push(userInputValue + 20);
    vectorAdapter.printSummary("VectorAdapter");

    // =====================================================================================
    // 4. VARIADIC TEMPLATE PARAMETER PACKS
    // =====================================================================================
    cout << "\n================ 4. VARIADIC TEMPLATE PARAMETER PACKS ================\n";

    ParameterPackInspector<int, double, string, char>::printDetails();
    ParameterPackInspector<float>::printDetails();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TEMPLATE PARAMETER TYPES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Parameter Type        | C++ Syntax Example                | Purpose / Capability              |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Type Parameter        | `template <typename T>`           | Generic types (int, string, class)|\n"
         << "| Integral NTTP         | `template <size_t N>`             | Compile-time stack sizes/counts   |\n"
         << "| Enum NTTP             | `template <OperatingMode Mode>`   | Compile-time configuration flags  |\n"
         << "| Pointer/Ref NTTP      | `template <TransformFunction Fn>` | Inlined compile-time function ptr |\n"
         << "| C++17 `auto` NTTP     | `template <auto Value>`           | Deduces constant parameter type   |\n"
         << "| Template Template     | `template <template<...> class C>`| Higher-order container wrappers   |\n"
         << "| Parameter Pack        | `template <typename... Args>`      | Arbitrary number of type arguments|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}