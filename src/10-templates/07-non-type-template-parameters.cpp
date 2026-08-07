/*
 * =====================================================================================
 * CONCEPT        : Non-Type Template Parameters (NTTP) under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, mechanics,
 *                  and applications of Non-Type Template Parameters (NTTP) in C++:
 *
 *                  1. Integral Non-Type Template Parameters :
 *                     - Passing compile-time constant integers (`std::size_t`, `int`, `char`) 
 *                       to configure container sizes, array dimensions, or operational limits.
 *
 *                  2. C++17 `auto` Non-Type Template Parameters :
 *                     - Deducing the type of non-type template parameters automatically at 
 *                       compile-time (`template <auto ConstantValue>`).
 *
 *                  3. Function Templates with Non-Type Template Parameters :
 *                     - Injecting compile-time constants (e.g., multiplier factors, loop counts) 
 *                       directly into generic functions for zero-runtime-cost inline scaling.
 *
 *                  4. Zero-Overhead Compile-Time Guarantees :
 *                     - NTTP values are evaluated at compile time and embedded into code,
 *                       enabling stack allocations, loop unrolling, and zero heap overhead.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, endl, flush)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for type inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Evaluation : O(1) evaluated entirely during compilation.
 *                    Runtime Execution      : O(1) Zero-cost abstraction (direct constants).
 * SPACE COMPLEXITY : Stack/Binary Overhead   : Fixed stack allocation based on NTTP values.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. CLASS TEMPLATE WITH INTEGRAL NTTP (FIXED 2D MATRIX CONTAINER)
// NTTPs Rows and Cols define exact compile-time array bounds allocated on the stack.
// =====================================================================================
template <typename T, std::size_t Rows, std::size_t Cols>
class FixedMatrix {
private:
    T data_[Rows][Cols]{};

public:
    FixedMatrix() = default;

    void fill(const T& val) {
        for (std::size_t r = 0; r < Rows; ++r) {
            for (std::size_t c = 0; c < Cols; ++c) {
                data_[r][c] = val;
            }
        }
    }

    void set(std::size_t r, std::size_t c, const T& val) {
        if (r < Rows && c < Cols) {
            data_[r][c] = val;
        }
    }

    [[nodiscard]] constexpr std::size_t getRows() const noexcept { return Rows; }
    [[nodiscard]] constexpr std::size_t getCols() const noexcept { return Cols; }
    [[nodiscard]] constexpr std::size_t totalElements() const noexcept { return Rows * Cols; }

    void printMatrix(const string& matrixName) const {
        cout << "    [" << matrixName << "] Matrix<Type: " << typeid(T).name() 
             << ", Dimensions: " << Rows << "x" << Cols << "> (" << totalElements() << " elements):\n";
        for (std::size_t r = 0; r < Rows; ++r) {
            cout << "      [ ";
            for (std::size_t c = 0; c < Cols; ++c) {
                cout << data_[r][c] << (c + 1 < Cols ? ", " : " ");
            }
            cout << "]\n";
        }
    }
};

// =====================================================================================
// 2. C++17 `auto` NON-TYPE TEMPLATE PARAMETER
// Compiler automatically deduces the type of the compile-time constant parameter.
// =====================================================================================
template <auto ConstantValue>
class CompileTimeValueHolder {
public:
    void printInfo(const string& label) const {
        cout << "    [" << label << "] Deduced NTTP Type: " << typeid(decltype(ConstantValue)).name()
             << " | Compile-Time Value: " << ConstantValue << "\n";
    }

    [[nodiscard]] constexpr auto getValue() const noexcept {
        return ConstantValue;
    }
};

// =====================================================================================
// 3. FUNCTION TEMPLATES WITH NTTP (COMPILE-TIME FACTOR SCALING)
// Multiplier is evaluated at compile time, eliminating runtime variable lookup.
// =====================================================================================
class FunctionNTTPDemo {
public:
    template <int Multiplier, typename T>
    [[nodiscard]] static T scaleValue(T val) {
        return val * static_cast<T>(Multiplier);
    }

    template <char OptionChar, typename T>
    static void processCommand(T payload) {
        cout << "    [Command Executed] Option OptionChar = '" << OptionChar 
             << "' | Payload (" << typeid(T).name() << "): " << payload << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Non-Type Template Parameters analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CLASS TEMPLATES WITH INTEGRAL NTTP
    // =====================================================================================
    cout << "\n================ 1. CLASS TEMPLATES WITH INTEGRAL NTTP ================\n";

    // Instantiating 2x3 Matrix of integers
    FixedMatrix<int, 2, 3> intMatrix;
    intMatrix.fill(userInputValue);
    intMatrix.set(0, 1, userInputValue + 50);
    intMatrix.set(1, 2, userInputValue + 99);
    intMatrix.printMatrix("IntMatrix_2x3");

    // Instantiating 3x2 Matrix of doubles (different dimensions create distinct template types)
    FixedMatrix<double, 3, 2> doubleMatrix;
    doubleMatrix.fill(static_cast<double>(userInputValue) * 1.5);
    doubleMatrix.printMatrix("DoubleMatrix_3x2");

    // =====================================================================================
    // 2. C++17 `auto` NON-TYPE TEMPLATE PARAMETERS
    // =====================================================================================
    cout << "\n================ 2. C++17 `auto` NON-TYPE TEMPLATE PARAMETERS ================\n";

    // Deduces auto = int
    CompileTimeValueHolder<500> intConstantHolder;
    intConstantHolder.printInfo("Integral auto NTTP");

    // Deduces auto = char
    CompileTimeValueHolder<'K'> charConstantHolder;
    charConstantHolder.printInfo("Char auto NTTP");

    // Deduces auto = std::size_t
    CompileTimeValueHolder<1000ULL> sizeConstantHolder;
    sizeConstantHolder.printInfo("Unsigned Long Long auto NTTP");

    // =====================================================================================
    // 3. FUNCTION TEMPLATES WITH NTTP
    // =====================================================================================
    cout << "\n================ 3. FUNCTION TEMPLATES WITH NTTP ================\n";

    // Scaling value using explicit compile-time multiplier NTTP = 10
    int scaledResult = FunctionNTTPDemo::scaleValue<10>(userInputValue);
    cout << "  - Function NTTP scaleValue<10>(" << userInputValue << ") = " << scaledResult << "\n";

    // Command processing with character NTTP = 'A'
    FunctionNTTPDemo::processCommand<'A'>(userInputValue + 200);
    FunctionNTTPDemo::processCommand<'Z'>("Cluster_Online");

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ NON-TYPE TEMPLATE PARAMETERS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| NTTP Category         | Syntax / Feature Example          | Architectural Effect & Benefit    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Integral NTTP         | `template <typename T, size_t N>` | Fixed stack sizes, no heap alloc  |\n"
         << "| Class Template NTTP   | `FixedMatrix<int, 2, 3> m;`       | Distinct types per dimension      |\n"
         << "| C++17 `auto` NTTP     | `template <auto Value>`           | Deduces constant parameter type   |\n"
         << "| Function NTTP         | `scaleValue<10>(val)`             | Inline compile-time value injection|\n"
         << "| Character / Enum NTTP | `processCommand<'A'>(payload)`     | Compile-time option dispatching   |\n"
         << "| Performance Guarantee | Stack allocation, zero lookup     | Pure zero-cost compile-time logic |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}