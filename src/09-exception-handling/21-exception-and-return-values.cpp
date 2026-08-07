/*
 * =====================================================================================
 * CONCEPT        : Exception and Return Values under Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the relationship, tradeoffs, and architectural patterns between
 *                  Exceptions and Return Values in Modern C++:
 *
 *                  1. Return Values vs. Exceptions (Expression Composability) :
 *                     - Traditional C-style error codes require out-parameters and 
 *                       verbose `if(err)` branching at every call site.
 *                     - Exceptions decouple error handling from return values, allowing
 *                       functions to return values directly and enabling clean expression
 *                       composition (e.g., `int z = add(divide(x, y), 10);`).
 *
 *                  2. Return Value Lifetime & Throw Mechanics :
 *                     - If an exception is thrown *before* or *during* the evaluation of a
 *                       `return` statement, no return value object is delivered to the caller.
 *                     - Local objects in the function frame are unwound via RAII before
 *                       control reaches the catch block.
 *
 *                  3. Modern Alternatives (Monadic Return Values - `std::optional` & `std::expected`) :
 *                     - For expected domain conditions (e.g., cache miss, lookup failure), 
 *                       returning `std::optional<T>` or `std::expected<T, E>` (C++23) is faster 
 *                       and more explicit than throwing exceptions.
 *                     - Exceptions should be reserved for *exceptional* conditions that the
 *                       immediate caller cannot reasonably handle.
 *
 *                  4. Exception Barrier Pattern (Exceptions -> Return Values) :
 *                     - At system, thread, or C-ABI boundaries, exceptions must be intercepted 
 *                       via `try/catch` and translated back into status return values or error codes.
 *
 * TIME COMPLEXITY  : Direct Return (Success Path)    : O(1) constant time.
 *                    Exception Dispatch (Failure)     : O(Depth of Call Stack).
 *                    Monadic Error Return (`optional`) : O(1) zero-overhead value return.
 * SPACE COMPLEXITY : Exception Storage Overhead       : ABI-managed runtime memory block.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <optional>
#include <variant>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. TRADITIONAL RETURN CODE vs. EXCEPTION RETURN VALUE COMPARISON
// =====================================================================================

// Legacy Style: Return value is used for error code; actual result passed via Out-Parameter
bool divideLegacy(int numerator, int denominator, int& outResult) noexcept {
    if (denominator == 0) {
        return false; // Return code indicates failure
    }
    outResult = numerator / denominator;
    return true; // Return code indicates success
}

// Modern C++ Exception Style: Result returned directly; errors communicated via Throw
int divideModern(int numerator, int denominator) {
    if (denominator == 0) {
        throw std::invalid_argument("Division by zero in divideModern()");
    }
    return numerator / denominator;
}

// =====================================================================================
// 2. MONADIC ERROR HANDLING SIMULATION (CUSTOM `Result<T, E>` / `std::expected` C++23)
// Lightweight template wrapping either a valid Return Value OR an Error Object.
// =====================================================================================
template <typename T, typename E>
class Result {
private:
    std::variant<T, E> storage_;

public:
    /* implicit */ Result(T value) : storage_(std::move(value)) {}
    
    struct ErrTag {};
    Result(ErrTag, E error) : storage_(std::move(error)) {}

    static Result<T, E> error(E err) {
        return Result(ErrTag{}, std::move(err));
    }

    [[nodiscard]] bool isSuccess() const noexcept {
        return std::holds_alternative<T>(storage_);
    }

    [[nodiscard]] const T& value() const {
        if (!isSuccess()) {
            throw std::bad_variant_access();
        }
        return std::get<T>(storage_);
    }

    [[nodiscard]] const E& error() const {
        if (isSuccess()) {
            throw std::bad_variant_access();
        }
        return std::get<E>(storage_);
    }
};

// Function returning Result<T, E> instead of throwing exceptions
Result<int, string> divideMonadic(int numerator, int denominator) noexcept {
    if (denominator == 0) {
        return Result<int, string>::error("Error: Cannot divide by zero in monadic operation!");
    }
    return numerator / denominator;
}

