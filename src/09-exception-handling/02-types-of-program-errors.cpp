/*
 * =====================================================================================
 * CONCEPT        : Types of Program Errors and Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the core classification of errors encountered in C++ programming and
 *                  how Modern C++ handles them:
 *
 *                  1. Compile-Time Errors (Syntax, Type Checking, Concepts, Static Asserts) :
 *                     - Errors detected by the compiler before execution (e.g., `static_assert`).
 *                  2. Link-Time Errors (Undefined Symbols, ODR Violations) :
 *                     - Errors occurring during linker resolution (e.g., missing definitions).
 *                  3. Run-Time Errors (Recoverable / Non-Fatal Errors) :
 *                     - Exceptional operational failures managed via `try`, `throw`, and `catch`.
 *                     - Standard Exception Hierarchy:
 *                        * `std::logic_error`   : Precondition/invariant bugs (e.g., `std::invalid_argument`,
 *                                                 `std::out_of_range`, `std::domain_error`, `std::length_error`).
 *                        * `std::runtime_error` : Environmental/external failures (e.g., `std::overflow_error`,
 *                                                 `std::underflow_error`, `std::range_error`, I/O errors).
 *                  4. Logic / Fatal Errors (Unrecoverable / Undefined Behavior) :
 *                     - Severe violations (e.g., null pointer dereferences, array out-of-bound writes,
 *                       divide-by-zero) that violate C++ language rules and should be guarded via assertions
 *                       or precondition checks.
 *
 * RESOLVED ISSUE : Removed `assert(ptr != nullptr)` from inside `processPointer()` that caused program
 *                  termination (SIGABRT / core dump) during debug execution when testing runtime `nullptr`
 *                  exception handling. Separated debug-mode assertion checks (`debugAssertDemo`) from
 *                  runtime defensive exception guards (`processPointer`) to ensure uninterrupted execution.
 *
 * TIME COMPLEXITY  : Normal Control Flow (No Exceptions) : O(1) zero-cost exception handling.
 *                    Exception Propagation / Unwinding   : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Storage Overhead          : Small compiler/runtime exception object.
 * =====================================================================================
 */

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <limits>
#include <cassert>
#include <cstddef>
#include <cmath>
#include <type_traits>

using namespace std;

// =====================================================================================
// 1. COMPILE-TIME ERRORS (PREVENTION VIA TYPE SYSTEM & STATIC ASSERTS)
// Compile-time checks prevent faulty logic from ever building or running.
// =====================================================================================
template <typename T>
class SafeNumericBuffer {
    // Compile-time error guard: Ensures buffer is only instantiated for numeric types
    static_assert(std::is_arithmetic_v<T>, "SafeNumericBuffer requires an arithmetic type (int, double, float)!");

private:
    vector<T> data_;

public:
    void add(T val) {
        data_.push_back(val);
    }

    [[nodiscard]] size_t size() const noexcept {
        return data_.size();
    }
};

// =====================================================================================
// 2. RUNTIME ERRORS: LOGIC ERRORS (`std::logic_error` & DERIVED)
// Faults in the internal logic of the program that are theoretically preventable.
// =====================================================================================
class LogicErrorDemo {
public:
    // A. Demonstrating `std::invalid_argument`
    static void setAge(int age) {
        if (age < 0 || age > 150) {
            throw std::invalid_argument("Logic Error: Age must be between 0 and 150! Provided: " + std::to_string(age));
        }
        cout << "    [LOGIC CHECK] Age successfully set to: " << age << "\n";
    }

    // B. Demonstrating `std::out_of_range`
    static void accessElement(const vector<int>& vec, size_t index) {
        if (index >= vec.size()) {
            throw std::out_of_range("Logic Error: Index " + std::to_string(index) + 
                                    " is out of bounds for vector of size " + std::to_string(vec.size()));
        }
        cout << "    [LOGIC CHECK] Element at index " << index << " is: " << vec.at(index) << "\n";
    }

    // C. Demonstrating `std::domain_error`
    static double calculateSquareRoot(double value) {
        if (value < 0.0) {
            throw std::domain_error("Logic Error: Square root domain error for negative value: " + std::to_string(value));
        }
        return std::sqrt(value);
    }

    // D. Demonstrating `std::length_error`
    static void allocateExcessiveVector(size_t maxElements) {
        if (maxElements > 1'000'000'000) { // Artificial guard limit
            throw std::length_error("Logic Error: Attempted to allocate length exceeding system policy limits!");
        }
        cout << "    [LOGIC CHECK] Requested allocation length " << maxElements << " is valid.\n";
    }
};

