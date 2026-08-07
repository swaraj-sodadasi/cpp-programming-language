/*
 * =====================================================================================
 * CONCEPT        : Modern Exception Handling Paradigm in Modern C++ (C++11 through C++23)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the core features, idioms, and architectural paradigms of Modern C++ 
 *                  Exception Handling:
 *
 *                  1. Exception Chaining & Nesting (`std::throw_with_nested`, `std::rethrow_if_nested`) :
 *                     - C++11 feature allowing high-level subsystem layers to wrap and rethrow
 *                       low-level exceptions while preserving the full root cause chain.
 *
 *                  2. Active Uncaught Exception Inspection (`std::uncaught_exceptions`) :
 *                     - C++17 replacement for deprecated `std::uncaught_exception()`.
 *                     - Counts the exact number of active, uncaught exceptions in the current thread,
 *                       enabling reliable ScopeGuards (ScopeExit, ScopeFail, ScopeSuccess).
 *
 *                  3. First-Class Exception Transport (`std::exception_ptr`) :
 *                     - Thread-safe handle to an exception object (`std::current_exception()`,
 *                       `std::rethrow_exception()`) allowing exceptions to cross thread and
 *                       coroutine boundaries safely.
 *
 *                  4. Monadic / Functional Alternatives (`std::optional`, `std::variant`, `Expected`) :
 *                     - Modern C++ complements exceptions with value-based error types for non-exceptional,
 *                       frequent domain failures, reserving thrown exceptions for truly exceptional errors.
 *
 *                  5. Modern `noexcept` Specifiers & Type-Trait Deduction :
 *                     - Conditional `noexcept(expr)` coupled with `<type_traits>` to deduce
 *                       compile-time exception guarantees for templates automatically.
 *
 * TIME COMPLEXITY  : Exception Nesting Unwind : O(Depth of Nested Exception Chain).
 *                    Exception Pointer Copy   : O(1) atomic ref-count increment.
 *                    Monadic Error Return     : O(1) zero-overhead value dispatch.
 * SPACE COMPLEXITY : Exception ABI Storage    : Managed runtime memory pool for exception payloads.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <type_traits>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// 1. NESTED EXCEPTION CHAINING (`std::throw_with_nested` / `std::rethrow_if_nested`)
// Enables multi-layered error context wrapping without losing original root cause.
// =====================================================================================
class InfrastructureIOException : public std::runtime_error {
private:
    int sysErrorCode_;

public:
    InfrastructureIOException(const string& msg, int code)
        : std::runtime_error(msg), sysErrorCode_(code) {}

    [[nodiscard]] int getSysErrorCode() const noexcept { return sysErrorCode_; }
};

class DatabaseServiceException : public std::runtime_error {
private:
    string queryId_;

public:
    DatabaseServiceException(const string& msg, string queryId)
        : std::runtime_error(msg), queryId_(std::move(queryId)) {}

    [[nodiscard]] const string& getQueryId() const noexcept { return queryId_; }
};

// Low-level function where root cause failure occurs
void lowLevelStorageDriver(int inputVal) {
    if (inputVal > 50) {
        throw InfrastructureIOException("I/O Failure: Hardware sector 0x7F4A corrupt or unreadable", 507);
    }
    cout << "    [Storage Driver] Data block written successfully.\n";
}

// Intermediate service function wrapping low-level exception with `std::throw_with_nested`
void midLevelDatabaseService(int inputVal) {
    try {
        lowLevelStorageDriver(inputVal);
    } catch (...) {
        cout << "    [Database Service] Intercepted low-level error. Nesting inside `DatabaseServiceException`...\n";
        std::throw_with_nested(
            DatabaseServiceException("Database Query Transaction Failed", "QRY-99812")
        );
    }
}

// Recursive helper function to unpack and print the complete nested exception chain
void printNestedExceptionChain(const std::exception& ex, int depth = 1) {
    string indent(depth * 2, ' ');
    cout << indent << "-> [Level " << depth << " Exception]: " << ex.what() 
         << " (RTTI Type: " << typeid(ex).name() << ")\n";

    try {
        // Rethrows inner nested exception if present
        std::rethrow_if_nested(ex);
    } catch (const std::exception& nestedEx) {
        printNestedExceptionChain(nestedEx, depth + 1);
    } catch (...) {
        cout << indent << "  -> [Level " << (depth + 1) << " Exception]: Non-std nested exception object\n";
    }
}

// =====================================================================================
// 2. ACTIVE UNCAUGHT EXCEPTION COUNTING (`std::uncaught_exceptions` - C++17)
// Demonstrates modern RAII Scope Fail/Success detection.
// =====================================================================================
class ModernUncaughtScopeGuard {
private:
    string scopeName_;
    int initialUncaughtCount_;

public:
    explicit ModernUncaughtScopeGuard(string name)
        : scopeName_(std::move(name)), initialUncaughtCount_(std::uncaught_exceptions()) {
        cout << "    [ScopeGuard Ctor] '" << scopeName_ << "' active. Initial uncaught count: " 
             << initialUncaughtCount_ << "\n";
    }

    ~ModernUncaughtScopeGuard() noexcept {
        int currentUncaughtCount = std::uncaught_exceptions();
        if (currentUncaughtCount > initialUncaughtCount_) {
            cout << "    [ScopeGuard Dtor] Scope exit for '" << scopeName_ 
                 << "' triggered by STACK UNWINDING! (Uncaught count: " << currentUncaughtCount << ")\n";
            cout << "    [ScopeGuard Action] Executing ROLLBACK operations...\n";
        } else {
            cout << "    [ScopeGuard Dtor] Scope exit for '" << scopeName_ 
                 << "' completed NORMALLY. (Uncaught count: " << currentUncaughtCount << ")\n";
            cout << "    [ScopeGuard Action] Executing COMMIT operations...\n";
        }
    }
};

// =====================================================================================
// 3. FIRST-CLASS EXCEPTION TRANSPORT (`std::exception_ptr`)
// Transporting exceptions safely across functions, threads, or asynchronous futures.
// =====================================================================================
class AsyncExceptionTransportDemo {
public:
    static std::exception_ptr executeWorkerTask(int inputVal) noexcept {
        try {
            if (inputVal > 20) {
                throw std::out_of_range("Async Worker Task: Input value exceeds threshold limit (20)!");
            }
            cout << "    [Async Worker] Task processed successfully.\n";
            return nullptr; // No exception occurred
        } catch (...) {
            cout << "    [Async Worker] Capturing current active exception via `std::current_exception()`...\n";
            return std::current_exception();
        }
    }

    static void rethrowAndHandle(std::exception_ptr exPtr) {
        if (exPtr) {
            cout << "    [Consumer Boundary] Exception pointer detected. Rethrowing via `std::rethrow_exception()`...\n";
            std::rethrow_exception(exPtr);
        }
    }
};

// =====================================================================================
// 4. MONADIC VALUE-BASED ERROR ALTERNATIVE (`Expected<T, E>`)
// Lightweight functional alternative to exceptions for expected non-fatal failures.
// =====================================================================================
template <typename T, typename E>
class Expected {
private:
    std::variant<T, E> storage_;

public:
    /* implicit */ Expected(T val) : storage_(std::move(val)) {}

    struct ErrTag {};
    Expected(ErrTag, E err) : storage_(std::move(err)) {}

    static Expected error(E err) {
        return Expected(ErrTag{}, std::move(err));
    }

    [[nodiscard]] bool hasValue() const noexcept {
        return std::holds_alternative<T>(storage_);
    }

    [[nodiscard]] const T& value() const {
        if (!hasValue()) {
            throw std::bad_variant_access();
        }
        return std::get<T>(storage_);
    }

    [[nodiscard]] const E& error() const {
        if (hasValue()) {
            throw std::bad_variant_access();
        }
        return std::get<E>(storage_);
    }
};

