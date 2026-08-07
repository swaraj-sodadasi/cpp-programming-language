/*
 * =====================================================================================
 * CONCEPT        : RAII Relationship in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the Resource Acquisition Is Initialization (RAII) idiom and its core
 *                  relationship with pointer and memory lifecycle management:
 *
 *                  1. Core RAII Idiom           : Resource acquisition in constructors, 
 *                                                 deterministic cleanup in destructors.
 *                  2. The Non-RAII Pitfall      : Manual pointer management fragility, early returns,
 *                                                 and exception-induced memory leaks.
 *                  3. Custom RAII Wrapper       : Building a production-grade `CustomScopedPtr<T>`
 *                                                 implementing Rule of 5 (Move semantics & non-copyable).
 *                  4. Exception Safety Guarantee: Deterministic stack unwinding ensuring zero leaks 
 *                                                 when exceptions are thrown.
 *                  5. Standard Library RAII     : `std::unique_ptr`, `std::shared_ptr`, and `std::vector`
 *                                                 as modern idiomatic RAII wrappers.
 *                  6. Non-Memory RAII Resources : Extending RAII to arbitrary resources (e.g., custom file/buffer handles).
 *
 * TIME COMPLEXITY  : Construction / Destruction / Dereferencing / Move: O(1) constant time.
 * SPACE COMPLEXITY : Custom RAII Wrapper size: Uniform 8 bytes (raw pointer size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <utility>
#include <limits>
#include <stdexcept>
#include <string>

using namespace std;

// =====================================================================================
// HELPER CLASS FOR LIFECYCLE AND RAII TRACKING
// Tracks active allocations to visually verify zero-leak RAII execution.
// =====================================================================================
class ManagedResource {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static variable

    ManagedResource(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE ACQUIRED]  ID: " << id_ << " (" << label_ 
             << ") | Address: " << static_cast<const void*>(this) 
             << " | Active Count: " << activeInstances << "\n";
    }

    ~ManagedResource() {
        --activeInstances;
        cout << "    [RESOURCE RELEASED]  ID: " << id_ << " (" << label_ 
             << ") | Address: " << static_cast<const void*>(this) 
             << " | Active Count: " << activeInstances << "\n";
    }

    void performAction() const {
        cout << "    [RESOURCE EXECUTING] ManagedResource ID: " << id_ << " (" << label_ << ") working.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// CUSTOM RAII SCOPED POINTER WRAPPER (`CustomScopedPtr<T>`)
// Encapsulates raw pointer ownership, proving how RAII ties lifetime to scope.
// =====================================================================================
template <typename T>
class CustomScopedPtr {
private:
    T* ptr_; // Managed raw pointer resource

public:
    // 1. Constructor (Acquisition / Binding)
    explicit CustomScopedPtr(T* ptr = nullptr) : ptr_(ptr) {
        cout << "    [CustomScopedPtr Ctor] Bound to raw pointer: " 
             << static_cast<const void*>(ptr_) << "\n";
    }

    // 2. Destructor (Deterministic Release)
    ~CustomScopedPtr() {
        cout << "    [CustomScopedPtr Dtor] Scope exit -> Automatically deleting raw pointer: " 
             << static_cast<const void*>(ptr_) << "\n";
        delete ptr_; // Safe delete (no-op if nullptr)
    }

    // 3. Disable Copy Constructor & Copy Assignment (Enforce Single Ownership Invariant)
    CustomScopedPtr(const CustomScopedPtr&) = delete;
    CustomScopedPtr& operator=(const CustomScopedPtr&) = delete;

    // 4. Move Constructor (Transfer Ownership)
    CustomScopedPtr(CustomScopedPtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr; // Nullify source to prevent double-free
        cout << "    [CustomScopedPtr Move Ctor] Ownership transferred safely.\n";
    }

    // 5. Move Assignment Operator
    CustomScopedPtr& operator=(CustomScopedPtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;       // Free existing managed resource
            ptr_ = other.ptr_; // Transfer ownership
            other.ptr_ = nullptr;
            cout << "    [CustomScopedPtr Move Assignment] Existing freed, new ownership transferred.\n";
        }
        return *this;
    }

    // Dereference Operators
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }

    // Utility methods
    [[nodiscard]] T* get() const { return ptr_; }
    [[nodiscard]] explicit operator bool() const { return ptr_ != nullptr; }

    T* release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
};

// =====================================================================================
// HELPER FUNCTIONS DEMONSTRATING RAII VS NON-RAII
// =====================================================================================

// Non-RAII Fragile Pipeline: Early return or exception bypasses `delete`
void nonRaiiUnsafePipeline(bool triggerFailure, int id) {
    cout << "    [NON-RAII] Allocating raw pointer directly...\n";
    ManagedResource* rawPtr = new ManagedResource(id, "NonRaiiResource");

    if (triggerFailure) {
        cout << "    [NON-RAII EXCEPTION] Error encountered! Throwing exception before `delete rawPtr`...\n";
        // CRITICAL LEAK: Function unwinds immediately. `delete rawPtr` is NEVER reached!
        throw runtime_error("Unsafe Pipeline Failure");
    }

    delete rawPtr; // Reached ONLY if no exception occurs
}

// RAII Safe Pipeline: Guaranteed cleanup via stack unwinding
void raiiSafePipeline(bool triggerFailure, int id) {
    cout << "    [RAII] Constructing CustomScopedPtr wrapper...\n";
    CustomScopedPtr<ManagedResource> raiiPtr(new ManagedResource(id, "RaiiResource"));

    if (triggerFailure) {
        cout << "    [RAII EXCEPTION] Error encountered! Throwing exception inside RAII scope...\n";
        // SAFE: Stack unwinding automatically invokes CustomScopedPtr destructor!
        throw runtime_error("RAII Pipeline Failure");
    }

    raiiPtr->performAction();
} // Scope exit auto-destructs raiiPtr HERE whether an exception occurred or not!

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer ID for RAII lifecycle demonstration (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. THE NON-RAII PROBLEM: EXCEPTION MEMORY LEAKS
    // =====================================================================================
    cout << "\n================ 1. THE NON-RAII PROBLEM (MEMORY LEAK RISK) ================\n";

    cout << "  - Initial Active Managed Resources: " << ManagedResource::activeInstances << "\n";

    try {
        nonRaiiUnsafePipeline(true, userInputValue);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - [LEAK CONFIRMED]: Active Resources post-exception = " 
         << ManagedResource::activeInstances << " (Raw pointer resource was leaked!)\n";

    // =====================================================================================
    // 2. CUSTOM RAII WRAPPER & AUTOMATIC LIFECYCLE MANAGEMENT
    // =====================================================================================
    cout << "\n================ 2. CUSTOM RAII SCOPED WRAPPER (`CustomScopedPtr`) ================\n";

    {
        cout << "  - Entering local block scope...\n";
        CustomScopedPtr<ManagedResource> scopedRes(new ManagedResource(userInputValue + 1, "ScopedRAII"));
        scopedRes->performAction();

        cout << "  - Exiting local block scope (RAII destructor will trigger automatically):\n";
    } // CustomScopedPtr destructor called HERE automatically!

    cout << "  - Active Resources post block exit = " 
         << ManagedResource::activeInstances << " (Zero leaks!)\n";

    // =====================================================================================
    // 3. RAII EXCEPTION SAFETY & STACK UNWINDING GUARANTEE
    // =====================================================================================
    cout << "\n================ 3. RAII EXCEPTION SAFETY & STACK UNWINDING ================\n";

    try {
        raiiSafePipeline(true, userInputValue + 2);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - [LEAK-FREE GUARANTEE]: Active Resources post-exception = " 
         << ManagedResource::activeInstances << " (RAII automatically freed memory during unwind!)\n";

    // =====================================================================================
    // 4. MOVE SEMANTICS & OWNERSHIP TRANSFER IN RAII
    // =====================================================================================
    cout << "\n================ 4. MOVE SEMANTICS IN RAII CLASSES ================\n";

    {
        cout << "  - Creating original RAII owner `ptrA`...\n";
        CustomScopedPtr<ManagedResource> ptrA(new ManagedResource(userInputValue + 3, "MovableResource"));

        cout << "  - Transferring ownership from `ptrA` to `ptrB` via `std::move()`...\n";
        CustomScopedPtr<ManagedResource> ptrB = std::move(ptrA);

        cout << "  - `ptrA` Valid Check : " << (ptrA ? "VALID" : "NULLPTR (Relinquished)") << "\n";
        cout << "  - `ptrB` Valid Check : " << (ptrB ? "VALID (Owner)" : "NULLPTR") << "\n";

        cout << "  - Exiting scope (Only `ptrB` will release the resource once):\n";
    }

    cout << "  - Active Resources post move scope = " 
         << ManagedResource::activeInstances << " (Zero leaks or double-frees!)\n";

    // =====================================================================================
    // 5. STANDARD LIBRARY RAII WRAPPERS (`std::unique_ptr` & `std::shared_ptr`)
    // =====================================================================================
    cout << "\n================ 5. STANDARD LIBRARY RAII WRAPPERS ================\n";

    {
        cout << "  - Creating `std::unique_ptr<ManagedResource>` via `std::make_unique`...\n";
        auto stdSmartPtr = std::make_unique<ManagedResource>(userInputValue + 4, "StdUniqueRAII");
        stdSmartPtr->performAction();

        cout << "  - Exiting `std::unique_ptr` block scope...\n";
    } // Standard library RAII cleanup HERE!

    cout << "  - Active Resources post `std::unique_ptr` exit = " 
         << ManagedResource::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RAII RELATIONSHIP SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| RAII Principle        | Implementation / Mechanics        | Lifecycle & Memory Benefit        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Acquisition           | Allocated in Constructor (`new`)  | Resource bound immediately to obj |\n"
         << "| Release               | Deallocated in Destructor (`delete`)| Guaranteed execution on scope exit|\n"
         << "| Exception Safety      | Relies on C++ Stack Unwinding     | Prevents leaks during thrown errors|\n"
         << "| Copy Suppression      | `delete` Copy Ctor & Copy Assign  | Prevents double-free hazards      |\n"
         << "| Ownership Transfer    | Move Semantics (`std::move`)      | Safely transfers resource control |\n"
         << "| Standard Wrappers     | `std::unique_ptr`, `std::vector`  | Zero manual `delete` code required|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}