/*
 * =====================================================================================
 * CONCEPT        : Memory Leaks in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how memory leaks occur, how to detect them via instance tracking, 
 *                  and how to fix them using Modern C++ RAII and Smart Pointers:
 *
 *                  1. What is a Memory Leak?   : Heap memory allocated via `new`/`malloc` 
 *                                                whose pointer reference is lost without calling `delete`.
 *                  2. Cause 1: Missing Delete  : Forgetting to explicitly call `delete` on raw pointers.
 *                  3. Cause 2: Reassignment    : Overwriting a pointer target address before freeing original memory.
 *                  4. Cause 3: Exception Path  : Early returns or thrown exceptions bypassing `delete` calls.
 *                  5. Cause 4: Mismatched Delete: Using scalar `delete` on array allocations (`new[]`).
 *                  6. Modern RAII Solution     : Utilizing `std::unique_ptr` & `std::shared_ptr` for leak-free code.
 *
 * TIME COMPLEXITY  : Allocation / Access: O(1) | Array Cleanup: O(N)
 * SPACE COMPLEXITY : Heap footprint: O(N) bytes until deallocated.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <limits>
#include <stdexcept>

using namespace std;

// =====================================================================================
// HELPER CLASS WITH RUNTIME ALLOCATION TRACKING
// Tracks active instances to visually detect memory leaks at runtime.
// =====================================================================================
class TrackerResource {
private:
    int id_;

public:
    static inline int activeInstances = 0; // C++17 inline static counter

    explicit TrackerResource(int id) : id_(id) {
        ++activeInstances;
        cout << "    [ALLOCATED]  TrackerResource(ID: " << id_ 
             << ") created | Active Instances: " << activeInstances << "\n";
    }

    ~TrackerResource() {
        --activeInstances;
        cout << "    [DESTRUCTED] TrackerResource(ID: " << id_ 
             << ") destroyed | Active Instances: " << activeInstances << "\n";
    }

    [[nodiscard]] int getId() const { return id_; }
};

// Helper function demonstrating early return / exception leak
void simulateExceptionLeak(bool throwError) {
    cout << "    Executing function allocating raw pointer...\n";
    TrackerResource* rawPtr = new TrackerResource(301);

    if (throwError) {
        cout << "    [EXCEPTION THROWN] Raising std::runtime_error before `delete rawPtr`!\n";
        // CRITICAL LEAK: Function exits immediately via exception stack unwinding.
        // rawPtr stack variable is popped, but heap allocation remains un-freed!
        throw runtime_error("Simulated runtime failure");
    }

    delete rawPtr; // Reached only if no exception is thrown
}

// Helper function demonstrating RAII / Smart Pointer exception safety
void simulateExceptionSafe(bool throwError) {
    cout << "    Executing function allocating std::unique_ptr...\n";
    auto smartPtr = std::make_unique<TrackerResource>(501);

    if (throwError) {
        cout << "    [EXCEPTION THROWN] Raising std::runtime_error inside RAII scope...\n";
        // SAFE: Stack unwinding automatically invokes std::unique_ptr destructor!
        throw runtime_error("Simulated runtime failure");
    }
} // smartPtr goes out of scope and frees memory automatically HERE if no exception

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userChoice = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a test scenario integer (e.g., 1 to run memory leak inspection): " << flush;
    if (!(cin >> userChoice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting scenario integer to 1." << endl;
        userChoice = 1;
    }

    (void)userChoice; // Suppress unused-variable warning

    // =====================================================================================
    // 1. CAUSE 1: MISSING DELETE (BASIC UNRELEASED HEAP MEMORY)
    // =====================================================================================
    cout << "\n================ 1. CAUSE 1: MISSING `delete` OPERATOR ================\n";
    cout << "  - Initial Active Instances: " << TrackerResource::activeInstances << "\n";

    {
        cout << "  - Allocating raw pointer inside local block scope...\n";
        TrackerResource* leakedPtr = new TrackerResource(101);
        (void)leakedPtr; // Intentionally demonstrating lost reference
        cout << "  - Exiting block scope WITHOUT calling `delete leakedPtr`...\n";
    } // leakedPtr stack variable is destroyed HERE, but the heap object is LEAKED!

    cout << "  - [LEAK DETECTED]: Active Instances after scope exit = " 
         << TrackerResource::activeInstances << " (Resource leaked on heap!)\n";

    // =====================================================================================
    // 2. CAUSE 2: POINTER REASSIGNMENT OVERWRITE
    // =====================================================================================
    cout << "\n================ 2. CAUSE 2: POINTER REASSIGNMENT OVERWRITE ================\n";

    TrackerResource* activePtr = new TrackerResource(201);
    cout << "  - `activePtr` holds address of ID 201: " << static_cast<const void*>(activePtr) << "\n";

    cout << "  - [REASSIGNMENT]: Overwriting `activePtr = new TrackerResource(202)` without freeing ID 201...\n";
    activePtr = new TrackerResource(202); // ID 201 address is now PERMANENTLY LOST!

    cout << "  - `activePtr` now holds address of ID 202: " << static_cast<const void*>(activePtr) << "\n";
    cout << "  - [LEAK DETECTED]: Active Instances = " << TrackerResource::activeInstances 
         << " (ID 201 is orphaned on heap!)\n";

    // Cleanup ID 202
    delete activePtr;
    activePtr = nullptr;

    // =====================================================================================
    // 3. CAUSE 3: EXCEPTION PATH & EARLY RETURN LEAKS
    // =====================================================================================
    cout << "\n================ 3. CAUSE 3: EXCEPTIONS & EARLY RETURNS ================\n";

    try {
        simulateExceptionLeak(true);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - [LEAK DETECTED]: Active Instances post-exception = " 
         << TrackerResource::activeInstances << " (Raw pointer cleanup was bypassed!)\n";

    // =====================================================================================
    // 4. CAUSE 4: MISMATCHED ARRAY DEALLOCATION (`new[]` VS `delete`)
    // =====================================================================================
    cout << "\n================ 4. CAUSE 4: MISMATCHED ARRAY DEALLOCATION ================\n";

    cout << "  - Allocating array of 2 TrackerResource objects via `new[]`...\n";
    TrackerResource* resourceArray = new TrackerResource[2]{TrackerResource(401), TrackerResource(402)};

    cout << "  - Active Instances during array lifetime: " << TrackerResource::activeInstances << "\n";
    cout << "  - Deallocating array properly via `delete[]`...\n";

    // CORRECT: Must use delete[] for array allocations
    delete[] resourceArray;
    resourceArray = nullptr;

    cout << "  - Active Instances post `delete[]`: " << TrackerResource::activeInstances << " (Array cleaned correctly!)\n";

    // =====================================================================================
    // 5. MODERN C++ SOLUTION: RAII & SMART POINTERS (`std::unique_ptr`)
    // =====================================================================================
    cout << "\n================ 5. MODERN SOLUTION: RAII & SMART POINTERS ================\n";

    cout << "  - Demonstrating Exception-Safe RAII with `std::unique_ptr`...\n";
    try {
        simulateExceptionSafe(true);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - [LEAK-FREE GUARANTEE]: Active Instances post-exception = " 
         << TrackerResource::activeInstances << " (RAII automatically freed memory!)\n";

    // Demonstrating unique_ptr scope cleanup
    {
        cout << "\n  - Entering block scope with `std::unique_ptr`...\n";
        auto smartScoped = std::make_unique<TrackerResource>(601);
        cout << "    Inside scope: Active Instances = " << TrackerResource::activeInstances << "\n";
        cout << "  - Exiting block scope without manual `delete`...\n";
    } // Automatic cleanup HERE!

    cout << "  - Active Instances post scope exit = " 
         << TrackerResource::activeInstances << " (Zero leaks!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MEMORY LEAKS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Leak Cause / Pitfall  | Hazardous Code Pattern            | RAII & Modern C++ Solution        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Missing `delete`      | `T* p = new T();` (no delete)     | `auto p = std::make_unique<T>();` |\n"
         << "| Pointer Overwrite     | `p = new T(); p = new T();`       | Smart pointers manage rebind free |\n"
         << "| Exception Unwinding   | Throw before `delete p;`          | RAII destructors run automatically|\n"
         << "| Mismatched Array Free | `new T[N]` freed with `delete p`  | Use `std::vector<T>` or `unique_ptr<T[]>`|\n"
         << "| Dangling Reference    | Using freed pointer address       | Set to `nullptr` or use smart ptrs|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}