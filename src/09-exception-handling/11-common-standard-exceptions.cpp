/*
 * =====================================================================================
 * CONCEPT        : Common Standard Exceptions in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the most frequently encountered C++ Standard Library exceptions, their
 *                  triggering conditions, exception categories, and handling patterns:
 *
 *                  1. Logic Errors (`std::logic_error` branch):
 *                     - `std::invalid_argument` : Invalid value passed to a function.
 *                     - `std::out_of_range`     : Element access beyond bounds (e.g., `std::vector::at()`).
 *                     - `std::length_error`     : Requesting container length exceeding max capacity.
 *
 *                  2. Runtime Errors (`std::runtime_error` branch):
 *                     - `std::runtime_error`    : Generic operational or environmental error.
 *                     - `std::overflow_error`   : Computation exceeds upper numerical limits.
 *                     - `std::underflow_error`  : Floating-point precision underflows to zero.
 *
 *                  3. Language & Runtime Subsystem Exceptions (`std::exception` descendants):
 *                     - `std::bad_alloc`           : Heap memory allocation failure (`new`).
 *                     - `std::bad_cast`            : Dynamic cast failure on polymorphic reference type.
 *                     - `std::bad_optional_access`   : Accessing empty `std::optional` value.
 *
 * RESOLVED ISSUE : Addressed `-Walloc-size-larger-than=` compiler warning during `std::bad_alloc`
 *                  testing by setting the allocation request size below `PTRDIFF_MAX` (4 TB allocation)
 *                  and applying localized diagnostic pragma guards across GCC and Clang compilers.
 *
 * TIME COMPLEXITY  : Normal Path (No Exception) : O(1) Zero-cost exception dispatch setup.
 *                    Exception Catching         : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Storage Footprint : Small compiler/runtime exception object.
 * =====================================================================================
 */

#include <iostream>
#include <stdexcept>
#include <new>
#include <typeinfo>
#include <optional>
#include <vector>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER CLASSES FOR `std::bad_cast` DEMONSTRATION
// =====================================================================================
class BaseShape {
public:
    virtual ~BaseShape() noexcept = default;
    virtual void draw() const {
        cout << "      [BaseShape] Drawing generic shape.\n";
    }
};

class CircleShape : public BaseShape {
public:
    void draw() const override {
        cout << "      [CircleShape] Drawing circle.\n";
    }

    void circleSpecificMethod() const {
        cout << "      [CircleShape] Executing circle-specific logic.\n";
    }
};

// =====================================================================================
// 1. DEMONSTRATING LOGIC ERRORS (`std::invalid_argument`, `std::out_of_range`, `std::length_error`)
// =====================================================================================
class LogicExceptionsDemo {
public:
    // Demonstrating `std::invalid_argument`
    static double calculateSquareRoot(double value) {
        if (value < 0.0) {
            throw std::invalid_argument("Invalid argument: Value cannot be negative for real square root!");
        }
        return value; // Simplification for demonstration
    }

    // Demonstrating `std::out_of_range`
    static int getVectorElement(const vector<int>& vec, size_t index) {
        // vec.at() throws std::out_of_range if index >= vec.size()
        return vec.at(index); 
    }

    // Demonstrating `std::length_error`
    static void allocateExcessiveVector(size_t requestedSize) {
        vector<int> temp;
        if (requestedSize > temp.max_size()) {
            throw std::length_error("Length error: Requested size exceeds maximum container limit!");
        }
        temp.resize(requestedSize);
    }
};

// =====================================================================================
// 2. DEMONSTRATING RUNTIME ERRORS (`std::runtime_error`, `std::overflow_error`, `std::underflow_error`)
// =====================================================================================
class RuntimeExceptionsDemo {
public:
    // Demonstrating `std::runtime_error`
    static void verifySystemConnection(bool isConnected) {
        if (!isConnected) {
            throw std::runtime_error("Runtime Error: Unable to establish database connection string!");
        }
    }

