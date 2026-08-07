/*
 * =====================================================================================
 * CONCEPT        : References and Pointers in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architectural interplay, syntax differences, memory layout, and
 *                  use-cases between Pointers (`T*`) and References (`T&`):
 *
 *                  1. Syntax & Binding Rules    : Mandatory initialization, nullability, 
 *                                                 and target rebinding vs alias immutability.
 *                  2. Memory Representation     : Address of pointer (`&ptr`) vs address of 
 *                                                 reference (`&ref` == `&target`).
 *                  3. Function Parameter Contract: Pass-by-pointer (`T*`), pass-by-reference (`T&`),
 *                                                 and reference-to-pointer (`T*&`).
 *                  4. Reference to Pointer      : Rebinding caller pointer variables without 
 *                                                 double pointers (`T**`).
 *                  5. Forbidden Constructs      : Pointer to reference (`T&*`), array of 
 *                                                 references (`T&[]`), and `std::reference_wrapper`.
 *                  6. Dangling Reference Hazard : Aliasing destroyed stack/heap memory locations.
 *
 * TIME COMPLEXITY  : Direct access / Dereferencing / Aliasing: O(1) constant time.
 * SPACE COMPLEXITY : Pointer: 8 bytes (64-bit) / 4 bytes (32-bit) storage variable.
 *                    Reference: Typically zero runtime overhead (compiler alias) or pointer.
 * =====================================================================================
 */

#include <iostream>
#include <functional>
#include <vector>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS FOR PARAMETER PASSING PARADIGMS
// =====================================================================================

// Paradigm 1: Pass-by-Pointer (Allows null values, requires defensive nullptr check)
void updateByPointer(int* ptr, int increment) {
    if (ptr != nullptr) {
        *ptr += increment; // Explicit dereference required
    }
}

// Paradigm 2: Pass-by-Reference (Non-nullable guarantee, clean syntax)
void updateByReference(int& ref, int increment) {
    ref += increment; // Direct syntax (no dereference operator needed)
}

