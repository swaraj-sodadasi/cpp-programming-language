/*
 * =====================================================================================
 * CONCEPT        : Creating Custom Exceptions in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architecture, design patterns, and best practices for creating 
 *                  Custom Exception classes in Modern C++:
 *
 *                  1. Standard Base Selection (`std::exception` vs `std::runtime_error`) :
 *                     - Inheriting from `std::runtime_error` or `std::logic_error` is preferred 
 *                       over `std::exception` because they manage internal string allocation 
 *                       and memory safety for `what()` automatically.
 *
 *                  2. Custom Stateful Payloads :
 *                     - Attaching domain-specific attributes (error codes, account numbers, 
 *                       shortfall balances, timestamps, module names) to custom exception objects.
 *
 *                  3. Call-Site Context Tracking :
 *                     - Capturing call-site source metadata (`__FILE__`, `__LINE__`, `__func__`) 
 *                       via macro wrappers to enable precise debugging without code clutter.
 *
 *                  4. Exception Safety & `noexcept` Contracts :
 *                     - Overriding `what()` and custom getters with `const noexcept` and `[[nodiscard]]`.
 *                     - Ensuring custom exception copy/move constructors never throw during allocation.
 *
 *                  5. Polymorphic Exception Catching & RTTI Downcasting :
 *                     - Catching custom exceptions by `const BaseException&` to preserve dynamic 
 *                       polymorphic dispatch and prevent object slicing.
 *
 * TIME COMPLEXITY  : Exception Construction / Payload Access : O(1) constant time.
 *                    Polymorphic Dispatch / Stack Unwinding  : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Custom Exception Object Footprint      : Small fixed memory block for state fields.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <iomanip>
#include <limits>
#include <typeinfo>

using namespace std;

// =====================================================================================
// 1. BASE CUSTOM EXCEPTION CLASS (`CoreAppException`)
// Inherits from std::runtime_error to leverage safe internal string management.
// =====================================================================================
class CoreAppException : public std::runtime_error {
private:
    int errorCode_;
    string subsystemModule_;
    string fileName_;
    int line_;
    string functionName_;

public:
    CoreAppException(
        const string& message,
        int errorCode,
        string subsystemModule,
        const char* file,
        int line,
        const char* func)
        : std::runtime_error(message),
          errorCode_(errorCode),
          subsystemModule_(std::move(subsystemModule)),
          fileName_(file ? file : "UnknownFile"),
          line_(line),
          functionName_(func ? func : "UnknownFunc") {}

    ~CoreAppException() noexcept override = default;

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }
    [[nodiscard]] const string& getSubsystemModule() const noexcept { return subsystemModule_; }
    [[nodiscard]] const string& getFileName() const noexcept { return fileName_; }
    [[nodiscard]] int getLine() const noexcept { return line_; }
    [[nodiscard]] const string& getFunctionName() const noexcept { return functionName_; }

    // Polymorphic diagnostic report virtual function
    [[nodiscard]] virtual string getDiagnosticReport() const {
        string report;
        report += "CoreAppException [Code: " + to_string(errorCode_) + 
                  " | Module: " + subsystemModule_ + "]\n";
        report += "  Message : " + string(what()) + "\n";
        report += "  Location: " + fileName_ + ":" + to_string(line_) + 
                  " in " + functionName_ + "()";
        return report;
    }
};

// Macro helper to automate capturing call-site file, line, and function name
#define THROW_CORE_APP_EX(ExType, msg, code, module, ...) \
    throw ExType((msg), (code), (module), __FILE__, __LINE__, __func__, ##__VA_ARGS__)

// =====================================================================================
// 2. DOMAIN-SPECIFIC CUSTOM EXCEPTION 1: `InsufficientFundsException`
// Encapsulates financial transaction failure data.
// =====================================================================================
class InsufficientFundsException : public CoreAppException {
private:
    string accountNumber_;
    double requestedAmount_;
    double currentBalance_;

public:
    InsufficientFundsException(
        const string& message,
        int errorCode,
        string subsystemModule,
        const char* file,
        int line,
        const char* func,
        string accountNumber,
        double requestedAmount,
        double currentBalance)
        : CoreAppException(message, errorCode, std::move(subsystemModule), file, line, func),
          accountNumber_(std::move(accountNumber)),
          requestedAmount_(requestedAmount),
          currentBalance_(currentBalance) {}

    ~InsufficientFundsException() noexcept override = default;

    [[nodiscard]] const string& getAccountNumber() const noexcept { return accountNumber_; }
    [[nodiscard]] double getRequestedAmount() const noexcept { return requestedAmount_; }
    [[nodiscard]] double getCurrentBalance() const noexcept { return currentBalance_; }
    [[nodiscard]] double getShortfallAmount() const noexcept { return requestedAmount_ - currentBalance_; }

    [[nodiscard]] string getDiagnosticReport() const override {
        ostringstream oss;
        oss << fixed << setprecision(2);
        oss << "InsufficientFundsException [Code: " << getErrorCode() 
            << " | Module: " << getSubsystemModule() << "]\n"
            << "  Message     : " << what() << "\n"
            << "  Account     : " << accountNumber_ << "\n"
            << "  Requested   : $" << requestedAmount_ << "\n"
            << "  Balance     : $" << currentBalance_ << "\n"
            << "  Shortfall   : $" << getShortfallAmount() << "\n"
            << "  Location    : " << getFileName() << ":" << getLine() 
            << " in " << getFunctionName() << "()";
        return oss.str();
    }
};

// Macro helper for InsufficientFundsException
#define THROW_INSUFFICIENT_FUNDS(msg, code, module, accNum, reqAmt, currBal) \
    throw InsufficientFundsException((msg), (code), (module), __FILE__, __LINE__, __func__, (accNum), (reqAmt), (currBal))

// =====================================================================================
// 3. DOMAIN-SPECIFIC CUSTOM EXCEPTION 2: `AuthenticationFailedException`
// Encapsulates user security/login authorization failure state.
// =====================================================================================
class AuthenticationFailedException : public CoreAppException {
private:
    string username_;
    int failedAttempts_;
    string clientIp_;

public:
    AuthenticationFailedException(
        const string& message,
        int errorCode,
        string subsystemModule,
        const char* file,
        int line,
        const char* func,
        string username,
        int failedAttempts,
        string clientIp)
        : CoreAppException(message, errorCode, std::move(subsystemModule), file, line, func),
          username_(std::move(username)),
          failedAttempts_(failedAttempts),
          clientIp_(std::move(clientIp)) {}

    ~AuthenticationFailedException() noexcept override = default;

    [[nodiscard]] const string& getUsername() const noexcept { return username_; }
    [[nodiscard]] int getFailedAttempts() const noexcept { return failedAttempts_; }
    [[nodiscard]] const string& getClientIp() const noexcept { return clientIp_; }

    [[nodiscard]] string getDiagnosticReport() const override {
        string report;
        report += "AuthenticationFailedException [Code: " + to_string(getErrorCode()) + 
                  " | Module: " + getSubsystemModule() + "]\n";
        report += "  Message    : " + string(what()) + "\n";
        report += "  User       : " + username_ + "\n";
        report += "  Attempts   : " + to_string(failedAttempts_) + "\n";
        report += "  Client IP  : " + clientIp_ + "\n";
        report += "  Location   : " + getFileName() + ":" + to_string(getLine()) + 
                  " in " + getFunctionName() + "()";
        return report;
    }
};

// Macro helper for AuthenticationFailedException
#define THROW_AUTH_FAILED(msg, code, module, user, attempts, ip) \
    throw AuthenticationFailedException((msg), (code), (module), __FILE__, __LINE__, __func__, (user), (attempts), (ip))

// =====================================================================================
// 4. BUSINESS LOGIC SERVICES DEMONSTRATING CUSTOM EXCEPTION THROWS
// =====================================================================================
class BankingService {
public:
    static void processWithdrawal(const string& accNum, double currentBalance, double amountToWithdraw) {
        cout << "    [BankingService] Processing withdrawal of $" << fixed << setprecision(2) 
             << amountToWithdraw << " for Account: " << accNum << "...\n";

        if (amountToWithdraw > currentBalance) {
            THROW_INSUFFICIENT_FUNDS(
                "Transaction Declined: Withdrawal exceeds available balance",
                4002,
                "BankingEngine::Ledger",
                accNum,
                amountToWithdraw,
                currentBalance
            );
        }

        cout << "    [BankingService] Withdrawal approved! Remaining Balance: $" 
             << (currentBalance - amountToWithdraw) << "\n";
    }
};

class IdentityService {
public:
    static void authenticateUser(const string& user, const string& password, int attemptCount, const string& ip) {
        cout << "    [IdentityService] Authenticating user '" << user << "' from IP " << ip << "...\n";

        if (password != "SecretPass123") {
            THROW_AUTH_FAILED(
                "Access Denied: Invalid password credentials provided",
                1009,
                "SecuritySubsystem::AuthGuard",
                user,
                attemptCount,
                ip
            );
        }

        cout << "    [IdentityService] Authentication successful for user '" << user << "'!\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Custom Exceptions analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CATCHING SPECIFIC CUSTOM EXCEPTION (`InsufficientFundsException`)
    // =====================================================================================
    cout << "\n================ 1. CUSTOM EXCEPTION (`InsufficientFundsException`) ================\n";

    try {
        string accountNumber = "ACC-US-998" + to_string(userInputValue);
        double initialBalance = 250.00;
        double requestedAmount = 1000.00;

        BankingService::processWithdrawal(accountNumber, initialBalance, requestedAmount);

    } catch (const InsufficientFundsException& ex) {
        cout << "  - [CAUGHT SPECIFIC TYPE `InsufficientFundsException`]:\n";
        cout << "    * What Message     : " << ex.what() << "\n";
        cout << "    * Account Number   : " << ex.getAccountNumber() << "\n";
        cout << "    * Requested Amount : $" << fixed << setprecision(2) << ex.getRequestedAmount() << "\n";
        cout << "    * Current Balance  : $" << fixed << setprecision(2) << ex.getCurrentBalance() << "\n";
        cout << "    * Shortfall Amount : $" << fixed << setprecision(2) << ex.getShortfallAmount() << "\n";
        cout << "\n    --- Full Diagnostic Report ---\n" << ex.getDiagnosticReport() << "\n";
    }

    // =====================================================================================
    // 2. CATCHING ANOTHER SPECIFIC CUSTOM EXCEPTION (`AuthenticationFailedException`)
    // =====================================================================================
    cout << "\n================ 2. CUSTOM EXCEPTION (`AuthenticationFailedException`) ================\n";

    try {
        string username = "admin_user_" + to_string(userInputValue);
        IdentityService::authenticateUser(username, "WrongPassword", 3, "192.168.1.105");

    } catch (const AuthenticationFailedException& ex) {
        cout << "  - [CAUGHT SPECIFIC TYPE `AuthenticationFailedException`]:\n";
        cout << "    * User Name       : " << ex.getUsername() << "\n";
        cout << "    * Failed Attempts : " << ex.getFailedAttempts() << "\n";
        cout << "    * Client IP       : " << ex.getClientIp() << "\n";
        cout << "\n    --- Full Diagnostic Report ---\n" << ex.getDiagnosticReport() << "\n";
    }

    // =====================================================================================
    // 3. POLYMORPHIC CATCHING VIA BASE CLASS (`CoreAppException`)
    // =====================================================================================
    cout << "\n================ 3. POLYMORPHIC CATCH VIA BASE (`CoreAppException`) ================\n";

    try {
        cout << "  - Triggering InsufficientFundsException through base catch handler...\n";
        BankingService::processWithdrawal("ACC-POLY-123", 100.0, 500.0);

    } catch (const CoreAppException& baseEx) {
        cout << "  - [POLYMORPHIC BASE CATCH]: Intercepted exception of RTTI type: " 
             << typeid(baseEx).name() << "\n";
        cout << "  - [DYNAMIC VIRTUAL REPORT DISPATCH]:\n";
        cout << baseEx.getDiagnosticReport() << "\n";

        // Safe Downcasting to inspect specific derived payload if needed
        if (const auto* fundsEx = dynamic_cast<const InsufficientFundsException*>(&baseEx)) {
            cout << "    -> [RTTI DOWNCAST SUCCESS]: Shortfall is $" 
                 << fixed << setprecision(2) << fundsEx->getShortfallAmount() << "\n";
        }
    }

    // =====================================================================================
    // 4. ROOT `std::exception` COMPATIBILITY DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 4. ROOT `std::exception` COMPATIBILITY ================\n";

    try {
        cout << "  - Triggering custom exception handled by standard C++ library handler...\n";
        IdentityService::authenticateUser("root", "BadPass", 5, "10.0.0.1");

    } catch (const std::exception& stdEx) {
        cout << "  - [STANDARD C++ CATCH]: Intercepted via `const std::exception&`:\n";
        cout << "    * Standard Message (`what()`) : " << stdEx.what() << "\n";
        cout << "    * RTTI Dynamic Type           : " << typeid(stdEx).name() << "\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CREATING CUSTOM EXCEPTIONS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Design Component      | Implementation Pattern            | Primary Benefit & Guarantee       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Standard Inheritance  | `class Ex : public runtime_error` | Automatic internal string memory  |\n"
         << "| Stateful Payload      | Add private fields & const getters| Carries rich domain debugging data|\n"
         << "| Call-Site Tracking    | `__FILE__`, `__LINE__`, `__func__`| Pinpoints exact throw site        |\n"
         << "| Macro Helper          | `THROW_EX(msg, code, ...)`        | Simplifies call-site throws       |\n"
         << "| `noexcept` Contract   | `~Ex() noexcept`, getters `noex`  | Prevents std::terminate during throw|\n"
         << "| Polymorphic Dispatch  | `virtual string report() const`   | Dynamic diagnostic reports        |\n"
         << "| Catch By Const Ref    | `catch (const BaseEx& ex)`        | Prevents slicing; keeps vtable    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}