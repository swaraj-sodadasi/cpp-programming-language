/*
 * =====================================================================================
 * CONCEPT        : Best Practices, Common Mistakes, and Debugging Exceptions in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architectural best practices, dangerous common pitfalls, and 
 *                  advanced debugging techniques for Modern C++ Exception Handling:
 *
 *                  1. BEST PRACTICES :
 *                     - Throw by Value, Catch by Const Reference (`const std::exception&`).
 *                     - Inherit custom exceptions from `std::exception` / `std::runtime_error`.
 *                     - Strict RAII for zero-leak stack unwinding (Smart Pointers, Lock Guards).
 *                     - Mark destructors, move constructors, and swap operations `noexcept`.
 *                     - Preserve dynamic type identity when re-throwing using bare `throw;`.
 *
 *                  2. COMMON MISTAKES (ANTI-PATTERNS) :
 *                     - Object Slicing: Catching or re-throwing by value (`throw ex;`).
 *                     - Swallowing Exceptions: Empty `catch (...)` blocks that hide failures.
 *                     - Throwing Primitive Types / Raw Pointers (`throw "error"`, `throw 404`).
 *                     - Resource Leaks: Manual raw memory allocation before a throw.
 *                     - Throwing from Destructors: Invokes immediate `std::terminate()`.
 *
 *                  3. DEBUGGING EXCEPTIONS :
 *                     - Call-Site Tracking: Automating file, line, and function context capture
 *                       using macros (`__FILE__`, `__LINE__`, `__func__`).
 *                     - RTTI Type Inspection: Using `typeid(ex).name()` and `dynamic_cast`.
 *                     - Custom Termination Handlers: Installing `std::set_terminate()` to capture 
 *                       diagnostic state before process exit on unhandled exceptions.
 *
 * RESOLVED ISSUE : Suppressed compiler diagnostic warning `-Wcatch-value` using localized 
 *                  `#pragma` diagnostic directives around the catch-by-value demonstration 
 *                  block across GCC and Clang compilers.
 *
 * TIME COMPLEXITY  : Normal Control Flow (No Throw) : O(1) Zero-cost table lookup overhead.
 *                    Exception Unwind & Debug Path  : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Payload Footprint     : Small runtime memory allocation.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <memory>
#include <limits>
#include <typeinfo>
#include <sstream>
#include <cstdlib>

using namespace std;

// =====================================================================================
// 1. DEBUGGING & BEST PRACTICE CUSTOM EXCEPTION CLASS
// Inherits from std::runtime_error and captures call-site diagnostic location context.
// =====================================================================================
class ProductionDebugException : public std::runtime_error {
private:
    int errorCode_;
    string fileName_;
    int lineNumber_;
    string functionName_;

public:
    ProductionDebugException(
        const string& message,
        int errorCode,
        const char* file,
        int line,
        const char* func)
        : std::runtime_error(message),
          errorCode_(errorCode),
          fileName_(file ? file : "UnknownFile"),
          lineNumber_(line),
          functionName_(func ? func : "UnknownFunc") {}

    ~ProductionDebugException() noexcept override = default;

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }
    [[nodiscard]] const string& getFileName() const noexcept { return fileName_; }
    [[nodiscard]] int getLineNumber() const noexcept { return lineNumber_; }
    [[nodiscard]] const string& getFunctionName() const noexcept { return functionName_; }

    // Dynamic virtual report for debugging inspection
    [[nodiscard]] virtual string getDiagnosticReport() const {
        ostringstream oss;
        oss << "ProductionDebugException [Code " << errorCode_ << "]\n"
            << "  Message  : " << what() << "\n"
            << "  Location : " << fileName_ << ":" << lineNumber_ 
            << " in " << functionName_ << "()";
        return oss.str();
    }
};

// Specialized derived exception to demonstrate Object Slicing anti-patterns
class DatabaseConnectionException : public ProductionDebugException {
private:
    string databaseHost_;

public:
    DatabaseConnectionException(
        const string& message,
        int errorCode,
        const char* file,
        int line,
        const char* func,
        string host)
        : ProductionDebugException(message, errorCode, file, line, func),
          databaseHost_(std::move(host)) {}

    ~DatabaseConnectionException() noexcept override = default;

    [[nodiscard]] const string& getDatabaseHost() const noexcept { return databaseHost_; }

    [[nodiscard]] string getDiagnosticReport() const override {
        ostringstream oss;
        oss << "DatabaseConnectionException [Code " << getErrorCode() << " | Host: " << databaseHost_ << "]\n"
            << "  Message  : " << what() << "\n"
            << "  Location : " << getFileName() << ":" << getLineNumber() 
            << " in " << getFunctionName() << "()";
        return oss.str();
    }
};

// Macro Helper for call-site context tracking (evaluated inside function body)
#define THROW_DEBUG_EX(msg, code) \
    throw ProductionDebugException((msg), (code), __FILE__, __LINE__, __func__)

#define THROW_DB_EX(msg, code, host) \
    throw DatabaseConnectionException((msg), (code), __FILE__, __LINE__, __func__, (host))

// =====================================================================================
// 2. DEMONSTRATING BEST PRACTICES (RAII, CATCH BY CONST REF, BARE RETHROW)
// =====================================================================================
class BestPracticesDemo {
public:
    // BEST PRACTICE 1: RAII Resource Management
    static void raiiResourceSafety(bool triggerFailure) {
        cout << "    [RAII Best Practice] Acquiring heap buffer via `std::unique_ptr`...\n";
        auto safeBuffer = std::make_unique<int[]>(100);

        if (triggerFailure) {
            cout << "    [RAII Best Practice THROW] Throwing exception inside RAII scope...\n";
            THROW_DEBUG_EX("Operation failed midway inside RAII-managed function", 1001);
        }

        cout << "    [RAII Best Practice] Work finished successfully.\n";
    } // safeBuffer is automatically deallocated HERE during stack unwinding!

    // BEST PRACTICE 2: Polymorphic Bare Rethrow (`throw;`)
    static void rethrowPolymorphic() {
        try {
            THROW_DB_EX("Database cluster unreachable", 5003, "db-prod-cluster.internal");
        } catch (const ProductionDebugException& caughtRef) {
            cout << "    [Intermediate Layer] Intercepted exception as `const ProductionDebugException&`.\n";
            cout << "    [Intermediate Layer] Executing bare `throw;` to preserve dynamic type...\n";
            throw; // CORRECT: Preserves dynamic DatabaseConnectionException object
        }
    }
};

// =====================================================================================
// 3. DEMONSTRATING COMMON MISTAKES (ANTI-PATTERNS)
// =====================================================================================
class CommonMistakesDemo {
public:
    // COMMON MISTAKE 1: Primitive Type Throwing
    static void throwPrimitiveType() {
        cout << "    [Anti-Pattern] Throwing raw C-string pointer `const char*`...\n";
        throw "CRITICAL ERROR: Primitive string thrown!"; // BAD PRACTICE
    }

    // COMMON MISTAKE 2: Object Slicing via Catch-by-Value
    static void catchByValueSlicing() {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wcatch-value"
#elif defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcatch-value"
#endif
        try {
            THROW_DB_EX("Primary database node connection lost", 5001, "db-node-01.internal");
        } catch (ProductionDebugException slicedCopy) { // BAD PRACTICE: Catch by value slices object!
            cout << "    [CATCH BY VALUE] Caught exception object by value!\n";
            cout << "    * Static RTTI Type Name : " << typeid(slicedCopy).name() << "\n";
            cout << "    * Diagnostic Output     : " << slicedCopy.getDiagnosticReport() << "\n";
            cout << "    [SLICING VERIFICATION]: Derived `DatabaseConnectionException` fields were stripped!\n";
        }
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif
    }

    // COMMON MISTAKE 3: Object Slicing via `throw ex;`
    static void slicedRethrow() {
        try {
            THROW_DB_EX("Replica database timeout", 5002, "db-replica-02.internal");
        } catch (const ProductionDebugException& caughtRef) {
            cout << "    [Intermediate Layer] Intercepted exception.\n";
            cout << "    [Anti-Pattern] Executing `throw caughtRef;` (Slices derived type!)...\n";
            throw caughtRef; // BAD PRACTICE: Slices exception down to static type!
        }
    }

    // COMMON MISTAKE 4: Silent Exception Swallowing
    static void swallowExceptionSilently() {
        try {
            THROW_DEBUG_EX("Silent failure demonstration", 9001);
        } catch (...) {
            // BAD PRACTICE: Empty catch block silently swallows errors without logging or handling!
            cout << "    [Anti-Pattern CATCH (...)] Exception caught and silently swallowed! (No logs/handling)\n";
        }
    }
};

// =====================================================================================
// 4. DEBUGGING EXCEPTIONS & CUSTOM TERMINATION HANDLER
// =====================================================================================
void customTerminateHandler() {
    cerr << "\n================ [CUSTOM TERMINATE HANDLER] ================\n";
    cerr << "  Process aborting due to unhandled exception or exception during unwinding!\n";
    cerr << "============================================================\n";
    std::abort();
}

void debugExceptionDetails(const std::exception& ex) {
    cout << "  - [DEBUG INSPECTOR]:\n";
    cout << "    * Standard Message (`what()`) : " << ex.what() << "\n";
    cout << "    * Dynamic RTTI Class Name     : " << typeid(ex).name() << "\n";

    // Downcasting safely to extract specific diagnostic context
    if (const auto* debugEx = dynamic_cast<const ProductionDebugException*>(&ex)) {
        cout << "    * Source File Location        : " << debugEx->getFileName() << ":" << debugEx->getLineNumber() << "\n";
        cout << "    * Enclosing Function Name     : " << debugEx->getFunctionName() << "()\n";
        cout << "    * Internal Error Code         : " << debugEx->getErrorCode() << "\n";
    }

    if (const auto* dbEx = dynamic_cast<const DatabaseConnectionException*>(&ex)) {
        cout << "    * Specialized Database Host   : " << dbEx->getDatabaseHost() << "\n";
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    // Install custom terminate handler for process debugging
    std::set_terminate(customTerminateHandler);

    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Best Practices & Debugging analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BEST PRACTICES DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. BEST PRACTICES DEMONSTRATION ================\n";

    // RAII Resource Safety
    try {
        cout << "  - Testing RAII Resource Safety with exception throw...\n";
        BestPracticesDemo::raiiResourceSafety(true);
    } catch (const std::exception& ex) {
        cout << "  - [MAIN CATCH]: " << ex.what() << "\n";
        cout << "    [VERIFICATION]: RAII unique_ptr automatically freed heap memory on stack unwind!\n";
    }

    // Polymorphic Bare Rethrow (`throw;`)
    try {
        cout << "\n  - Testing Polymorphic Bare Rethrow (`throw;`)...\n";
        BestPracticesDemo::rethrowPolymorphic();
    } catch (const ProductionDebugException& mainRef) {
        cout << "  - [MAIN CATCH]: Re-caught exception in main.\n";
        debugExceptionDetails(mainRef);
        cout << "    [VERIFICATION]: Dynamic type `DatabaseConnectionException` was PRESERVED!\n";
    }

    // =====================================================================================
    // 2. COMMON MISTAKES & ANTI-PATTERNS
    // =====================================================================================
    cout << "\n================ 2. COMMON MISTAKES & ANTI-PATTERNS ================\n";

    // Mistake 1: Object Slicing on Catch-by-Value
    cout << "  - Scenario A: Object Slicing via Catch-by-Value:\n";
    CommonMistakesDemo::catchByValueSlicing();

    // Mistake 2: Object Slicing on `throw ex;`
    cout << "\n  - Scenario B: Object Slicing via `throw ex;` rethrow:\n";
    try {
        CommonMistakesDemo::slicedRethrow();
    } catch (const ProductionDebugException& slicedRef) {
        cout << "  - [MAIN CATCH]: Re-caught exception in main.\n";
        debugExceptionDetails(slicedRef);
        cout << "    [SLICING VERIFICATION]: Dynamic type was SLICED down to base ProductionDebugException!\n";
    }

    // Mistake 3: Throwing Primitive Types
    cout << "\n  - Scenario C: Throwing Primitive Types (`const char*`):\n";
    try {
        CommonMistakesDemo::throwPrimitiveType();
    } catch (const char* rawMsg) {
        cout << "  - [CAUGHT PRIMITIVE TYPE]: \"" << rawMsg << "\"\n";
        cout << "    [WARNING]: Primitive types lack std::exception interface, RTTI, and line context!\n";
    }

    // Mistake 4: Silent Exception Swallowing
    cout << "\n  - Scenario D: Silent Exception Swallowing (`catch (...)`):\n";
    CommonMistakesDemo::swallowExceptionSilently();

    // =====================================================================================
    // 3. DEBUGGING & CALL-SITE CONTEXT INSPECTION
    // =====================================================================================
    cout << "\n================ 3. DEBUGGING & CALL-SITE CONTEXT INSPECTION ================\n";

    try {
        cout << "  - Triggering macro-assisted call-site context exception...\n";
        THROW_DEBUG_EX("Database query timeout during user authentication", 4008);
    } catch (const std::exception& ex) {
        debugExceptionDetails(ex);
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ BEST PRACTICES, MISTAKES & DEBUGGING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Category              | C++ Pattern / Anti-Pattern        | Architectural Impact / Solution   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Best Practice         | Catch by `const BaseEx&`          | Preserves dynamic vtable & fields |\n"
         << "| Best Practice         | Bare `throw;` for re-throwing     | Preserves dynamic runtime type    |\n"
         << "| Best Practice         | Standard RAII Wrappers            | Zero-leak stack unwinding         |\n"
         << "| Best Practice         | `noexcept` Dtors, Swaps, Moves    | Prevents std::terminate crashes   |\n"
         << "| Common Mistake        | Catch by Value (`catch (Base ex)`) | SLICES derived exception objects  |\n"
         << "| Common Mistake        | `throw caughtEx;` on re-throw     | SLICES derived object to base copy|\n"
         << "| Common Mistake        | Throw primitive (`throw \"err\"`)   | Lacks standard what() / type safety|\n"
         << "| Common Mistake        | Empty `catch (...)` block         | Hides bugs & swallows errors      |\n"
         << "| Debugging Technique   | `__FILE__`, `__LINE__`, `__func__`| Pinpoints call-site throw origin  |\n"
         << "| Debugging Technique   | `typeid(ex).name()` & `dynamic_cast`| Dynamic RTTI inspection & downcast|\n"
         << "| Debugging Technique   | `std::set_terminate()`            | Custom process exit diagnostics   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}