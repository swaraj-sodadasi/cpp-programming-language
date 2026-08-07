/*
 * =====================================================================================
 * CONCEPT        : Wild Pointers, Null Pointers, and Void Pointers in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the definitions, behaviors, memory mechanics, and safety rules for
 *                  three fundamental pointer categories:
 *
 *                  1. Wild Pointers  : Uninitialized pointers containing indeterminate 
 *                                      garbage addresses. Prevention via mandatory initialization.
 *                  2. Null Pointers  : `nullptr` (C++11 type-safe null literal) vs legacy `NULL`/`0`.
 *                                      Defensive checks, disarming pointers post-`delete`.
 *                  3. Void Pointers  : Type-agnostic generic pointers (`void*`). Address storage,
 *                                      dereferencing restrictions, and explicit `static_cast`.
 *                  4. Combined Safety: Generic memory buffer handlers using safe `void*` 
 *                                      and `nullptr` disarming patterns.
 *
 * TIME COMPLEXITY  : Address inspection, defensive checks, and casting: O(1) constant time.
 * SPACE COMPLEXITY : Pointer size: Uniform 8 bytes (64-bit systems) / 4 bytes (32-bit systems).
 * =====================================================================================
 */

#include <iostream>
#include <limits>
#include <string>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS FOR VOID POINTER DEMONSTRATION
// Demonstrates type-agnostic generic printing using void* and dynamic type tags
// =====================================================================================
enum class DataType {
    INT,
    DOUBLE,
    STRING
};

