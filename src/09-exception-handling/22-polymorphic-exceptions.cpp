/*
 * =====================================================================================
 * CONCEPT        : Polymorphic Exceptions under Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the rules, mechanics, guarantees, and anti-patterns of Polymorphic
 *                  Exception Handling in Modern C++:
 *
 *                  1. Exception Inheritance Hierarchy :
 *                     - Designing polymorphic custom exception hierarchies derived from
 *                       `std::exception` or `std::runtime_error`.
 *                     - Overriding virtual member functions (`what()`, custom diagnostic functions)
 *                       to provide dynamic polymorphic behavior.
 *
 *                  2. Catching by Reference vs. Value (Object Slicing Anti-Pattern) :
 *                     - Catching by Reference (`const BaseException&`) preserves dynamic dispatch,
 *                       virtual function overrides, and object identity.
 *                     - Catching by Value (`BaseException`) causes OBJECT SLICING, stripping away
 *                       derived class state and forcing static dispatch of base methods.
 *
 *                  3. Dynamic Rethrowing (`throw;` vs `throw ex;`) :
 *                     - Bare `throw;` rethrows the original active exception object intact,
 *                       preserving its dynamic runtime type across call stack boundaries.
 *                     - `throw ex;` copies the parameter object into a new exception throw, 
 *                       which SLICES derived exceptions down to the static type of `ex`.
 *
 *                  4. RTTI and Dynamic Downcasting :
 *                     - Inspecting polymorphic exceptions caught via base references using
 *                       `typeid` and `dynamic_cast` to safely retrieve derived payload fields.
 *
 * RESOLVED ISSUE : Moved `#pragma` diagnostic directives outside the `try ... catch` block construct. 
 *                  In C++ grammar, `#pragma` directives placed between a `try` block's closing brace 
 *                  and the `catch` keyword are syntax errors because `catch` must directly follow `try`.
 *                  Placing the `#pragma` outside surrounding the entire `try ... catch` block resolves 
 *                  all compiler parser errors cleanly.
 *
 * TIME COMPLEXITY  : Dynamic Virtual Dispatch / RTTI  : O(1) constant time lookup.
 *                    Polymorphic Unwinding / Matching : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Polymorphic Exception Vtable     : Standard ABI dynamic vtable pointer per object.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>
#include <typeinfo>

using namespace std;

// =====================================================================================
// 1. POLYMORPHIC EXCEPTION HIERARCHY
// Root base class and specialized derived exception classes.
// =====================================================================================

// Base Exception Class
class BaseAppException : public std::runtime_error {
private:
    int errorCode_;

public:
    BaseAppException(const string& message, int errorCode)
        : std::runtime_error(message), errorCode_(errorCode) {}

    ~BaseAppException() noexcept override = default;

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }

    // Polymorphic Virtual Method for Dynamic Diagnostic Reporting
    [[nodiscard]] virtual string getDiagnosticReport() const {
        return "BaseAppException [Code: " + to_string(errorCode_) + "]: " + what();
    }
};

// Derived Exception Class 1: Database System Errors
class DatabaseException : public BaseAppException {
private:
    string serverHost_;
    string sqlQuery_;

public:
    DatabaseException(const string& message, int errorCode, string serverHost, string sqlQuery)
        : BaseAppException(message, errorCode),
          serverHost_(std::move(serverHost)),
          sqlQuery_(std::move(sqlQuery)) {}

    ~DatabaseException() noexcept override = default;

    [[nodiscard]] const string& getServerHost() const noexcept { return serverHost_; }
    [[nodiscard]] const string& getSqlQuery() const noexcept { return sqlQuery_; }

    // Overriding Virtual Diagnostic Method
    [[nodiscard]] string getDiagnosticReport() const override {
        return "DatabaseException [Code: " + to_string(getErrorCode()) +
               " | Host: " + serverHost_ +
               " | Query: '" + sqlQuery_ + "']: " + what();
    }
};

// Derived Exception Class 2: Network Timeout Errors
class NetworkTimeoutException : public BaseAppException {
private:
    string endpointUrl_;
    int timeoutMs_;

public:
    NetworkTimeoutException(const string& message, int errorCode, string endpointUrl, int timeoutMs)
        : BaseAppException(message, errorCode),
          endpointUrl_(std::move(endpointUrl)),
          timeoutMs_(timeoutMs) {}

    ~NetworkTimeoutException() noexcept override = default;

    [[nodiscard]] const string& getEndpointUrl() const noexcept { return endpointUrl_; }
    [[nodiscard]] int getTimeoutMs() const noexcept { return timeoutMs_; }

    // Overriding Virtual Diagnostic Method
    [[nodiscard]] string getDiagnosticReport() const override {
        return "NetworkTimeoutException [Code: " + to_string(getErrorCode()) +
               " | Endpoint: " + endpointUrl_ +
               " | Timeout: " + to_string(timeoutMs_) + " ms]: " + what();
    }
};

// =====================================================================================
// 2. HELPER FUNCTIONS DEMONSTRATING POLYMORPHIC EXCEPTION SCENARIOS
// =====================================================================================

// Function that throws a dynamic DatabaseException
void triggerDatabaseOperation(int inputVal) {
    if (inputVal > 50) {
        throw DatabaseException(
            "Transaction aborted due to lock contention",
            1205,
            "db-primary-node-01.internal",
            "UPDATE accounts SET balance = balance - 500 WHERE id = " + to_string(inputVal)
        );
    }
    cout << "    [Database] Operation completed for ID: " << inputVal << "\n";
}

// Function that throws a dynamic NetworkTimeoutException
void triggerNetworkOperation(int inputVal) {
    if (inputVal > 20) {
        throw NetworkTimeoutException(
            "Socket read timeout while awaiting headers",
            504,
            "https://api.gateway.internal/v2/orders?id=" + to_string(inputVal),
            3000
        );
    }
    cout << "    [Network] Operation completed for ID: " << inputVal << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Polymorphic Exceptions analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CORRECT PATTERN: CATCHING BY REFERENCE (`const BaseAppException&`)
    // =====================================================================================
    cout << "\n================ 1. CATCHING BY REFERENCE (PRESERVES POLYMORPHISM) ================\n";

    try {
        cout << "  - Triggering database operation with input = " << userInputValue << "...\n";
        triggerDatabaseOperation(userInputValue);
    } catch (const BaseAppException& exRef) {
        cout << "  - [CAUGHT BY REFERENCE `const BaseAppException&`]:\n";
        cout << "    * Dynamic RTTI Type Name : " << typeid(exRef).name() << "\n";
        cout << "    * Virtual Method Output  : " << exRef.getDiagnosticReport() << "\n";
        cout << "    [RESULT]: Polymorphic dispatch invoked `DatabaseException::getDiagnosticReport()` successfully!\n";
    }

    // =====================================================================================
    // 2. ANTI-PATTERN: CATCHING BY VALUE (OBJECT SLICING)
    // =====================================================================================
    cout << "\n================ 2. CATCHING BY VALUE (OBJECT SLICING ANTI-PATTERN) ================\n";

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wcatch-value"
#elif defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcatch-value"
#endif

    try {
        cout << "  - Triggering database operation with input = " << userInputValue << "...\n";
        triggerDatabaseOperation(userInputValue);
    } catch (BaseAppException exVal) { // CATCH BY VALUE - SLICES DERIVED OBJECT!
        cout << "  - [CAUGHT BY VALUE `BaseAppException`]:\n";
        cout << "    * Static RTTI Type Name  : " << typeid(exVal).name() << "\n";
        cout << "    * Virtual Method Output  : " << exVal.getDiagnosticReport() << "\n";
        cout << "    [RESULT]: OBJECT SLICING OCCURRED! Derived `DatabaseException` fields were stripped,\n"
             << "              and virtual dispatch fell back to `BaseAppException::getDiagnosticReport()`!\n";
    }

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif

    // =====================================================================================
    // 3. POLYMORPHIC RETHROWING (`throw;` vs `throw ex;`)
    // =====================================================================================
    cout << "\n================ 3. POLYMORPHIC RETHROWING (`throw;` vs `throw ex;`) ================\n";

    // Scenario A: Bare `throw;` preserves dynamic type
    cout << "  - Scenario A: Intercepting and rethrowing via bare `throw;`...\n";
    try {
        try {
            triggerDatabaseOperation(userInputValue);
        } catch (const BaseAppException& caughtRef) {
            cout << "    [Intermediate Layer] Intercepted exception as `const BaseAppException&`.\n";
            cout << "    [Intermediate Layer] Executing bare `throw;` to rethrow original active object...\n";
            throw; // CORRECT: Preserves dynamic DatabaseException object
        }
    } catch (const BaseAppException& mainRef) {
        cout << "  - [MAIN CATCH] Re-caught exception:\n";
        cout << "    * RTTI Type Name        : " << typeid(mainRef).name() << "\n";
        cout << "    * Diagnostic Method     : " << mainRef.getDiagnosticReport() << "\n";
        cout << "    [VERIFICATION]: Dynamic type `DatabaseException` was PRESERVED across rethrow!\n";
    }

    // Scenario B: Sliced `throw ex;` loses dynamic type
    cout << "\n  - Scenario B: Intercepting and rethrowing via sliced `throw caughtEx;`...\n";
    try {
        try {
            triggerDatabaseOperation(userInputValue);
        } catch (const BaseAppException& caughtRef) {
            cout << "    [Intermediate Layer] Intercepted exception as `const BaseAppException&`.\n";
            cout << "    [Intermediate Layer] Executing `throw caughtRef;` (Anti-pattern - Slices object!)...\n";
            throw caughtRef; // SLICES derived exception down to static type BaseAppException!
        }
    } catch (const BaseAppException& mainRef) {
        cout << "  - [MAIN CATCH] Re-caught exception:\n";
        cout << "    * RTTI Type Name        : " << typeid(mainRef).name() << "\n";
        cout << "    * Diagnostic Method     : " << mainRef.getDiagnosticReport() << "\n";
        cout << "    [VERIFICATION]: Dynamic type was SLICED down to BaseAppException on rethrow!\n";
    }

    // =====================================================================================
    // 4. RTTI AND DYNAMIC DOWNCASTING ON POLYMORPHIC EXCEPTIONS
    // =====================================================================================
    cout << "\n================ 4. RTTI AND DYNAMIC DOWNCASTING ================\n";

    try {
        cout << "  - Triggering network operation with input = " << userInputValue << "...\n";
        triggerNetworkOperation(userInputValue);
    } catch (const std::exception& stdRef) {
        cout << "  - [CAUGHT VIA ROOT `const std::exception&`]:\n";
        cout << "    * Standard Message (`what()`) : " << stdRef.what() << "\n";
        cout << "    * RTTI Dynamic Type           : " << typeid(stdRef).name() << "\n";

        // Downcasting safely to inspect specialized fields
        if (const auto* netEx = dynamic_cast<const NetworkTimeoutException*>(&stdRef)) {
            cout << "    -> [RTTI DOWNCAST SUCCESS]: Identified `NetworkTimeoutException`!\n";
            cout << "       Target Endpoint: " << netEx->getEndpointUrl() << "\n";
            cout << "       Timeout Limit  : " << netEx->getTimeoutMs() << " ms\n";
        } else if (const auto* dbEx = dynamic_cast<const DatabaseException*>(&stdRef)) {
            cout << "    -> [RTTI DOWNCAST SUCCESS]: Identified `DatabaseException`!\n";
            cout << "       Target Server  : " << dbEx->getServerHost() << "\n";
        }
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ POLYMORPHIC EXCEPTIONS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Mechanism / Pattern   | C++ Implementation Syntax         | Architectural Effect & Guarantee  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Catch by Reference    | `catch (const BaseException& ex)` | Preserves dynamic vtable & fields |\n"
         << "| Catch by Value        | `catch (BaseException ex)`        | SLICES derived object to base copy|\n"
         << "| Bare Rethrow          | `throw;`                          | Preserves original dynamic object |\n"
         << "| Copy Rethrow          | `throw caughtEx;`                 | SLICES exception to static type   |\n"
         << "| RTTI Downcasting      | `dynamic_cast<const Derived*>(&e)`| Safely inspects derived details   |\n"
         << "| Root Standard Catch   | `catch (const std::exception& e)` | Universal polymorphic exception   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}