// Function returning std::optional<T> for simple lookup failures
std::optional<int> parseInteger(const string& str) noexcept {
    try {
        size_t processedChars = 0;
        int parsedVal = std::stoi(str, &processedChars);
        if (processedChars == str.length()) {
            return parsedVal;
        }
    } catch (...) {
        // Fallthrough to return nullopt
    }
    return std::nullopt;
}

// =====================================================================================
// 3. LIFECYCLE TRACKER TO DEMONSTRATE RETURN VALUE DESTRUCTION ON THROW
// =====================================================================================
class ReturnValueLifecycleTracker {
private:
    string label_;

public:
    explicit ReturnValueLifecycleTracker(string label) : label_(std::move(label)) {
        cout << "      [Tracker Ctor] Created object: '" << label_ << "'\n";
    }

    ~ReturnValueLifecycleTracker() noexcept {
        cout << "      [Tracker Dtor] Destructed object: '" << label_ << "'\n";
    }

    ReturnValueLifecycleTracker(const ReturnValueLifecycleTracker& other)
        : label_(other.label_ + "_copy") {
        cout << "      [Tracker Copy Ctor] Copied to: '" << label_ << "'\n";
    }

    ReturnValueLifecycleTracker(ReturnValueLifecycleTracker&& other) noexcept
        : label_(std::move(other.label_)) {
        other.label_ += "_moved_from";
        cout << "      [Tracker Move Ctor] Moved to: '" << label_ << "'\n";
    }
};

ReturnValueLifecycleTracker demonstrateReturnValueThrow(bool triggerThrow) {
    cout << "    -> Entering `demonstrateReturnValueThrow()`...\n";
    ReturnValueLifecycleTracker localObj("LocalStackObject");

    if (triggerThrow) {
        cout << "    -> Throwing exception BEFORE return statement executes...\n";
        throw std::runtime_error("Exception thrown before return value evaluation!");
    }

    cout << "    -> Executing return statement (RVO / NRVO engaged)...\n";
    return localObj;
}

// =====================================================================================
// 4. EXCEPTION BARRIER (CONVERTING EXCEPTIONS TO RETURN VALUES)
// Used at C-ABI or RPC boundaries where exceptions cannot cross.
// =====================================================================================
enum class ApiStatus {
    Success = 0,
    InvalidArgument = 1,
    RuntimeError = 2,
    UnknownError = 99
};