// =====================================================================================
// 3. RUNTIME ERRORS: ENVIRONMENTAL / SYSTEM ERRORS (`std::runtime_error` & DERIVED)
// Events caused by external conditions or dynamic data scaling during execution.
// =====================================================================================
class SystemRuntimeErrorDemo {
public:
    // A. Demonstrating `std::overflow_error`
    static int8_t safeAddInt8(int8_t a, int8_t b) {
        int result = static_cast<int>(a) + static_cast<int>(b);
        if (result > numeric_limits<int8_t>::max()) {
            throw std::overflow_error("Runtime Error: 8-bit Signed Integer Overflow detected (" + 
                                       std::to_string(result) + " > " + 
                                       std::to_string(numeric_limits<int8_t>::max()) + ")!");
        }
        return static_cast<int8_t>(result);
    }

    // B. Demonstrating `std::underflow_error`
    static double computeFloatingUnderflow(double tinyValue, double divisor) {
        double result = tinyValue / divisor;
        if (result == 0.0 && tinyValue != 0.0) {
            throw std::underflow_error("Runtime Error: Floating-point underflow occurred! Result lost precision to zero.");
        }
        return result;
    }

    // C. Demonstrating Custom Environmental Failure (`std::runtime_error`)
    static void connectToRemoteDatabase(const string& connectionString) {
        if (connectionString.find("invalid_host") != string::npos) {
            throw std::runtime_error("Runtime Error: Network connection failed to host '" + connectionString + "'!");
        }
        cout << "    [SYSTEM CHECK] Database connected successfully to '" << connectionString << "'.\n";
    }
};

// =====================================================================================
// 4. UNRECOVERABLE / FATAL ERRORS & DEFENSIVE GUARDS
// Errors that violate language semantics (e.g., divide-by-zero, null dereference).
// =====================================================================================
class FatalErrorGuardDemo {
public:
    static double safeDivide(double numerator, double denominator) {
        // Defensive check: Preventing divide-by-zero before execution
        if (denominator == 0.0) {
            throw std::invalid_argument("Precondition Failure: Division by zero is mathematically undefined!");
        }
        return numerator / denominator;
    }

    static void processPointer(const int* ptr) {
        // Defensive runtime check: prevents null pointer dereference by throwing exception
        if (ptr == nullptr) {
            throw std::invalid_argument("Fatal Error Guard: Attempted to dereference nullptr!");
        }
        cout << "    [FATAL GUARD CHECK] Dereferenced pointer value: " << *ptr << "\n";
    }

