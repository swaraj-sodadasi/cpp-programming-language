/*
 * =====================================================================================
 * CONCEPT        : Basics of Pointers in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade implementation covering:
 *                  1. Pointer Declaration & Syntax : Declaring pointers (`T* ptr`).
 *                  2. Address-of (`&`) & Dereference (`*`) : Retrieving addresses and accessing values.
 *                  3. Null Pointer Safety (`nullptr`)  : Modern C++ initialization & safe checks.
 *                  4. Uniform Pointer Sizes            : All raw pointer types share uniform size on CPU.
 *                  5. Re-pointing vs Value Mutation    : Changing pointer target vs modifying target value.
 *                  6. Pointers vs References Basics    : Key architectural and syntax differences.
 *
 * TIME COMPLEXITY  : Address extraction & Dereference access: O(1) constant time.
 * SPACE COMPLEXITY : Raw pointer size: 8 bytes (64-bit systems) / 4 bytes (32-bit systems).
 * =====================================================================================
 */

#include <iostream>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER FUNCTION: DEMONSTRATING POINTER VS REFERENCE AS PARAMETERS
// =====================================================================================
void updateByPointer(int* ptr) {
    if (ptr != nullptr) {
        *ptr *= 2; // Mutates caller variable via dereference
    }
}

void updateByReference(int& ref) {
    ref *= 2;  // Mutates caller variable directly via alias
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userVal = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value to inspect in memory (e.g., 42): " << flush;
    if (!(cin >> userVal)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 42." << endl;
        userVal = 42;
    }

    // =====================================================================================
    // 1. POINTER DECLARATION, ADDRESS-OF (&), AND DEREFERENCE (*)
    // =====================================================================================
    cout << "\n================ 1. POINTER DECLARATION & OPERATORS ================\n";

    int target = userVal;
    int* ptrToTarget = &target; // Store memory address of target variable

    cout << "  - Target Variable Value (`target`)     : " << target << "\n";
    cout << "  - Memory Address of Target (`&target`) : " << static_cast<const void*>(&target) << "\n";
    cout << "  - Pointer Stored Value (`ptrToTarget`)  : " << static_cast<const void*>(ptrToTarget) << "\n";
    cout << "  - Dereferenced Pointer (`*ptrToTarget`) : " << *ptrToTarget << "\n";

    // Indirect mutation through pointer dereferencing
    *ptrToTarget = 100;
    cout << "  - Value of `target` after `*ptr = 100`  : " << target << "\n";

    // =====================================================================================
    // 2. NULL POINTER SAFETY (`nullptr`)
    // =====================================================================================
    cout << "\n================ 2. NULL POINTER SAFETY (`nullptr`) ================\n";

    int* safePtr = nullptr; // Modern C++ nullptr (Replaces legacy NULL / 0)

    cout << "  - Address of `nullptr` initialized ptr  : " << static_cast<const void*>(safePtr) << "\n";

    // Safe defensive check before dereferencing
    if (safePtr != nullptr) {
        cout << "  - Pointer Value: " << *safePtr << "\n";
    } else {
        cout << "  - [SAFE]: Pointer check prevented null pointer dereference crash!\n";
    }

    // Binding pointer safely later
    safePtr = &target;
    cout << "  - Bound `safePtr` to `target` Address   : " << static_cast<const void*>(safePtr) << "\n";
    cout << "  - Dereferenced Value post-binding      : " << *safePtr << "\n";

    // =====================================================================================
    // 3. UNIFORM POINTER SIZES ACROSS TYPES
    // =====================================================================================
    cout << "\n================ 3. UNIFORM POINTER SIZES ================\n";

    int* pInt = nullptr;
    double* pDouble = nullptr;
    char* pChar = nullptr;

    cout << "  - `sizeof(int*)`    : " << sizeof(pInt) << " bytes\n";
    cout << "  - `sizeof(double*)` : " << sizeof(pDouble) << " bytes\n";
    cout << "  - `sizeof(char*)`   : " << sizeof(pChar) << " bytes\n";
    cout << "  - Architecture Note : All raw pointers have fixed size matching native architecture (" 
         << (sizeof(void*) == 8 ? "64-bit" : "32-bit") << ").\n";

    // =====================================================================================
    // 4. RE-POINTING VS DEREFERENCED VALUE MUTATION
    // =====================================================================================
    cout << "\n================ 4. RE-POINTING VS VALUE MUTATION ================\n";

    int varA = 10;
    int varB = 20;
    int* activePtr = &varA;

    cout << "  - Initially pointing to `varA`         : " << static_cast<const void*>(activePtr) 
         << " | Value: " << *activePtr << "\n";

    // Re-pointing pointer to a different memory address
    activePtr = &varB;
    cout << "  - Re-bound `activePtr` to `varB`        : " << static_cast<const void*>(activePtr) 
         << " | Value: " << *activePtr << "\n";

    // Modifying value at current target (varB)
    *activePtr = 999;
    cout << "  - Modified `*activePtr = 999`, `varB` = : " << varB << " | `varA` = " << varA << "\n";

    // =====================================================================================
    // 5. POINTERS VS REFERENCES BASICS
    // =====================================================================================
    cout << "\n================ 5. POINTERS VS REFERENCES BASICS ================\n";

    int paramValue = 50;
    cout << "  - Initial Parameter Value              : " << paramValue << "\n";

    updateByPointer(&paramValue);
    cout << "  - After `updateByPointer(&paramValue)`  : " << paramValue << "\n";

    updateByReference(paramValue);
    cout << "  - After `updateByReference(paramValue)`: " << paramValue << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ BASICS OF POINTERS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Operator    | Syntax / Expression               | Behavioral Meaning                |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Pointer Declaration   | `T* ptr = nullptr;`               | Creates variable storing address  |\n"
         << "| Address-of Operator   | `&variable`                       | Extracts memory address of object |\n"
         << "| Dereference Operator  | `*ptr`                            | Accesses value at target address  |\n"
         << "| Null Initialization   | `nullptr`                         | Safe empty address state          |\n"
         << "| Re-pointing           | `ptr = &otherVariable;`           | Changes memory address stored     |\n"
         << "| Pointer Modification  | `*ptr = newValue;`                | Mutates value at target address   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}