void printGenericData(const void* rawPtr, DataType type) {
    if (rawPtr == nullptr) {
        cout << "    [GENERIC PRINT] Error: Received nullptr address!\n";
        return;
    }

    // void* CANNOT be dereferenced directly. It MUST be cast to the specific target type first.
    switch (type) {
        case DataType::INT: {
            const auto* typedPtr = static_cast<const int*>(rawPtr);
            cout << "    [GENERIC PRINT - INT] Address: " << static_cast<const void*>(typedPtr) 
                 << " | Dereferenced Value: " << *typedPtr << "\n";
            break;
        }
        case DataType::DOUBLE: {
            const auto* typedPtr = static_cast<const double*>(rawPtr);
            cout << "    [GENERIC PRINT - DOUBLE] Address: " << static_cast<const void*>(typedPtr) 
                 << " | Dereferenced Value: " << *typedPtr << "\n";
            break;
        }
        case DataType::STRING: {
            const auto* typedPtr = static_cast<const string*>(rawPtr);
            cout << "    [GENERIC PRINT - STRING] Address: " << static_cast<const void*>(typedPtr) 
                 << " | Dereferenced Value: \"" << *typedPtr << "\"\n";
            break;
        }
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for pointer inspection (e.g., 88): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 88." << endl;
        userInputValue = 88;
    }

    // =====================================================================================
    // 1. WILD POINTERS & MANDATORY INITIALIZATION
    // =====================================================================================
    cout << "\n================ 1. WILD POINTERS & PREVENTION ================\n";

    // CONCEPT: A wild pointer is an uninitialized raw pointer that stores a garbage memory address.
    // Reading or writing through a wild pointer causes immediate Undefined Behavior (UB),
    // segmentation faults, or silent memory corruption.

    cout << "  - [HAZARD]: Uninitialized `int* wildPtr;` contains random stack memory garbage.\n";
    cout << "  - [RULE]: NEVER dereference an uninitialized pointer!\n";

    // PREVENTATIVE FIX: Always initialize pointers at declaration (either with nullptr or a valid address).
    int validTarget = userInputValue;
    int* disarmedWildPtr = nullptr;      // Fix A: Initialize to nullptr
    int* initializedPtr = &validTarget;  // Fix B: Direct binding to valid address

    cout << "  - [FIX A - Null Initialization] Address: " << static_cast<const void*>(disarmedWildPtr) << "\n";
    cout << "  - [FIX B - Direct Binding]      Address: " << static_cast<const void*>(initializedPtr) 
         << " | Value: " << *initializedPtr << "\n";

    // =====================================================================================
    // 2. NULL POINTERS (`nullptr` vs LEGACY `NULL` / `0`)
    // =====================================================================================
    cout << "\n================ 2. NULL POINTERS (`nullptr` SAFETY) ================\n";

    // C++11 std::nullptr_t type-safe null pointer literal
    int* safeNullPtr = nullptr;

    cout << "  - `nullptr` Memory Representation Address: " << static_cast<const void*>(safeNullPtr) << "\n";

    // Defensive Guard Pattern
    if (safeNullPtr != nullptr) {
        cout << "  - Value: " << *safeNullPtr << "\n";
    } else {
        cout << "  - [DEFENSIVE CHECK PASSED]: Pointer is null. Prevented illegal dereference crash!\n";
    }

    // Disarming heap pointers post-delete
    int* heapValue = new int(250);
    cout << "  - Heap Object Allocated Address          : " << static_cast<const void*>(heapValue) << "\n";

    delete heapValue;   // Returns heap memory to OS
    heapValue = nullptr; // DISARM: Converts potential dangling pointer into a safe null pointer

    cout << "  - Post-`delete` Reset Address             : " << static_cast<const void*>(heapValue) 
         << " (Safe Disarmed State)\n";

    // =====================================================================================
    // 3. VOID POINTERS (`void*` GENERIC POINTERS)
    // =====================================================================================
    cout << "\n================ 3. VOID POINTERS (`void*`) ================\n";

    int integerVar = userInputValue;
    double doubleVar = 99.95;
    string stringVar = "Modern C++ Pointers";

    // A void* can hold the address of ANY data type without an explicit cast
    void* genericPtr = &integerVar;
    cout << "  - `void*` storing `int` Address    : " << genericPtr << "\n";

    genericPtr = &doubleVar;
    cout << "  - `void*` storing `double` Address : " << genericPtr << "\n";

    genericPtr = &stringVar;
    cout << "  - `void*` storing `string` Address : " << genericPtr << "\n";

    // RESTRICTION 1: Cannot dereference void* directly -> `*genericPtr` is a COMPILE ERROR.
    // RESTRICTION 2: Cannot perform pointer arithmetic directly on void* (`genericPtr++`).

    // CORRECTION: Must use static_cast<T*> to restore original type before dereferencing
    auto* typedStringPtr = static_cast<string*>(genericPtr);
    cout << "  - Dereferenced post-`static_cast<string*>`: \"" << *typedStringPtr << "\"\n";

    // Demonstrating generic helper function with type tag
    cout << "\n  - Demonstrating generic `void*` passing into helper functions:\n";
    printGenericData(&integerVar, DataType::INT);
    printGenericData(&doubleVar, DataType::DOUBLE);
    printGenericData(&stringVar, DataType::STRING);

    // =====================================================================================
    // 4. COMBINED PRACTICAL APPLICATION: SAFE GENERIC BUFFER DISARMING
    // =====================================================================================
    cout << "\n================ 4. COMBINED SAFE MEMORY CLEANUP ================\n";

    // Allocate dynamic void buffer
    void* rawBuffer = ::operator new(sizeof(int) * 2);
    cout << "  - Allocated Raw Generic Byte Buffer Address: " << rawBuffer << "\n";

    // Free raw void buffer
    ::operator delete(rawBuffer);
    rawBuffer = nullptr; // Safe disarm

    cout << "  - Raw Buffer Freed & Disarmed Address      : " 
         << static_cast<const void*>(rawBuffer) << " (Safe Null State)\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ WILD VS NULL VS VOID POINTERS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Pointer Category      | Address State     | Dereference Safety| Primary Use & Fix Strategy        |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Wild Pointer          | Uninitialized     | ILLEGAL (Crash/UB)| Fix: Always init (`T* p = nullptr`)|\n"
         << "|                       | Garbage Address   |                   |                                   |\n"
         << "| Null Pointer          | `0x0` (`nullptr`) | Safe to check     | Represents disarmed/empty pointer |\n"
         << "|                       |                   | (`if (p != nullptr)`)| Guard before dereferencing        |\n"
         << "| Void Pointer (`void*`)| Holds address of  | Requires explicit | Type-agnostic raw memory storage  |\n"
         << "|                       | ANY data type     | `static_cast<T*>` | Common in generic C-APIs / buffers|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}