    // Demonstrating `std::overflow_error`
    static int8_t computeSigned8BitAdd(int8_t a, int8_t b) {
        int result = static_cast<int>(a) + static_cast<int>(b);
        if (result > numeric_limits<int8_t>::max()) {
            throw std::overflow_error("Overflow Error: Addition result exceeds 8-bit signed integer maximum (127)!");
        }
        return static_cast<int8_t>(result);
    }

    // Demonstrating `std::underflow_error`
    static double calculateFloatingUnderflow(double tinyValue, double divisor) {
        double result = tinyValue / divisor;
        if (result == 0.0 && tinyValue != 0.0) {
            throw std::underflow_error("Underflow Error: Floating-point precision lost; value underflowed to 0.0!");
        }
        return result;
    }
};

// =====================================================================================
// 3. DEMONSTRATING LANGUAGE & SUBSYSTEM EXCEPTIONS (`std::bad_alloc`, `std::bad_cast`, `std::bad_optional_access`)
// =====================================================================================
class SubsystemExceptionsDemo {
public:
    // Demonstrating `std::bad_alloc`
    static void triggerBadAlloc() {
        cout << "    [Attempting Allocation] Trying to allocate gigantic memory block...\n";
        
        // Request 1 Teraint array (~4 TB total bytes).
        // Capped well below PTRDIFF_MAX (9223372036854775807 bytes) to avoid GCC -Walloc-size-larger-than=
        constexpr std::size_t safeGiganticCount = 0x10000000000ULL;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Walloc-size-larger-than="
#elif defined(__GNUC__) && (__GNUC__ >= 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than="
#endif
        int* ptr = new int[safeGiganticCount];
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && (__GNUC__ >= 7)
#pragma GCC diagnostic pop
#endif

        delete[] ptr; // Unreachable
    }

    // Demonstrating `std::bad_cast`
    static void triggerBadCast() {
        BaseShape baseObj;
        cout << "    [Attempting Dynamic Cast] Casting BaseShape reference to CircleShape reference...\n";
        // Casting a reference (not pointer) that fails throws std::bad_cast
        [[maybe_unused]] const CircleShape& circleRef = dynamic_cast<const CircleShape&>(baseObj); 
    }

