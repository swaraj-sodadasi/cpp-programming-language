/*
 * =====================================================================================
 * CONCEPT        : Dynamic Memory Pitfalls in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the primary anti-patterns, runtime hazards, security vulnerabilities,
 *                  and mitigations when dealing with dynamic heap memory:
 *
 *                  1. Memory Leaks              : Unreleased heap allocations and orphaned addresses.
 *                  2. Use-After-Free (UAF)      : Accessing/dereferencing deallocated pointer targets.
 *                  3. Double Free Hazards       : Invoking `delete` multiple times on the same pointer.
 *                  4. Mismatched Alloc/Dealloc  : Mixing `new[]`/`delete`, `malloc`/`delete`, `new`/`free`.
 *                  5. Heap Buffer Overflow      : Accessing out-of-bounds heap memory indices.
 *                  6. Exception Unwind Pitfalls : Bypassing cleanup code when exceptions trigger stack unwinding.
 *                  7. Modern RAII Mitigations   : `std::unique_ptr`, `std::shared_ptr`, and `std::vector`.
 *
 * TIME COMPLEXITY  : Allocation / Access / Cleanup: O(1) constant time (O(N) for array ops).
 * SPACE COMPLEXITY : Heap footprint: Dependent on allocation size N.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <vector>
#include <limits>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>

using namespace std;

// =====================================================================================
// HELPER CLASS FOR ALLOCATION TRACKING AND PITFALL DEMONSTRATION
// =====================================================================================
class HeapResource {
private:
    int id_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    explicit HeapResource(int id) : id_(id) {
        ++activeCount;
        cout << "    [ALLOCATED]  HeapResource(ID: " << id_ 
             << ") created | Active Count: " << activeCount << "\n";
    }

    ~HeapResource() {
        --activeCount;
        cout << "    [DESTRUCTED] HeapResource(ID: " << id_ 
             << ") destroyed | Active Count: " << activeCount << "\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    void performOperation() const {
        cout << "    [EXECUTE]    Resource ID: " << id_ << " executing safely.\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer ID for dynamic resource tracking (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. PITFALL 1: MEMORY LEAKS & ORPHANED HEAP ADDRESSES
    // =====================================================================================
    cout << "\n================ 1. PITFALL 1: MEMORY LEAKS & ORPHANED ADDRESSES ================\n";

    cout << "  - Initial Active Heap Resources: " << HeapResource::activeCount << "\n";

    // Scenario A: Missing delete
    {
        cout << "  - Allocating raw pointer without calling `delete`...\n";
        HeapResource* leakedPtr = new HeapResource(userInputValue);
        (void)leakedPtr; // Intentionally un-freed reference
        cout << "  - Exiting block scope without deallocation...\n";
    } // Local pointer variable goes out of scope, but heap memory is LEAKED!

    cout << "  - [LEAK DETECTED]: Active Heap Resources = " << HeapResource::activeCount 
         << " (1 resource orphaned on heap!)\n";

    // Scenario B: Pointer Reassignment Overwrite
    HeapResource* rebindPtr = new HeapResource(userInputValue + 1);
    cout << "  - `rebindPtr` initially holds ID " << rebindPtr->getId() 
         << " at address: " << static_cast<const void*>(rebindPtr) << "\n";

    cout << "  - Overwriting `rebindPtr = new HeapResource(" << userInputValue + 2 << ")` without deleting old memory...\n";
    rebindPtr = new HeapResource(userInputValue + 2); // ID userInputValue + 1 address is permanently lost!

    cout << "  - [LEAK DETECTED]: Active Heap Resources = " << HeapResource::activeCount 
         << " (Previous allocation orphaned!)\n";

    // Cleanup current active pointer
    delete rebindPtr;
    rebindPtr = nullptr;

    // =====================================================================================
    // 2. PITFALL 2: USE-AFTER-FREE (UAF) & DANGLING POINTERS
    // =====================================================================================
    cout << "\n================ 2. PITFALL 2: USE-AFTER-FREE (UAF) ================\n";

    HeapResource* uafPtr = new HeapResource(userInputValue + 10);
    cout << "  - Allocated Object Address : " << static_cast<const void*>(uafPtr) << "\n";

    // Deallocate memory
    delete uafPtr; // Memory returned to OS, but uafPtr still holds address!

    cout << "  - Memory Freed via `delete uafPtr`. Stored Address: " 
         << static_cast<const void*>(uafPtr) << " (DANGLING POINTER!)\n";
    cout << "  - [PITFALL HAZARD]: Dereferencing `uafPtr->performOperation()` causes Use-After-Free (UAF) Undefined Behavior!\n";

    // MITIGATION: Immediate nullptr disarm
    uafPtr = nullptr;
    cout << "  - [MITIGATION]: Reset pointer to `nullptr`. Defensive check: ";
    if (uafPtr == nullptr) {
        cout << "Pointer is null, prevented illegal Use-After-Free invocation.\n";
    }

    // =====================================================================================
    // 3. PITFALL 3: DOUBLE FREE HAZARDS
    // =====================================================================================
    cout << "\n================ 3. PITFALL 3: DOUBLE FREE HAZARDS ================\n";

    HeapResource* doubleFreePtr = new HeapResource(userInputValue + 20);

    cout << "  - First `delete doubleFreePtr` execution...\n";
    delete doubleFreePtr; // Valid deallocation

    cout << "  - [PITFALL HAZARD]: Executing second `delete doubleFreePtr` without resetting to nullptr causes double-free crash/corruption!\n";
    cout << "  - [SAFE PRACTICE]: In C++, calling `delete nullptr` is a guaranteed safe no-op.\n";

    doubleFreePtr = nullptr; // Resetting disarms double-free
    delete doubleFreePtr;   // Completely safe no-op!
    cout << "  - Safely executed `delete nullptr` post-reset without crash.\n";

    // =====================================================================================
    // 4. PITFALL 4: MISMATCHED ALLOCATORS & DEALLOCATORS
    // =====================================================================================
    cout << "\n================ 4. PITFALL 4: MISMATCHED ALLOCATORS / DEALLOCATORS ================\n";

    cout << "  - [MISMATCH RULE 1]: Memory allocated with `new[]` MUST be freed with `delete[]`.\n";
    cout << "    * Using scalar `delete ptr` on an array allocated via `new T[N]` skips destructors for remaining elements!\n";

    cout << "\n  - Demonstrating Correct Array Allocation & Cleanup:\n";
    HeapResource* resourceArr = new HeapResource[2]{ HeapResource(201), HeapResource(202) };

    cout << "  - Executing `delete[] resourceArr`...\n";
    delete[] resourceArr; // Destructs all N items in reverse order
    resourceArr = nullptr;

    cout << "\n  - [MISMATCH RULE 2]: NEVER mix C-style `malloc`/`free` with C++ `new`/`delete`!\n";
    cout << "    * `malloc` allocates raw bytes without invoking C++ constructors.\n";
    cout << "    * `delete` on a `malloc` pointer or `free` on a `new` pointer leads to runtime heap corruption.\n";

    // =====================================================================================
    // 5. PITFALL 5: HEAP BUFFER OVERFLOW / OUT-OF-BOUNDS ACCESS
    // =====================================================================================
    cout << "\n================ 5. PITFALL 5: HEAP BUFFER OVERFLOW ================\n";

    constexpr size_t BUF_SIZE = 3;
    int* heapBuffer = new int[BUF_SIZE]{ 10, 20, 30 };

    cout << "  - Valid Heap Buffer Indices [0..2]: ";
    for (size_t i = 0; i < BUF_SIZE; ++i) {
        cout << heapBuffer[i] << " ";
    }
    cout << "\n";

    cout << "  - [PITFALL HAZARD]: Accessing `heapBuffer[3]` causes Heap Buffer Overflow / Memory Corruption!\n";
    cout << "  - Raw dynamic pointers carry no runtime bounds checking or length metadata.\n";

    delete[] heapBuffer;
    heapBuffer = nullptr;

    // =====================================================================================
    // 6. MODERN SOLUTION: EXCEPTION-SAFE RAII & SMART POINTERS
    // =====================================================================================
    cout << "\n================ 6. MODERN SOLUTION: RAII & SMART POINTERS ================\n";

    cout << "  - Demonstrating Exception-Safe Leak-Free RAII via `std::unique_ptr`...\n";

    try {
        auto smartScoped = std::make_unique<HeapResource>(userInputValue + 50);
        smartScoped->performOperation();

        cout << "  - Simulating stack unwinding due to thrown exception...\n";
        throw runtime_error("Simulated business logic failure");

        // Any manual `delete` placed here would be bypassed!
    } catch (const exception& e) {
        cout << "  - Caught Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - [RAII GUARANTEE]: Smart pointer automatically freed heap memory during stack unwinding!\n";
    cout << "  - Current Active Heap Resources = " << HeapResource::activeCount << "\n";

    // Demonstrating standard container safety over raw dynamic arrays
    cout << "\n  - [BEST PRACTICE]: Use `std::vector<T>` for dynamic arrays (handles sizing, bounds, cleanup):\n";
    vector<int> safeVector = { 100, 200, 300 };
    safeVector.push_back(400);

    cout << "    Vector size: " << safeVector.size() 
         << " | Element at index 3: " << safeVector.at(3) << " (Bounds-checked!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DYNAMIC MEMORY PITFALLS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Memory Pitfall        | Hazardous Code Pattern            | Modern C++ Best Practice / Fix    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Memory Leak           | `T* p = new T();` (Missing delete)| Use `std::unique_ptr` / `vector`  |\n"
         << "| Pointer Overwrite     | `p = new T(); p = new T();`       | Rebind smart pointers safely      |\n"
         << "| Use-After-Free (UAF)  | Accessing pointer post `delete`   | Set to `nullptr` or use smart ptrs|\n"
         << "| Double Free           | Calling `delete p` twice          | Set pointer to `nullptr` post free|\n"
         << "| Mismatched Dealloc    | `new T[]` freed with scalar `delete`| Match `new[]`/`delete[]` or use vector|\n"
         << "| Allocator Mismatch    | `malloc` paired with `delete`     | Never mix C and C++ memory APIs   |\n"
         << "| Heap Buffer Overflow  | `p[N]` out-of-bounds array access | Use `std::vector::at()` with bounds|\n"
         << "| Unwind Leak           | Throwing exception before `delete`| RAII destructors handle auto-free |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}