// Demonstrating Monadic Return Value
Expected<double, string> safeCalculateRatio(double numerator, double denominator) noexcept {
    if (denominator == 0.0) {
        return Expected<double, string>::error("Value Error: Mathematical division by zero!");
    }
    return numerator / denominator;
}

// =====================================================================================
// 5. CONDITIONAL `noexcept` DEDUCTION & TYPE TRAITS
// =====================================================================================
template <typename T>
void modernGenericSwap(T& a, T& b) noexcept(
    std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>
) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Modern Exception Analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. EXCEPTION CHAINING & NESTING (`std::throw_with_nested`)
    // =====================================================================================
    cout << "\n================ 1. EXCEPTION CHAINING & NESTING (`std::throw_with_nested`) ================\n";

    try {
        cout << "  - Calling `midLevelDatabaseService(" << userInputValue << ")`...\n";
        midLevelDatabaseService(userInputValue);
    } catch (const std::exception& topLevelEx) {
        cout << "  - [CAPTURED TOP-LEVEL EXCEPTION]:\n";
        printNestedExceptionChain(topLevelEx);
        cout << "    [VERIFICATION]: Unpacked complete chain from high-level database exception\n"
             << "                    down to low-level hardware I/O sector error!\n";
    }

    // =====================================================================================
    // 2. ACTIVE UNCAUGHT EXCEPTION COUNTING (`std::uncaught_exceptions`)
    // =====================================================================================
    cout << "\n================ 2. ACTIVE UNCAUGHT EXCEPTION COUNTING (C++17) ================\n";

    cout << "  - Scenario A: Scope exit with exception throw...\n";
    try {
        ModernUncaughtScopeGuard scopeGuardA("TransactionalScope_A");
        cout << "    [Scope Body] Simulating operation failure...\n";
        throw std::runtime_error("Simulated Operational Failure");
    } catch (const std::exception& ex) {
        cout << "    [Main Catch] Handled: \"" << ex.what() << "\"\n";
    }

    cout << "\n  - Scenario B: Scope exit completed normally...\n";
    {
        ModernUncaughtScopeGuard scopeGuardB("TransactionalScope_B");
        cout << "    [Scope Body] Work executed successfully.\n";
    }

    // =====================================================================================
    // 3. FIRST-CLASS EXCEPTION TRANSPORT (`std::exception_ptr`)
    // =====================================================================================
    cout << "\n================ 3. FIRST-CLASS EXCEPTION TRANSPORT (`std::exception_ptr`) ================\n";

    cout << "  - Executing async worker task with input = " << userInputValue << "...\n";
    std::exception_ptr transportedEx = AsyncExceptionTransportDemo::executeWorkerTask(userInputValue);

    if (transportedEx) {
        try {
            cout << "  - Transported exception handle received in main context.\n";
            AsyncExceptionTransportDemo::rethrowAndHandle(transportedEx);
        } catch (const std::out_of_range& capturedEx) {
            cout << "  - [MAIN HANDLER CAUGHT TRANSPORTED EXCEPTION]: \"" << capturedEx.what() << "\"\n";
            cout << "    [VERIFICATION]: Exception object was captured and rethrown across execution contexts!\n";
        }
    }

    // =====================================================================================
    // 4. MONADIC FUNCTIONAL ERROR ALTERNATIVE (`Expected<T, E>`)
    // =====================================================================================
    cout << "\n================ 4. MONADIC VALUE-BASED ERROR ALTERNATIVES ================\n";

    cout << "  - Testing `safeCalculateRatio(" << userInputValue << ", 4.0)`...\n";
    auto validRatio = safeCalculateRatio(static_cast<double>(userInputValue), 4.0);
    if (validRatio.hasValue()) {
        cout << "    * Result = " << validRatio.value() << "\n";
    }

    cout << "  - Testing `safeCalculateRatio(" << userInputValue << ", 0.0)`...\n";
    auto invalidRatio = safeCalculateRatio(static_cast<double>(userInputValue), 0.0);
    if (!invalidRatio.hasValue()) {
        cout << "    * Monadic Error Handled Without Exception Throw: \"" << invalidRatio.error() << "\"\n";
    }

    // =====================================================================================
    // 5. CONDITIONAL `noexcept` SPECIFIER EVALUATION
    // =====================================================================================
    cout << "\n================ 5. CONDITIONAL `noexcept` & TYPE TRAITS ================\n";

    int valA = 10;
    int valB = 20;

    cout << "  - Inspecting `modernGenericSwap(int, int)`:\n";
    cout << "    * Is `modernGenericSwap` nothrow? : " 
         << (noexcept(modernGenericSwap(valA, valB)) ? "TRUE (Nothrow guaranteed)" : "FALSE") << "\n";

    modernGenericSwap(valA, valB);
    cout << "    * Swapped Values: valA = " << valA << ", valB = " << valB << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MODERN EXCEPTION HANDLING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Modern Feature        | C++ Standard / API                | Architectural Advantage           |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Exception Nesting     | `std::throw_with_nested(ex)`      | Wraps errors keeping root causes  |\n"
         << "| Chain Unpacking       | `std::rethrow_if_nested(ex)`      | Recursively inspects error trees  |\n"
         << "| Uncaught Inspector    | `std::uncaught_exceptions()`      | C++17 safe ScopeGuard implementation|\n"
         << "| Exception Transport   | `std::exception_ptr` & `rethrow`  | Passes exceptions across threads  |\n"
         << "| Monadic Return        | `Expected<T, E>` / `optional`     | Zero-overhead value error returns |\n"
         << "| Conditional Specifier | `noexcept(is_nothrow_v<T>)`       | Automatic template safety traits  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}