ApiStatus apiExceptionBarrier(int numerator, int denominator, int* outResult) noexcept {
    try {
        if (!outResult) {
            throw std::invalid_argument("Null output pointer provided to API barrier!");
        }
        // Core C++ calculation returning value
        *outResult = divideModern(numerator, denominator);
        return ApiStatus::Success;

    } catch (const std::invalid_argument& ex) {
        cout << "    [API Barrier Catch] Intercepted `invalid_argument`: \"" << ex.what() << "\"\n";
        return ApiStatus::InvalidArgument;
    } catch (const std::runtime_error& ex) {
        cout << "    [API Barrier Catch] Intercepted `runtime_error`: \"" << ex.what() << "\"\n";
        return ApiStatus::RuntimeError;
    } catch (...) {
        cout << "    [API Barrier Catch] Intercepted unknown exception!\n";
        return ApiStatus::UnknownError;
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Exception & Return Value analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. RETURN VALUES vs. EXCEPTIONS (EXPRESSION COMPOSABILITY)
    // =====================================================================================
    cout << "\n================ 1. RETURN VALUES vs. EXCEPTIONS =================\n";

    // Scenario A: Legacy Out-Parameters & Return Codes
    cout << "  - Scenario A: Legacy Return Codes & Out-Parameters:\n";
    int res1 = 0;
    int res2 = 0;
    if (divideLegacy(userInputValue, 2, res1) && divideLegacy(res1, 5, res2)) {
        cout << "    * Result (100 / 2 / 5) = " << res2 << " (Requires nested `if` checks)\n";
    }

    // Scenario B: Direct Exception Return Value Composition
    cout << "\n  - Scenario B: Modern Direct Return Value Composition:\n";
    try {
        // Direct expression composition enabled by returning values directly!
        int modernRes = divideModern(divideModern(userInputValue, 2), 5);
        cout << "    * Result (100 / 2 / 5) = " << modernRes << " (Composed cleanly in 1 line!)\n";
    } catch (const std::exception& ex) {
        cout << "    [UNREACHABLE]: " << ex.what() << "\n";
    }

    // Scenario C: Modern Exception Trigger on Divide By Zero
    try {
        cout << "\n  - Scenario C: Attempting divideByZero with modern exceptions...\n";
        [[maybe_unused]] int invalidRes = divideModern(userInputValue, 0);
    } catch (const std::exception& ex) {
        cout << "    * [CAUGHT EXCEPTION]: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // 2. RETURN VALUE LIFECYCLE & THROW MECHANICS
    // =====================================================================================
    cout << "\n================ 2. RETURN VALUE LIFECYCLE ON THROW ================\n";

    try {
        cout << "  - Calling function that throws before return completes...\n";
        [[maybe_unused]] auto resultObj = demonstrateReturnValueThrow(true);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT EXCEPTION IN MAIN]: \"" << ex.what() << "\"\n";
        cout << "    [VERIFICATION]: Local object was destroyed via RAII; no return value was constructed!\n";
    }

    // =====================================================================================
    // 3. MONADIC ERROR HANDLING (`Result<T, E>` & `std::optional<T>`)
    // =====================================================================================
    cout << "\n================ 3. MONADIC ERROR HANDLING (`Result` & `std::optional`) ================\n";

    // Testing Monadic Result
    auto monRes1 = divideMonadic(userInputValue, 4);
    if (monRes1.isSuccess()) {
        cout << "  - Monadic Success (" << userInputValue << " / 4) = " << monRes1.value() << "\n";
    }

    auto monRes2 = divideMonadic(userInputValue, 0);
    if (!monRes2.isSuccess()) {
        cout << "  - Monadic Failure Handled Without Exceptions: \"" << monRes2.error() << "\"\n";
    }

    // Testing std::optional
    cout << "\n  - Testing `std::optional<int>` parsing:\n";
    auto opt1 = parseInteger("42");
    if (opt1.has_value()) {
        cout << "    * Parsed '42' -> " << opt1.value() << "\n";
    }

    auto opt2 = parseInteger("InvalidNumber_99");
    if (!opt2.has_value()) {
        cout << "    * Parsing 'InvalidNumber_99' failed gracefully -> returned `std::nullopt`\n";
    }

    // =====================================================================================
    // 4. EXCEPTION BARRIERS (EXCEPTIONS TO RETURN VALUES)
    // =====================================================================================
    cout << "\n================ 4. EXCEPTION BARRIER (EXCEPTIONS -> RETURN CODES) ================\n";

    int barrierOut = 0;
    
    cout << "  - Calling API barrier with valid input (100 / 2)...\n";
    ApiStatus status1 = apiExceptionBarrier(userInputValue, 2, &barrierOut);
    cout << "    * Status: " << static_cast<int>(status1) << " (Success), Output = " << barrierOut << "\n";

    cout << "\n  - Calling API barrier with divide by zero (100 / 0)...\n";
    ApiStatus status2 = apiExceptionBarrier(userInputValue, 0, &barrierOut);
    cout << "    * Status: " << static_cast<int>(status2) << " (InvalidArgument - Handled at barrier)\n";

    cout << "\n  - Calling API barrier with null output pointer...\n";
    ApiStatus status3 = apiExceptionBarrier(userInputValue, 2, nullptr);
    cout << "    * Status: " << static_cast<int>(status3) << " (InvalidArgument - Handled at barrier)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION & RETURN VALUES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Strategy / Mechanism  | C++ Implementation Standard       | Primary Benefit & Use Case        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Return Codes (Legacy) | `bool func(int& outResult)`       | Out-parameter verbose checks      |\n"
         << "| C++ Exceptions        | `throw std::runtime_error(...)`   | Direct returns & clean composition|\n"
         << "| Monadic Result        | `Result<T, E>` / `std::expected`  | Explicit non-throwing domain errors|\n"
         << "| `std::optional<T>`    | `std::optional<T> parse()`        | Lightweight zero-cost null state  |\n"
         << "| Exception Barrier     | `try { ... } catch(...) { return}`| Safely wraps C-ABI / RPC edges    |\n"
         << "| Throw Lifecycle       | Destructs frame before return     | Guarantees no leak or partial copy|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}