/*
 * =====================================================================================
 * CONCEPT        : Dereferencing Pointers (`*` Operator) in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade implementation of pointer dereferencing:
 *                  1. Indirection / Dereference (`*`) : Accessing and mutating the value stored at a target memory address.
 *                  2. Arrow Operator (`->`)            : Member selection shorthand for `(*ptr).member`.
 *                  3. Multi-Level Dereferencing (`**`): Navigating pointer-to-pointer indirection chains.
 *                  4. Const-Correctness                : `const T*` (Read-only data) vs `T* const` (Read-only pointer address).
 *                  5. Defensive Safety Checks          : Preventing crash-inducing Null / Dangling dereferences.
 *                  6. Smart Pointer Dereferencing     : Modern RAII dereference parity (`std::unique_ptr`).
 *
 * TIME COMPLEXITY  : Dereference access & member selection: O(1) constant time.
 * SPACE COMPLEXITY : Pointer size: 8 bytes (64-bit systems) / 4 bytes (32-bit systems).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER STRUCT FOR MEMBER SELECTION DEMONSTRATION
// =====================================================================================
struct PlayerProfile {
    string username;
    int level;
    double score;

    void printStats() const {
        cout << "    [Player Profile] Name: " << username 
             << " | Level: " << level 
             << " | Score: " << score << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInitialValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value to mutate via dereferencing (e.g., 50): " << flush;
    if (!(cin >> userInitialValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 50." << endl;
        userInitialValue = 50;
    }

    // =====================================================================================
    // 1. BASIC DEREFERENCING: READING & MUTATING VALUES
    // =====================================================================================
    cout << "\n================ 1. READING & MUTATING VIA DEREFERENCING ================\n";

    int targetVar = userInitialValue;
    int* ptrToTarget = &targetVar;

    cout << "  - Original `targetVar` Value           : " << targetVar << "\n";
    cout << "  - Stored Memory Address (`ptrToTarget`) : " << static_cast<const void*>(ptrToTarget) << "\n";
    cout << "  - Read Value via Dereference (`*ptr`)  : " << *ptrToTarget << "\n";

    // Value mutation via dereference operator
    *ptrToTarget += 100;
    cout << "  - Value after `*ptrToTarget += 100`     : " << targetVar 
         << " (Mutated target memory directly!)\n";

    // =====================================================================================
    // 2. MEMBER SELECTION DEREFERENCING (`->` VS `(*ptr).`)
    // =====================================================================================
    cout << "\n================ 2. MEMBER ACCESS: ARROW OPERATOR (`->`) ================\n";

    PlayerProfile player{"CyberKnight", 42, 9850.5};
    PlayerProfile* playerPtr = &player;

    // Method A: Explicit dereference with parenthesized dot access
    cout << "  - Method A: Parenthesized `(*playerPtr).username` : " << (*playerPtr).username << "\n";

    // Method B: Preferred syntactic shorthand using arrow operator ->
    cout << "  - Method B: Arrow Operator `playerPtr->username`   : " << playerPtr->username << "\n";

    // Mutating struct members via pointer arrow operator
    playerPtr->level += 1;
    playerPtr->score += 500.0;
    cout << "  - Updated Player Stats via `playerPtr->`:\n";
    playerPtr->printStats();

    // =====================================================================================
    // 3. MULTI-LEVEL DEREFERENCING (DOUBLE POINTERS)
    // =====================================================================================
    cout << "\n================ 3. MULTI-LEVEL DEREFERENCING (`**dPtr`) ================\n";

    int baseValue = 777;
    int* singlePtr = &baseValue;
    int** doublePtr = &singlePtr;

    cout << "  - Base Value (`baseValue`)                       : " << baseValue << "\n";
    cout << "  - Single Pointer (`*singlePtr`)                 : " << *singlePtr << "\n";
    cout << "  - Single Dereference Double Ptr (`*doublePtr`)   : " << static_cast<const void*>(*doublePtr) 
         << " (Address of baseValue)\n";
    cout << "  - Double Dereference Double Ptr (`**doublePtr`)  : " << **doublePtr << "\n";

    // Mutating value through 2 layers of indirection
    **doublePtr = 999;
    cout << "  - Base Value after `**doublePtr = 999`           : " << baseValue << "\n";

    // =====================================================================================
    // 4. CONST-CORRECTNESS & DEREFERENCING PERMISSIONS
    // =====================================================================================
    cout << "\n================ 4. CONST-CORRECTNESS IN DEREFERENCING ================\n";

    int valA = 10;
    int valB = 20;

    // Case 1: Pointer to Const Data (Cannot dereference-write data, CAN rebind address)
    const int* ptrToConst = &valA;
    cout << "  - `const int*` Dereferenced Value              : " << *ptrToConst << "\n";
    // *ptrToConst = 30; // COMPILE ERROR: Data is read-only!
    ptrToConst = &valB;  // ALLOWED: Pointer address rebind is valid
    cout << "  - `const int*` Rebound to valB                 : " << *ptrToConst << "\n";

    // Case 2: Const Pointer to Non-Const Data (CAN dereference-write data, CANNOT rebind address)
    int* const constPtr = &valA;
    *constPtr = 50;      // ALLOWED: Data mutation is valid
    // constPtr = &valB; // COMPILE ERROR: Pointer address is read-only!
    cout << "  - `int* const` Value after `*constPtr = 50`     : " << valA << "\n";

    // =====================================================================================
    // 5. DEFENSIVE NULL POINTER DEREFERENCE PROTECTION
    // =====================================================================================
    cout << "\n================ 5. DEFENSIVE DEREFERENCE SAFETY ================\n";

    int* unsafeNullPtr = nullptr;

    cout << "  - [SAFETY RULE]: Dereferencing `nullptr` causes an immediate Segmentation Fault / Crash!\n";

    // Defensive Guard Pattern
    if (unsafeNullPtr != nullptr) {
        cout << "  - Value: " << *unsafeNullPtr << "\n";
    } else {
        cout << "  - [CHECK PASSED]: Nullptr check prevented illegal memory dereference!\n";
    }

    // =====================================================================================
    // 6. MODERN SMART POINTER DEREFERENCING (`std::unique_ptr`)
    // =====================================================================================
    cout << "\n================ 6. SMART POINTER DEREFERENCING ================\n";

    std::unique_ptr<PlayerProfile> smartPlayer = std::make_unique<PlayerProfile>(PlayerProfile{"ShadowNinja", 88, 15400.0});

    // Dereferencing smart pointers uses identical `*` and `->` operator syntax!
    cout << "  - Smart Pointer Dereference (`*smartPlayer`): " << (*smartPlayer).username << "\n";
    cout << "  - Smart Pointer Member Access (`smartPlayer->`):\n";
    smartPlayer->printStats();

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ DEREFERENCING OPERATORS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Syntax / Operator     | Expression Example                | Operation Performed               |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Value Dereference     | `int val = *ptr;`                 | Reads value at target address     |\n"
         << "| Dereference Write     | `*ptr = 100;`                     | Mutates value at target address   |\n"
         << "| Arrow Member Access   | `ptr->member;`                    | Shorthand for `(*ptr).member`     |\n"
         << "| Double Dereference    | `int val = **dPtr;`               | Navigates 2 levels of indirection |\n"
         << "| Pointer to Const      | `const T* ptr;`                   | `*ptr` is READ-ONLY               |\n"
         << "| Const Pointer         | `T* const ptr;`                   | `*ptr` is MUTABLE (`ptr` frozen)  |\n"
         << "| Smart Pointer         | `*uPtr` / `uPtr->`                | Overloaded RAII dereference parity|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}