// Paradigm 3: Reference-to-Pointer (Allows function to rebind caller's pointer variable)
void rebindPointerViaReference(int*& ptrRef, int* newAddress) {
    ptrRef = newAddress; // Rebinds caller's actual single pointer variable directly!
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for pointer vs reference comparison (e.g., 50): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 50." << endl;
        userInputValue = 50;
    }

    int primaryVar = userInputValue;
    int secondaryVar = 200;

    // =====================================================================================
    // 1. DECLARATION, INITIALIZATION, AND REBINDING RULES
    // =====================================================================================
    cout << "\n================ 1. DECLARATION, INITIALIZATION & REBINDING ================\n";

    // Pointer Declaration: Can be initialized to nullptr, rebindable target
    int* ptr = nullptr;
    ptr = &primaryVar; // Bound to primaryVar

    // Reference Declaration: MUST be initialized at declaration, alias binding is IMMUTABLE
    int& ref = primaryVar; // Aliases primaryVar

    cout << "  - Primary Variable `primaryVar` Value : " << primaryVar << "\n";
    cout << "  - Pointer `ptr` Value (`*ptr`)        : " << *ptr << " | Stored Addr: " << static_cast<const void*>(ptr) << "\n";
    cout << "  - Reference `ref` Value               : " << ref << " | Alias Addr : " << static_cast<const void*>(&ref) << "\n";

    // Rebinding Demonstration
    ptr = &secondaryVar; // ALLOWED: Pointer now points to secondaryVar address
    ref = secondaryVar;  // TRAP WARNING: Does NOT rebind reference! Assigns secondaryVar value into primaryVar!

    cout << "\n  - [AFTER `ptr = &secondaryVar` AND `ref = secondaryVar`]:\n";
    cout << "    * `primaryVar` (Mutated via `ref` assignment) : " << primaryVar << "\n";
    cout << "    * `secondaryVar`                              : " << secondaryVar << "\n";
    cout << "    * Pointer `ptr` Target Value (`*ptr`)        : " << *ptr << " (Points to secondaryVar)\n";
    cout << "    * Reference `ref` Value                      : " << ref << " (Still aliases primaryVar!)\n";

    // Reset primaryVar back to original input
    primaryVar = userInputValue;

    // =====================================================================================
    // 2. MEMORY FOOTPRINT & ADDRESS ANALYSIS
    // =====================================================================================
    cout << "\n================ 2. MEMORY FOOTPRINT & ADDRESS ANALYSIS ================\n";

    cout << "  - Address of Variable `&primaryVar` : " << static_cast<const void*>(&primaryVar) << "\n";
    cout << "  - Address of Reference `&ref`        : " << static_cast<const void*>(&ref) 
         << " (Identical to `&primaryVar` - Ref has no distinct address!)\n";
    cout << "  - Value inside Pointer `ptr`        : " << static_cast<const void*>(ptr) << "\n";
    cout << "  - Address of Pointer Var `&ptr`      : " << static_cast<const void*>(&ptr) 
         << " (Distinct variable location on Stack!)\n";

    cout << "\n  - `sizeof(ptr)` (Pointer variable footprint)  : " << sizeof(ptr) << " bytes\n";
    cout << "  - `sizeof(ref)` (Yields `sizeof(aliased type)`): " << sizeof(ref) << " bytes (sizeof(int))\n";

    // =====================================================================================
    // 3. FUNCTION PARAMETER CONTRACTS (`T*` vs `T&` vs `T*&`)
    // =====================================================================================
    cout << "\n================ 3. FUNCTION PARAMETER CONTRACTS ================\n";

    int targetVal = 100;
    cout << "  - Target Value Before Function Calls : " << targetVal << "\n";

    updateByPointer(&targetVal, 25);
    cout << "  - After `updateByPointer(&targetVal, 25)`   : " << targetVal << "\n";

    updateByReference(targetVal, 25);
    cout << "  - After `updateByReference(targetVal, 25)`  : " << targetVal << "\n";

    // Rebinding caller's pointer variable using Reference-to-Pointer (int*&)
    int* callerPtr = &primaryVar;
    cout << "\n  - `callerPtr` initially points to `primaryVar` Address : " << static_cast<const void*>(callerPtr) << "\n";

    rebindPointerViaReference(callerPtr, &secondaryVar);
    cout << "  - `callerPtr` post `rebindPointerViaReference` Address : " << static_cast<const void*>(callerPtr) 
         << " (Rebound directly to secondaryVar!)\n";

    // =====================================================================================
    // 4. FORBIDDEN CONSTRUCTS & `std::reference_wrapper`
    // =====================================================================================
    cout << "\n================ 4. FORBIDDEN CONSTRUCTS & REFERENCE WRAPPERS ================\n";

    // FORBIDDEN CONSTRUCT 1: Pointer to a Reference (`T&*`)
    // int&* ptrToRef = &ref; // COMPILE ERROR: References are not objects, cannot have pointers to references!

    // FORBIDDEN CONSTRUCT 2: Array of References (`T& arr[N]`)
    // int& refArr[3] = {primaryVar, secondaryVar, targetVal}; // COMPILE ERROR: Array elements must be objects!

    // SOLUTION: Use std::reference_wrapper to store rebindable/container-friendly references
    cout << "  - [FIX]: Storing references in containers using `std::reference_wrapper<int>`:\n";
    vector<reference_wrapper<int>> refVector = {primaryVar, secondaryVar, targetVal};

    for (size_t i = 0; i < refVector.size(); ++i) {
        cout << "    * Container Ref [" << i << "] Value: " << refVector[i].get() 
             << " | Address: " << static_cast<const void*>(&refVector[i].get()) << "\n";
    }

    // Mutating through reference_wrapper
    refVector[0].get() = 999;
    cout << "  - Mutated `refVector[0]` -> `primaryVar` Value is now: " << primaryVar << "\n";

    // =====================================================================================
    // 5. DANGLING REFERENCE HAZARD (HEAP DEALLOCATION ALIASING)
    // =====================================================================================
    cout << "\n================ 5. DANGLING REFERENCE HAZARDS ================\n";

    int* heapAlloc = new int(777);
    int& danglingRef = *heapAlloc; // Reference aliases heap object

    cout << "  - Heap Object Value via Reference    : " << danglingRef << "\n";

    delete heapAlloc; // Deallocating underlying heap target memory!
    heapAlloc = nullptr;

    cout << "  - [DANGLING REFERENCE HAZARD]: Underlying heap memory was freed via `delete`!\n";
    cout << "  - Accessing `danglingRef` now results in Use-After-Free Undefined Behavior (UB).\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ REFERENCES VS POINTERS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Characteristic        | Pointer (`T*`)                    | Reference (`T&`)                  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Definition            | Variable storing a memory address | Alias (nickname) to existing obj  |\n"
         << "| Initialization        | Optional (`int* p = nullptr;`)    | Mandatory (`int& r = var;`)       |\n"
         << "| Nullability           | Nullable (`nullptr`)              | Non-nullable (Guaranteed object)  |\n"
         << "| Rebindability         | Rebindable (`ptr = &otherVar;`)   | Immutable binding (Cannot rebind) |\n"
         << "| Indirection Syntax    | Explicit dereference (`*ptr`)     | Direct syntax (`ref`)             |\n"
         << "| Memory Address        | Has its own distinct memory address| Shares exact address with target  |\n"
         << "| Indirection Layers    | Multi-level allowed (`T**`, `T***`)| Single alias layer only (`T&`)    |\n"
         << "| Container Storage     | `std::vector<T*>` supported       | Use `std::reference_wrapper<T>`   |\n"
         << "| Ref-to-Pointer Combo  | `T*&` (Ref to pointer variable)   | `T&*` (Pointer to ref) = ILLEGAL  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}