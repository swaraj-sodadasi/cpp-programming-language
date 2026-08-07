/*
 * =====================================================================================
 * CONCEPT        : Deterministic Destruction in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  Deterministic Destruction—the foundational guarantee of C++ RAII where
 *                  resource cleanup occurs at an exact, predictable, and compile-time 
 *                  known point in execution (unlike non-deterministic garbage collection):
 *
 *                  1. Predictable Lifetime Bounds : Resources are released immediately upon
 *                                                   crossing closing brace `}` scope boundaries.
 *                  2. LIFO Stack Reversal Order   : Deterministic destruction of stack variables
 *                                                   in strict reverse order of construction.
 *                  3. Control-Flow Determinism   : Guaranteed cleanup regardless of exit path
 *                                                   (early `return`, `break`, `continue`, `goto`).
 *                  4. Explicit Scope Blocks       : Using micro block scopes `{ ... }` to 
 *                                                   minimise resource hold times deterministically.
 *                  5. Sub-Object Teardown Order   : Deterministic destruction order of member 
 *                                                   variables relative to parent class bodies.
 *                  6. Stack Unwinding Determinism : Immediate resource release when exceptions 
 *                                                   propagate out of scopes.
 *
 * TIME COMPLEXITY  : Destructor Invocation / Resource Release: O(1) constant time.
 * SPACE COMPLEXITY : RAII Handle Overhead: Uniform 8-16 bytes (raw pointer/handle size).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: DETERMINISTIC LIFECYCLE TRACKER
// Tracks allocation and destruction timing to prove exact point-in-time destruction.
// =====================================================================================
class DeterministicResourceNode {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    DeterministicResourceNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [ACQUIRED (Ctor)] ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~DeterministicResourceNode() noexcept {
        --activeInstances;
        cout << "    [RELEASED (Dtor)] ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    // Disable copies to preserve unambiguous 1:1 construction/destruction tracking
    DeterministicResourceNode(const DeterministicResourceNode&) = delete;
    DeterministicResourceNode& operator=(const DeterministicResourceNode&) = delete;

    // Move Construction
    DeterministicResourceNode(DeterministicResourceNode&& other) noexcept 
        : id_(other.id_), label_(std::move(other.label_) + "_Moved") {
        other.id_ = -1;
    }

    DeterministicResourceNode& operator=(DeterministicResourceNode&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            label_ = std::move(other.label_) + "_Moved";
            other.id_ = -1;
        }
        return *this;
    }

    void executeWork() const {
        if (id_ != -1) {
            cout << "    [PAYLOAD BUSY]    ID: " << id_ << " (" << label_ << ") performing critical task.\n";
        }
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
    [[nodiscard]] const string& getLabel() const noexcept { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM SCOPED HANDLE FOR DETERMINISTIC LIFETIME MANAGEMENT
// =====================================================================================
template <typename T>
class DeterministicScopeHandle {
private:
    T* resource_;

public:
    explicit DeterministicScopeHandle(T* res = nullptr) noexcept : resource_(res) {
        cout << "    [ScopeHandle Ctor] Bound handle to memory: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    ~DeterministicScopeHandle() noexcept {
        cout << "    [ScopeHandle Dtor] Scope boundary crossed! Executing deterministic teardown...\n";
        delete resource_; // Safe delete (no-op if nullptr)
    }

    // Non-copyable invariant
    DeterministicScopeHandle(const DeterministicScopeHandle&) = delete;
    DeterministicScopeHandle& operator=(const DeterministicScopeHandle&) = delete;

    // Move semantics
    DeterministicScopeHandle(DeterministicScopeHandle&& other) noexcept : resource_(other.resource_) {
        other.resource_ = nullptr;
    }

    DeterministicScopeHandle& operator=(DeterministicScopeHandle&& other) noexcept {
        if (this != &other) {
            delete resource_;
            resource_ = other.resource_;
            other.resource_ = nullptr;
        }
        return *this;
    }

    // Manual deterministic reset before scope exit
    void reset(T* newRes = nullptr) noexcept {
        if (resource_ != newRes) {
            T* oldRes = resource_;
            resource_ = newRes;
            cout << "    [ScopeHandle reset] Explicitly deleting old resource handle before scope exit...\n";
            delete oldRes;
        }
    }

    T& operator*() const { return *resource_; }
    T* operator->() const { return resource_; }

    [[nodiscard]] T* get() const noexcept { return resource_; }
    [[nodiscard]] explicit operator bool() const noexcept { return resource_ != nullptr; }
};

// =====================================================================================
// HELPER CLASS 3: COMPOSITE PARENT (SUB-OBJECT DETERMINISTIC TEARDOWN)
// Demonstrates that member sub-objects are destroyed deterministically in reverse order.
// =====================================================================================
class CompositeParent {
private:
    DeterministicResourceNode member1_;
    DeterministicResourceNode member2_;

public:
    CompositeParent(int baseId, const string& tag)
        : member1_(baseId, tag + "_Member1"),
          member2_(baseId + 1, tag + "_Member2") {
        cout << "    [COMPOSITE CTOR BODY] Parent object fully constructed.\n";
    }

    ~CompositeParent() noexcept {
        cout << "    [COMPOSITE DTOR BODY] Parent object destructor body executing...\n";
    }

    void process() const {
        member1_.executeWork();
        member2_.executeWork();
    }
};

// =====================================================================================
// DEMONSTRATION FUNCTIONS FOR CONTROL FLOW & EXCEPTIONS
// =====================================================================================

void demonstrateEarlyReturn(int baseId) {
    cout << "  - Entering `demonstrateEarlyReturn()`...\n";
    DeterministicResourceNode funcScopeRes(baseId, "EarlyReturnNode");
    funcScopeRes.executeWork();

    cout << "  - Executing premature `return;` statement...\n";
    return; // Destructor executes deterministically HERE before control transfers back to main!

    // Unreachable code
    [[maybe_unused]] int x = 42;
}

void demonstrateExceptionUnwinding(int baseId) {
    cout << "  - Entering `demonstrateExceptionUnwinding()`...\n";
    DeterministicResourceNode unwindNode1(baseId, "UnwindNode1");
    DeterministicResourceNode unwindNode2(baseId + 1, "UnwindNode2");

    cout << "  - Throwing runtime exception inside function scope...\n";
    throw runtime_error("Simulated error triggering stack unwinding!");

    // Unreachable code, but stack unwinding forces unwindNode2 then unwindNode1 destructors!
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for deterministic destruction analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC LEXICAL SCOPE BOUNDARY (EXACT POINT-IN-TIME DESTRUCTION)
    // =====================================================================================
    cout << "\n================ 1. BASIC LEXICAL SCOPE (EXACT TIMING) ================\n";

    cout << "  - Before entering block scope. Active Count: " << DeterministicResourceNode::activeInstances << "\n";

    {
        cout << "  - Entered block scope `{ ... }`\n";
        DeterministicResourceNode scopedObj(userInputValue, "ScopeBoundNode");
        scopedObj.executeWork();

        cout << "  - Right BEFORE closing brace `}`...\n";
    } // Destructor fires EXACTLY at this closing brace!

    cout << "  - Right AFTER closing brace `}`. Active Count: " << DeterministicResourceNode::activeInstances << "\n";
    cout << "  - [DETERMINISM PROOF]: Resource was released IMMEDIATELY without waiting for a Garbage Collector.\n";

    // =====================================================================================
    // 2. LIFO (LAST-IN, FIRST-OUT) DESTRUCTION SEQUENCE
    // =====================================================================================
    cout << "\n================ 2. LIFO STACK DESTRUCTION SEQUENCE ================\n";

    {
        cout << "  - Creating objects sequentially: ResA -> ResB -> ResC...\n";
        DeterministicResourceNode resA(userInputValue + 10, "ResA_First");
        DeterministicResourceNode resB(userInputValue + 11, "ResB_Second");
        DeterministicResourceNode resC(userInputValue + 12, "ResC_Third");

        cout << "  - Exiting block scope (Watch destructor invocation order):\n";
    } // ResC destructs first, then ResB, then ResA (Strict LIFO order)

    cout << "  - Active Count Post-LIFO Scope: " << DeterministicResourceNode::activeInstances << "\n";

    // =====================================================================================
    // 3. EXPLICIT MICRO-SCOPES TO MINIMIZE RESOURCE RETENTION
    // =====================================================================================
    cout << "\n================ 3. EXPLICIT MICRO-SCOPES (SHORT RESOURCE LIFETIME) ================\n";

    cout << "  - Starting long-running workflow...\n";

    // Custom explicit block scope to acquire, use, and immediately release a heavy resource
    {
        cout << "    [MICRO-SCOPE ENTER] Acquiring database handle...\n";
        DeterministicResourceNode dbLock(userInputValue + 20, "DatabaseTransactionLock");
        dbLock.executeWork();
        cout << "    [MICRO-SCOPE EXIT] Exiting micro-scope to release database handle before heavy computation...\n";
    } // Database lock freed HERE deterministically!

    cout << "  - Performing long computation (Database handle is ALREADY freed!)...\n";

    // =====================================================================================
    // 4. CONTROL-FLOW DETERMINISM (EARLY RETURNS & LOOPS)
    // =====================================================================================
    cout << "\n================ 4. CONTROL-FLOW DETERMINISM (EARLY RETURN & LOOPS) ================\n";

    cout << "  - Case A: Testing Early Return...\n";
    demonstrateEarlyReturn(userInputValue + 30);
    cout << "  - Active Count Post-Early Return: " << DeterministicResourceNode::activeInstances << "\n";

    cout << "\n  - Case B: Testing Loop Break...\n";
    for (int i = 0; i < 3; ++i) {
        cout << "    Loop iteration " << i << "...\n";
        DeterministicResourceNode loopRes(userInputValue + 40 + i, "LoopResource_" + to_string(i));
        if (i == 1) {
            cout << "    [LOOP BREAK] Breaking out of loop prematurely at iteration 1...\n";
            break; // Loop resource destructs deterministically HERE on break!
        }
    }
    cout << "  - Active Count Post-Loop Break: " << DeterministicResourceNode::activeInstances << "\n";

    // =====================================================================================
    // 5. COMPOSITE MEMBER SUB-OBJECT DESTRUCTION
    // =====================================================================================
    cout << "\n================ 5. COMPOSITE MEMBER SUB-OBJECT DESTRUCTION ================\n";

    {
        cout << "  - Instantiating `CompositeParent`...\n";
        CompositeParent parentObj(userInputValue + 50, "ParentGroup");
        parentObj.process();

        cout << "  - Exiting scope containing composite object...\n";
        // Destructor Order:
        // 1. Parent Destructor Body runs
        // 2. member2_ Destructor runs (reverse declaration order)
        // 3. member1_ Destructor runs
    }

    cout << "  - Active Count Post-Composite Scope: " << DeterministicResourceNode::activeInstances << "\n";

    // =====================================================================================
    // 6. EXPLICIT VS IMPLICIT DETERMINISTIC DESTRUCTION
    // =====================================================================================
    cout << "\n================ 6. EXPLICIT VS IMPLICIT DETERMINISTIC DESTRUCTION ================\n";

    {
        cout << "  - Creating `DeterministicScopeHandle`...\n";
        DeterministicScopeHandle<DeterministicResourceNode> handle(
            new DeterministicResourceNode(userInputValue + 60, "ExplicitResetNode")
        );

        cout << "  - Calling `.reset()` explicitly BEFORE scope exit...\n";
        handle.reset(); // Resource freed explicitly HERE!

        cout << "  - Continuing inside scope (Resource is ALREADY freed)...\n";
        cout << "  - Exiting scope (Handle destructor runs as no-op)...\n";
    }

    cout << "  - Active Count Post-Reset Scope: " << DeterministicResourceNode::activeInstances << "\n";

    // =====================================================================================
    // 7. DETERMINISTIC DESTRUCTION DURING EXCEPTION UNWINDING
    // =====================================================================================
    cout << "\n================ 7. DETERMINISTIC STACK UNWINDING ON EXCEPTION ================\n";

    try {
        demonstrateExceptionUnwinding(userInputValue + 70);
    } catch (const std::exception& e) {
        cout << "  - [CATCH BLOCK] Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Count Post-Exception Unwind: " << DeterministicResourceNode::activeInstances 
         << " (RAII GUARANTEE: Zero leaks during exception unwinding!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DETERMINISTIC DESTRUCTION IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Concept     | Modern C++ RAII Behavior          | Comparison with GC (Java/Python)  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Teardown Timing       | Immediate upon hitting scope `}`  | Non-deterministic; GC thread runs |\n"
         << "| Execution Order       | Strict LIFO (Last-In, First-Out)  | Unpredictable finalizer order     |\n"
         << "| Control-Flow Exit     | Auto-frees on `return`/`break`    | Requires manual `try-finally`     |\n"
         << "| Memory/Handles Release| Exact point known at compile-time | Delayed until memory pressure     |\n"
         << "| Explicit Micro-Scopes | `{ RAII obj; }` limits lifetime   | GC cannot guarantee immediate free|\n"
         << "| Sub-Object Cleanup    | Auto-destructs members in reverse | GC tracks objects independently   |\n"
         << "| Exception Unwinding   | Unconditional stack frame cleanup | Requires `try-finally` blocks     |\n"
         << "| Resource Types        | Universal (Heap, Files, Mutexes)  | Primary focus is heap memory only |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}