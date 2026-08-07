/*
 * =====================================================================================
 * CONCEPT        : Introduction to Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the fundamentals, standard exception hierarchy, custom exception types,
 *                  RAII stack unwinding, exception rethrowing, and `noexcept` guarantees:
 *
 *                  1. Try, Catch, and Throw Mechanics :
 *                     - `try`   : Encloses code that might throw an exception.
 *                     - `throw` : Signals an anomaly or error condition, interrupting normal control flow.
 *                     - `catch` : Handles specific exception types caught during execution.
 *
 *                  2. Standard Exception Hierarchy :
 *                     - Base class `std::exception` (`what()` method returning explanation string).
 *                     - Derived standard types (`std::runtime_error`, `std::invalid_argument`, 
 *                       `std::out_of_range`).
 *
 *                  3. Custom Exception Classes :
 *                     - Extending `std::runtime_error` or `std::exception` to encapsulate custom
 *                       domain-specific error state (e.g., account balances, error codes).
 *
 *                  4. Catch-by-Reference & Polymorphism :
 *                     - ALWAYS catch exceptions by const reference (`const std::exception&`) to prevent
 *                       Object Slicing and preserve dynamic `what()` polymorphic dispatch.
 *
 *                  5. RAII & Stack Unwinding :
 *                     - Automatic destruction of stack-bound objects in LIFO order when an 
 *                       exception propagates out of a block before entering a matching `catch` handler.
 *
 *                  6. Exception Rethrowing & Catch-All :
 *                     - `catch (...)` handles any unhandled exception type.
 *                     - `throw;` rethrows the current exception up the call stack intact.
 *
 * TIME COMPLEXITY  : Normal Control Flow (No Exceptions) : Zero runtime overhead (zero-cost exceptions).
 *                    Exception Propagation & Unwinding   : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Storage Footprint         : Small heap/compiler runtime exception object.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// 1. CUSTOM EXCEPTION CLASS
// Inherits from std::runtime_error to encapsulate specific domain data.
// =====================================================================================
class InsufficientFundsException : public std::runtime_error {
private:
    double requestedAmount_;
    double currentBalance_;

public:
    InsufficientFundsException(double requested, double balance)
        : std::runtime_error("Transaction Refused: Withdrawal amount exceeds available balance!"),
          requestedAmount_(requested),
          currentBalance_(balance) {}

    [[nodiscard]] double getRequestedAmount() const noexcept { return requestedAmount_; }
    [[nodiscard]] double getCurrentBalance() const noexcept { return currentBalance_; }
};

// =====================================================================================
// 2. RAII TRACER CLASS (DEMONSTRATING STACK UNWINDING CLEANUP)
// Shows that destructors run automatically when exceptions propagate out of scope.
// =====================================================================================
class TransactionSessionGuard {
private:
    string sessionName_;

public:
    explicit TransactionSessionGuard(string name) : sessionName_(std::move(name)) {
        cout << "    [RAII GUARD CTOR] Acquired transaction lock: '" << sessionName_ << "'\n";
    }

    ~TransactionSessionGuard() noexcept {
        cout << "    [RAII GUARD DTOR] Automatically releasing lock: '" << sessionName_ << "' (Clean Stack Unwind)\n";
    }
};

// =====================================================================================
// 3. DOMAIN CLASS WITH EXCEPTION-THROWING METHODS
// =====================================================================================
class BankAccount {
private:
    int accountId_;
    double balance_;

public:
    BankAccount(int id, double initialBalance)
        : accountId_(id), balance_(initialBalance) {
        
        if (id <= 0) {
            throw std::invalid_argument("Account ID must be strictly positive!");
        }
        if (initialBalance < 0.0) {
            throw std::invalid_argument("Initial account balance cannot be negative!");
        }

        cout << "    [BankAccount Ctor] Account #" << accountId_ 
             << " created with initial balance: $" << fixed << setprecision(2) << balance_ << "\n";
    }

    // Method demonstrating RAII stack unwinding and throwing custom/standard exceptions
    void withdraw(double amount) {
        // Local RAII resource guard (will be safely destructed if throw occurs below)
        TransactionSessionGuard sessionLock("Session_Lock_Account_" + to_string(accountId_));

        if (amount <= 0.0) {
            throw std::invalid_argument("Withdrawal amount must be strictly greater than zero!");
        }
        if (amount > balance_) {
            throw InsufficientFundsException(amount, balance_);
        }

        balance_ -= amount;
        cout << "    [SUCCESS] Withdrew $" << fixed << setprecision(2) << amount 
             << " | Remaining Balance: $" << balance_ << "\n";
    }

    // Method marked `noexcept` - promises to never leak exceptions to caller
    [[nodiscard]] double getBalance() const noexcept {
        return balance_;
    }

    [[nodiscard]] int getAccountId() const noexcept {
        return accountId_;
    }
};

// Helper function demonstrating Exception Rethrowing (`throw;`)
void intermediateTransactionManager(BankAccount& account, double amount) {
    try {
        cout << "  - [Intermediate Manager] Attempting withdrawal of $" << amount << "...\n";
        account.withdraw(amount);
    } catch (const InsufficientFundsException& ex) {
        cout << "  - [Intermediate Manager] Logged error internally: \"" << ex.what() << "\"\n";
        cout << "  - [Intermediate Manager] Rethrowing exception up call stack...\n";
        throw; // Rethrows the exact same exception object intact
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Exception Handling analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. STANDARD EXCEPTIONS & TRY/CATCH BASICS
    // =====================================================================================
    cout << "\n================ 1. STANDARD EXCEPTIONS (`std::invalid_argument`) ================\n";

    try {
        cout << "  - Attempting to instantiate BankAccount with negative ID (-5)...\n";
        BankAccount invalidAcc(-5, 500.0);
    } catch (const std::invalid_argument& ex) { // Catching specific standard exception by const ref
        cout << "  - [CAUGHT `std::invalid_argument`]: \"" << ex.what() << "\"\n";
    } catch (const std::exception& ex) { // Base fallback handler
        cout << "  - [CAUGHT `std::exception`]: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // 2. CUSTOM EXCEPTION & CATCH-BY-REFERENCE
    // =====================================================================================
    cout << "\n================ 2. CUSTOM EXCEPTION (`InsufficientFundsException`) ================\n";

    try {
        BankAccount validAcc(userInputValue, 250.0);
        cout << "  - Attempting to withdraw $1000.00 from $250.00 balance...\n";
        validAcc.withdraw(1000.0);
    } catch (const InsufficientFundsException& ex) { // Catching custom exception type
        cout << "  - [CAUGHT CUSTOM EXCEPTION]: \"" << ex.what() << "\"\n";
        cout << "    * Requested Amount : $" << fixed << setprecision(2) << ex.getRequestedAmount() << "\n";
        cout << "    * Available Balance: $" << fixed << setprecision(2) << ex.getCurrentBalance() << "\n";
    }

    // =====================================================================================
    // 3. RAII STACK UNWINDING GUARANTEE
    // =====================================================================================
    cout << "\n================ 3. RAII & AUTOMATED STACK UNWINDING ================\n";

    try {
        BankAccount account(userInputValue + 1, 100.0);
        cout << "  - Calling `withdraw(500.0)` which triggers RAII guard followed by an exception...\n";
        account.withdraw(500.0);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT EXCEPTION AFTER STACK UNWIND]: \"" << ex.what() << "\"\n";
        cout << "    Notice above that TransactionSessionGuard's destructor executed BEFORE this catch block!\n";
    }

    // =====================================================================================
    // 4. RETHROWING EXCEPTIONS (`throw;`) & CATCH-ALL (`catch (...)`)
    // =====================================================================================
    cout << "\n================ 4. EXCEPTION RETHROWING & CATCH-ALL (`catch (...)`) ================\n";

    try {
        BankAccount account(userInputValue + 2, 50.0);
        intermediateTransactionManager(account, 200.0);
    } catch (const InsufficientFundsException& ex) {
        cout << "  - [MAIN CAUGHT RETHROWN EXCEPTION]: \"" << ex.what() << "\"\n";
    } catch (...) { // Catch-All handler catches non-std or unknown exceptions
        cout << "  - [CATCH-ALL HANDLER]: Caught unknown exception type!\n";
    }

    // =====================================================================================
    // 5. FUNCTION `noexcept` SPECIFIER GUARANTEE
    // =====================================================================================
    cout << "\n================ 5. `noexcept` SPECIFIER GUARANTEE ================\n";

    BankAccount safeAccount(userInputValue + 3, 500.0);
    static_assert(noexcept(safeAccount.getBalance()), "`getBalance()` must be marked noexcept!");

    cout << "  - Safely querying balance via `noexcept` method: $" 
         << safeAccount.getBalance() << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION HANDLING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Exception Component   | Syntax / Implementation           | Primary Purpose & Rule            |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| `try` Block           | `try { /* code */ }`              | Encloses code that might throw    |\n"
         << "| `throw` Statement     | `throw std::runtime_error(\"...\");`| Interrupts flow; signals error    |\n"
         << "| `catch` Handler       | `catch (const std::exception& e)` | Handles caught error by const ref |\n"
         << "| Catch-All Handler     | `catch (...)`                     | Intercepts ANY unhandled exception|\n"
         << "| Exception Rethrow     | `throw;` (inside catch block)     | Propagates exact exception upward |\n"
         << "| Custom Exception      | `class Ex : public std::exception`| Encapsulates domain error state   |\n"
         << "| Stack Unwinding       | Automatic RAII stack teardown     | Guarantees destructors run on fail|\n"
         << "| `noexcept` Specifier  | `double getVal() const noexcept;` | Guarantees function never throws  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}