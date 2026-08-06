/*
 * =====================================================================================
 * CONCEPT        : Const Correctness in C++ (Const Variables, Pointers, Member Functions, Mutable)
 * DESCRIPTION    : Comprehensive implementation detailing C++ const correctness principles:
 *                  1. Const Variables        : Read-only variable declarations preventing modification.
 *                  2. Const with Pointers    : Distinguishing `const T*` (pointer to const data),
 *                                              `T* const` (const pointer to data), and `const T* const`.
 *                  3. Const Member Functions : Member functions marked `const` guarantee they will
 *                                              not modify object state (`this` becomes `const T*`).
 *                  4. The `mutable` Keyword  : Allows modifying specific logical state (e.g. caches,
 *                                              mutexes, access counters) within const functions.
 *                  5. Const References & Pass: Using `const T&` for zero-copy read-only parameters.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct const evaluations and scalar access.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. CLASS DEMONSTRATING CONST MEMBER FUNCTIONS AND MUTABLE KEYWORD
// =====================================================================================

class AccountProfile {
private:
    string accountHolder_;
    double balance_;
    mutable size_t lookupCount_; // 'mutable' allows mutation even inside 'const' member functions

public:
    AccountProfile(string holder, double initialBalance)
        : accountHolder_(std::move(holder)), balance_(initialBalance), lookupCount_(0) {}

    // CONST MEMBER FUNCTION: Promises not to mutate non-mutable member variables
    [[nodiscard]] double getBalance() const {
        ++lookupCount_; // Allowed because lookupCount_ is declared 'mutable'
        return balance_;
    }

    [[nodiscard]] const string& getAccountHolder() const {
        ++lookupCount_;
        return accountHolder_;
    }

    [[nodiscard]] size_t getLookupCount() const {
        return lookupCount_;
    }

    // NON-CONST MEMBER FUNCTION: Can mutate any member variable
    void deposit(double amount) {
        if (amount > 0.0) {
            balance_ += amount;
            cout << "  - [Mutating Operation] Deposited $" << fixed << setprecision(2) << amount 
                 << " | New Balance: $" << balance_ << endl;
        }
    }
};

// =====================================================================================
// 2. CONST PARAMETER PASSING HELPERS
// =====================================================================================

// Pass by Const Reference: Zero-copy read-only access to custom class objects
void printAccountSummary(const AccountProfile& profile) {
    // profile.deposit(100.0); // COMPILER ERROR: Cannot call non-const member function on const reference!
    cout << "  [Const Ref Reader] Account: " << profile.getAccountHolder() 
         << " | Balance: $" << fixed << setprecision(2) << profile.getBalance() 
         << " (Lookups performed: " << profile.getLookupCount() << ")" << endl;
}

// =====================================================================================
// 3. CONST POINTER DEMONSTRATIONS
// =====================================================================================

void demonstrateConstPointers(int initialVal) {
    cout << "\n================ 2. CONST POINTER SEMANTICS ================" << endl;

    int targetA = initialVal;
    int targetB = initialVal * 2;

    // A. Pointer to Const Data (const T* or T const*)
    // The pointed-to data cannot be modified through this pointer, but the pointer CAN point elsewhere.
    const int* ptrToConst = &targetA;
    cout << "A. const int* (Pointer to Const Data) :" << endl;
    cout << "   *ptrToConst = " << *ptrToConst << " (Read-only data)" << endl;
    // *ptrToConst = 999; // COMPILER ERROR: Cannot modify data through const pointer
    ptrToConst = &targetB; // VALID: Pointer itself is mutable and can be reassigned
    cout << "   Reassigned ptrToConst to targetB -> *ptrToConst = " << *ptrToConst << endl;

    // B. Const Pointer to Non-Const Data (T* const)
    // The pointer cannot point elsewhere, but the pointed-to data CAN be modified.
    int* const constPtr = &targetA;
    cout << "\nB. int* const (Const Pointer to Data) :" << endl;
    cout << "   *constPtr = " << *constPtr << endl;
    *constPtr = 777; // VALID: Data can be modified
    cout << "   Modified data via constPtr -> targetA is now = " << targetA << endl;
    // constPtr = &targetB; // COMPILER ERROR: Cannot reassign a const pointer

    // C. Const Pointer to Const Data (const T* const)
    // Neither the pointer nor the data can be modified.
    const int* const constPtrToConst = &targetA;
    cout << "\nC. const int* const (Const Pointer to Const Data) :" << endl;
    cout << "   *constPtrToConst = " << *constPtrToConst << " (Neither pointer nor data can change)" << endl;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double initialDeposit = 0.0;
    string ownerName;

    // Dynamic input collection with stream flushing
    cout << "Enter account owner name (e.g., Alice): " << flush;
    if (!(cin >> ownerName) || ownerName.empty()) {
        ownerName = "Alice";
    }

    cout << "Enter initial deposit amount (e.g., 500.00): " << flush;
    if (!(cin >> initialDeposit) || initialDeposit <= 0.0) {
        cout << "Invalid deposit amount. Defaulting to 500.00." << endl;
        initialDeposit = 500.00;
    }

    // 1. PRIMITIVE CONST VARIABLES
    cout << "\n================ 1. CONST PRIMITIVE VARIABLES ================" << endl;
    const double TAX_RATE = 0.075; // Immutability enforced at compile-time
    cout << "Const TAX_RATE initialized to: " << (TAX_RATE * 100.0) << "%" << endl;
    // TAX_RATE = 0.10; // COMPILER ERROR: Assignment of read-only variable

    // 2. CONST POINTER SEMANTICS
    demonstrateConstPointers(static_cast<int>(initialDeposit / 10));

    // 3. CONST MEMBER FUNCTIONS & OBJECT CONSTNESS
    cout << "\n================ 3. CONST MEMBER FUNCTIONS & MUTABLE ================" << endl;
    
    // Mutable Account Object
    AccountProfile mutableAccount(ownerName, initialDeposit);
    mutableAccount.deposit(150.00); // Calls non-const member function
    printAccountSummary(mutableAccount);

    // Const Account Object
    cout << "\nCreating a 'const AccountProfile' object:" << endl;
    const AccountProfile constAccount("Bob (Const Object)", 1200.00);
    
    // constAccount.deposit(200.0); // COMPILER ERROR: Cannot invoke non-const member function on const object!
    cout << "Invoking const getter functions on const object:" << endl;
    cout << "  - Account Holder : " << constAccount.getAccountHolder() << endl;
    cout << "  - Balance        : $" << constAccount.getBalance() << endl;
    cout << "  - Lookup Count   : " << constAccount.getLookupCount() << " (Updated via 'mutable' keyword)" << endl;

    printAccountSummary(constAccount);

    cout << "\n================ CONST CORRECTNESS SUMMARY ================" << endl;
    cout << "1. `const T`          : Data cannot be modified after initialization." << endl;
    cout << "2. `const T*`         : Pointer can change address, but target data is read-only." << endl;
    cout << "3. `T* const`         : Pointer address is fixed, but target data can be modified." << endl;
    cout << "4. `const T* const`   : Both pointer address and target data are read-only." << endl;
    cout << "5. `void func() const`: Promises not to alter object state (`this` is const)." << endl;
    cout << "6. `mutable`          : Permits specific data member mutations inside const member functions." << endl;

    return 0;
}