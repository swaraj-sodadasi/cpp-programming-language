/*
 * =====================================================================================
 * CONCEPT        : Exception Information in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how to attach, capture, extract, and format diagnostic Exception 
 *                  Information in Modern C++:
 *
 *                  1. Standard Exception Messages (`std::exception::what()`) :
 *                     - Standardized string payload detailing the exception reason.
 *
 *                  2. Runtime Type Information (RTTI - `typeid(e).name()`) :
 *                     - Querying the exact dynamic runtime type of an exception object.
 *
 *                  3. Call-Site Source Location Context (`__FILE__`, `__LINE__`, `__func__`) :
 *                     - Capturing file name, line number, and function name at the exact 
 *                       `throw` site via a macro wrapper (`THROW_DIAGNOSTIC_EX`).
 *
 *                  4. Domain-Specific Error Context :
 *                     - Attaching custom error codes, module names, timestamp identifiers,
 *                       and state snapshots to exception objects.
 *
 *                  5. System Error & Category Metadata (`std::system_error`) :
 *                     - Inspecting POSIX/OS error codes (`value()`), error categories (`category()`),
 *                       and system error messages (`message()`).
 *
 *                  6. Nested Exception Chains (`std::nested_exception`) :
 *                     - Extracting root cause exception information wrapped inside higher-level
 *                       domain exceptions using `std::rethrow_if_nested`.
 *
 * RESOLVED ISSUE : Replaced default argument `__func__` in the constructor declaration with 
 *                  explicit call-site instantiation via a macro `THROW_DIAGNOSTIC_EX`. 
 *                  This resolves `Predefined identifier is only valid inside function` 
 *                  because `__func__` is evaluated inside the function body where the throw occurs.
 *
 * TIME COMPLEXITY  : Exception Information Extraction : O(1) constant time (string lookup/formatting).
 *                    Nested Chain Inspection         : O(Depth of Nested Chain).
 * SPACE COMPLEXITY : Context Payload Overhead         : Small runtime memory block for context fields.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>
#include <typeinfo>
#include <system_error>

using namespace std;

// =====================================================================================
// 1. RICH CUSTOM EXCEPTION CLASS WITH SOURCE LOCATION & DIAGNOSTIC CONTEXT
// Encapsulates detailed diagnostic information captured at the call site.
// =====================================================================================
class DiagnosticContextException : public std::runtime_error {
private:
    int errorCode_;
    string subsystemModule_;
    string fileName_;
    int line_;
    string functionName_;

public:
    DiagnosticContextException(
        const string& message, 
        int errorCode, 
        string subsystemModule,
        const char* file,
        int line,
        const char* func)
        : std::runtime_error(message),
          errorCode_(errorCode),
          subsystemModule_(std::move(subsystemModule)),
          fileName_(file ? file : "Unknown"),
          line_(line),
          functionName_(func ? func : "Unknown") {}

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }
    [[nodiscard]] const string& getSubsystemModule() const noexcept { return subsystemModule_; }
    [[nodiscard]] const string& getFileName() const noexcept { return fileName_; }
    [[nodiscard]] int getLine() const noexcept { return line_; }
    [[nodiscard]] const string& getFunctionName() const noexcept { return functionName_; }

    // Generates a fully formatted diagnostic information summary
    [[nodiscard]] string getFormattedDiagnosticReport() const {
        string report;
        report += "    +-----------------------------------------------------------------+\n";
        report += "    | EXCEPTION DIAGNOSTIC INFORMATION REPORT                         |\n";
        report += "    +-----------------------------------------------------------------+\n";
        report += "    | Payload Message : " + string(what()) + "\n";
        report += "    | Error Code      : " + to_string(errorCode_) + "\n";
        report += "    | Subsystem       : " + subsystemModule_ + "\n";
        report += "    | Source File     : " + fileName_ + "\n";
        report += "    | Line Number     : " + to_string(line_) + "\n";
        report += "    | Function Name   : " + functionName_ + "\n";
        report += "    +-----------------------------------------------------------------+";
        return report;
    }
};

// Macro helper to construct and throw DiagnosticContextException with call-site metadata
#define THROW_DIAGNOSTIC_EX(msg, code, module) \
    throw DiagnosticContextException((msg), (code), (module), __FILE__, __LINE__, __func__)

// =====================================================================================
// 2. HELPER FUNCTIONS TO GENERATE VARIOUS EXCEPTION SCENARIOS
// =====================================================================================

// Function throwing custom rich context exception using the call-site macro
void executeDatabaseQuery(int queryId, const string& queryStr) {
    if (queryId <= 0) {
        THROW_DIAGNOSTIC_EX(
            "SQL Query Execution Aborted: Invalid Query Identifier (" + to_string(queryId) + ")",
            1044,
            "DatabaseEngine::QueryProcessor"
        );
    }
    cout << "    [Database Engine] Query #" << queryId << " ('" << queryStr << "') executed successfully.\n";
}

// Function throwing std::system_error
void openSystemSocket(int port) {
    if (port < 1024) {
        throw std::system_error(
            make_error_code(errc::permission_denied),
            "Failed to bind socket on restricted port " + to_string(port)
        );
    }
    cout << "    [Network Subsystem] Socket opened successfully on port " << port << ".\n";
}

// Low-level function for nested exception chaining
void lowLevelDiskAccess() {
    throw std::runtime_error("I/O Error: Disk sector 0x7F4A unreadable");
}

// High-level function wrapping low-level exception
void highLevelDataService() {
    try {
        lowLevelDiskAccess();
    } catch (...) {
        std::throw_with_nested(
            std::runtime_error("Data Service Failure: Unable to load user profile cache")
        );
    }
}

// Unpacks and displays all nested exception information recursively
void printNestedExceptionInfo(const std::exception& ex, int depth = 1) {
    string indent(depth * 2, ' ');
    cout << indent << "-> [Level " << depth << " Info]: " << ex.what() 
         << " (RTTI Type: " << typeid(ex).name() << ")\n";

    try {
        std::rethrow_if_nested(ex);
    } catch (const std::exception& nestedEx) {
        printNestedExceptionInfo(nestedEx, depth + 1);
    } catch (...) {
        cout << indent << "  -> [Level " << (depth + 1) << " Info]: Non-std nested exception\n";
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Exception Information analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC EXCEPTION INFORMATION (`what()` & RTTI `typeid`)
    // =====================================================================================
    cout << "\n================ 1. BASIC EXCEPTION INFORMATION (`what()` & RTTI) ================\n";

    try {
        cout << "  - Triggering `std::out_of_range` exception...\n";
        string sampleStr = "Modern C++";
        [[maybe_unused]] char ch = sampleStr.at(99); // Triggers out_of_range
    } catch (const std::exception& ex) {
        cout << "  - [CAPTURED EXCEPTION INFO]:\n";
        cout << "    * Message Payload (`what()`) : " << ex.what() << "\n";
        cout << "    * RTTI Type Name (`typeid`)  : " << typeid(ex).name() << "\n";
    }

    // =====================================================================================
    // 2. RICH CONTEXTUAL EXCEPTION INFO & CALL-SITE LOCATION
    // =====================================================================================
    cout << "\n================ 2. RICH CONTEXT & CALL-SITE LOCATION ================\n";

    try {
        cout << "  - Executing `executeDatabaseQuery(-5)`...\n";
        executeDatabaseQuery(-5, "SELECT * FROM users WHERE id = " + to_string(userInputValue));
    } catch (const DiagnosticContextException& ex) {
        cout << "  - [CAPTURED RICH CONTEXT EXCEPTION INFO]:\n";
        cout << ex.getFormattedDiagnosticReport() << "\n";
    }

    // =====================================================================================
    // 3. SYSTEM ERROR METADATA (`std::system_error` & `std::error_code`)
    // =====================================================================================
    cout << "\n================ 3. SYSTEM ERROR & CATEGORY METADATA ================\n";

    try {
        cout << "  - Calling `openSystemSocket(80)` on restricted port...\n";
        openSystemSocket(80);
    } catch (const std::system_error& sysEx) {
        cout << "  - [CAPTURED SYSTEM ERROR INFO]:\n";
        cout << "    * What String (`what()`)            : " << sysEx.what() << "\n";
        cout << "    * Integer Code Value (`code().val`) : " << sysEx.code().value() << "\n";
        cout << "    * Category Name (`category().name`) : " << sysEx.code().category().name() << "\n";
        cout << "    * Category Message (`code().msg`)   : " << sysEx.code().message() << "\n";
    }

    // =====================================================================================
    // 4. NESTED EXCEPTION INFORMATION CHAINS
    // =====================================================================================
    cout << "\n================ 4. UNPACKING NESTED EXCEPTION INFORMATION CHAINS ================\n";

    try {
        cout << "  - Calling `highLevelDataService()` which wraps low-level errors...\n";
        highLevelDataService();
    } catch (const std::exception& topEx) {
        cout << "  - [CAPTURED NESTED EXCEPTION INFORMATION CHAIN]:\n";
        printNestedExceptionInfo(topEx);
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION INFORMATION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Exception Info Source | Extraction Mechanism              | Diagnostic Information Provided   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Standard Message      | `ex.what()`                       | Explanatory textual string        |\n"
         << "| Type Information      | `typeid(ex).name()`               | Dynamic RTTI class name           |\n"
         << "| Call-Site Location    | `__FILE__`, `__LINE__`, `__func__`| File, Line, and Function name     |\n"
         << "| System Error Details  | `sysEx.code().value()` / `msg()`  | OS error code & error category    |\n"
         << "| Domain Context        | Custom getters in Exception Class | Error codes, module name, state   |\n"
         << "| Nested Cause Chain    | `std::rethrow_if_nested(ex)`      | Root cause exception hierarchy    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}