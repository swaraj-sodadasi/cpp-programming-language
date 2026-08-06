/*
 * =====================================================================================
 * CONCEPT        : Best Practices in C++ Control Flow
 * DESCRIPTION    : Comprehensive implementation showcasing modern, clean, and safe
 *                  control flow patterns in C++:
 *                  1. Guard Clauses & Early Returns : Eliminating deeply nested conditionals
 *                                                     (The "Arrow Anti-Pattern").
 *                  2. C++17 Init-Statements         : Confining variable lifetime strictly
 *                                                     to the if/switch block scope.
 *                  3. Type-Safe Enum Switches       : Exhaustive switch handling using 'enum class'
 *                                                     to ensure all states are accounted for.
 *                  4. Expressive & Safe Looping     : Preferring range-based for loops with 'const auto&'
 *                                                     to eliminate out-of-bounds indexing errors.
 *                  5. Defensive Condition Bounds    : Avoiding floating-point exact equality and
 *                                                     unsigned integer underflow in loop conditions.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear time validation and traversal over dynamic collections.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack storage for control state evaluation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include <iomanip>

using namespace std;

// SCOPED ENUM FOR TYPE-SAFE CONTROL FLOW
enum class AccountStatus {
    Active,
    Suspended,
    Closed
};

// DATA MODEL FOR DEMONSTRATION
struct UserAccount {
    string accountId;
    double balance;
    AccountStatus status;

    UserAccount(string id, double bal, AccountStatus st)
        : accountId(std::move(id)), balance(bal), status(st) {}
};

/**
 * @brief BEST PRACTICE 1: Guard Clauses & Early Returns
 * Replaces deeply nested if-else structures ("Arrow Anti-Pattern") with clear,
 * top-level validation checks that exit early.
 */
optional<double> processWithdrawal(UserAccount& account, double requestedAmount) {
    // Guard 1: Validate withdrawal amount positive
    if (requestedAmount <= 0.0) {
        cout << "  [Guard Triggered] Invalid requested amount ($" << requestedAmount << ")." << endl;
        return nullopt;
    }

    // Guard 2: Validate account operational status
    if (account.status != AccountStatus::Active) {
        cout << "  [Guard Triggered] Account " << account.accountId << " is not active." << endl;
        return nullopt;
    }

    // Guard 3: Validate sufficient funds
    if (account.balance < requestedAmount) {
        cout << "  [Guard Triggered] Insufficient funds (Balance: $" << account.balance << ")." << endl;
        return nullopt;
    }

    // Main Happy Path (Unindented, clean root level)
    account.balance -= requestedAmount;
    cout << "  [Success] Withdrew $" << requestedAmount << ". Remaining balance: $" << account.balance << endl;
    return account.balance;
}

/**
 * @brief BEST PRACTICE 2: Type-Safe Enum Switches with Exhaustive Pattern Matching
 */
void printAccountStatusInfo(AccountStatus status) {
    cout << "Account Status Category: ";
    
    // Switch on enum class without 'default' forces compiler alerts if an enum case is missed
    switch (status) {
        case AccountStatus::Active:
            cout << "ACTIVE (Full operational access granted)" << endl;
            break;
        case AccountStatus::Suspended:
            cout << "SUSPENDED (Pending compliance review)" << endl;
            break;
        case AccountStatus::Closed:
            cout << "CLOSED (Archived / Non-operational)" << endl;
            break;
    }
}

/**
 * @brief BEST PRACTICE 3: Safe Iteration & Defensive Loop Conditions
 */
