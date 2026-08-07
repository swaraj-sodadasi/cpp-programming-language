/*
 * =====================================================================================
 * CONCEPT        : Common Mistakes in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the most prevalent anti-patterns, pitfalls, and bugs developers commit 
 *                  when attempting to implement or utilize the Resource Acquisition Is 
 *                  Initialization (RAII) idiom—along with their canonical Modern C++ fixes:
 *
 *                  1. Anonymous Temporary Lock Hazard : Creating un-named temporary RAII objects 
 *                                                       that destroy immediately at statement end.
 *                  2. Throwing inside Destructors     : Allowing exceptions to escape destructors, 
 *                                                       triggering `std::terminate()` during stack unwinding.
 *                  3. Shallow Copies & Double-Free   : Failing to delete copy operations on custom handles, 
 *                                                       leading to duplicate deallocations and crashes.
 *                  4. Outliving RAII Lifetime        : Storing raw pointers extracted via `.get()` 
 *                                                       beyond the parent RAII object's scope boundary.
 *                  5. Bypassed Manual Cleanup        : Relying on manual cleanup calls that get skipped 
 *                                                       by early returns or runtime exceptions.
 *                  6. Cyclic Reference Memory Leaks  : Creating `std::shared_ptr` cycles that prevent 
 *                                                       reference counts from ever reaching zero.
 *                  7. Mismatched Allocation Dealloc  : Pairing `new[]` with `delete` or `malloc` with 
 *                                                       `delete` in custom RAII deleters.
 *
 * TIME COMPLEXITY  : Acquisition / Release / Move / Access: O(1) constant time.
 * SPACE COMPLEXITY : RAII Wrapper Footprint: Uniform 8-16 bytes (raw pointer / handle size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <mutex>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS: TRACKED RESOURCE FOR LIFECYCLE MONITORING
// =====================================================================================
class PitfallTrackerNode {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    PitfallTrackerNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE ALLOCATED] ID: " << setw(3) << id_ << " (" << setw(22) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    ~PitfallTrackerNode() noexcept {
        --activeInstances;
        cout << "    [RESOURCE DEALLOCATED] ID: " << setw(3) << id_ << " (" << setw(22) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    void execute() const {
        cout << "    [PAYLOAD EXECUTED]   ID: " << id_ << " (" << label_ << ") performing work.\n";
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
    [[nodiscard]] const string& getLabel() const noexcept { return label_; }
};

// =====================================================================================
// MISTAKE 3 DEMONSTRATION: CUSTOM HANDLE FAILING TO DELETE COPY OPERATIONS
// Anti-Pattern: Missing Rule of 5 copy suppression -> Causes double-free crashes!
// =====================================================================================
class DangerousShallowHandle {
private:
    PitfallTrackerNode* res_;

public:
    explicit DangerousShallowHandle(PitfallTrackerNode* res) : res_(res) {}
    
    ~DangerousShallowHandle() noexcept {
        delete res_; // DANGEROUS if shallow copied: Deletes same address twice!
    }

    // MISTAKE: Compiler generates default copy constructor performing memberwise pointer copy!
    // FIX: ScopedUniqueHandle explicitly deletes copy operations:
};

class CorrectMoveOnlyHandle {
private:
    PitfallTrackerNode* res_;

public:
    explicit CorrectMoveOnlyHandle(PitfallTrackerNode* res = nullptr) noexcept : res_(res) {}

    ~CorrectMoveOnlyHandle() noexcept {
        delete res_;
    }

    // FIX: Explicitly delete copy constructor and copy assignment
    CorrectMoveOnlyHandle(const CorrectMoveOnlyHandle&) = delete;
    CorrectMoveOnlyHandle& operator=(const CorrectMoveOnlyHandle&) = delete;

    // FIX: Enable safe move semantics
    CorrectMoveOnlyHandle(CorrectMoveOnlyHandle&& other) noexcept : res_(other.res_) {
        other.res_ = nullptr; // Disarm source handle
    }

    CorrectMoveOnlyHandle& operator=(CorrectMoveOnlyHandle&& other) noexcept {
        if (this != &other) {
            delete res_;
            res_ = other.res_;
            other.res_ = nullptr;
        }
        return *this;
    }

    [[nodiscard]] PitfallTrackerNode* get() const noexcept { return res_; }
    PitfallTrackerNode* operator->() const noexcept { return res_; }
};

// =====================================================================================
// MISTAKE 6 DEMONSTRATION: SHARED POINTER CYCLIC REFERENCES
// =====================================================================================
struct CyclicNodeBad {
    int id;
    std::shared_ptr<CyclicNodeBad> neighbor; // MISTAKE: Strong reference cycle prevents deletion!

    explicit CyclicNodeBad(int i) : id(i) {
        cout << "    [CyclicNodeBad Ctor]  Created Node " << id << "\n";
    }
    ~CyclicNodeBad() {
        cout << "    [CyclicNodeBad Dtor]  Destroyed Node " << id << "\n";
    }
};

struct CyclicNodeGood {
    int id;
    std::weak_ptr<CyclicNodeGood> neighbor; // FIX: Weak reference breaks cycle!

    explicit CyclicNodeGood(int i) : id(i) {
        cout << "    [CyclicNodeGood Ctor] Created Node " << id << "\n";
    }
    ~CyclicNodeGood() {
        cout << "    [CyclicNodeGood Dtor] Destroyed Node " << id << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for common RAII mistakes analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // MISTAKE 1: ANONYMOUS TEMPORARY SCOPE LOCK (THE "INVISIBLE BUG")
    // =====================================================================================
    cout << "\n================ 1. MISTAKE: ANONYMOUS TEMPORARY RAII OBJECT ================\n";

    mutex resourceMutex;

    {
        cout << "  - Attempting to lock mutex using unnamed temporary...\n";
        
        // MISTAKE: Missing variable name! Creates an anonymous temporary lock that 
        // constructs and IMMEDIATELY DESTRUCTS on this exact line, leaving critical region UNPROTECTED!
        // std::lock_guard<mutex>(resourceMutex); // MISTAKE!

        // FIX: Name the lock variable to bind its lifetime to the current block scope `{ ... }`:
        std::lock_guard<mutex> lock(resourceMutex); // CORRECT FIX

        cout << "    [CORRECT FIX] Lock variable `lock` bound to block scope. Critical section protected.\n";
        cout << "  - Leaving critical section block scope...\n";
    } // Mutex unlocked automatically HERE!

    // =====================================================================================
    // MISTAKE 2: THROWING EXCEPTIONS INSIDE DESTRUCTORS
    // =====================================================================================
    cout << "\n================ 2. MISTAKE: THROWING EXCEPTIONS INSIDE DESTRUCTORS ================\n";
    cout << "  - MISTAKE : Allowing an exception to escape a destructor during stack unwinding\n";
    cout << "              forces C++ to immediately invoke `std::terminate()`, crashing the process!\n";
    cout << "  - CORRECT : Always mark destructors `noexcept` and catch internal errors defensively:\n";

    {
        class SafeDestructor {
        public:
            ~SafeDestructor() noexcept { // Destructors are implicitly noexcept in C++11+
                try {
                    // Simulate an internal cleanup failure
                    throw std::runtime_error("OS handle flush failure inside destructor!");
                } catch (const std::exception& e) {
                    // FIX: Handle/log failure locally; NEVER allow exception to escape!
                    cout << "    [CORRECT FIX] Caught error inside destructor locally: \"" << e.what() << "\"\n";
                }
            }
        };

        SafeDestructor safeObj;
        cout << "  - Exiting scope containing `SafeDestructor`...\n";
    }

    // =====================================================================================
    // MISTAKE 3: SHALLOW COPIES & DOUBLE-FREE HAZARDS
    // =====================================================================================
    cout << "\n================ 3. MISTAKE: SHALLOW COPIES & DOUBLE-FREE HAZARDS ================\n";
    cout << "  - MISTAKE : Writing custom RAII handles without deleting copy constructors.\n";
    cout << "              Compiler performs memberwise shallow pointer copies, resulting in two\n";
    cout << "              handles holding the same raw address -> Double-free crash on scope exit!\n";

    {
        cout << "  - [CORRECT FIX]: Enforcing Move-Only semantics (`= delete` on copy ops)...\n";
        CorrectMoveOnlyHandle handle1(new PitfallTrackerNode(userInputValue, "MoveOnlyNode"));

        // CorrectMoveOnlyHandle copyAttempt = handle1; 
        // COMPILE ERROR: Copy constructor deleted! Prevents double-free crash.

        CorrectMoveOnlyHandle handle2 = std::move(handle1); // Safe move transfer
        cout << "    Successfully transferred handle ownership without duplication.\n";
        cout << "  - Exiting move-only scope...\n";
    }

    cout << "  - Active Instances Post Move Scope: " << PitfallTrackerNode::activeInstances << "\n";

    // =====================================================================================
    // MISTAKE 4: OUTLIVING RAII LIFETIME (DANGLING RAW POINTERS)
    // =====================================================================================
    cout << "\n================ 4. MISTAKE: OUTLIVING RAII LIFETIME (DANGLING POINTER) ================\n";

    PitfallTrackerNode* danglingRawPtr = nullptr;

    {
        cout << "  - Creating RAII handle inside local scope...\n";
        auto scopedNode = std::make_unique<PitfallTrackerNode>(userInputValue + 10, "TemporaryRaiiNode");

        // MISTAKE: Extracting raw pointer via `.get()` and saving it outside the RAII scope!
        danglingRawPtr = scopedNode.get();

        cout << "    Extracted raw pointer address: " << static_cast<const void*>(danglingRawPtr) << "\n";
        cout << "  - Exiting local scope (RAII handle destroys underlying object!)...\n";
    } // scopedNode frees memory HERE!

    cout << "  - [DANGLING POINTER HAZARD]: `danglingRawPtr` now points to FREED MEMORY!\n";
    cout << "  - Accessing `danglingRawPtr` now causes Undefined Behavior (Use-After-Free).\n";
    cout << "  - [CORRECT FIX]: Always ensure RAII handle ownership outlives all observer references.\n";

    // =====================================================================================
    // MISTAKE 5: BYPASSED MANUAL CLEANUP CALLS
    // =====================================================================================
    cout << "\n================ 5. MISTAKE: BYPASSED MANUAL CLEANUP CALLS ================\n";

    auto legacyPipelineWithBug = [](int id) {
        cout << "  - MISTAKE: Allocating raw resource and relying on manual `delete` at end of function...\n";
        PitfallTrackerNode* rawPtr = new PitfallTrackerNode(id, "BypassedRawNode");

        bool earlyErrorTriggered = true;
        if (earlyErrorTriggered) {
            cout << "    [EARLY RETURN TRIGGERED] Returning prematurely before `delete rawPtr`!\n";
            return; // MISTAKE: Bypasses manual delete call below -> PERMANENT MEMORY LEAK!
        }

        delete rawPtr; // Unreachable code!
    };

    cout << "  - Executing unsafe raw pipeline...\n";
    legacyPipelineWithBug(userInputValue + 20);

    cout << "  - Active Instances Post Raw Leak: " << PitfallTrackerNode::activeInstances 
         << " (LEAK CONFIRMED: Raw node leaked!)\n";

    // =====================================================================================
    // MISTAKE 6: SHARED POINTER CYCLIC REFERENCES
    // =====================================================================================
    cout << "\n================ 6. MISTAKE: SHARED POINTER CYCLIC REFERENCES ================\n";

    {
        cout << "  - Demonstration A (MISTAKE: Cycle using std::shared_ptr):\n";
        auto nodeA = std::make_shared<CyclicNodeBad>(1);
        auto nodeB = std::make_shared<CyclicNodeBad>(2);
        
        nodeA->neighbor = nodeB;
        nodeB->neighbor = nodeA; // MISTAKE: Creates strong reference cycle (Ref count never drops to 0)!

        cout << "    Node A Ref Count: " << nodeA.use_count() << "\n";
        cout << "    Node B Ref Count: " << nodeB.use_count() << "\n";
        cout << "  - Exiting scope (Neither node will be destroyed!)...\n";
    } // Memory leaked silently!

    {
        cout << "\n  - Demonstration B (CORRECT FIX: Cycle broken using std::weak_ptr):\n";
        auto node1 = std::make_shared<CyclicNodeGood>(10);
        auto node2 = std::make_shared<CyclicNodeGood>(20);

        node1->neighbor = node2;
        node2->neighbor = node1; // FIX: weak_ptr does not increment strong reference count!

        cout << "    Node 1 Ref Count: " << node1.use_count() << "\n";
        cout << "    Node 2 Ref Count: " << node2.use_count() << "\n";
        cout << "  - Exiting scope...\n";
    } // Both nodes destroyed cleanly HERE!

    // =====================================================================================
    // MISTAKE 7: MISMATCHED ALLOCATION AND DEALLOCATION OPERATORS
    // =====================================================================================
    cout << "\n================ 7. MISTAKE: MISMATCHED ALLOCATION & DEALLOCATION ================\n";
    cout << "  - MISTAKE : Pairing `new T[]` with `delete ptr` (missing array brackets `delete[]`).\n";
    cout << "              This results in undefined behavior (only first array element destroyed).\n";
    cout << "  - CORRECT : Use `std::unique_ptr<T[]>` which automatically invokes `delete[]`:\n";

    {
        cout << "  - Allocating dynamic array via `std::unique_ptr<int[]>`...\n";
        std::unique_ptr<int[]> smartArray = std::make_unique<int[]>(5);
        smartArray[0] = 100;
        smartArray[4] = 500;
        cout << "    [CORRECT FIX] Array automatically freed using correct `delete[]` operator on exit.\n";
    }

    cout << "\n  - Final Active Resource Instances Count: " << PitfallTrackerNode::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ COMMON RAII MISTAKES & SOLUTIONS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| RAII Mistake / Bug    | Technical Cause of Bug / Hazard   | Modern C++ RAII Correct Fix       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Anonymous Temp Lock   | `std::lock_guard(mtx);` (No name) | Name variable: `lock_guard lock(m)`|\n"
         << "| Throw in Destructor   | Exception escapes during unwind  | Mark `noexcept`, catch errors inside|\n"
         << "| Shallow Copy Hazard   | Missing `= delete` on custom RAII | Delete copy operations / Use Move |\n"
         << "| Dangling Raw Pointer  | Storing `.get()` past RAII scope  | Extend RAII wrapper scope lifetime|\n"
         << "| Manual Cleanup Leak   | Early `return` bypasses `delete`  | Bind handle to RAII wrapper scope |\n"
         << "| Shared Pointer Cycle  | Mutual strong `shared_ptr` refs   | Use `std::weak_ptr` for back-refs |\n"
         << "| Mismatched Dealloc    | `new[]` paired with single `delete`| Use `std::unique_ptr<T[]>`        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}