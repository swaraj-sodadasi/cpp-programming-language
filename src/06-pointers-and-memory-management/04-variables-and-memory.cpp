/*
 * =====================================================================================
 * CONCEPT        : Variables and Memory in Pointers and Memory Management
 * DESCRIPTION    : Comprehensive guide and implementation analyzing C++ variables, 
 *                  their memory locations, lifetime scope, and pointer binding:
 *                  1. Variable Storage Regions : Stack, Heap, Static/Data segment variables.
 *                  2. Pointer Variables        : Storing memory addresses of other variables.
 *                  3. Pointer Rebinding        : Re-pointing pointers to different variables vs 
 *                                                 reference immutability.
 *                  4. Variable Modification    : Direct mutation vs Indirect pointer mutation.
 *                  5. Memory Footprint         : Alignment, size inspection (`sizeof`), 
 *                                                 and variable lifetime scope.
 *                  6. Modern Variable Safety   : Smart pointer variable management (`std::unique_ptr`).
 *
 * TIME COMPLEXITY  : Variable Access / Indirect Access: O(1)
 * SPACE COMPLEXITY : Stack allocations: O(1) | Heap allocations: O(1) system overhead
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <memory>
#include <limits>

using namespace std;

// Global / Static Variable (Data Segment)
int g_globalVar = 500;

// Helper functions demonstrating variable pass-by-value vs pass-by-pointer
void modifyValueCopy(int varCopy) {
    varCopy += 100; // Modifies local stack frame copy only
}

void modifyViaPointer(int* varPtr) {
    if (varPtr != nullptr) {
        *varPtr += 100; // Modifies caller's original variable in memory
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInitialValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value for variable memory inspection (e.g., 25): " << flush;
    if (!(cin >> userInitialValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 25." << endl;
        userInitialValue = 25;
    }

    // =====================================================================================
    // 1. STACK VARIABLES & MEMORY ADDRESS INSPECTION
    // =====================================================================================
    cout << "\n================ 1. STACK VARIABLES & MEMORY ADDRESSES ================\n";

    int firstVar = userInitialValue;
    int secondVar = 1000;

    cout << "  - `firstVar`  | Value: " << setw(5) << firstVar 
         << " | Memory Address: " << static_cast<const void*>(&firstVar) 
         << " | Size: " << sizeof(firstVar) << " bytes\n";
    cout << "  - `secondVar` | Value: " << setw(5) << secondVar 
         << " | Memory Address: " << static_cast<const void*>(&secondVar) 
         << " | Size: " << sizeof(secondVar) << " bytes\n";

    // =====================================================================================
    // 2. POINTER VARIABLES & REBINDING
    // =====================================================================================
    cout << "\n================ 2. POINTER VARIABLES & REBINDING ================\n";

    // Pointer variable storing the address of firstVar
    int* ptrVar = &firstVar;

    cout << "  - Pointer `ptrVar` initial target       : `firstVar` (Address: " 
         << static_cast<const void*>(ptrVar) << " | Value: " << *ptrVar << ")\n";

    // Rebinding pointer variable to secondVar
    ptrVar = &secondVar;

    cout << "  - Pointer `ptrVar` re-bound target      : `secondVar` (Address: " 
         << static_cast<const void*>(ptrVar) << " | Value: " << *ptrVar << ")\n";

    // Reference contrast (Reference binding is immutable)
    int& refVar = firstVar;
    cout << "  - Reference `refVar` bound to `firstVar`: Address: " 
         << static_cast<const void*>(&refVar) << " | Value: " << refVar << "\n";

    // =====================================================================================
    // 3. VARIABLE LIFETIMES ACROSS MEMORY SEGMENTS
    // =====================================================================================
    cout << "\n================ 3. VARIABLE LIFETIMES & SEGMENT LOCATIONS ================\n";

    static int staticVar = 250;       // Persistent lifetime in Data Segment
    int stackScopedVar = 50;          // Stack lifetime (Auto destroyed at scope exit)
    int* heapAllocVar = new int(750); // Heap lifetime (Manual or RAII controlled)

    cout << "  - Global Data Segment Var (`g_globalVar`)   Addr: " 
         << static_cast<const void*>(&g_globalVar) << " | Value: " << g_globalVar << "\n";
    cout << "  - Static Data Segment Var (`staticVar`)     Addr: " 
         << static_cast<const void*>(&staticVar) << " | Value: " << staticVar << "\n";
    cout << "  - Local Stack Frame Var (`stackScopedVar`) Addr: " 
         << static_cast<const void*>(&stackScopedVar) << " | Value: " << stackScopedVar << "\n";
    cout << "  - Dynamic Heap Buffer Var (`heapAllocVar`) Addr: " 
         << static_cast<const void*>(heapAllocVar) << " | Value: " << *heapAllocVar << "\n";

    delete heapAllocVar;
    heapAllocVar = nullptr;

    // =====================================================================================
    // 4. VALUE COPYING VS INDIRECT POINTER MUTATION
    // =====================================================================================
    cout << "\n================ 4. VALUE COPYING VS INDIRECT POINTER MUTATION ================\n";

    int targetValue = 50;
    cout << "  - Target Variable Before Calls : " << targetValue << "\n";

    modifyValueCopy(targetValue);
    cout << "  - After `modifyValueCopy()`    : " << targetValue << " (Unchanged - local copy modified)\n";

    modifyViaPointer(&targetValue);
    cout << "  - After `modifyViaPointer()`   : " << targetValue << " (Mutated in-place via memory address)\n";

    // =====================================================================================
    // 5. VARIABLE SCOPE & DANGLING POINTER HAZARD
    // =====================================================================================
    cout << "\n================ 5. VARIABLE SCOPE & DANGLING POINTER PREVENTION ================\n";

    int* danglingPtr = nullptr;
    {
        int scopedVar = 999;
        danglingPtr = &scopedVar;
        cout << "  - Inside Scope: `scopedVar` value = " << *danglingPtr << "\n";
    } // `scopedVar` stack memory becomes invalid here!

    cout << "  - [SCOPING HAZARD]: `danglingPtr` now references out-of-scope stack variable memory!\n";
    danglingPtr = nullptr; // Reset pointer to safe state
    cout << "  - [FIX]: Pointer disarmed by assigning to `nullptr`.\n";

    // =====================================================================================
    // 6. MODERN RAII VARIABLE MANAGEMENT (`std::unique_ptr`)
    // =====================================================================================
    cout << "\n================ 6. MODERN SMART POINTER VARIABLE MANAGEMENT ================\n";

    std::unique_ptr<int> smartVar = std::make_unique<int>(12345);

    cout << "  - Smart Pointer Managed Variable Value  : " << *smartVar << "\n";
    cout << "  - Managed Heap Variable Address          : " << static_cast<const void*>(smartVar.get()) << "\n";
    cout << "  - [RAII BENEFITS]: Automatic heap variable cleanup when `smartVar` leaves scope.\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ VARIABLES & MEMORY SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Variable Category     | Memory Segment    | Lifetime Duration | Pointer / Binding Traits          |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Global / Static Var   | Data / BSS        | Entire Program    | Constant address throughout run   |\n"
         << "| Local Stack Var       | Stack Frame       | Enclosing Scope   | Rebindable pointers can point to it|\n"
         << "| Dynamic Heap Var      | Heap Buffer       | `new` to `delete` | Accessed exclusively via pointers |\n"
         << "| Reference Variable    | Stack / Alias     | Enclosing Scope   | Immutable binding (Cannot rebind) |\n"
         << "| Smart Pointer Var     | Stack + Heap      | RAII Scope        | Safe ownership & automatic cleanup|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}