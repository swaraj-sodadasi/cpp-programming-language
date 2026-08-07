/*
 * =====================================================================================
 * CONCEPT        : Access Specifiers in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the rules, scope boundaries, and inheritance semantics of Access Specifiers:
 *
 *                  1. Member Access Visibility :
 *                     - `public`    : Accessible anywhere from outside or inside the class.
 *                     - `protected` : Accessible within the class and derived classes; hidden outside.
 *                     - `private`   : Accessible ONLY within the defining class; hidden outside/derived.
 *
 *                  2. Class vs. Struct Defaults :
 *                     - `class` default access  : `private` for members and inheritance.
 *                     - `struct` default access : `public` for members and inheritance.
 *
 *                  3. Inheritance Access Modes :
 *                     - Public Inheritance    (`class D : public B`)   : Keeps public/protected as public/protected.
 *                     - Protected Inheritance (`class D : protected B`): Converts public/protected to protected.
 *                     - Private Inheritance   (`class D : private B`)  : Converts public/protected to private.
 *
 *                  4. Friend Declarations & Access Adjustments :
 *                     - Granting private access via `friend` functions and classes.
 *                     - Restoring base visibility in derived classes via `using` declarations.
 *
 * TIME COMPLEXITY  : Access Control Checks : Executed entirely at compile-time (O(1) runtime cost).
 * SPACE COMPLEXITY : Access specifiers consume 0 bytes at runtime (pure compile-time layout rules).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// Forward declaration for Friend Class demonstration
class AuditorFriend;

// =====================================================================================
// 1. BASE CLASS DEMONSTRATING PUBLIC, PROTECTED, AND PRIVATE MEMBERS
// =====================================================================================
class BaseAccount {
public:
    int accountId_; // Public: Accessible everywhere

protected:
    string accountHolder_; // Protected: Accessible in BaseAccount and derived classes
    double balance_;

private:
    int securityPin_; // Private: Accessible ONLY inside BaseAccount

public:
    BaseAccount(int id, string holder, double initialBalance, int pin)
        : accountId_(id), accountHolder_(std::move(holder)), balance_(initialBalance), securityPin_(pin) {
        if (id <= 0) throw invalid_argument("Account ID must be positive!");
        if (pin < 1000 || pin > 9999) throw invalid_argument("PIN must be 4 digits!");
        
        cout << "    [BaseAccount Ctor] Initialized Account #" << accountId_ 
             << " for '" << accountHolder_ << "'\n";
    }

    virtual ~BaseAccount() noexcept = default;

    // Public method exposing controlled access to private data
    [[nodiscard]] bool authenticatePin(int pinInput) const noexcept {
        return pinInput == securityPin_;
    }

    void displayBaseInfo() const {
        cout << "    [BaseAccount Info] ID: " << accountId_ 
             << " | Holder: " << accountHolder_ 
             << " | Balance: $" << fixed << setprecision(2) << balance_ << "\n";
    }

    // Friend Function Grant
    friend void externalInspectorFunction(const BaseAccount& account);

    // Friend Class Grant
    friend class AuditorFriend;
};

// =====================================================================================
// 2. INHERITANCE MODES DEMONSTRATION (PUBLIC, PROTECTED, PRIVATE INHERITANCE)
// =====================================================================================

// A. PUBLIC INHERITANCE: Preserves visibility (Public -> Public, Protected -> Protected)
class PublicDerivedAccount : public BaseAccount {
public:
    PublicDerivedAccount(int id, string holder, double balance, int pin)
        : BaseAccount(id, std::move(holder), balance, pin) {}

    void inspectInheritedMembers() const {
        cout << "    [PublicDerived] Accessing public ID: " << accountId_ << "\n";
        cout << "    [PublicDerived] Accessing protected Holder: " << accountHolder_ << "\n";
        cout << "    [PublicDerived] Accessing protected Balance: $" << balance_ << "\n";
        // int pin = securityPin_; // COMPILE ERROR: Cannot access private base member!
    }
};

// B. PROTECTED INHERITANCE: Downgrades public members to protected
class ProtectedDerivedAccount : protected BaseAccount {
public:
    ProtectedDerivedAccount(int id, string holder, double balance, int pin)
        : BaseAccount(id, std::move(holder), balance, pin) {}

    void inspectInheritedMembers() const {
        cout << "    [ProtectedDerived] Accessing downgraded public ID as protected: " << accountId_ << "\n";
        cout << "    [ProtectedDerived] Accessing protected Holder: " << accountHolder_ << "\n";
    }

    // Access Adjustment: Exposing inherited method publicly using `using`
    using BaseAccount::displayBaseInfo;
};

// C. PRIVATE INHERITANCE: Downgrades public/protected members to private
class PrivateDerivedAccount : private BaseAccount {
public:
    PrivateDerivedAccount(int id, string holder, double balance, int pin)
        : BaseAccount(id, std::move(holder), balance, pin) {}

    void inspectInheritedMembers() const {
        cout << "    [PrivateDerived] Accessing downgraded public ID as private: " << accountId_ << "\n";
    }

    // Access Adjustment: Restoring selective public interface
    using BaseAccount::authenticatePin;
    using BaseAccount::displayBaseInfo;
};

// =====================================================================================
// 3. FRIEND FUNCTIONS AND FRIEND CLASSES
// Demonstrates bypassing private access restrictions via explicit grants.
// =====================================================================================

// Friend Function Definition
void externalInspectorFunction(const BaseAccount& account) {
    cout << "    [FRIEND FUNCTION] Direct Access to private securityPin_: " 
         << account.securityPin_ << "\n";
}

// Friend Class Definition
class AuditorFriend {
public:
    void auditAccountDetails(const BaseAccount& account) const {
        cout << "    [FRIEND CLASS] Auditing Private PIN: " << account.securityPin_ 
             << " | Protected Holder: " << account.accountHolder_ 
             << " | Balance: $" << account.balance_ << "\n";
    }
};

// =====================================================================================
// 4. CLASS VS STRUCT DEFAULT ACCESS SPECIFIERS
// =====================================================================================
class DefaultClassDemo {
    int defaultPrivateVar = 10; // Default access in `class` is PRIVATE
public:
    int getVar() const { return defaultPrivateVar; }
};

struct DefaultStructDemo {
    int defaultPublicVar = 20; // Default access in `struct` is PUBLIC
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Access Specifiers analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MEMBER ACCESS VISIBILITY
    // =====================================================================================
    cout << "\n================ 1. MEMBER ACCESS VISIBILITY (PUBLIC/PROTECTED/PRIVATE) ================\n";

    BaseAccount account(userInputValue, "Alice Smith", 2500.0, 4321);

    // Public member access
    cout << "  - Direct Public Access: account.accountId_ = " << account.accountId_ << "\n";
    account.displayBaseInfo();

    // Protected & Private members are inaccessible from outside:
    // string holder = account.accountHolder_; // COMPILE ERROR: 'accountHolder_' is protected!
    // double bal = account.balance_;          // COMPILE ERROR: 'balance_' is protected!
    // int pin = account.securityPin_;         // COMPILE ERROR: 'securityPin_' is private!

    // Controlled private member authentication via public function
    cout << "  - Authenticating PIN via Public Interface (4321): " 
         << (account.authenticatePin(4321) ? "SUCCESS" : "FAILED") << "\n";

    // =====================================================================================
    // 2. INHERITANCE ACCESS MODES
    // =====================================================================================
    cout << "\n================ 2. INHERITANCE ACCESS MODES ================\n";

    {
        cout << "  - A. Public Inheritance (`public BaseAccount`):\n";
        PublicDerivedAccount pubDerived(userInputValue + 10, "Bob Jones", 1200.0, 1111);
        pubDerived.inspectInheritedMembers();
        cout << "    Direct External Access to Inherited Public Member: ID=" << pubDerived.accountId_ << "\n";

        cout << "\n  - B. Protected Inheritance (`protected BaseAccount`):\n";
        ProtectedDerivedAccount protDerived(userInputValue + 20, "Charlie Brown", 3400.0, 2222);
        protDerived.inspectInheritedMembers();
        // protDerived.accountId_; // COMPILE ERROR: Inherited public member downgraded to protected!
        protDerived.displayBaseInfo(); // Accessible due to 'using BaseAccount::displayBaseInfo'

        cout << "\n  - C. Private Inheritance (`private BaseAccount`):\n";
        PrivateDerivedAccount privDerived(userInputValue + 30, "Diana Prince", 5000.0, 3333);
        privDerived.inspectInheritedMembers();
        privDerived.displayBaseInfo(); // Restored via using declaration
    }

    // =====================================================================================
    // 3. FRIEND FUNCTIONS AND FRIEND CLASSES
    // =====================================================================================
    cout << "\n================ 3. FRIEND FUNCTIONS AND FRIEND CLASSES ================\n";

    cout << "  - Invoking Friend Function:\n";
    externalInspectorFunction(account);

    cout << "  - Invoking Friend Class Method:\n";
    AuditorFriend auditor;
    auditor.auditAccountDetails(account);

    // =====================================================================================
    // 4. CLASS VS STRUCT DEFAULT ACCESS
    // =====================================================================================
    cout << "\n================ 4. CLASS VS STRUCT DEFAULT ACCESS ================\n";

    DefaultClassDemo cDemo;
    DefaultStructDemo sDemo;

    // cout << cDemo.defaultPrivateVar; // COMPILE ERROR: Default class member is private!
    cout << "  - Class Default Member (via Getter) : " << cDemo.getVar() << "\n";
    cout << "  - Struct Default Member (Direct)    : " << sDemo.defaultPublicVar << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ ACCESS SPECIFIERS IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-------------------+-------------------+-------------------+\n"
         << "| Access Specifier      | Within Same Class | Derived Classes   | Outside Callers   |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+\n"
         << "| public                | YES               | YES               | YES               |\n"
         << "| protected             | YES               | YES               | NO                |\n"
         << "| private               | YES               | NO                | NO                |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+\n"
         << "| Inheritance Mode      | Public Members    | Protected Members | Private Members   |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+\n"
         << "| public Base           | Become public     | Become protected  | Inaccessible      |\n"
         << "| protected Base        | Become protected  | Become protected  | Inaccessible      |\n"
         << "| private Base          | Become private    | Become private    | Inaccessible      |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+\n";

    return 0;
}