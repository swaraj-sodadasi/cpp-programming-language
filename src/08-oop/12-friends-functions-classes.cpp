/*
 * =====================================================================================
 * CONCEPT        : Friends in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the rules, mechanics, and architectural applications of `friend`:
 *
 *                  1. Global Friend Functions   : Standalone non-member functions granted direct 
 *                                                 access to `private` and `protected` class state.
 *                  2. Friend Operator Overloads : Global operator functions (e.g., `operator<<`) 
 *                                                 granted private access for clean stream output.
 *                  3. Member Friend Functions   : Specifying a specific member function of Class A 
 *                                                 as a friend inside Class B (requires forward decl).
 *                  4. Friend Classes            : Granting an entire external class full access 
 *                                                 privileges to all private and protected members.
 *                  5. Core Rules of Friendship  :
 *                     - Non-Symmetric (One-way) : Class A befriending Class B does NOT grant A 
 *                                                 access to B's private members.
 *                     - Non-Transitive          : If B is a friend of A, and C is a friend of B, 
 *                                                 C is NOT automatically a friend of A.
 *                     - Non-Inherited           : Derived classes do NOT inherit friend status 
 *                                                 from their base classes.
 *
 * TIME COMPLEXITY  : Access Control Bypass : Executed entirely at compile-time (O(1) runtime cost).
 * SPACE COMPLEXITY : Memory Footprint     : 0 bytes overhead (pure access control declaration).
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
// FORWARD DECLARATIONS (REQUIRED FOR MEMBER FRIEND FUNCTIONS)
// =====================================================================================
class SecureVault; // Forward declaration of target class

// Class containing a member function that will be declared as a friend inside SecureVault
class SystemController {
public:
    void recalibrateVaultBalance(SecureVault& vault, double newBalance);
};

// =====================================================================================
// 1. TARGET CLASS WITH FRIEND DECLARATIONS
// =====================================================================================
class SecureVault {
private:
    int vaultId_;
    string masterKey_;
    double reserveBalance_;

public:
    SecureVault(int id, string masterKey, double balance)
        : vaultId_(id), masterKey_(std::move(masterKey)), reserveBalance_(balance) {
        if (id <= 0) {
            throw invalid_argument("Vault ID must be strictly positive!");
        }
        if (balance < 0.0) {
            throw invalid_argument("Initial reserve balance cannot be negative!");
        }
        cout << "    [SecureVault Ctor] Initialized Vault #" << vaultId_ 
             << " | Reserve: $" << fixed << setprecision(2) << reserveBalance_ << "\n";
    }

    ~SecureVault() noexcept = default;

    // Public read-only accessors
    [[nodiscard]] int getVaultId() const noexcept { return vaultId_; }
    [[nodiscard]] double getReserveBalance() const noexcept { return reserveBalance_; }

    // =================================================================================
    // FRIEND DECLARATIONS
    // =================================================================================

    // A. Global Friend Function
    friend void inspectVaultGlobally(const SecureVault& vault);

    // B. Global Friend Operator Overload
    friend ostream& operator<<(ostream& os, const SecureVault& vault);

    // C. Specific Member Function of Another Class as Friend
    friend void SystemController::recalibrateVaultBalance(SecureVault& vault, double newBalance);

    // D. Entire Class as Friend
    friend class VaultAuditorService;
};

// =====================================================================================
// 2. DEFINITIONS OF FRIEND FUNCTIONS AND FRIEND CLASSES
// =====================================================================================

// Implementation of Member Friend Function (Must be defined AFTER SecureVault definition)
void SystemController::recalibrateVaultBalance(SecureVault& vault, double newBalance) {
    if (newBalance < 0.0) {
        throw invalid_argument("New balance cannot be negative!");
    }
    cout << "    [MEMBER FRIEND FUNC] SystemController directly updating private state:\n";
    cout << "      Vault ID: " << vault.vaultId_ 
         << " | Balance Change: $" << vault.reserveBalance_ << " -> $" << newBalance << "\n";
    
    // Direct access to private member `reserveBalance_`
    vault.reserveBalance_ = newBalance;
}

// Global Friend Function Definition
void inspectVaultGlobally(const SecureVault& vault) {
    cout << "    [GLOBAL FRIEND FUNC] Direct access to private members:\n";
    cout << "      Vault ID: " << vault.vaultId_ 
         << " | Master Key: '" << vault.masterKey_ << "'"
         << " | Balance: $" << fixed << setprecision(2) << vault.reserveBalance_ << "\n";
}

// Global Friend Operator Overload Definition
ostream& operator<<(ostream& os, const SecureVault& vault) {
    // Directly accesses private fields `vaultId_`, `masterKey_`, and `reserveBalance_`
    os << "SecureVault[ID=" << vault.vaultId_ 
       << ", Key='" << vault.masterKey_ 
       << "', Reserve=$" << fixed << setprecision(2) << vault.reserveBalance_ << "]";
    return os;
}

// Friend Class Definition
class VaultAuditorService {
public:
    void performFullAudit(const SecureVault& vault) const {
        cout << "    [FRIEND CLASS] VaultAuditorService auditing private fields:\n";
        cout << "      Inspecting Master Key: '" << vault.masterKey_ << "'\n";
        cout << "      Inspecting Reserve Balance: $" << vault.reserveBalance_ << "\n";
    }

    void emergencyKeyOverride(SecureVault& vault, const string& newKey) const {
        if (newKey.empty()) {
            throw invalid_argument("Emergency key cannot be empty!");
        }
        cout << "    [FRIEND CLASS] VaultAuditorService overriding private `masterKey_`:\n";
        cout << "      Old Key: '" << vault.masterKey_ << "' -> New Key: '" << newKey << "'\n";
        vault.masterKey_ = newKey; // Direct private access
    }
};

// =====================================================================================
// 3. PROOF CLASSES FOR FRIENDSHIP RULES (NON-INHERITED, NON-TRANSIENT, NON-SYMMETRIC)
// =====================================================================================

// Proof 1: Non-Inherited Friendship
class DerivedVaultAuditor : public VaultAuditorService {
public:
    void attemptDerivedAudit([[maybe_unused]] const SecureVault& vault) const {
        // double bal = vault.reserveBalance_; // COMPILE ERROR: Derived class does NOT inherit friend privileges!
        cout << "    [NON-INHERITED PROOF] Derived classes cannot access base friend targets directly.\n";
    }
};

// Derived Target Class to prove base friend doesn't grant automatic access to derived private members
class ExtendedSubVault : public SecureVault {
private:
    string subVaultSecret_{"SubVault_Secret_99"};

public:
    ExtendedSubVault(int id, string key, double bal) : SecureVault(id, std::move(key), bal) {}

    // VaultAuditorService is a friend of SecureVault (Base), but NOT ExtendedSubVault (Derived)!
    // void auditSubSecret(const ExtendedSubVault& sub) { string s = sub.subVaultSecret_; } // COMPILE ERROR!
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Friends analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. GLOBAL FRIEND FUNCTION & STREAM OPERATOR OVERLOAD
    // =====================================================================================
    cout << "\n================ 1. GLOBAL FRIEND FUNCTIONS & OPERATORS ================\n";

    SecureVault vault(userInputValue, "MK-ALPHA-9981", 50000.0);

    cout << "  - Invoking Global Friend Function `inspectVaultGlobally()`:\n";
    inspectVaultGlobally(vault);

    cout << "\n  - Invoking Global Friend Stream Operator `operator<<`:\n";
    cout << "    Stream Output: " << vault << "\n";

    // =====================================================================================
    // 2. MEMBER FRIEND FUNCTION
    // =====================================================================================
    cout << "\n================ 2. MEMBER FRIEND FUNCTION ================\n";

    SystemController controller;
    cout << "  - Calling `SystemController::recalibrateVaultBalance()` (Member Friend):\n";
    controller.recalibrateVaultBalance(vault, 75000.0);

    cout << "  - Updated State Output: " << vault << "\n";

    // =====================================================================================
    // 3. FRIEND CLASS
    // =====================================================================================
    cout << "\n================ 3. FRIEND CLASS ================\n";

    VaultAuditorService auditor;
    cout << "  - Calling `VaultAuditorService::performFullAudit()`:\n";
    auditor.performFullAudit(vault);

    cout << "\n  - Calling `VaultAuditorService::emergencyKeyOverride()`:\n";
    auditor.emergencyKeyOverride(vault, "MK-OMEGA-7712");

    cout << "  - Post-Override Output: " << vault << "\n";

    // =====================================================================================
    // 4. FRIENDSHIP RULES (NON-INHERITED, NON-TRANSIENT, NON-SYMMETRIC)
    // =====================================================================================
    cout << "\n================ 4. FRIENDSHIP RULES & BOUNDARIES ================\n";

    cout << "  - Rule A (Non-Symmetric / One-Way): SecureVault cannot access private members of VaultAuditorService.\n";
    cout << "  - Rule B (Non-Transitive)          : A friend of B, and B friend of C does NOT make A friend of C.\n";
    cout << "  - Rule C (Non-Inherited)           : Derived classes do NOT inherit friend status.\n";

    DerivedVaultAuditor derivedAuditor;
    derivedAuditor.attemptDerivedAudit(vault);

    // =====================================================================================
    // 5. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 5. FRIEND FUNCTION INVARIANT GUARDS ================\n";

    try {
        cout << "  - Attempting invalid negative balance update via controller friend...\n";
        controller.recalibrateVaultBalance(vault, -100.0);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    try {
        cout << "  - Attempting empty key override via auditor friend...\n";
        auditor.emergencyKeyOverride(vault, "");
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ FRIENDS IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Friend Mechanism      | Syntax / Declaration              | Architectural Role / Rule         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Global Friend Func    | `friend void func(const T&);`     | Free function with private access |\n"
         << "| Friend Operator       | `friend ostream& operator<<(...);`| Enables streaming of private state|\n"
         << "| Member Friend Func    | `friend void ClassA::func(T&);`   | Specific method of Class A friend |\n"
         << "| Friend Class          | `friend class AuditorService;`    | Entire external class granted access|\n"
         << "| Non-Symmetric Rule    | Friendship is strictly One-Way    | Target does not gain friend access|\n"
         << "| Non-Transitive Rule   | A -> B -> C != A -> C             | Friendship cannot chain           |\n"
         << "| Non-Inherited Rule    | Derived does NOT inherit friend   | Friendship boundary stops at Base |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}