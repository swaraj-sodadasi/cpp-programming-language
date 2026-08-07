/*
 * =====================================================================================
 * CONCEPT        : Const and Pointers in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  const-correctness, qualifier placements, and memory mutation rules:
 *
 *                  1. Pointer to Const Data (`const T*`) : Read-only target data, rebindable address.
 *                  2. Const Pointer to Data (`T* const`) : Mutable target data, read-only frozen address.
 *                  3. Const Pointer to Const Data        : Read-only data AND read-only frozen address.
 *                  4. Const with Double Pointers         : Multi-level const qualifiers (`const T**`, 
 *                                                         `T* const*`, `const T* const* const`).
 *                  5. Const in Dynamic Memory Allocation : Allocating const heap objects (`new const T`).
 *                  6. Const with Smart Pointers          : `unique_ptr<const T>` vs `const unique_ptr<T>`.
 *
 * TIME COMPLEXITY  : Reading / Modifying / Rebinding: O(1) constant time.
 * SPACE COMPLEXITY : Pointer size: Uniform 8 bytes (64-bit) / 4 bytes (32-bit).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS DEMONSTRATING CONST PARAMETER CONTRACTS
// =====================================================================================

// Parameter contract 1: Read-only data access guarantee for caller
void readOnlyInspector(const int* ptr) {
    if (ptr != nullptr) {
        cout << "    [READ-ONLY FUNCTION] Inspected Value: " << *ptr << "\n";
        // *ptr = 999; // COMPILE ERROR: Cannot modify value through pointer-to-const!
    }
}

// Parameter contract 2: Mutable data, but function local pointer cannot be rebound internally
void lockedAddressModifier(int* const ptr, int newValue) {
    if (ptr != nullptr) {
        *ptr = newValue; // ALLOWED: Target data is mutable
        // int dummy = 0;
        // ptr = &dummy; // COMPILE ERROR: Cannot rebind const pointer address!
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInitialVal = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value for const pointer analysis (e.g., 42): " << flush;
    if (!(cin >> userInitialVal)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting value to 42." << endl;
        userInitialVal = 42;
    }

    int primaryVar = userInitialVal;
    int secondaryVar = 100;

    // =====================================================================================
    // 1. POINTER TO CONSTANT DATA (`const T*` or `T const*`)
    // Read the rule right-to-left: "ptrToConst is a pointer to an int that is const"
    // =====================================================================================
    cout << "\n================ 1. POINTER TO CONSTANT DATA (`const T*`) ================\n";

    const int* ptrToConst = &primaryVar;

    cout << "  - Initial Target Address : " << static_cast<const void*>(ptrToConst) 
         << " | Value: " << *ptrToConst << "\n";

    // Data mutation via ptrToConst is forbidden
    // *ptrToConst = 99; // COMPILE ERROR: Data is read-only through this pointer!

    // Direct mutation of the underlying variable is still allowed if variable itself isn't const
    primaryVar = 88;
    cout << "  - Value after direct variable mutation (`primaryVar = 88`): " << *ptrToConst << "\n";

    // Pointer address rebinding IS allowed
    ptrToConst = &secondaryVar;
    cout << "  - Rebound `ptrToConst` Address to `secondaryVar`: " << static_cast<const void*>(ptrToConst) 
         << " | Value: " << *ptrToConst << "\n";

    // =====================================================================================
    // 2. CONSTANT POINTER TO MUTABLE DATA (`T* const`)
    // Read right-to-left: "constPtr is a const pointer to an int"
    // =====================================================================================
    cout << "\n================ 2. CONSTANT POINTER TO DATA (`T* const`) ================\n";

    int* const constPtr = &primaryVar;

    cout << "  - Frozen Address Stored (`constPtr`): " << static_cast<const void*>(constPtr) 
         << " | Value: " << *constPtr << "\n";

    // Data mutation through constPtr IS allowed
    *constPtr = 500;
    cout << "  - Value after `*constPtr = 500`       : " << primaryVar << "\n";

    // Pointer address rebinding is forbidden
    // constPtr = &secondaryVar; // COMPILE ERROR: Pointer address is immutable!

    // =====================================================================================
    // 3. CONSTANT POINTER TO CONSTANT DATA (`const T* const`)
    // Read right-to-left: "fullyConst is a const pointer to an int that is const"
    // =====================================================================================
    cout << "\n================ 3. CONSTANT POINTER TO CONSTANT DATA ================\n";

    const int* const fullyConst = &primaryVar;

    cout << "  - Fully Const Pointer Address : " << static_cast<const void*>(fullyConst) 
         << " | Value: " << *fullyConst << "\n";

    // Both operations below are strictly forbidden:
    // *fullyConst = 999;          // COMPILE ERROR: Cannot mutate value!
    // fullyConst = &secondaryVar; // COMPILE ERROR: Cannot rebind address!

    // =====================================================================================
    // 4. CONST WITH DOUBLE POINTERS (`const T**`, `T* const*`, `const T* const* const`)
    // =====================================================================================
    cout << "\n================ 4. CONST WITH MULTI-LEVEL INDIRECTION ================\n";

    int valA = 10, valB = 20;
    int* pA = &valA;
    int* pB = &valB;

    cout << "  - Pointer pA Target: " << *pA << " | Pointer pB Target: " << *pB << "\n";

    // A. Pointer to pointer to const data (`const int**`)
    const int valConst = 30;
    const int* pConst = &valConst;
    const int** pToPConst = &pConst;
    cout << "  - `const int**` Dereferenced Value (**pToPConst) : " << **pToPConst << "\n";

    // B. Pointer to const pointer to data (`int* const*`)
    int* const constPA = pA; // Uses pA
    int* const* pToConstPtr = &constPA;
    cout << "  - `int* const*` Target Value (**pToConstPtr)    : " << **pToConstPtr << "\n";

    // C. Fully Const Double Pointer (`const int* const* const`)
    const int* const* const ultraConst = &pConst;
    cout << "  - `const int* const* const` Target Value         : " << ***(&ultraConst) << "\n";

    // Demonstrate pB usage
    pB = &valA;
    cout << "  - Rebound pB to valA Target Value                 : " << *pB << "\n";

    // =====================================================================================
    // 5. CONST IN DYNAMIC MEMORY ALLOCATION (`new const T`)
    // =====================================================================================
    cout << "\n================ 5. CONST IN DYNAMIC HEAP ALLOCATION ================\n";

    // Allocating an immutable object directly on the heap
    const int* heapConst = new const int(777);

    cout << "  - Allocated Dynamic Const Heap Value   : " << *heapConst 
         << " | Address: " << static_cast<const void*>(heapConst) << "\n";

    // Deallocating const heap memory is fully valid in C++
    delete heapConst;
    heapConst = nullptr;

    // =====================================================================================
    // 6. CONST-CORRECTNESS WITH SMART POINTERS (`std::unique_ptr`)
    // =====================================================================================
    cout << "\n================ 6. CONST WITH SMART POINTERS ================\n";

    // Case A: std::unique_ptr<const int> -> Equivalent to `const T*` (Data is const, pointer is movable)
    std::unique_ptr<const int> smartDataConst = std::make_unique<const int>(123);
    cout << "  - `unique_ptr<const T>` Value          : " << *smartDataConst << "\n";
    // *smartDataConst = 456; // COMPILE ERROR: Managed data is read-only!

    // Case B: const std::unique_ptr<int> -> Equivalent to `T* const` (Pointer is frozen, data is mutable)
    const std::unique_ptr<int> smartPtrConst = std::make_unique<int>(456);
    *smartPtrConst = 789; // ALLOWED: Managed data can be mutated
    cout << "  - `const unique_ptr<T>` Mutated Value  : " << *smartPtrConst << "\n";
    // smartPtrConst.reset(); // COMPILE ERROR: Const smart pointer cannot be rebound or reset!

    // Demonstrating helper functions
    cout << "\n  - Invoking helper functions demonstrating parameter const guarantees:\n";
    readOnlyInspector(&primaryVar);
    lockedAddressModifier(&primaryVar, 999);
    cout << "    Primary Var Value post `lockedAddressModifier`: " << primaryVar << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ CONST AND POINTERS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Pointer Const Form    | Syntax Example    | Can Mutate Data?  | Can Rebind Pointer Address?       |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Pointer to Const Data | `const T* ptr`    | NO (Read-Only)    | YES (`ptr = &otherVar`)           |\n"
         << "| Const Pointer to Data | `T* const ptr`    | YES (`*ptr = val`)| NO (Frozen Address)               |\n"
         << "| Const Ptr to Const    | `const T* const p`| NO (Read-Only)    | NO (Frozen Address)               |\n"
         << "| Dynamic Heap Const    | `new const T(val)`| NO (Read-Only)    | Requires `delete` on `const T*`   |\n"
         << "| Smart Ptr Const Data  | `unique_ptr<const>`| NO (Read-Only)   | YES (`uPtr = std::move(other)`)   |\n"
         << "| Const Smart Pointer   | `const unique_ptr`| YES (`*uPtr = x`) | NO (Cannot reset/move smart ptr)  |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}