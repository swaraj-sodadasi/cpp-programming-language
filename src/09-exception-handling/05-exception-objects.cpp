/*
 * =====================================================================================
 * CONCEPT        : Exception Objects under Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the creation, lifetime, storage, copying, slicing, and polymorphic 
 *                  behavior of Exception Objects in Modern C++:
 *
 *                  1. Nature & Storage of Exception Objects :
 *                     - Exception objects are stored in a dedicated, thread-safe runtime
 *                       buffer managed by the C++ ABI (`__cxa_allocate_exception`), independent
 *                       of normal stack frames and standard user heap allocations.
 *                     - Exception objects persist until the matching `catch` block finishes executing.
 *
 *                  2. Lifetime, Copying, and Moving :
 *                     - When an exception is thrown, the compiler constructs or copies the 
 *                       exception object into the runtime exception buffer.
 *                     - Exception classes MUST have accessible (non-deleted) copy/move constructors 
 *                       and non-throwing destructors (`noexcept`).
 *
 *                  3. Stateful Custom Exception Objects :
 *                     - Extending standard exception types (`std::runtime_error`, `std::exception`)
 *                       to store rich contextual data (e.g., error codes, query strings, timestamps).
 *
 *                  4. Catching Exception Objects (Polymorphism vs. Slicing) :
 *                     - Catching by Value (`catch (BaseException ex)`) -> Slices derived attributes 
 *                       and loses dynamic virtual table dispatch!
 *                     - Catching by Const Reference (`catch (const BaseException& ex)`) -> Preserves 
 *                       derived exception attributes and dynamic `what()` virtual dispatch.
 *
 *                  5. Transporting Exception Objects (`std::exception_ptr`) :
 *                     - Managing exception object lifetimes across execution contexts or threads 
 *                       using `std::current_exception()`, `std::exception_ptr`, and `std::rethrow_exception()`.
 *
 * RESOLVED ISSUE : Moved `#pragma` diagnostic directives outside the `try` block construct. 
 *                  In C++ grammar, `#pragma` directives placed between a `try` block's closing brace 
 *                  and the `catch` keyword are syntax errors because `catch` must directly follow `try`.
 *                  Placing the `#pragma` outside surrounding the entire `try ... catch` block resolves 
 *                  all compiler parser errors cleanly.
 *
 * TIME COMPLEXITY  : Normal Path (No Exceptions)     : O(1) Zero-cost table lookup overhead.
 *                    Exception Object Propagation    : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Storage Overhead     : ABI-managed runtime memory block per active throw.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>

using namespace std;

// =====================================================================================
// 1. TRACEABLE EXCEPTION OBJECT (TO DEMONSTRATE CONSTRUCTION, COPY, MOVE, & DTOR)
// Traces memory lifecycle operations performed by the compiler during throw/catch.
// =====================================================================================
class LifetimeTracedException : public std::exception {
private:
    string message_;
    int traceId_;

public:
    LifetimeTracedException(string msg, int id)
        : message_(std::move(msg)), traceId_(id) {
        cout << "      [EXCEPTION OBJ CTOR]  Created exception object (ID: " << traceId_ 
             << ") at address: " << static_cast<const void*>(this) << "\n";
    }

    // Copy Constructor
    LifetimeTracedException(const LifetimeTracedException& other)
        : std::exception(other), message_(other.message_), traceId_(other.traceId_ + 100) {
        cout << "      [EXCEPTION OBJ COPY]  Cloned exception object from ID " << other.traceId_ 
             << " to ID " << traceId_ << " at address: " << static_cast<const void*>(this) << "\n";
    }

    // Move Constructor
    LifetimeTracedException(LifetimeTracedException&& other) noexcept
        : std::exception(std::move(other)), message_(std::move(other.message_)), traceId_(other.traceId_) {
        cout << "      [EXCEPTION OBJ MOVE]  Moved exception object (ID: " << traceId_ 
             << ") to address: " << static_cast<const void*>(this) << "\n";
    }

    ~LifetimeTracedException() noexcept override {
        cout << "      [EXCEPTION OBJ DTOR]  Destroyed exception object (ID: " << traceId_ 
             << ") at address: " << static_cast<const void*>(this) << "\n";
    }

    [[nodiscard]] const char* what() const noexcept override {
        return message_.c_str();
    }

    [[nodiscard]] int getTraceId() const noexcept { return traceId_; }
};

// =====================================================================================
// 2. POLYMORPHIC STATEFUL EXCEPTION HIERARCHY
// =====================================================================================
class DatabaseException : public std::runtime_error {
private:
    int errorCode_;
    string sqlState_;

public:
    DatabaseException(const string& msg, int errorCode, string sqlState)
        : std::runtime_error(msg), errorCode_(errorCode), sqlState_(std::move(sqlState)) {}

    ~DatabaseException() noexcept override = default;

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }
    [[nodiscard]] const string& getSqlState() const noexcept { return sqlState_; }

    [[nodiscard]] virtual string getFullDiagnostic() const {
        return "DatabaseException [ErrorCode: " + to_string(errorCode_) + 
               ", SQLState: " + sqlState_ + "] -> " + what();
    }
};

class QueryExecutionException : public DatabaseException {
private:
    string failedQuery_;
    double executionTimeMs_;

public:
    QueryExecutionException(const string& msg, int errorCode, string sqlState, string query, double execTimeMs)
        : DatabaseException(msg, errorCode, std::move(sqlState)),
          failedQuery_(std::move(query)),
          executionTimeMs_(execTimeMs) {}

    ~QueryExecutionException() noexcept override = default;

    [[nodiscard]] const string& getFailedQuery() const noexcept { return failedQuery_; }
    [[nodiscard]] double getExecutionTimeMs() const noexcept { return executionTimeMs_; }

    // Overriding virtual method to demonstrate polymorphic dynamic dispatch on exception objects
    [[nodiscard]] string getFullDiagnostic() const override {
        return "QueryExecutionException [ErrorCode: " + to_string(getErrorCode()) + 
               ", SQLState: " + getSqlState() + ", Query: '" + failedQuery_ + 
               "', Time: " + to_string(executionTimeMs_) + "ms] -> " + what();
    }
};

// =====================================================================================
// 3. HELPER FUNCTIONS DEMONSTRATING EXCEPTION OBJECT BEHAVIORS
// =====================================================================================

// Function causing Object Slicing (Pass Catch-By-Value)
void demonstrateSlicingByValue(const DatabaseException& ex) {
    cout << "      -> Caught By Value (`catch (DatabaseException ex)`):\n";
    cout << "         Diagnostic: " << ex.getFullDiagnostic() << "\n";
}

// Function preventing Object Slicing (Pass Catch-By-Reference)
void demonstratePolymorphicReference(const DatabaseException& ex) {
    cout << "      -> Caught By Const Reference (`catch (const DatabaseException& ex)`):\n";
    cout << "         Diagnostic: " << ex.getFullDiagnostic() << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Exception Object analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. LIFETIME & STORAGE MEMORY MECHANICS OF EXCEPTION OBJECTS
    // =====================================================================================
    cout << "\n================ 1. EXCEPTION OBJECT LIFETIME & MEMORY STORAGE ================\n";

    try {
        cout << "  - Creating local `LifetimeTracedException` object on stack frame...\n";
        LifetimeTracedException localStackObj("Database Connection Timeout", userInputValue);
        
        cout << "  - Executing `throw localStackObj;` (Triggers copy into runtime exception buffer)...\n";
        throw localStackObj; // Stack object copied into ABI runtime storage pool; stack unwinds!

    } catch (const LifetimeTracedException& caughtObj) {
        cout << "  - [CATCH BLOCK ACTIVE] Exception object address in catch handler: " 
             << static_cast<const void*>(&caughtObj) << "\n";
        cout << "    * Exception Message : \"" << caughtObj.what() << "\"\n";
        cout << "    * Exception Trace ID: " << caughtObj.getTraceId() << "\n";
        cout << "  - Exiting catch block scope...\n";
    } // Exception object in runtime buffer is destroyed HERE at handler exit!

    // =====================================================================================
    // 2. POLYMORPHISM VS OBJECT SLICING ON EXCEPTION OBJECTS
    // =====================================================================================
    cout << "\n================ 2. CATCHING EXCEPTION OBJECTS (SLICING VS POLYMORPHISM) ================\n";

    // Scenario A: Demonstrating Object Slicing (Catch by Value)
    cout << "  - Scenario A: Throwing `QueryExecutionException` and catching BY VALUE:\n";

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wcatch-value"
#elif defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcatch-value"
#endif

    try {
        throw QueryExecutionException("Table 'users' does not exist", 1146, "42S02", 
                                      "SELECT * FROM users WHERE id = " + to_string(userInputValue), 14.2);
    } catch (DatabaseException slicedValEx) { // Catch by value causes SLICING!
        demonstrateSlicingByValue(slicedValEx);
        cout << "    [SLICING RESULT]: Derived `QueryExecutionException` attributes were STRIPPED!\n";
        cout << "    [SLICING RESULT]: Dynamic `getFullDiagnostic()` virtual dispatch was LOST!\n";
    }

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif

    // Scenario B: Demonstrating Polymorphic Preservation (Catch by Const Reference)
    cout << "\n  - Scenario B: Throwing `QueryExecutionException` and catching BY CONST REFERENCE:\n";
    try {
        throw QueryExecutionException("Syntax error near 'WHERE'", 1064, "42000", 
                                      "SELECT FROM logs WHERE", 2.1);
    } catch (const DatabaseException& refEx) { // Catch by const reference preserves polymorphism!
        demonstratePolymorphicReference(refEx);
        cout << "    [POLYMORPHIC RESULT]: Derived attributes and virtual dispatch fully PRESERVED!\n";
    }

    // =====================================================================================
    // 3. STATEFUL EXCEPTION OBJECT INSPECTION
    // =====================================================================================
    cout << "\n================ 3. STATEFUL EXCEPTION OBJECT INSPECTION ================\n";

    try {
        cout << "  - Throwing detailed `QueryExecutionException` object...\n";
        throw QueryExecutionException("Deadlock detected during transaction lock acquisition", 1213, "40001",
                                      "UPDATE accounts SET balance = balance - 100 WHERE id = " + to_string(userInputValue), 128.5);
    } catch (const QueryExecutionException& queryEx) {
        cout << "  - [CAUGHT DERIVED EXCEPTION OBJECT]:\n";
        cout << "    * Standard Message (`what()`)    : " << queryEx.what() << "\n";
        cout << "    * Error Code (`getErrorCode()`)   : " << queryEx.getErrorCode() << "\n";
        cout << "    * SQL State (`getSqlState()`)     : " << queryEx.getSqlState() << "\n";
        cout << "    * Failed Query (`getFailedQuery()`): " << queryEx.getFailedQuery() << "\n";
        cout << "    * Execution Time                  : " << queryEx.getExecutionTimeMs() << " ms\n";
    }

    // =====================================================================================
    // 4. TRANSPORTING EXCEPTION OBJECTS (`std::exception_ptr`)
    // =====================================================================================
    cout << "\n================ 4. TRANSPORTING EXCEPTION OBJECTS (`std::exception_ptr`) ================\n";

    std::exception_ptr storedException = nullptr;

    // Capture exception object into smart transport container
    try {
        cout << "  - Throwing exception object inside source scope...\n";
        throw QueryExecutionException("Connection reset by peer", 2006, "HY000", 
                                      "PING", 5000.0);
    } catch (...) {
        cout << "  - Capturing current active exception object via `std::current_exception()`...\n";
        storedException = std::current_exception(); // Extends exception object lifetime in ABI buffer!
    }

    // Inspect and rethrow exception object in target scope
    if (storedException) {
        cout << "  - Exception object successfully preserved in `std::exception_ptr`.\n";
        try {
            cout << "  - Rethrowing preserved exception object via `std::rethrow_exception()`...\n";
            std::rethrow_exception(storedException);
        } catch (const DatabaseException& rethrownEx) {
            cout << "  - [RETHROWN CATCH SUCCESS]: Restored stateful exception object intact:\n";
            cout << "    * Diagnostic: " << rethrownEx.getFullDiagnostic() << "\n";
        }
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION OBJECTS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Topic / Rule          | C++ Implementation Mechanism      | Architectural Benefit / Risk      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Storage Location      | ABI Buffer (`__cxa_allocate_ex`)  | Persists beyond stack unwinding   |\n"
         << "| Copy / Move Rule      | Must be Copyable/Movable & `noex` | Allows runtime buffer allocation  |\n"
         << "| Catch By Value        | `catch (BaseException ex)`        | Slices derived object state (BAD) |\n"
         << "| Catch By Const Ref    | `catch (const BaseException& ex)` | Preserves polymorphism & state    |\n"
         << "| Stateful Exceptions   | Add private fields to custom ex   | Carries rich domain debugging info|\n"
         << "| Exception Lifetime    | Active from `throw` to catch exit | Automatically freed by runtime    |\n"
         << "| Exception Transport   | `std::exception_ptr`              | Safely defers/transports objects  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}