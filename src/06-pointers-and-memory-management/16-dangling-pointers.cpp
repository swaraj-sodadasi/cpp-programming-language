/*
 * =====================================================================================
 * CONCEPT        : Dangling Pointers in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how dangling pointers arise, their security hazards (Undefined Behavior),
 *                  and how to detect and eliminate them:
 *
 *                  1. What is a Dangling Pointer? : A pointer pointing to a memory location 
 *                                                   that has been deallocated/freed.
 *                  2. Cause 1: Explicit Deallocation: Calling `delete` without resetting to `nullptr`.
 *                  3. Cause 2: Stack Scope Exit   : Accessing addresses of destroyed local 
 *                                                   stack variables after scope exit.
 *                  4. Cause 3: Dynamic Reallocation: Iterator / Pointer invalidation when dynamic 
 *                                                   containers (`std::vector`) resize.
 *                  5. Hazards & Consequences      : Undefined Behavior (UB), silent data corruption, 
 *                                                   and Use-After-Free (UAF) security vulnerabilities.
 *                  6. Modern C++ Solutions        : Nullptr resetting, `std::unique_ptr`, and 
 *                                                   `std::weak_ptr` with `std::shared_ptr`.
 *
 * TIME COMPLEXITY  : Access / Pointer Disarming: O(1) constant time.
 * SPACE COMPLEXITY : Pointer size: Uniform 8 bytes (64-bit) / 4 bytes (32-bit).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <vector>
#include <limits>
#include <cstdint>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS DEMONSTRATING SAFE SCOPED MEMORY MANAGEMENT
// =====================================================================================

// SAFE PRACTICE: Return heap memory via RAII smart pointer instead of stack addresses
std::unique_ptr<int> createSafeSmartPointer(int initialValue) {
    return std::make_unique<int>(initialValue);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInitialVal = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value to trace pointer lifetime safety (e.g., 99): " << flush;
    if (!(cin >> userInitialVal)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting value to 99." << endl;
        userInitialVal = 99;
    }

    // =====================================================================================
    // 1. CAUSE 1: DEALLOCATION WITHOUT NULLPTR RESET (USE-AFTER-FREE)
    // =====================================================================================
    cout << "\n================ 1. CAUSE 1: POST-`delete` DANGLING POINTER ================\n";

    int* heapValue = new int(userInitialVal);

    cout << "  - Allocated Heap Memory Address   : " << static_cast<const void*>(heapValue) << "\n";
    cout << "  - Value Before Deallocation        : " << *heapValue << "\n";

    // Deallocate heap memory
    delete heapValue; // Memory is returned to OS, BUT heapValue variable STILL holds raw address!

    cout << "  - Address Stored After `delete`    : " << static_cast<const void*>(heapValue) 
         << " (DANGLING POINTER! Points to freed memory)\n";
    cout << "  - [HAZARD WARNING]: Dereferencing `*heapValue` causes Use-After-Free (UAF) Undefined Behavior!\n";

    // SOLUTION 1: Disarm raw dangling pointer immediately by assigning to nullptr
    heapValue = nullptr;

    cout << "  - [FIX 1 - Disarmed Pointer] Address: " << static_cast<const void*>(heapValue) << "\n";
    if (heapValue == nullptr) {
        cout << "    Defensive nullptr check prevented illegal Use-After-Free access.\n";
    }

    // =====================================================================================
    // 2. CAUSE 2: OUT-OF-SCOPE STACK MEMORY REFERENCE
    // =====================================================================================
    cout << "\n================ 2. CAUSE 2: OUT-OF-SCOPE STACK REFERENCE ================\n";

    uintptr_t danglingStackAddr = 0;
    {
        int tempBlockVar = 777;
        int* danglingStackPtr = &tempBlockVar;
        danglingStackAddr = reinterpret_cast<uintptr_t>(danglingStackPtr);
        cout << "  - Inside Local Block Scope         : Address = " 
             << static_cast<const void*>(danglingStackPtr) << " | Value = " << *danglingStackPtr << "\n";
    } // tempBlockVar stack frame is popped & destroyed HERE!

    cout << "  - Outside Local Block Scope        : Address = " 
         << reinterpret_cast<const void*>(danglingStackAddr) << " (DANGLING POINTER! Scope exited)\n";
    cout << "  - [HAZARD WARNING]: Stack variable no longer exists in memory.\n";

    // =====================================================================================
    // 3. CAUSE 3: DYNAMIC CONTAINER REALLOCATION (POINTER INVALIDATION)
    // =====================================================================================
    cout << "\n================ 3. CAUSE 3: DYNAMIC CONTAINER REALLOCATION ================\n";

    std::vector<int> numbers = {10, 20};
    int* ptrToFirstElement = &numbers[0];

    cout << "  - Vector Capacity before reallocation: " << numbers.capacity() << "\n";
    cout << "  - Pointer to Element [0] Address      : " << static_cast<const void*>(ptrToFirstElement) 
         << " | Value: " << *ptrToFirstElement << "\n";

    // Triggering vector re-allocation by pushing elements beyond capacity
    cout << "  - Pushing 100 elements to force vector memory buffer relocation...\n";
    for (int i = 0; i < 100; ++i) {
        numbers.push_back(i);
    }

    cout << "  - Vector Capacity after reallocation : " << numbers.capacity() << "\n";
    cout << "  - [INVALIDATION HAZARD]: `ptrToFirstElement` now points to DEALLOCATED original vector buffer!\n";

    // FIX: Re-bind pointer to new vector base address
    ptrToFirstElement = &numbers[0];
    cout << "  - [FIX]: Re-bound Pointer to New Base : " << static_cast<const void*>(ptrToFirstElement) 
         << " | Value: " << *ptrToFirstElement << "\n";

    // =====================================================================================
    // 4. MODERN SOLUTION: RAII SMART POINTERS (`std::unique_ptr`)
    // =====================================================================================
    cout << "\n================ 4. MODERN SOLUTION: RAII (`std::unique_ptr`) ================\n";

    std::unique_ptr<int> safePtr = createSafeSmartPointer(userInitialVal * 2);

    cout << "  - Safe Smart Pointer Address       : " << static_cast<const void*>(safePtr.get()) << "\n";
    cout << "  - Safe Smart Pointer Value         : " << *safePtr << "\n";
    cout << "  - [RAII ADVANTAGE]: Automatic deallocation and ownership tracking prevents dangling pointers!\n";

    // =====================================================================================
    // 5. MODERN SOLUTION: OBSERVER PATTERN (`std::weak_ptr` WITH `std::shared_ptr`)
    // =====================================================================================
    cout << "\n================ 5. OBSERVER PATTERN (`std::weak_ptr`) ================\n";

    std::weak_ptr<int> weakObserver;

    {
        std::shared_ptr<int> sharedOwner = std::make_shared<int>(500);
        weakObserver = sharedOwner; // Non-owning observer reference

        cout << "  - Inside Scope: Shared Owner Use Count = " << sharedOwner.use_count() << "\n";
        if (auto locked = weakObserver.lock()) {
            cout << "  - Observer Lock Successful            : Value = " << *locked << "\n";
        }
    } // sharedOwner is destroyed HERE!

    cout << "  - Outside Scope: Checking if observed memory was freed via `weakObserver.expired()`...\n";
    if (weakObserver.expired()) {
        cout << "  - [SAFE WEAK_PTR]: Detected target object was destroyed! Prevented dangling dereference.\n";
    }

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ DANGLING POINTERS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Cause of Dangling Ptr | Problem / Consequence             | Modern C++ Best Practice / Fix    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Post-`delete` Raw Ptr | Memory freed, address remains     | Reset to `nullptr` or use smart ptr|\n"
         << "| Out-of-Scope Stack Var| Pointing to destroyed stack frame | Do not return/store stack addrs   |\n"
         << "| Vector Re-allocation  | Buffer relocated, ptr invalidated | Re-index or re-bind ptr after push|\n"
         << "| Function Stack Return | Stored addr invalidated on return | Return `std::unique_ptr<T>` / value|\n"
         << "| Non-owning Observer   | Object deleted under observer     | Use `std::weak_ptr` with `.lock()`|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}