void demonstrateSafeLoopingPatterns(const vector<UserAccount>& accounts) {
    cout << "\n================ 3. EXPRESSIVE & SAFE LOOPING PATTERNS ================" << endl;

    // Pattern A: Range-based for loop with const reference (No accidental copies or mutation)
    cout << "A. Range-Based for Loop (const auto& for efficiency & safety):" << endl;
    for (const auto& account : accounts) {
        cout << "  |- Account: " << account.accountId 
             << " | Balance: $" << fixed << setprecision(2) << account.balance << endl;
    }

    // Pattern B: Defensive Loop Bound (Avoiding Floating-Point Equality '!=')
    cout << "\nB. Safe Floating-Point Iteration (Using '<' bounds instead of '!='):" << endl;
    double threshold = 0.0;
    int stepCount = 0;

    // GOOD PRACTICE: Using '<' bound rather than 'threshold != 1.0' prevents infinite loops caused by precision errors
    while (threshold < 0.5) {
        cout << "  Step " << ++stepCount << " | Threshold: " << threshold << endl;
        threshold += 0.15;
    }

    // Pattern C: Safe Container Reverse Traversal (Avoiding Unsigned Underflow)
    cout << "\nC. Safe Reverse Traversal (Preventing size_t underflow):" << endl;
    if (!accounts.empty()) {
        for (size_t i = accounts.size(); i > 0; --i) {
            size_t targetIdx = i - 1; // Safely mapped index
            cout << "  Reverse Index " << targetIdx << " -> Account " << accounts[targetIdx].accountId << endl;
        }
    }
}

int main() {
    double inputAmount = 0.0;
    int statusChoice = 1;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter withdrawal amount to test guard clauses (e.g., 150.0): " << flush;
    if (!(cin >> inputAmount)) {
        cout << "Invalid input. Defaulting amount to 150.0." << endl;
        inputAmount = 150.0;
    }

    cout << "Select test account status [1: Active, 2: Suspended, 3: Closed]: " << flush;
    if (!(cin >> statusChoice) || statusChoice < 1 || statusChoice > 3) {
        cout << "Invalid status choice. Defaulting to Active (1)." << endl;
        statusChoice = 1;
    }

    auto initialStatus = static_cast<AccountStatus>(statusChoice - 1);

    // 2. DEMONSTRATING GUARD CLAUSES & EARLY RETURNS
    cout << "\n================ 1. GUARD CLAUSES & EARLY RETURNS ================" << endl;
    UserAccount primaryUser("ACC-10928", 500.00, initialStatus);

    cout << "Attempting transaction on " << primaryUser.accountId << "..." << endl;
    processWithdrawal(primaryUser, inputAmount);

    // 3. DEMONSTRATING C++17 INIT-STATEMENTS & SCOPE CONFINEMENT
    cout << "\n================ 2. C++17 SCOPED INIT-STATEMENTS ================" << endl;
    
    // C++17 if with initializer: 'result' is strictly scoped inside the if/else block
    if (auto result = processWithdrawal(primaryUser, 50.0); result.has_value()) {
        cout << "  [C++17 Scoped Check] Second withdrawal succeeded. New balance: $" 
             << result.value() << endl;
    } else {
        cout << "  [C++17 Scoped Check] Second withdrawal was rejected by guards." << endl;
    }
    // 'result' is automatically cleaned up and out of scope here!

    // C++17 switch with initializer
    switch (AccountStatus currentSt = primaryUser.status; currentSt) {
        case AccountStatus::Active:
            printAccountStatusInfo(currentSt);
            break;
        case AccountStatus::Suspended:
        case AccountStatus::Closed:
            cout << "Account requires administrative attention." << endl;
            break;
    }

    // 4. DEMONSTRATING SAFE LOOPING & EXHAUSTIVE SWITCHES
    vector<UserAccount> accountList = {
        {"ACC-1001", 1250.50, AccountStatus::Active},
        {"ACC-1002", 0.00,    AccountStatus::Closed},
        {"ACC-1003", 8900.25, AccountStatus::Active}
    };

    demonstrateSafeLoopingPatterns(accountList);

    // 5. SUMMARY OF BEST PRACTICES
    cout << "\n================ CONTROL FLOW BEST PRACTICES SUMMARY ================" << endl;
    cout << "1. Use Guard Clauses   : Check error conditions first and return early to keep main logic flat." << endl;
    cout << "2. Limit Variable Scope: Prefer C++17 'if (init; cond)' to keep variables local." << endl;
    cout << "3. Exhaustive Switches : Omit default in 'enum class' switches to let the compiler catch missing cases." << endl;
    cout << "4. Safe Loop Bounds    : Never use '!=' with floating-point counters or unsigned reverse loops." << endl;
    cout << "5. Prefer Range-For    : Use 'for (const auto& item : container)' to eliminate index-out-of-bound errors." << endl;

    return 0;
}