    // Demonstrating `std::bad_optional_access`
    static int triggerBadOptionalAccess() {
        std::optional<int> emptyOpt = std::nullopt;
        cout << "    [Attempting Optional Access] Reading value from empty std::optional...\n";
        // Accessing value() on std::nullopt throws std::bad_optional_access
        return emptyOpt.value(); 
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Common Standard Exceptions analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. COMMON LOGIC EXCEPTIONS (`std::invalid_argument`, `std::out_of_range`, `std::length_error`)
    // =====================================================================================
    cout << "\n================ 1. COMMON LOGIC EXCEPTIONS (`std::logic_error`) ================\n";

    // Scenario A: std::invalid_argument
    try {
        cout << "  - Testing `std::invalid_argument` with value = -5.0...\n";
        LogicExceptionsDemo::calculateSquareRoot(-5.0);
    } catch (const std::invalid_argument& ex) {
        cout << "  - [CAUGHT `std::invalid_argument`]: \"" << ex.what() << "\"\n";
    }

    // Scenario B: std::out_of_range
    try {
        vector<int> numbers = {10, 20, 30};
        cout << "  - Testing `std::out_of_range` by accessing vector index 5 (size = 3)...\n";
        LogicExceptionsDemo::getVectorElement(numbers, 5);
    } catch (const std::out_of_range& ex) {
        cout << "  - [CAUGHT `std::out_of_range`]: \"" << ex.what() << "\"\n";
    }

    // Scenario C: std::length_error
    try {
        cout << "  - Testing `std::length_error` with massive allocation request...\n";
        LogicExceptionsDemo::allocateExcessiveVector(numeric_limits<size_t>::max());
    } catch (const std::length_error& ex) {
        cout << "  - [CAUGHT `std::length_error`]: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // 2. COMMON RUNTIME EXCEPTIONS (`std::runtime_error`, `std::overflow_error`, `std::underflow_error`)
    // =====================================================================================
    cout << "\n================ 2. COMMON RUNTIME EXCEPTIONS (`std::runtime_error`) ================\n";

    // Scenario A: std::runtime_error
    try {
        cout << "  - Testing `std::runtime_error` with disconnected status...\n";
        RuntimeExceptionsDemo::verifySystemConnection(false);
    } catch (const std::runtime_error& ex) {
        cout << "  - [CAUGHT `std::runtime_error`]: \"" << ex.what() << "\"\n";
    }

    // Scenario B: std::overflow_error
    try {
        cout << "  - Testing `std::overflow_error` (120 + " << userInputValue << " in signed 8-bit int)...\n";
        RuntimeExceptionsDemo::computeSigned8BitAdd(120, static_cast<int8_t>(userInputValue));
    } catch (const std::overflow_error& ex) {
        cout << "  - [CAUGHT `std::overflow_error`]: \"" << ex.what() << "\"\n";
    }

    // Scenario C: std::underflow_error
    try {
        cout << "  - Testing `std::underflow_error` (1e-308 / 1e100)...\n";
        RuntimeExceptionsDemo::calculateFloatingUnderflow(1e-308, 1e100);
    } catch (const std::underflow_error& ex) {
        cout << "  - [CAUGHT `std::underflow_error`]: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // 3. COMMON SUBSYSTEM EXCEPTIONS (`std::bad_alloc`, `std::bad_cast`, `std::bad_optional_access`)
    // =====================================================================================
    cout << "\n================ 3. SUBSYSTEM EXCEPTIONS (`std::bad_alloc`, `std::bad_cast`, `std::bad_optional_access`) ================\n";

    // Scenario A: std::bad_alloc
    try {
        SubsystemExceptionsDemo::triggerBadAlloc();
    } catch (const std::bad_alloc& ex) {
        cout << "  - [CAUGHT `std::bad_alloc`]: \"" << ex.what() << "\"\n";
    }

    // Scenario B: std::bad_cast
    try {
        SubsystemExceptionsDemo::triggerBadCast();
    } catch (const std::bad_cast& ex) {
        cout << "  - [CAUGHT `std::bad_cast`]: \"" << ex.what() << "\"\n";
    }

    // Scenario C: std::bad_optional_access
    try {
        SubsystemExceptionsDemo::triggerBadOptionalAccess();
    } catch (const std::bad_optional_access& ex) {
        cout << "  - [CAUGHT `std::bad_optional_access`]: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ COMMON STANDARD EXCEPTIONS SUMMARY =================\n";
    cout << "+---------------------------+-----------------------------------+-----------------------------------+\n"
         << "| Standard Exception Class  | Base Category Class               | Typical Triggering Condition      |\n"
         << "+---------------------------+-----------------------------------+-----------------------------------+\n"
         << "| `std::invalid_argument`   | `std::logic_error`                | Invalid argument value to function|\n"
         << "| `std::out_of_range`       | `std::logic_error`                | Container access index bounds fail|\n"
         << "| `std::length_error`       | `std::logic_error`                | Requesting size exceeding capacity|\n"
         << "| `std::runtime_error`      | `std::exception`                  | Generic operational failure       |\n"
         << "| `std::overflow_error`     | `std::runtime_error`              | Numeric calculation upper overflow|\n"
         << "| `std::underflow_error`    | `std::runtime_error`              | Floating precision lost to zero   |\n"
         << "| `std::bad_alloc`          | `std::exception`                  | Heap memory allocation failure    |\n"
         << "| `std::bad_cast`           | `std::exception`                  | Failed dynamic_cast on reference  |\n"
         << "| `std::bad_optional_access`| `std::exception`                  | Reading empty `std::optional`     |\n"
         << "+---------------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}