    static void debugAssertDemo(const int* ptr) {
        // Debug-mode assertion: Guarantees invariant holds for valid pointers
        assert(ptr != nullptr && "Fatal Error: Null pointer passed to debugAssertDemo!");
        cout << "    [ASSERT CHECK] Pointer invariant verified. Dereferenced value: " << *ptr << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Program Errors analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. COMPILE-TIME ERRORS (PREVENTION DEMONSTRATION)
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME ERRORS (STATIC CHECKS) ================\n";

    SafeNumericBuffer<double> validBuffer;
    validBuffer.add(static_cast<double>(userInputValue));
    cout << "  - Compile-time static check passed for SafeNumericBuffer<double>.\n";
    cout << "  - Buffer Size: " << validBuffer.size() << "\n";

    // SafeNumericBuffer<std::string> invalidBuffer; 
    // UNCOMMENTING ABOVE CAUSES COMPILE-TIME ERROR via static_assert!

    // =====================================================================================
    // 2. RUNTIME LOGIC ERRORS (`std::logic_error` CATEGORY)
    // =====================================================================================
    cout << "\n================ 2. LOGIC ERRORS (`std::logic_error`) ================\n";

    // A. Testing std::invalid_argument
    try {
        cout << "  - A. Testing `std::invalid_argument` with age = -10...\n";
        LogicErrorDemo::setAge(-10);
    } catch (const std::invalid_argument& ex) {
        cout << "  - [CAUGHT LOGIC ERROR]: " << ex.what() << "\n";
    }

    // B. Testing std::out_of_range
    try {
        cout << "\n  - B. Testing `std::out_of_range` with index 5 on vector of size 3...\n";
        vector<int> sampleVec = {10, 20, 30};
        LogicErrorDemo::accessElement(sampleVec, 5);
    } catch (const std::out_of_range& ex) {
        cout << "  - [CAUGHT LOGIC ERROR]: " << ex.what() << "\n";
    }

    // C. Testing std::domain_error
    try {
        cout << "\n  - C. Testing `std::domain_error` with sqrt(-25.0)...\n";
        double res = LogicErrorDemo::calculateSquareRoot(-25.0);
        cout << "    Result: " << res << "\n";
    } catch (const std::domain_error& ex) {
        cout << "  - [CAUGHT LOGIC ERROR]: " << ex.what() << "\n";
    }

    // D. Testing std::length_error
    try {
        cout << "\n  - D. Testing `std::length_error` with 2,000,000,000 elements...\n";
        LogicErrorDemo::allocateExcessiveVector(2'000'000'000);
    } catch (const std::length_error& ex) {
        cout << "  - [CAUGHT LOGIC ERROR]: " << ex.what() << "\n";
    }

    // =====================================================================================
    // 3. RUNTIME SYSTEM / ENVIRONMENTAL ERRORS (`std::runtime_error` CATEGORY)
    // =====================================================================================
    cout << "\n================ 3. RUNTIME SYSTEM ERRORS (`std::runtime_error`) ================\n";

    // A. Testing std::overflow_error
    try {
        cout << "  - A. Testing `std::overflow_error` (120 + 20 in signed 8-bit int)...\n";
        int8_t a = 120;
        int8_t b = 20;
        int8_t sum = SystemRuntimeErrorDemo::safeAddInt8(a, b);
        cout << "    Sum: " << static_cast<int>(sum) << "\n";
    } catch (const std::overflow_error& ex) {
        cout << "  - [CAUGHT RUNTIME ERROR]: " << ex.what() << "\n";
    }

    // B. Testing std::underflow_error
    try {
        cout << "\n  - B. Testing `std::underflow_error` (1e-308 / 1e100)...\n";
        SystemRuntimeErrorDemo::computeFloatingUnderflow(1e-308, 1e100);
    } catch (const std::underflow_error& ex) {
        cout << "  - [CAUGHT RUNTIME ERROR]: " << ex.what() << "\n";
    }

    // C. Testing Generic std::runtime_error
    try {
        cout << "\n  - C. Testing `std::runtime_error` with invalid database host...\n";
        SystemRuntimeErrorDemo::connectToRemoteDatabase("db://invalid_host:5432");
    } catch (const std::runtime_error& ex) {
        cout << "  - [CAUGHT RUNTIME ERROR]: " << ex.what() << "\n";
    }

    // =====================================================================================
    // 4. UNRECOVERABLE / FATAL ERROR GUARDS
    // =====================================================================================
    cout << "\n================ 4. UNRECOVERABLE / FATAL ERROR GUARDS ================\n";

    // A. Division by Zero Guard
    try {
        cout << "  - A. Testing Division by Zero guard...\n";
        double divRes = FatalErrorGuardDemo::safeDivide(static_cast<double>(userInputValue), 0.0);
        cout << "    Result: " << divRes << "\n";
    } catch (const std::invalid_argument& ex) {
        cout << "  - [CAUGHT FATAL GUARD]: " << ex.what() << "\n";
    }

    // B. Null Pointer Dereference Guard (Runtime Exception)
    try {
        cout << "\n  - B. Testing Null Pointer Guard with nullptr...\n";
        const int* nullPtr = nullptr;
        FatalErrorGuardDemo::processPointer(nullPtr);
    } catch (const std::invalid_argument& ex) {
        cout << "  - [CAUGHT FATAL GUARD]: " << ex.what() << "\n";
    }

    // C. Debug Assertion Check (Valid Pointer)
    cout << "\n  - C. Testing Debug Assertion Check with valid pointer...\n";
    int validVal = userInputValue;
    FatalErrorGuardDemo::debugAssertDemo(&validVal);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ PROGRAM ERROR TYPES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Error Category        | Detection Phase / Class           | Primary Cause / Solution          |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Compile-Time Error    | Compiler (`static_assert`, Types) | Syntax/Type mismatch; Fix build   |\n"
         << "| Link-Time Error       | Linker (Symbol Resolution)        | Missing definition; Fix linkage   |\n"
         << "| Logic Error           | Runtime (`std::logic_error`)      | Flawed code design; Fix algorithm |\n"
         << "|   - Invalid Argument  | `std::invalid_argument`           | Bad parameter passed to function  |\n"
         << "|   - Out of Range      | `std::out_of_range`               | Element index outside valid bounds|\n"
         << "|   - Domain Error      | `std::domain_error`               | Input outside mathematical domain |\n"
         << "|   - Length Error      | `std::length_error`               | Exceeds maximum valid capacity    |\n"
         << "| Runtime Error         | Runtime (`std::runtime_error`)    | External environmental failures   |\n"
         << "|   - Overflow Error    | `std::overflow_error`             | Result exceeds upper value type   |\n"
         << "|   - Underflow Error   | `std::underflow_error`            | Loss of floating-point precision  |\n"
         << "| Fatal/UB Error        | Defensive Guards / `assert`       | Null dereference, div-by-zero     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}