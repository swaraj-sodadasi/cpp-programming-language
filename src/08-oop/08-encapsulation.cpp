/*
 * =====================================================================================
 * CONCEPT        : Encapsulation in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the principles, patterns, and access control mechanics of Encapsulation:
 *
 *                  1. Data Hiding & Access Control  : Restricting direct access to data members 
 *                                                     using `private` and `protected` specifiers.
 *                  2. Class Invariants & Validation : Protecting object integrity by enforcing 
 *                                                     domain rules in constructors and mutators.
 *                  3. Implementation Hiding         : Exposing clean, high-level public APIs 
 *                                                     while hiding internal logic and algorithms.
 *                  4. Read-Only State Exposure      : Providing const accessors (`[[nodiscard]]`) 
 *                                                     to inspect state without risking mutation.
 *                  5. Protected Inheritance Boundary: Sharing encapsulated state selectively 
 *                                                     with derived classes via `protected`.
 *
 * TIME COMPLEXITY  : Accessors / Mutators / Invariant Checks : O(1) constant time.
 * SPACE COMPLEXITY : Object Footprint : Sum of member variable sizes (+ padding).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// 1. FULLY ENCAPSULATED DOMAIN CLASS
// Demonstrates data hiding, invariant enforcement, and implementation hiding.
// =====================================================================================
class EncapsulatedBankAccount {
private:
    // Encapsulated State Members (Private Data Members)
    int accountId_;
    string accountHolder_;
    double balance_;
    int pinCode_; // Hidden sensitive credential
    bool isLocked_{false};
    int failedAttempts_{0};

    // Private Helper Member Functions (Hidden Implementation Logic)
    [[nodiscard]] bool verifyPin(int inputPin) const noexcept {
        return inputPin == pinCode_;
    }

    void recordFailedAttempt() noexcept {
        ++failedAttempts_;
        if (failedAttempts_ >= 3) {
            isLocked_ = true;
            cout << "    [SECURITY EVENT] Account #" << accountId_ 
                 << " LOCKED due to 3 consecutive invalid PIN attempts!\n";
        }
    }

    void resetFailedAttempts() noexcept {
        failedAttempts_ = 0;
    }

public:
    // Explicit Constructor enforcing Class Invariants
    EncapsulatedBankAccount(int id, string holder, double initialDeposit, int pin)
        : accountId_(id), accountHolder_(std::move(holder)), balance_(0.0), pinCode_(pin) {
        if (id <= 0) {
            throw invalid_argument("Account ID must be strictly positive!");
        }
        if (initialDeposit < 0.0) {
            throw invalid_argument("Initial deposit cannot be negative!");
        }
        if (pin < 1000 || pin > 9999) {
            throw invalid_argument("PIN must be a 4-digit positive integer (1000-9999)!");
        }

        balance_ = initialDeposit;
        cout << "    [Ctor] Created EncapsulatedBankAccount #" << accountId_ 
             << " for '" << accountHolder_ << "' | Initial Balance: $" 
             << fixed << setprecision(2) << balance_ << "\n";
    }

    ~EncapsulatedBankAccount() noexcept = default;

    // Mutator Method (Setter with Invariant Checks & Security Validation)
    void deposit(double amount) {
        if (isLocked_) {
            throw runtime_error("Operation rejected: Account is LOCKED!");
        }
        if (amount <= 0.0) {
            throw invalid_argument("Deposit amount must be strictly positive!");
        }

        balance_ += amount;
        cout << "    [DEPOSIT SUCCESS] Account #" << accountId_ 
             << " deposited: $" << amount << " | New Balance: $" << balance_ << "\n";
    }

    void withdraw(double amount, int pin) {
        if (isLocked_) {
            throw runtime_error("Operation rejected: Account is LOCKED!");
        }
        if (!verifyPin(pin)) {
            recordFailedAttempt();
            throw invalid_argument("Authentication failure: Invalid PIN!");
        }

        resetFailedAttempts();

        if (amount <= 0.0) {
            throw invalid_argument("Withdrawal amount must be strictly positive!");
        }
        if (amount > balance_) {
            throw runtime_error("Insufficient funds for withdrawal!");
        }

        balance_ -= amount;
        cout << "    [WITHDRAWAL SUCCESS] Account #" << accountId_ 
             << " withdrew: $" << amount << " | Remaining Balance: $" << balance_ << "\n";
    }

    void changePin(int oldPin, int newPin) {
        if (isLocked_) {
            throw runtime_error("Operation rejected: Account is LOCKED!");
        }
        if (!verifyPin(oldPin)) {
            recordFailedAttempt();
            throw invalid_argument("Authentication failure: Incorrect old PIN!");
        }
        if (newPin < 1000 || newPin > 9999) {
            throw invalid_argument("New PIN must be a 4-digit positive integer!");
        }

        resetFailedAttempts();
        pinCode_ = newPin;
        cout << "    [PIN CHANGED SUCCESS] PIN updated securely for Account #" << accountId_ << "\n";
    }

    // Read-Only Accessors (Const Member Functions returning state safely)
    [[nodiscard]] int getAccountId() const noexcept { return accountId_; }
    [[nodiscard]] const string& getAccountHolder() const noexcept { return accountHolder_; }
    [[nodiscard]] double getBalance() const noexcept { return balance_; }
    [[nodiscard]] bool isLocked() const noexcept { return isLocked_; }

    void displaySummary() const {
        cout << "    [ACCOUNT SUMMARY] ID: " << setw(4) << accountId_ 
             << " | Holder: " << setw(18) << left << accountHolder_ 
             << " | Balance: $" << setw(8) << fixed << setprecision(2) << balance_ 
             << " | Status: " << (isLocked_ ? "LOCKED" : "ACTIVE") << "\n";
    }
};

// =====================================================================================
// 2. ENCAPSULATION ACROSS INHERITANCE (PROTECTED SPECIFIER)
// Demonstrates selective state access within inheritance boundaries.
// =====================================================================================
class BaseSecureVault {
protected:
    int vaultId_;
    string vaultTier_;

private:
    double totalReserveValue_; // Private to BaseSecureVault, inaccessible even to derived classes

public:
    BaseSecureVault(int id, string tier, double reserve)
        : vaultId_(id), vaultTier_(std::move(tier)), totalReserveValue_(reserve) {}

    virtual ~BaseSecureVault() noexcept = default;

    [[nodiscard]] double getReserveValue() const noexcept {
        return totalReserveValue_;
    }
};

class DerivedCommercialVault : public BaseSecureVault {
private:
    double commercialDepositLimit_;

public:
    DerivedCommercialVault(int id, string tier, double reserve, double limit)
        : BaseSecureVault(id, std::move(tier), reserve), commercialDepositLimit_(limit) {}

    void inspectVaultMetrics() const {
        // Can access 'protected' base members directly: vaultId_, vaultTier_
        cout << "    [PROTECTED ACCESS] Derived class accessing Base Vault ID: " << vaultId_ 
             << " (Tier: " << vaultTier_ << ")\n";

        // Cannot access 'private' base members directly:
        // double val = totalReserveValue_; // COMPILE ERROR!
        cout << "    [PUBLIC INTERFACE] Base Vault Reserve Value: $" << getReserveValue() << "\n";
        cout << "    [DERIVED STATE] Commercial Deposit Limit: $" << commercialDepositLimit_ << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Encapsulation analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. DATA HIDING & CONTROLLED MUTATION
    // =====================================================================================
    cout << "\n================ 1. DATA HIDING & CONTROLLED MUTATION ================\n";

    EncapsulatedBankAccount account(userInputValue, "Bob Miller", 1000.0, 1234);

    // Direct access to data members is blocked by access specifiers:
    // account.balance_ = 1000000.0; // COMPILE ERROR: 'balance_' is private!
    // account.pinCode_ = 0;         // COMPILE ERROR: 'pinCode_' is private!

    cout << "  - Depositing funds via public setter interface...\n";
    account.deposit(500.0);

    cout << "  - Withdrawing funds with correct PIN authentication...\n";
    account.withdraw(200.0, 1234);

    account.displaySummary();

    // =====================================================================================
    // 2. INVARIANT PROTECTION & EXCEPTION SAFETY
    // =====================================================================================
    cout << "\n================ 2. INVARIANT PROTECTION & EXCEPTION SAFETY ================\n";

    try {
        cout << "  - Attempting invalid negative deposit (-$50.0)...\n";
        account.deposit(-50.0);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    try {
        cout << "  - Attempting withdrawal exceeding account balance ($5,000.0)...\n";
        account.withdraw(5000.0, 1234);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // 3. SECURITY AUTHENTICATION & AUTOMATIC ACCOUNT LOCKOUT
    // =====================================================================================
    cout << "\n================ 3. SECURITY & AUTOMATIC ACCOUNT LOCKOUT ================\n";

    cout << "  - Testing failed PIN attempts to trigger automatic lockout mechanism...\n";
    for (int i = 1; i <= 3; ++i) {
        try {
            cout << "    Attempt " << i << ": Entering invalid PIN (9999)...\n";
            account.withdraw(100.0, 9999);
        } catch (const exception& e) {
            cout << "    -> Exception Caught: \"" << e.what() << "\"\n";
        }
    }

    cout << "\n  - Verifying account lock state after 3 failed attempts...\n";
    account.displaySummary();

    try {
        cout << "  - Attempting deposit on LOCKED account...\n";
        account.deposit(200.0);
    } catch (const exception& e) {
        cout << "  - [SECURITY GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // 4. ENCAPSULATION ACROSS INHERITANCE BOUNDARIES (PROTECTED ACCESS)
    // =====================================================================================
    cout << "\n================ 4. PROTECTED SPECIFIER & INHERITANCE ================\n";

    {
        cout << "  - Instantiating DerivedCommercialVault object...\n";
        DerivedCommercialVault vault(userInputValue + 10, "Titanium", 5000000.0, 1000000.0);
        vault.inspectVaultMetrics();
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ ENCAPSULATION IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Encapsulation Concept | Implementation Technique          | Architectural Benefit / Safety    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Data Hiding           | `private` data members            | Prevents invalid external state   |\n"
         << "| Read-Only Access      | Const getters (`[[nodiscard]]`)   | Exposes state without mutation    |\n"
         << "| Controlled Mutation   | Public setters / mutator methods  | Enforces domain invariants & rules|\n"
         << "| Implementation Hiding | `private` helper functions        | Conceals internal algorithms/logic|\n"
         << "| Derived Accessibility | `protected` data / methods        | Selective inheritance boundary    |\n"
         << "| Class Invariants      | Constructor & Setter validation   | Prevents corrupt object existence |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}