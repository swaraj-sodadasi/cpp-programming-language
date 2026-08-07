/*
 * =====================================================================================
 * CONCEPT        : Lifetime Management in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how the Resource Acquisition Is Initialization (RAII) paradigm 
 *                  strictly binds the lifecycle of system resources (heap memory, 
 *                  file descriptors, locks) to the deterministic lifetime of stack objects:
 *
 *                  1. Storage Durations & Lifetimes: Automatic (stack), Dynamic (heap), 
 *                                                   Member sub-objects, and Static durations.
 *                  2. LIFO Teardown Mechanics     : Strict Last-In, First-Out destruction 
 *                                                   order across nested and local block scopes.
 *                  3. Compositional Lifetimes     : Sub-object initialization and teardown sequence 
 *                                                   relative to parent composite objects.
 *                  4. Lifetime Transfers          : Transferring resource ownership and extending 
 *                                                   resource lifetime via Move Semantics (Rule of 5).
 *                  5. Shared Lifetime & Observation: Managing co-owned resource lifetimes via 
 *                                                   reference counting (`std::shared_ptr`) and 
 *                                                   non-extending observation (`std::weak_ptr`).
 *                  6. Exception Unwinding Safety  : Guaranteed, leak-free object lifetime termination 
 *                                                   during exception propagation.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Ownership Move: O(1) constant time.
 * SPACE COMPLEXITY : RAII Handle Overhead: Uniform 8-16 bytes (raw pointer / handle size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE LIFETIME TRACKER
// Logs creation, movement, usage, and destruction to trace lifetimes in real time.
// =====================================================================================
class LifetimeTrackerNode {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    LifetimeTrackerNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [LIFETIME BORN]    ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") created at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~LifetimeTrackerNode() noexcept {
        --activeInstances;
        cout << "    [LIFETIME ENDED]   ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") destroyed at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    // Disable copies to ensure lifetime ownership is unambiguous
    LifetimeTrackerNode(const LifetimeTrackerNode&) = delete;
    LifetimeTrackerNode& operator=(const LifetimeTrackerNode&) = delete;

    // Move Construction (Transfers ownership, update tag)
    LifetimeTrackerNode(LifetimeTrackerNode&& other) noexcept 
        : id_(other.id_), label_(std::move(other.label_) + "_Moved") {
        other.id_ = -1;
        cout << "    [LIFETIME MOVED]   Ownership transferred to ID: " << id_ << " (" << label_ << ")\n";
    }

    LifetimeTrackerNode& operator=(LifetimeTrackerNode&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            label_ = std::move(other.label_) + "_Moved";
            other.id_ = -1;
            cout << "    [LIFETIME ASSIGNED] Ownership transferred to ID: " << id_ << " (" << label_ << ")\n";
        }
        return *this;
    }

    void executeWork() const {
        if (id_ != -1) {
            cout << "    [LIFETIME ACTIVE]  ID: " << id_ << " (" << label_ << ") performing work.\n";
        }
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
    [[nodiscard]] const string& getLabel() const noexcept { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM SCOPED RAII LIFETIME WRAPPER
// Production-grade move-only handle binding dynamic lifetime to stack lexical scope.
// =====================================================================================
template <typename T>
class CustomLifetimeHandle {
private:
    T* resource_;

public:
    explicit CustomLifetimeHandle(T* res = nullptr) noexcept : resource_(res) {
        cout << "    [CustomLifetimeHandle Ctor] Bound handle to memory: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    ~CustomLifetimeHandle() noexcept {
        cout << "    [CustomLifetimeHandle Dtor] Scope boundary reached -> Releasing: " 
             << static_cast<const void*>(resource_) << "\n";
        delete resource_; // Safe delete (no-op if nullptr)
    }

    // Non-copyable invariant
    CustomLifetimeHandle(const CustomLifetimeHandle&) = delete;
    CustomLifetimeHandle& operator=(const CustomLifetimeHandle&) = delete;

    // Move semantics for safe lifetime extension and ownership transfer
    CustomLifetimeHandle(CustomLifetimeHandle&& other) noexcept : resource_(other.resource_) {
        other.resource_ = nullptr; // Disarm source handle
        cout << "    [CustomLifetimeHandle Move Ctor] Extended lifetime via transfer.\n";
    }

    CustomLifetimeHandle& operator=(CustomLifetimeHandle&& other) noexcept {
        if (this != &other) {
            delete resource_;          // End lifetime of current resource
            resource_ = other.resource_; // Transfer new resource
            other.resource_ = nullptr;   // Disarm source
            cout << "    [CustomLifetimeHandle Move Assign] Previous ended, lifetime extended.\n";
        }
        return *this;
    }

    T& operator*() const { return *resource_; }
    T* operator->() const { return resource_; }

    [[nodiscard]] T* get() const noexcept { return resource_; }
    [[nodiscard]] explicit operator bool() const noexcept { return resource_ != nullptr; }
};

// =====================================================================================
// HELPER CLASS 3: COMPOSITE OBJECT (MEMBER SUB-OBJECT LIFETIME SEQUENCING)
// Proves member sub-objects are constructed before parent body and destructed after.
// =====================================================================================
class CompositeLifetimeParent {
private:
    LifetimeTrackerNode memberA_;
    LifetimeTrackerNode memberB_;

public:
    CompositeLifetimeParent(int baseId, const string& tag)
        : memberA_(baseId, tag + "::MemberA"),
          memberB_(baseId + 1, tag + "::MemberB") {
        cout << "    [COMPOSITE PARENT CTOR] Composite Parent object fully initialized.\n";
    }

    ~CompositeLifetimeParent() noexcept {
        cout << "    [COMPOSITE PARENT DTOR] Composite Parent destructor body executing...\n";
    }

    void executeMembers() const {
        memberA_.executeWork();
        memberB_.executeWork();
    }
};

// =====================================================================================
// DEMONSTRATION FUNCTIONS FOR TEMPORARIES & EXCEPTIONS
// =====================================================================================

LifetimeTrackerNode createTemporaryTracker(int id, const string& label) {
    return LifetimeTrackerNode(id, label);
}

void exceptionScopePipeline(int baseId) {
    cout << "  - Entering exception scope pipeline...\n";
    CustomLifetimeHandle<LifetimeTrackerNode> safeNode1(new LifetimeTrackerNode(baseId, "StackNode1"));
    CustomLifetimeHandle<LifetimeTrackerNode> safeNode2(new LifetimeTrackerNode(baseId + 1, "StackNode2"));

    safeNode1->executeWork();
    safeNode2->executeWork();

    cout << "  - Throwing runtime exception inside scope pipeline...\n";
    throw runtime_error("Simulated error forcing stack unwinding!");

    // Code below is unreachable, but stack unwinding guarantees safeNode2 then safeNode1 destructors run!
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for lifetime management analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. AUTOMATIC (STACK) LIFETIME & LIFO TEARDOWN ORDER
    // =====================================================================================
    cout << "\n================ 1. AUTOMATIC STACK LIFETIME (LIFO TEARDOWN) ================\n";

    {
        cout << "  - Entering local lexical scope...\n";
        LifetimeTrackerNode stackA(userInputValue, "StackObj_A");
        LifetimeTrackerNode stackB(userInputValue + 1, "StackObj_B");
        LifetimeTrackerNode stackC(userInputValue + 2, "StackObj_C");

        stackA.executeWork();
        stackB.executeWork();
        stackC.executeWork();

        cout << "  - Reached closing brace `}` of local lexical scope...\n";
        cout << "  - [NOTICE]: Destructors fire in reverse order of creation (LIFO: C -> B -> A):\n";
    } // Stack objects destroyed HERE in reverse order (LIFO)

    cout << "  - Active Instances Post-Scope: " << LifetimeTrackerNode::activeInstances << "\n";

    // =====================================================================================
    // 2. MEMBER SUB-OBJECT LIFETIME SEQUENCING
    // =====================================================================================
    cout << "\n================ 2. MEMBER SUB-OBJECT LIFETIME SEQUENCING ================\n";

    {
        cout << "  - Instantiating CompositeLifetimeParent object...\n";
        CompositeLifetimeParent parent(userInputValue + 10, "ParentComposite");

        cout << "  - Executing payload on composite members...\n";
        parent.executeMembers();

        cout << "  - Exiting composite scope...\n";
        // Lifetime Teardown Order:
        // 1. Composite Parent Dtor Body executes
        // 2. memberB_ Dtor executes (reverse declaration order)
        // 3. memberA_ Dtor executes
    }

    cout << "  - Active Instances Post-Composite Scope: " << LifetimeTrackerNode::activeInstances << "\n";

    // =====================================================================================
    // 3. LIFETIME EXTENSION VIA MOVE SEMANTICS & REFERENCE BINDING
    // =====================================================================================
    cout << "\n================ 3. LIFETIME TRANSFERS & EXTENSION ================\n";

    {
        cout << "  - Case A: Transferring heap resource lifetime to a outer scope via `std::move()`...\n";
        CustomLifetimeHandle<LifetimeTrackerNode> outerHandle;

        {
            cout << "    - Entering inner scope... Creating `innerHandle`...\n";
            CustomLifetimeHandle<LifetimeTrackerNode> innerHandle(
                new LifetimeTrackerNode(userInputValue + 20, "MovableLifetimeNode")
            );

            cout << "    - Transferring ownership from `innerHandle` to `outerHandle`...\n";
            outerHandle = std::move(innerHandle);

            cout << "    - Exiting inner scope (Resource lifetime extends past this exit!)...\n";
        } // innerHandle destructor runs as no-op because ownership was transferred

        cout << "  - Exited inner scope. Resource is STILL ALIVE in outer scope:\n";
        outerHandle->executeWork();

        cout << "  - Exiting outer scope...\n";
    } // outerHandle destructor frees resource HERE!

    cout << "\n  - Case B: Binding temporary object to `const LifetimeTrackerNode&` (Lifetime Extension):\n";
    {
        const LifetimeTrackerNode& boundRef = createTemporaryTracker(userInputValue + 21, "BoundTempRef");
        boundRef.executeWork();

        cout << "  - Leaving scope containing bound const reference...\n";
    } // Temporary object destructs HERE when reference goes out of scope!

    cout << "  - Active Instances Post-Extension: " << LifetimeTrackerNode::activeInstances << "\n";

    // =====================================================================================
    // 4. SHARED LIFETIMES & NON-OWNING OBSERVATION
    // =====================================================================================
    cout << "\n================ 4. SHARED LIFETIMES & NON-OWNING OBSERVATION ================\n";

    std::weak_ptr<LifetimeTrackerNode> weakObserver;

    {
        cout << "  - Creating primary shared owner `shared1` via `std::make_shared`...\n";
        std::shared_ptr<LifetimeTrackerNode> shared1 = 
            std::make_shared<LifetimeTrackerNode>(userInputValue + 30, "SharedLifetimeNode");
        
        weakObserver = shared1; // Observe without taking ownership / extending lifetime
        cout << "    * Ref Count (1 owner) = " << shared1.use_count() << "\n";

        {
            cout << "  - Entering inner scope... Creating secondary owner `shared2 = shared1`...\n";
            std::shared_ptr<LifetimeTrackerNode> shared2 = shared1;
            cout << "    * Ref Count (2 owners) = " << shared1.use_count() << "\n";

            if (auto locked = weakObserver.lock()) {
                cout << "    * Lock Successful via weak_ptr! Active Ref Count = " << locked.use_count() << "\n";
            }
            cout << "  - Exiting inner scope...\n";
        } // shared2 destroyed, ref count decrements to 1

        cout << "  - Exited inner scope. Ref Count = " << shared1.use_count() << "\n";
        cout << "  - Resetting final shared owner `shared1`...\n";
        shared1.reset(); // Final reference dropped -> Lifetime ends HERE!
    }

    cout << "  - Post-Destruction `weakObserver.expired()` check: " 
         << (weakObserver.expired() ? "TRUE (Lifetime safely ended!)" : "FALSE") << "\n";

    // =====================================================================================
    // 5. LIFETIME MANAGEMENT DURING EXCEPTION STACK UNWINDING
    // =====================================================================================
    cout << "\n================ 5. LIFETIME DURING EXCEPTION STACK UNWINDING ================\n";

    try {
        cout << "  - Executing `exceptionScopePipeline()` inside try block...\n";
        exceptionScopePipeline(userInputValue + 40);
    } catch (const std::exception& e) {
        cout << "  - [CATCH BLOCK] Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post-Exception Unwind: " << LifetimeTrackerNode::activeInstances 
         << " (RAII GUARANTEE: Zero leaks during exception unwinding!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ LIFETIME MANAGEMENT IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Storage / Duration    | Lifetime Binding Mechanism        | Primary Operational Safety Trait  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Automatic (Stack)     | Lexical block scope `{ ... }`     | Deterministic LIFO teardown order |\n"
         << "| Dynamic Heap (RAII)   | Bound to wrapper object lifetime  | Auto-freed via `delete` on exit   |\n"
         << "| Member Sub-Objects    | Bound to parent composite object  | Created before Parent, freed after|\n"
         << "| Move Ownership        | `wrapperA = std::move(wrapperB)`  | Transfers handle & extends lifetime|\n"
         << "| Temporary Binding     | Bound to `const T&` or `T&&`      | Extended to reference variable scope|\n"
         << "| Shared Ref-Counted    | `std::shared_ptr` control block   | Destroyed when use_count drops to 0|\n"
         << "| Non-Owning Observer   | `std::weak_ptr` / raw observer ptr| Inspects without extending lifetime|\n"
         << "| Exception Unwinding   | C++ runtime stack frame unwind    | Guaranteed teardown on throw      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}