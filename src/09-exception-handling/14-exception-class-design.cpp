/*
 * =====================================================================================
 * CONCEPT        : Exception Class Design in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architectural principles, best practices, and design patterns for
 *                  creating robust, production-grade Exception Classes in Modern C++:
 *
 *                  1. Inheritance Hierarchy Design :
 *                     - Inheriting from `std::runtime_error` (or `std::logic_error`) to
 *                       leverage standard `what()` string ownership and exception contracts.
 *
 *                  2. Immutable & Exception-Safe State Payload :
 *                     - Storing structured domain data (ErrorCode, Severity, Subsystem, 
 *                       Timestamps, Flags) with read-only `const` accessors marked `noexcept`.
 *
 *                  3. Call-Site Context Tracking :
 *                     - Automating call-site origin metadata capture (`__FILE__`, `__LINE__`, `__func__`)
 *                       using macro factories to pinpoint throw locations without boilerplate.
 *
 *                  4. Nothrow Copy/Move Guarantees :
 *                     - Ensuring exception objects can be safely copied or moved by the C++ ABI
 *                       during stack unwinding without throwing secondary exceptions.
 *
 *                  5. Polymorphic Diagnostics & RTTI Downcasting :
 *                     - Providing extensible virtual diagnostic reports (`getDiagnosticReport()`)
 *                       while supporting safe `dynamic_cast` for derived context inspection.
 *
 * TIME COMPLEXITY  : Exception Construction / Field Lookup : O(1) constant time.
 *                    Polymorphic Dispatch / Stack Unwind   : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Payload Footprint           : Small fixed runtime memory block.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>
#include <sstream>
#include <typeinfo>

using namespace std;

// =====================================================================================
// 1. DOMAIN VALUE TYPES & ENUMERATIONS FOR EXCEPTION STATE
// =====================================================================================
enum class ErrorSeverity {
    Info,
    Warning,
    Error,
    Critical
};

enum class ErrorCode {
    GenericFailure      = 1000,
    InvalidInput        = 1001,
    ResourceNotFound    = 1002,
    DatabaseConnection  = 2001,
    QueryExecution      = 2002,
    NetworkTimeout      = 3001,
    AccessDenied        = 4001
};

[[nodiscard]] constexpr const char* severityToString(ErrorSeverity severity) noexcept {
    switch (severity) {
        case ErrorSeverity::Info:     return "INFO";
        case ErrorSeverity::Warning:  return "WARNING";
        case ErrorSeverity::Error:    return "ERROR";
        case ErrorSeverity::Critical: return "CRITICAL";
    }
    return "UNKNOWN";
}

[[nodiscard]] string errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::GenericFailure:     return "GENERIC_FAILURE (1000)";
        case ErrorCode::InvalidInput:       return "INVALID_INPUT (1001)";
        case ErrorCode::ResourceNotFound:   return "RESOURCE_NOT_FOUND (1002)";
        case ErrorCode::DatabaseConnection: return "DATABASE_CONNECTION_FAIL (2001)";
        case ErrorCode::QueryExecution:     return "QUERY_EXECUTION_FAIL (2002)";
        case ErrorCode::NetworkTimeout:     return "NETWORK_TIMEOUT (3001)";
        case ErrorCode::AccessDenied:       return "ACCESS_DENIED (4001)";
    }
    return "CUSTOM_ERROR (" + to_string(static_cast<int>(code)) + ")";
}

// =====================================================================================
// 2. ROOT APPLICATION EXCEPTION BASE CLASS (`ApplicationException`)
// Derived from `std::runtime_error` to enforce standard interface compatibility.
// =====================================================================================
class ApplicationException : public std::runtime_error {
private:
    ErrorCode errorCode_;
    ErrorSeverity severity_;
    string subsystemModule_;
    string fileName_;
    int line_;
    string functionName_;

public:
    ApplicationException(
        const string& message,
        ErrorCode code,
        ErrorSeverity severity,
        string subsystemModule,
        const char* file,
        int line,
        const char* func)
        : std::runtime_error(message),
          errorCode_(code),
          severity_(severity),
          subsystemModule_(std::move(subsystemModule)),
          fileName_(file ? file : "UnknownFile"),
          line_(line),
          functionName_(func ? func : "UnknownFunc") {}

    ~ApplicationException() noexcept override = default;

    // Read-only getters marked `noexcept` for exception-safety guarantees
    [[nodiscard]] ErrorCode getErrorCode() const noexcept { return errorCode_; }
    [[nodiscard]] ErrorSeverity getSeverity() const noexcept { return severity_; }
    [[nodiscard]] const string& getSubsystemModule() const noexcept { return subsystemModule_; }
    [[nodiscard]] const string& getFileName() const noexcept { return fileName_; }
    [[nodiscard]] int getLine() const noexcept { return line_; }
    [[nodiscard]] const string& getFunctionName() const noexcept { return functionName_; }

    // Polymorphic Virtual Diagnostic Method
    [[nodiscard]] virtual string getDiagnosticReport() const {
        ostringstream oss;
        oss << "[" << severityToString(severity_) << "] ApplicationException\n"
            << "  Code      : " << errorCodeToString(errorCode_) << "\n"
            << "  Subsystem : " << subsystemModule_ << "\n"
            << "  Message   : " << what() << "\n"
            << "  Location  : " << fileName_ << ":" << line_ << " in " << functionName_ << "()";
        return oss.str();
    }
};

// Macro factory helper to automate capturing call-site metadata
#define THROW_APP_EX(msg, code, severity, module) \
    throw ApplicationException((msg), (code), (severity), (module), __FILE__, __LINE__, __func__)

// =====================================================================================
// 3. DERIVED EXCEPTION CLASS A: `DatabaseQueryException`
// Specialization adding database query metadata and retry flags.
// =====================================================================================
class DatabaseQueryException : public ApplicationException {
private:
    string sqlState_;
    string failedQuery_;
    bool isRetryable_;

public:
    DatabaseQueryException(
        const string& message,
        ErrorCode code,
        ErrorSeverity severity,
        string subsystemModule,
        const char* file,
        int line,
        const char* func,
        string sqlState,
        string failedQuery,
        bool isRetryable)
        : ApplicationException(message, code, severity, std::move(subsystemModule), file, line, func),
          sqlState_(std::move(sqlState)),
          failedQuery_(std::move(failedQuery)),
          isRetryable_(isRetryable) {}

    ~DatabaseQueryException() noexcept override = default;

    [[nodiscard]] const string& getSqlState() const noexcept { return sqlState_; }
    [[nodiscard]] const string& getFailedQuery() const noexcept { return failedQuery_; }
    [[nodiscard]] bool isRetryable() const noexcept { return isRetryable_; }

    [[nodiscard]] string getDiagnosticReport() const override {
        ostringstream oss;
        oss << "[" << severityToString(getSeverity()) << "] DatabaseQueryException\n"
            << "  Code      : " << errorCodeToString(getErrorCode()) << "\n"
            << "  Subsystem : " << getSubsystemModule() << "\n"
            << "  Message   : " << what() << "\n"
            << "  SQL State : " << sqlState_ << "\n"
            << "  Query     : '" << failedQuery_ << "'\n"
            << "  Retryable : " << (isRetryable_ ? "YES" : "NO") << "\n"
            << "  Location  : " << getFileName() << ":" << getLine() << " in " << getFunctionName() << "()";
        return oss.str();
    }
};

#define THROW_DB_EX(msg, code, severity, module, sqlState, query, retryable) \
    throw DatabaseQueryException((msg), (code), (severity), (module), __FILE__, __LINE__, __func__, (sqlState), (query), (retryable))

// =====================================================================================
// 4. DERIVED EXCEPTION CLASS B: `NetworkTimeoutException`
// Specialization adding endpoint URLs and timeout threshold durations.
// =====================================================================================
class NetworkTimeoutException : public ApplicationException {
private:
    string endpointUrl_;
    int timeoutMs_;

public:
    NetworkTimeoutException(
        const string& message,
        ErrorCode code,
        ErrorSeverity severity,
        string subsystemModule,
        const char* file,
        int line,
        const char* func,
        string endpointUrl,
        int timeoutMs)
        : ApplicationException(message, code, severity, std::move(subsystemModule), file, line, func),
          endpointUrl_(std::move(endpointUrl)),
          timeoutMs_(timeoutMs) {}

    ~NetworkTimeoutException() noexcept override = default;

    [[nodiscard]] const string& getEndpointUrl() const noexcept { return endpointUrl_; }
    [[nodiscard]] int getTimeoutMs() const noexcept { return timeoutMs_; }

    [[nodiscard]] string getDiagnosticReport() const override {
        ostringstream oss;
        oss << "[" << severityToString(getSeverity()) << "] NetworkTimeoutException\n"
            << "  Code      : " << errorCodeToString(getErrorCode()) << "\n"
            << "  Subsystem : " << getSubsystemModule() << "\n"
            << "  Message   : " << what() << "\n"
            << "  Endpoint  : " << endpointUrl_ << "\n"
            << "  Timeout   : " << timeoutMs_ << " ms\n"
            << "  Location  : " << getFileName() << ":" << getLine() << " in " << getFunctionName() << "()";
        return oss.str();
    }
};

#define THROW_NET_EX(msg, code, severity, module, endpoint, timeout) \
    throw NetworkTimeoutException((msg), (code), (severity), (module), __FILE__, __LINE__, __func__, (endpoint), (timeout))

// =====================================================================================
// 5. BUSINESS LOGIC SERVICES DEMONSTRATING EXCEPTION DESIGN IN PRACTICE
// =====================================================================================
class DataAccessLayer {
public:
    static void executeUserQuery(int userId) {
        if (userId <= 0) {
            THROW_APP_EX(
                "User ID must be strictly positive!",
                ErrorCode::InvalidInput,
                ErrorSeverity::Warning,
                "DataAccessLayer::Validation"
            );
        }

        if (userId == 404) {
            THROW_DB_EX(
                "Target table 'users_shard_02' query execution failed",
                ErrorCode::QueryExecution,
                ErrorSeverity::Error,
                "DataAccessLayer::SQLPool",
                "42S02",
                "SELECT * FROM users_shard_02 WHERE id = 404",
                true // Retryable
            );
        }

        cout << "    [DataAccessLayer] Query executed successfully for User ID: " << userId << "\n";
    }
};

class RemoteServiceClient {
public:
    static void fetchRemoteConfig(const string& endpoint) {
        if (endpoint.find("timeout") != string::npos) {
            THROW_NET_EX(
                "HTTP socket read operation timed out prior to receiving headers",
                ErrorCode::NetworkTimeout,
                ErrorSeverity::Critical,
                "RemoteServiceClient::HTTP",
                endpoint,
                5000
            );
        }
        cout << "    [RemoteServiceClient] Fetched config from endpoint: " << endpoint << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Exception Class Design analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASE EXCEPTION CATCH & CALL-SITE METADATA INSPECTION
    // =====================================================================================
    cout << "\n================ 1. BASE EXCEPTION & CALL-SITE TRACKING ================\n";

    try {
        cout << "  - Attempting invalid user query with ID = -1...\n";
        DataAccessLayer::executeUserQuery(-1);
    } catch (const ApplicationException& ex) {
        cout << "  - [CAUGHT BASE `ApplicationException`]:\n";
        cout << "    * What String  : " << ex.what() << "\n";
        cout << "    * Error Code   : " << errorCodeToString(ex.getErrorCode()) << "\n";
        cout << "    * Severity     : " << severityToString(ex.getSeverity()) << "\n";
        cout << "    * File & Line  : " << ex.getFileName() << ":" << ex.getLine() << "\n";
        cout << "    * Function     : " << ex.getFunctionName() << "()\n";
    }

    // =====================================================================================
    // 2. DERIVED EXCEPTION WITH SPECIFIC CONTEXT (`DatabaseQueryException`)
    // =====================================================================================
    cout << "\n================ 2. DERIVED EXCEPTION (`DatabaseQueryException`) ================\n";

    try {
        cout << "  - Attempting database query with ID = 404 (Triggers DB Exception)...\n";
        DataAccessLayer::executeUserQuery(404);
    } catch (const DatabaseQueryException& dbEx) {
        cout << "  - [CAUGHT SPECIFIC `DatabaseQueryException`]:\n";
        cout << "    * SQL State    : " << dbEx.getSqlState() << "\n";
        cout << "    * Failed Query : " << dbEx.getFailedQuery() << "\n";
        cout << "    * Retryable    : " << (dbEx.isRetryable() ? "TRUE" : "FALSE") << "\n";
        cout << "\n  --- Full Diagnostic Report ---\n" << dbEx.getDiagnosticReport() << "\n";
    }

    // =====================================================================================
    // 3. NETWORK EXCEPTION WITH TIMEOUT METADATA (`NetworkTimeoutException`)
    // =====================================================================================
    cout << "\n================ 3. DERIVED EXCEPTION (`NetworkTimeoutException`) ================\n";

    try {
        cout << "  - Calling remote endpoint 'https://api.internal/v1/timeout'...\n";
        RemoteServiceClient::fetchRemoteConfig("https://api.internal/v1/timeout?node=" + to_string(userInputValue));
    } catch (const NetworkTimeoutException& netEx) {
        cout << "  - [CAUGHT SPECIFIC `NetworkTimeoutException`]:\n";
        cout << "    * Endpoint URL : " << netEx.getEndpointUrl() << "\n";
        cout << "    * Timeout Limit: " << netEx.getTimeoutMs() << " ms\n";
        cout << "\n  --- Full Diagnostic Report ---\n" << netEx.getDiagnosticReport() << "\n";
    }

    // =====================================================================================
    // 4. POLYMORPHIC CATCHING & RTTI DOWNCASTING
    // =====================================================================================
    cout << "\n================ 4. POLYMORPHIC DISPATCH & RTTI DOWNCASTING ================\n";

    try {
        cout << "  - Triggering Database Exception handled via polymorphic base reference...\n";
        DataAccessLayer::executeUserQuery(404);
    } catch (const ApplicationException& baseRef) {
        cout << "  - [POLYMORPHIC CATCH HANDLER] Intercepted exception of type: " 
             << typeid(baseRef).name() << "\n";
        cout << "\n  --- Polymorphic Virtual Report Output ---\n";
        cout << baseRef.getDiagnosticReport() << "\n";

        // Dynamic Downcasting to inspect specific derived payload if needed
        if (const auto* dbPtr = dynamic_cast<const DatabaseQueryException*>(&baseRef)) {
            cout << "\n    -> [RTTI DOWNCAST SUCCESS] Handled Database Exception:\n";
            cout << "       SQL State: " << dbPtr->getSqlState() 
                 << " | Retry Recommended: " << (dbPtr->isRetryable() ? "YES" : "NO") << "\n";
        }
    }

    // =====================================================================================
    // 5. STANDARD C++ `std::exception` COMPATIBILITY
    // =====================================================================================
    cout << "\n================ 5. STANDARD `std::exception` COMPATIBILITY ================\n";

    try {
        RemoteServiceClient::fetchRemoteConfig("https://api.internal/v1/timeout");
    } catch (const std::exception& stdEx) {
        cout << "  - [STANDARD C++ HANDLER] Intercepted via `const std::exception&`:\n";
        cout << "    * Standard Message (`what()`) : " << stdEx.what() << "\n";
        cout << "    * Dynamic RTTI Class          : " << typeid(stdEx).name() << "\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION CLASS DESIGN SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Design Pillar         | C++ Implementation Pattern        | Architectural Advantage / Goal    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Standard Inheritance  | `class Ex : public runtime_error` | Leverages built-in `what()` owner |\n"
         << "| Call-Site Tracking    | `__FILE__`, `__LINE__`, `__func__`| Pinpoints exact throw location    |\n"
         << "| Immutable Payload     | Private fields + `const noex` get | Thread-safe, non-throw read state |\n"
         << "| Structured Error Code | Enum class `ErrorCode`            | Enables programmatic handling     |\n"
         << "| Severity Classification| Enum class `ErrorSeverity`       | Drives logging / alert policies   |\n"
         << "| Nothrow Guarantees    | `noexcept` destructors & getters  | Prevents `std::terminate` unwinds |\n"
         << "| Polymorphic Reports   | `virtual string report() const`   | Dynamic, detailed diagnostic logs |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}