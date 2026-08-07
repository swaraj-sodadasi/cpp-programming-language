/*
 * =====================================================================================
 * CONCEPT        : Common Mistakes in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the most frequent developer mistakes, anti-patterns, undefined behaviors,
 *                  and their modern C++ solutions:
 *
 *                  1. Uninitialized (Wild) Pointers : Dereferencing garbage stack memory addresses.
 *                  2. Null Pointer Dereferencing    : Accessing `nullptr` without defensive checking.
 *                  3. Memory Leaks & Overwrites     : Forgetting `delete` and orphaned allocations.
 *                  4. Use-After-Free (UAF)          : Dereferencing pointers pointing to freed memory.
 *                  5. Double Free Crashes           : Executing `delete` twice on non-null pointers.
 *                  6. Mismatched Allocators         : Mixing `new[]`/`delete`, `malloc`/`delete`.
 *                  7. Dangling Stack References     : Accessing addresses of out-of-scope stack variables.
 *                  8. Out-of-Bounds Heap Access     : Indexing beyond dynamic buffer boundaries.
 *                  9. Bypassing Delete in Exceptions: Exception unwinding leaks raw pointers.
 *                 10. Modern RAII Solutions         : `std::unique_ptr`, `std::vector`, and `nullptr` checks.
 *
 * TIME COMPLEXITY  : Allocation / Dereference / Inspection: O(1) constant time.
 * SPACE COMPLEXITY : Heap footprint: O(N) bytes depending on allocation size.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <limits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// =====================================================================================
// HELPER CLASS FOR RUNTIME OBJECT & MEMORY LEAK TRACKING
// =====================================================================================
class ResourceTracker {
private:
    int id_;
    string tag_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    ResourceTracker(int id, string tag) : id_(id), tag_(std::move(tag)) {
        ++activeCount;
        cout << "    [ALLOCATED]  ResourceTracker(ID: " << id_ << ", Tag: " << tag_ 
             << ") created | Active Instances: " << activeCount << "\n";
    }

    ~ResourceTracker() {
        --activeCount;
        cout << "    [DESTRUCTED] ResourceTracker(ID: " << id_ << ", Tag: " << tag_ 
             << ") destroyed | Active Instances: " << activeCount << "\n";
    }

    void execute() const {
        cout << "    [EXECUTE]    ResourceTracker(ID: " << id_ << ") performing work.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
};

// Helper function demonstrating exception leakage with raw pointers
void ExceptionLeakDemo(bool throwError, int id) {
    cout << "    [RAW POINTER] Allocating heap memory inside function...\n";
    ResourceTracker* rawPtr = new ResourceTracker(id, "ExceptionRawLeak");

    if (throwError) {
        cout << "    [EXCEPTION THROWN] Throwing runtime_error before `delete rawPtr`!\n";
        // MISTAKE: Exception unwinds stack frame, bypassing `delete rawPtr` -> PERMANENT LEAK!
        throw runtime_error("Simulated execution failure inside raw function");
    }

    delete rawPtr; // Reached only if no exception occurs
}

// Helper function demonstrating RAII exception safety
void ExceptionSafeRaiiDemo(bool throwError, int id) {
    cout << "    [RAII SMART PTR] Allocating make_unique inside function...\n";
    auto smartPtr = std::make_unique<ResourceTracker>(id, "RaiiSafeObject");

    if (throwError) {
        cout << "    [EXCEPTION THROWN] Throwing runtime_error inside RAII scope...\n";
        // SAFE: Stack unwinding automatically executes std::unique_ptr destructor!
        throw runtime_error("Simulated execution failure inside RAII function");
    }
} // smartPtr goes out of scope and frees heap memory HERE automatically

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer ID for common mistake analysis (e.g., 300): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting ID to 300." << endl;
        userInputValue = 300;
    }

    // =====================================================================================
    // 1. MISTAKE 1 & 2: UNINITIALIZED (WILD) POINTERS & NULL POINTER DEREFERENCING
    // =====================================================================================
    cout << "\n================ 1. UNINITIALIZED POINTERS & NULL DEREFERENCING ================\n";

    // MISTAKE 1: Uninitialized pointer contains random garbage memory address
    cout << "  - [MISTAKE 1]: `ResourceTracker* wildPtr;` holds garbage memory address.\n";
    cout << "    * Dereferencing an uninitialized wild pointer causes instant crash / Undefined Behavior.\n";
    cout << "    * FIX: Always initialize raw pointers to `nullptr` or a valid address at declaration.\n";

    ResourceTracker* safeNullPtr = nullptr; // Correct initialized disarmed state

    // MISTAKE 2: Dereferencing nullptr without defensive guard
    cout << "\n  - [MISTAKE 2]: Accessing `*safeNullPtr` without checking validity.\n";
    cout << "  - [FIX 2 - Defensive Guard Pattern]:\n";

    if (safeNullPtr != nullptr) {
        safeNullPtr->execute();
    } else {
        cout << "    Defensive nullptr check passed: Pointer is null, prevented segmentation fault!\n";
    }

    // =====================================================================================
    // 2. MISTAKE 3: MEMORY LEAKS & REASSIGNMENT OVERWRITES
    // =====================================================================================
    cout << "\n================ 2. MEMORY LEAKS & POINTER OVERWRITES ================\n";

    cout << "  - Initial Active Heap Objects: " << ResourceTracker::activeCount << "\n";

    // Mistake 3A: Forgetting to delete heap memory
    {
        cout << "  - Allocating object on heap without calling `delete`...\n";
        ResourceTracker* leakedPtr = new ResourceTracker(userInputValue, "LeakedMemory");
        (void)leakedPtr; // Intentionally un-freed reference
        cout << "  - Exiting scope without deallocation...\n";
    } // Pointer variable popped off stack, heap memory LEAKED!

    cout << "  - [LEAK CONFIRMED]: Active Objects = " << ResourceTracker::activeCount 
         << " (Resource orphaned on heap!)\n";

    // Mistake 3B: Overwriting active pointer address before freeing target
    ResourceTracker* activePtr = new ResourceTracker(userInputValue + 1, "FirstTarget");
    cout << "  - `activePtr` holds address of FirstTarget: " << static_cast<const void*>(activePtr) << "\n";

    cout << "  - [OVERWRITE MISTAKE]: `activePtr = new ResourceTracker(" << userInputValue + 2 
         << ")` without freeing FirstTarget...\n";
    activePtr = new ResourceTracker(userInputValue + 2, "SecondTarget"); // FirstTarget address LOST!

    cout << "  - [LEAK CONFIRMED]: Active Objects = " << ResourceTracker::activeCount 
         << " (FirstTarget orphaned on heap!)\n";

    delete activePtr; // Deallocates SecondTarget
    activePtr = nullptr;

    // =====================================================================================
    // 3. MISTAKE 4 & 5: USE-AFTER-FREE (UAF) & DOUBLE FREE ERRORS
    // =====================================================================================
    cout << "\n================ 3. USE-AFTER-FREE (UAF) & DOUBLE FREE ERRORS ================\n";

    ResourceTracker* uafPtr = new ResourceTracker(userInputValue + 10, "UafTarget");
    cout << "  - Allocated Memory Address : " << static_cast<const void*>(uafPtr) << "\n";

    delete uafPtr; // Memory returned to OS, BUT `uafPtr` variable STILL holds address!

    cout << "  - Executed `delete uafPtr`. Stored Address: " 
         << static_cast<const void*>(uafPtr) << " (DANGLING POINTER!)\n";
    cout << "  - [MISTAKE 4 - Use-After-Free]: Dereferencing `uafPtr->execute()` causes UAF vulnerability!\n";

    cout << "  - [MISTAKE 5 - Double Free]: Executing `delete uafPtr` again causes double-free crash!\n";

    // FIX: Always reset pointers to nullptr immediately after calling delete
    uafPtr = nullptr; // DISARM POINTER

    cout << "  - [FIX]: Reset pointer to `nullptr` post-delete.\n";
    delete uafPtr; // In C++, calling `delete nullptr` is guaranteed safe no-op!
    cout << "    Safely executed `delete nullptr` without double-free crash.\n";

    // =====================================================================================
    // 4. MISTAKE 6: MISMATCHED ALLOCATORS & DEALLOCATORS
    // =====================================================================================
    cout << "\n================ 4. MISMATCHED ALLOCATION / DEALLOCATION ================\n";

    cout << "  - [MISTAKE 6A]: Allocating dynamic array via `new T[N]` but freeing via scalar `delete p`.\n";
    cout << "    * Consequence: Skips destructors for elements 1 to N-1 and corrupts heap metadata!\n";

    cout << "\n  - [FIX 6A]: Always pair `new[]` with `delete[]`:\n";
    ResourceTracker* arrayBuffer = new ResourceTracker[2]{ 
        ResourceTracker(401, "Array1"), 
        ResourceTracker(402, "Array2") 
    };

    delete[] arrayBuffer; // Destructs all elements correctly
    arrayBuffer = nullptr;

    cout << "\n  - [MISTAKE 6B]: Pairing C `malloc`/`free` with C++ `new`/`delete`.\n";
    cout << "    * `malloc` does NOT invoke C++ constructors.\n";
    cout << "    * Calling `delete` on `malloc` memory or `free` on `new` memory causes undefined behavior.\n";

    // =====================================================================================
    // 5. MISTAKE 7 & 8: OUT-OF-SCOPE STACK MEMORY & HEAP BUFFER OVERFLOW
    // =====================================================================================
    cout << "\n================ 5. DANGLING STACK REFERENCES & HEAP OVERFLOW ================\n";

    // Mistake 7: Dangling pointer to out-of-scope stack variable
    uintptr_t danglingStackAddr = 0;
    {
        int localScopedVar = 999;
        int* danglingStackPtr = &localScopedVar;
        danglingStackAddr = reinterpret_cast<uintptr_t>(danglingStackPtr);
        cout << "  - Inside Block Scope: `localScopedVar` Addr = " 
             << static_cast<const void*>(danglingStackPtr) << " | Value = " << *danglingStackPtr << "\n";
    } // `localScopedVar` stack frame popped HERE!

    cout << "  - Outside Block Scope: Address stored = " 
         << reinterpret_cast<const void*>(danglingStackAddr) << " (DANGLING POINTER! Stack scope exited)\n";

    // Mistake 8: Heap Buffer Overflow
    constexpr size_t HEAP_SIZE = 3;
    int* heapBuffer = new int[HEAP_SIZE]{ 10, 20, 30 };

    cout << "\n  - Heap Buffer Valid Range [0..2]: ";
    for (size_t i = 0; i < HEAP_SIZE; ++i) {
        cout << heapBuffer[i] << " ";
    }
    cout << "\n";

    cout << "  - [MISTAKE 8]: Accessing `heapBuffer[3]` causes Heap Buffer Overflow / Memory Corruption!\n";
    cout << "  - [FIX]: Use `std::vector<T>` with bounds checking (`vec.at(index)`).\n";

    delete[] heapBuffer;
    heapBuffer = nullptr;

    // =====================================================================================
    // 6. MISTAKE 9 vs MODERN FIX: EXCEPTION LEAKS & RAII SAFETY
    // =====================================================================================
    cout << "\n================ 6. EXCEPTION UNWINDING LEAKS VS RAII FIX ================\n";

    // Mistake 9 Demonstration: Exception unwinding bypasses manual delete
    try {
        ExceptionLeakDemo(true, userInputValue + 50);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }
    cout << "  - Active Objects post raw exception leak = " << ResourceTracker::activeCount 
         << " (Raw pointer memory leaked!)\n";

    // Modern C++ Fix: std::unique_ptr guarantees cleanup during unwinding
    cout << "\n  - Executing ExceptionSafeRaiiDemo with `std::unique_ptr`...\n";
    try {
        ExceptionSafeRaiiDemo(true, userInputValue + 60);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }
    cout << "  - [RAII GUARANTEE]: Active Objects post exception = " << ResourceTracker::activeCount 
         << " (Smart pointer auto-freed heap memory!)\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ COMMON MISTAKES & FIXES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Common Mistake        | Hazardous Code Pattern            | Modern C++ Best Practice / Fix    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Wild Pointer          | `T* p;` (Uninitialized garbage)   | Initialize to `nullptr` or addr   |\n"
         << "| Null Pointer Crash    | Dereferencing `p` without check   | Defensive `if (p != nullptr)` check|\n"
         << "| Memory Leak           | Missing `delete` / Reassigning p  | Use `std::unique_ptr` / `vector`  |\n"
         << "| Use-After-Free (UAF)  | Accessing pointer post `delete`   | Set `p = nullptr;` immediately    |\n"
         << "| Double Free Crash     | Executing `delete p` twice        | Reset `p = nullptr` (`delete null` ok)|\n"
         << "| Mismatched Array Free | `new T[]` freed with `delete p`   | Match `new[]`/`delete[]` or vector|\n"
         << "| Allocator Mismatch    | `malloc` paired with `delete`     | Never mix C and C++ memory APIs   |\n"
         << "| Stack Pointer Dangling| Storing address of local scope var| Return values or `std::unique_ptr`|\n"
         << "| Heap Buffer Overflow  | `p[N]` out-of-bounds array access | Use `std::vector::at()` with bounds|\n"
         << "| Exception Unwind Leak | Throwing error before `delete`    | RAII destructors auto-free memory |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}