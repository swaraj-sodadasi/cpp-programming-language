/*
 * =====================================================================================
 * CONCEPT        : Best & Most Powerful Features in C++ Functions (C++11 through C++20)
 * DESCRIPTION    : Comprehensive implementation consolidating the top functional features in C++:
 *                  1. Zero-Copy & Move Semantics (`const T&`, `T&&`) + `[[nodiscard]]` API Safety
 *                  2. Compile-Time Function Execution (`constexpr` / C++20 `consteval`)
 *                  3. Expressive Return Types (`auto`, `std::tuple`, C++17 Structured Bindings)
 *                  4. First-Class Callables & State Preservation (Lambdas & `std::function`)
 *                  5. Compile-Time Conditional Branching (`if constexpr` - C++17)
 *                  6. Constrained Generic Functions (C++20 Concepts with SFINAE fallback)
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct/Compile-time evaluation across core mechanisms.
 * SPACE COMPLEXITY : Best Case: O(1) — Stack-allocated frame execution with zero-copy overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <iomanip>
#include <vector>
#include <cstdint> // Required for uint64_t
#include <type_traits>
#include <functional>

// Feature-detection macros for seamless cross-compiler compatibility (C++14/17/20)
#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
    #include <concepts>
    #define HAS_CPP20_CONCEPTS 1
#else
    #define HAS_CPP20_CONCEPTS 0
#endif

#if defined(__cpp_consteval) && __cpp_consteval >= 201811L
    #define HAS_CPP20_CONSTEVAL 1
#else
    #define HAS_CPP20_CONSTEVAL 0
#endif

using namespace std;

// =====================================================================================
// FEATURE 1: PASS SEMANTICS & API SAFETY (`const T&`, `T&&`, `[[nodiscard]]`)
// =====================================================================================

class DataPayload {
private:
    string tag_;
    size_t size_;

public:
    explicit DataPayload(string tag, size_t size = 100) 
        : tag_(std::move(tag)), size_(size) {}

    [[nodiscard]] const string& getTag() const { return tag_; }
    [[nodiscard]] size_t getSize() const { return size_; }
};

// High-performance function taking const reference and returning status with [[nodiscard]]
[[nodiscard]] bool inspectPayload(const DataPayload& payload) {
    if (payload.getSize() == 0) return false;
    cout << "  - [Pass-by-Const-Ref] Inspected '" << payload.getTag() 
         << "' (Size: " << payload.getSize() << " bytes)\n";
    return true;
}

// =====================================================================================
// FEATURE 2: COMPILE-TIME EXECUTION (`constexpr` / `consteval`)
// =====================================================================================

// Evaluated at compile-time if inputs are constant expressions; otherwise at runtime
constexpr uint64_t computeFactorial(unsigned int n) {
    return (n <= 1) ? 1 : (n * computeFactorial(n - 1));
}

// Guaranteed compile-time immediate function (C++20) with constexpr fallback
#if HAS_CPP20_CONSTEVAL
consteval uint64_t computeImmediateSquare(uint64_t x) {
    return x * x;
}
#else
constexpr uint64_t computeImmediateSquare(uint64_t x) {
    return x * x;
}
#endif

// =====================================================================================
// FEATURE 3: MULTI-VALUE RETURNS & STRUCTURED BINDINGS (C++17)
// =====================================================================================

auto analyzeDataset(const vector<int>& data) -> tuple<int, double, string> {
    if (data.empty()) return {0, 0.0, "EMPTY"};

    int sum = 0;
    for (int val : data) sum += val;

    double avg = static_cast<double>(sum) / data.size();
    string status = (avg > 10.0) ? "ABOVE_AVERAGE" : "NORMAL";

    return {sum, avg, status};
}

// =====================================================================================
// FEATURE 4: COMPILE-TIME CONDITIONAL BRANCHING (`if constexpr` - C++17)
// =====================================================================================

template <typename T>
void displayFormattedValue(T val) {
    if constexpr (is_same_v<T, string>) {
        cout << "  - [if constexpr] String Type    : \"" << val << "\"\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "  - [if constexpr] Floating Point : " << fixed << setprecision(2) << val << "\n";
    } else {
        cout << "  - [if constexpr] Integral Value : " << val << "\n";
    }
}

// =====================================================================================
// FEATURE 5: CONSTRAINED GENERIC FUNCTIONS (C++20 CONCEPTS / SFINAE)
// =====================================================================================

#if HAS_CPP20_CONCEPTS
template <typename T>
concept Numeric = is_arithmetic_v<T>;

// C++20 Abbreviated Function Template using Concept
auto addNumericValues(Numeric auto a, Numeric auto b) {
    return a + b;
}
#else
template <typename T, typename U, 
          typename = std::enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>>>
auto addNumericValues(T a, U b) {
    return a + b;
}
#endif

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userNum = 0;
    string userTag;

    // Dynamic input collection with stream flushing
    cout << "Enter a string tag for DataPayload (e.g., CoreBuffer): " << flush;
    if (!(cin >> userTag) || userTag.empty()) {
        userTag = "CoreBuffer";
    }

    cout << "Enter a small positive integer for computation (e.g., 5): " << flush;
    if (!(cin >> userNum) || userNum < 1) {
        cout << "Invalid integer. Defaulting to 5." << endl;
        userNum = 5;
    }

    // 1. PASS SEMANTICS & [[nodiscard]] SAFETY
    cout << "\n================ 1. PASS SEMANTICS & API SAFETY ================" << endl;
    DataPayload payload(userTag, static_cast<size_t>(userNum * 100));
    bool isPayloadValid = inspectPayload(payload);
    cout << "  - Payload Inspection Status: " << (isPayloadValid ? "VALID" : "INVALID") << endl;

    // 2. COMPILE-TIME EXECUTION
    cout << "\n================ 2. COMPILE-TIME EXECUTION (constexpr/consteval) ================" << endl;
    constexpr uint64_t compileTimeFact = computeFactorial(6); // Evaluated at compile-time
    cout << "  - Compile-Time Factorial(6) = " << compileTimeFact << endl;

    uint64_t runtimeFact = computeFactorial(static_cast<unsigned int>(userNum)); // Evaluated at runtime
    cout << "  - Runtime Factorial(" << userNum << ")      = " << runtimeFact << endl;

    constexpr uint64_t compileTimeSquare = computeImmediateSquare(15);
    cout << "  - Immediate Square (15^2)     = " << compileTimeSquare << endl;

    // 3. STRUCTURED BINDINGS & TUPLE RETURNS
    cout << "\n================ 3. MULTI-VALUE RETURNS & STRUCTURED BINDINGS ================" << endl;
    vector<int> numbers = {userNum, userNum * 2, userNum * 3, 25};
    
    // Cleanly unpacking multi-value tuple return using C++17 structured bindings
    auto [totalSum, avgVal, statusTag] = analyzeDataset(numbers);
    cout << "  - Unpacked Dataset Metrics:\n";
    cout << "    |- Total Sum     : " << totalSum << "\n";
    cout << "    |- Average Value : " << fixed << setprecision(2) << avgVal << "\n";
    cout << "    |- Status Tag    : " << statusTag << "\n";

    // 4. FIRST-CLASS CALLABLES & LAMBDAS
    cout << "\n================ 4. INLINE LAMBDAS & FIRST-CLASS CALLABLES ================" << endl;
    int multiplier = 3;
    auto scaleValue = [multiplier](int x) { return x * multiplier; }; // Stateful capture closure
    cout << "  - Lambda scaling (" << userNum << " * " << multiplier << ") = " << scaleValue(userNum) << endl;

    std::function<int(int)> genericCallable = scaleValue; // Type-erased callable wrapper
    cout << "  - std::function execution = " << genericCallable(10) << endl;

    // 5. COMPILE-TIME BRANCHING
    cout << "\n================ 5. COMPILE-TIME BRANCHING (if constexpr) ================" << endl;
    displayFormattedValue(string("Modern C++ Functions"));
    displayFormattedValue(3.14159);
    displayFormattedValue(userNum);

    // 6. CONSTRAINED GENERIC FUNCTIONS
    cout << "\n================ 6. CONSTRAINED GENERICS (CONCEPTS / SFINAE) ================" << endl;
    auto sumResult = addNumericValues(userNum, 100);
    cout << "  - addNumericValues(" << userNum << ", 100) = " << sumResult << endl;

    cout << "\n================ SUMMARY: TOP C++ FUNCTION FEATURES ================" << endl;
    cout << "1. Zero-Copy Performance : `const T&` avoids copies; `T&&` enables move semantics." << endl;
    cout << "2. Compile-Time Power    : `constexpr` & `consteval` eliminate runtime overhead." << endl;
    cout << "3. Structured Bindings   : `auto [a, b, c] = tupleFunc()` simplifies multi-value returns." << endl;
    cout << "4. Inline First-Class    : Lambdas enable flexible, local, stateful behavior injection." << endl;
    cout << "5. Modern Type Safety    : `if constexpr`, `[[nodiscard]]`, and C++20 `Concepts` make APIs safer." << endl;

    return 0;
}