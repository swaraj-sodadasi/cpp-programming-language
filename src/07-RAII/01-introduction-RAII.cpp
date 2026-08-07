/*
 * =====================================================================================
 * CONCEPT        : Introduction to RAII (Resource Acquisition Is Initialization)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the fundamental C++ architectural pattern of RAII:
 *
 *                  1. What is RAII?              : Binding resource lifecycle (memory, files, locks) 
 *                                                  directly to object lifetime (stack scope).
 *                  2. The Non-RAII Fragility     : Manual resource management pitfalls (leaks, early returns, 
 *                                                  and exception unwinding failures).
 *                  3. Core RAII Mechanics        : Resource acquisition in constructors (`Ctor`), 
 *                                                  deterministic release in destructors (`Dtor`).
 *                  4. Building a Custom Wrapper  : Designing an exception-safe templated `RAIIHandle<T>`
 *                                                  class adhering to the Rule of 5.
 *                  5. Non-Memory RAII Examples   : Encapsulating non-memory system handles 
 *                                                  (e.g., file handles, socket locks).
 *                  6. Standard Library RAII      : Built-in RAII abstractions (`std::unique_ptr`, `std::shared_ptr`).
 *
 * TIME COMPLEXITY  : Construction / Destruction / Dereferencing / Move: O(1) constant time.
 * SPACE COMPLEXITY : RAII Wrapper size: Uniform 8 bytes (raw pointer / handle size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE (FOR TRACKING LIFECYCLE & ACTIVE INSTANCES)
// =====================================================================================
class ResourceNode {
private:
    int id_;
    string label_;

public:
    static inline int activeCount = 0; // C++17 inline static variable

    ResourceNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeCount;
        cout << "    [RESOURCE ACQUIRED] ID: " << id_ << " (" << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeCount << "\n";
    }

    ~ResourceNode() {
        --activeCount;
        cout << "    [RESOURCE RELEASED] ID: " << id_ << " (" << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeCount << "\n";
    }

    void process() const {
        cout << "    [RESOURCE PROCESSING] Resource ID: " << id_ << " (" << label_ << ") executing.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM GENERIC RAII WRAPPER (`RAIIHandle<T>`)
// Implements Rule of 5 to manage raw pointer lifetimes safely.
// =====================================================================================
template <typename T>
class RAIIHandle {
private:
    T* resource_;

public:
    // 1. Constructor: Acquires & binds resource immediately on initialization
    explicit RAIIHandle(T* res = nullptr) : resource_(res) {
        cout << "    [RAIIHandle Ctor] Bound resource address: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    // 2. Destructor: Deterministic cleanup upon scope exit
    ~RAIIHandle() {
        cout << "    [RAIIHandle Dtor] Scope exit detected -> Automatically deleting resource at: " 
             << static_cast<const void*>(resource_) << "\n";
        delete resource_; // Safe delete (no-op if nullptr)
    }

    // 3. Rule of 5: Suppress Copy Semantics (Prevent double-free hazards)
    RAIIHandle(const RAIIHandle&) = delete;
    RAIIHandle& operator=(const RAIIHandle&) = delete;

    // 4. Rule of 5: Enable Move Semantics (Transfer resource ownership)
    RAIIHandle(RAIIHandle&& other) noexcept : resource_(other.resource_) {
        other.resource_ = nullptr; // Nullify source to transfer ownership
        cout << "    [RAIIHandle Move Ctor] Resource ownership transferred successfully.\n";
    }

    RAIIHandle& operator=(RAIIHandle&& other) noexcept {
        if (this != &other) {
            delete resource_;       // Free current resource
            resource_ = other.resource_; // Transfer new resource
            other.resource_ = nullptr;
            cout << "    [RAIIHandle Move Assign] Previous resource freed, ownership transferred.\n";
        }
        return *this;
    }

    // Dereference Operators
    T& operator*() const { return *resource_; }
    T* operator->() const { return resource_; }

    // Utility Accessors
    [[nodiscard]] T* get() const { return resource_; }
    [[nodiscard]] explicit operator bool() const { return resource_ != nullptr; }
};

// =====================================================================================
// HELPER CLASS 3: NON-MEMORY RAII RESOURCE (SCOPED FILE HANDLE SIMULATOR)
// Demonstrates that RAII applies to ANY system resource, not just heap memory.
// =====================================================================================
class ScopedFileSimulator {
private:
    string filename_;
    bool isOpen_;

public:
    explicit ScopedFileSimulator(string filename)
        : filename_(std::move(filename)), isOpen_(true) {
        cout << "    [FILE HANDLE OPEN] System resource for file '" << filename_ << "' acquired.\n";
    }

    ~ScopedFileSimulator() {
        if (isOpen_) {
            cout << "    [FILE HANDLE CLOSE] System resource for file '" << filename_ << "' flushed & released.\n";
            isOpen_ = false;
        }
    }

    void writePayload(const string& payload) const {
        if (isOpen_) {
            cout << "    [FILE WRITE] Writing payload \"" << payload << "\" to '" << filename_ << "'\n";
        }
    }

    // Non-copyable resource handle
    ScopedFileSimulator(const ScopedFileSimulator&) = delete;
    ScopedFileSimulator& operator=(const ScopedFileSimulator&) = delete;
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for RAII lifecycle demonstration (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. THE NON-RAII PROBLEM: MANUAL LIFECYCLE & MEMORY LEAKS
    // =====================================================================================
    cout << "\n================ 1. THE NON-RAII PROBLEM (MANUAL MANAGEMENT) ================\n";

    cout << "  - Active Instances Before Allocation: " << ResourceNode::activeCount << "\n";

    // Non-RAII pattern: Manual allocation without guaranteed cleanup
    {
        cout << "  - Allocating raw pointer on heap (`new ResourceNode`)...\n";
        ResourceNode* rawNode = new ResourceNode(userInputValue, "NonRaiiNode");
        rawNode->process();

        cout << "  - [NON-RAII HAZARD]: Forgetting `delete rawNode` or throwing an exception causes a PERMANENT LEAK!\n";
        // Manual delete omitted intentionally here to demonstrate the problem
        (void)rawNode; // Suppress unused variable warning
    }

    cout << "  - Active Instances After Block Exit: " << ResourceNode::activeCount 
         << " (RESOURCE LEAKED! Constructor ran, but Destructor never executed!)\n";

    // =====================================================================================
    // 2. CORE RAII PHILOSOPHY: BINDING RESOURCE TO SCOPE
    // =====================================================================================
    cout << "\n================ 2. CORE RAII MECHANICS (AUTOMATIC LIFECYCLE) ================\n";

    {
        cout << "  - Entering local block scope...\n";
        // Stack object creation: Constructor acquires resource, Destructor automatically releases it
        ResourceNode stackNode(userInputValue + 1, "StackScopeRAII");
        stackNode.process();

        cout << "  - Leaving local block scope...\n";
    } // `stackNode` destructor triggered automatically HERE at scope boundary!

    cout << "  - Active Instances After Block Exit: " << ResourceNode::activeCount 
         << " (ZERO LEAKS: Destructor ran automatically!)\n";

    // =====================================================================================
    // 3. CUSTOM RAII WRAPPER (`RAIIHandle<T>`)
    // =====================================================================================
    cout << "\n================ 3. CUSTOM RAII WRAPPER (`RAIIHandle<T>`) ================\n";

    {
        cout << "  - Binding dynamic allocation to `RAIIHandle` wrapper...\n";
        RAIIHandle<ResourceNode> managedNode(new ResourceNode(userInputValue + 2, "WrappedRAII"));

        // Arrow operator dereferencing
        managedNode->process();

        cout << "  - Exiting wrapper block scope...\n";
    } // `managedNode` goes out of scope -> RAIIHandle Dtor invokes `delete` on raw pointer HERE!

    cout << "  - Active Instances Post Wrapper Exit: " << ResourceNode::activeCount << "\n";

    // =====================================================================================
    // 4. STACK UNWINDING & EXCEPTION SAFETY GUARANTEE
    // =====================================================================================
    cout << "\n================ 4. STACK UNWINDING & EXCEPTION SAFETY ================\n";

    try {
        cout << "  - Entering try block with RAII managed resource...\n";
        RAIIHandle<ResourceNode> exceptionSafeNode(new ResourceNode(userInputValue + 3, "ExceptionSafeNode"));

        cout << "  - Simulating an unexpected runtime error (Throwing exception)...\n";
        throw runtime_error("Simulated execution failure");

        // Code here is bypassed, but stack unwinding guarantees destructor calls!
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post Exception: " << ResourceNode::activeCount 
         << " (RAII GUARANTEE: Memory freed during stack unwinding!)\n";

    // =====================================================================================
    // 5. RAII BEYOND MEMORY: NON-MEMORY SYSTEM RESOURCES
    // =====================================================================================
    cout << "\n================ 5. RAII FOR NON-MEMORY SYSTEM RESOURCES ================\n";

    {
        ScopedFileSimulator fileHandle("app_log.txt");
        fileHandle.writePayload("Initializing application state...");
        fileHandle.writePayload("Performing calculations...");

        cout << "  - Exiting scope containing file handle...\n";
    } // ScopedFileSimulator destructor runs HERE -> File closed automatically!

    // =====================================================================================
    // 6. STANDARD LIBRARY RAII ABSTRACTIONS (`std::unique_ptr`)
    // =====================================================================================
    cout << "\n================ 6. STANDARD LIBRARY RAII TYPES ================\n";

    {
        cout << "  - Allocating via `std::make_unique<ResourceNode>`...\n";
        auto stdSmartPtr = std::make_unique<ResourceNode>(userInputValue + 4, "StdUniqueRAII");
        stdSmartPtr->process();

        cout << "  - Exiting `std::unique_ptr` scope...\n";
    } // std::unique_ptr destructor automatically frees heap memory HERE!

    cout << "  - Final Active Instances Count: " << ResourceNode::activeCount << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RAII PRINCIPLES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| RAII Pillar / Concept | Mechanism & Implementation        | Key Benefit & Safety Guarantee    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Resource Acquisition  | Performed in Constructor (`Ctor`)  | Binds resource directly to object |\n"
         << "| Resource Release      | Performed in Destructor (`Dtor`)   | Guaranteed execution on scope exit|\n"
         << "| Exception Safety      | Relies on C++ Stack Unwinding     | Prevents leaks during thrown errors|\n"
         << "| Ownership Semantics   | Suppress Copy / Enable Move       | Eliminates double-free bugs       |\n"
         << "| Non-Memory Resources  | Files, Sockets, Mutex Locks       | Auto-flushes and closes system handles|\n"
         << "| Standard Wrappers     | `std::unique_ptr`, `std::vector`  | Zero manual `delete` required     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}