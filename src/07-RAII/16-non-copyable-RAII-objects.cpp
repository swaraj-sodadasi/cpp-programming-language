/*
 * =====================================================================================
 * CONCEPT        : Non-Copyable RAII Objects in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architectural design and mechanics of non-copyable RAII objects:
 *
 *                  1. Why Non-Copyable?          : Resources with strict 1:1 single-owner invariants 
 *                                                   (unique heap memory, file handles, OS sockets, 
 *                                                   mutex locks) must prohibit shallow/duplicate copying 
 *                                                   to eliminate double-free crashes and Use-After-Free (UAF).
 *                  2. Compiler Enforcement (`= delete`): Explicitly deleting the copy constructor and copy 
 *                                                   assignment operator enforces single ownership at compile-time.
 *                  3. Move Semantics (Ownership Transfer): Implementing `noexcept` move operations allows 
 *                                                   safe, zero-cost transfer of ownership (Move-Only Types).
 *                  4. Function Ownership Contracts: Distinguishing between "Sink Functions" (takes 
 *                                                   exclusive ownership via move) and "Borrowing Functions" 
 *                                                   (temporary non-owning observer references).
 *                  5. Standard Non-Copyable RAII : Examining canonical standard abstractions like 
 *                                                   `std::unique_ptr`, `std::ofstream`, and `std::lock_guard`.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Ownership Move: O(1) constant time.
 * SPACE COMPLEXITY : Non-Copyable Handle Overhead: Uniform 8 bytes (raw pointer/handle size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <fstream>
#include <mutex>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE WITH ACTIVE LIFECYCLE TRACKING
// Used to trace creation, execution, and deterministic destruction in real-time.
// =====================================================================================
class ExclusiveResource {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    ExclusiveResource(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE CREATED]   ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~ExclusiveResource() noexcept {
        --activeInstances;
        cout << "    [RESOURCE DESTROYED] ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    void executePayload() const {
        cout << "    [PAYLOAD EXECUTED]   Resource ID: " << id_ << " (" << label_ << ") performing work.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM NON-COPYABLE RAII WRAPPER (`ScopedUniqueHandle<T>`)
// Production-grade implementation of a move-only, non-copyable RAII handle.
// =====================================================================================
template <typename T>
class ScopedUniqueHandle {
private:
    T* resource_;

public:
    // 1. Constructor: Takes direct ownership of raw pointer
    explicit ScopedUniqueHandle(T* resource = nullptr) noexcept : resource_(resource) {
        cout << "    [ScopedUniqueHandle Ctor] Bound exclusive resource at: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    // 2. Destructor: Deterministic cleanup upon scope exit
    ~ScopedUniqueHandle() noexcept {
        cout << "    [ScopedUniqueHandle Dtor] Releasing resource at: " 
             << static_cast<const void*>(resource_) << "\n";
        delete resource_; // Safe delete (no-op if nullptr)
    }

    // =================================================================================
    // NON-COPYABLE CONTRACT: EXPLICITLY DELETE COPY OPERATIONS
    // =================================================================================
    // Prevents compiler-generated memberwise shallow copies that lead to double-free crashes.
    ScopedUniqueHandle(const ScopedUniqueHandle&) = delete;
    ScopedUniqueHandle& operator=(const ScopedUniqueHandle&) = delete;

    // =================================================================================
    // MOVE-ONLY CONTRACT: ENABLE MOVE SEMANTICS (RULE OF 5)
    // =================================================================================
    // Allows explicit transfer of control/ownership while invalidating the source handle.
    ScopedUniqueHandle(ScopedUniqueHandle&& other) noexcept : resource_(other.resource_) {
        other.resource_ = nullptr; // Relinquish ownership from source
        cout << "    [ScopedUniqueHandle Move Ctor] Resource ownership transferred successfully.\n";
    }

    ScopedUniqueHandle& operator=(ScopedUniqueHandle&& other) noexcept {
        if (this != &other) {
            delete resource_;          // Clean up currently held resource
            resource_ = other.resource_; // Transfer ownership from source
            other.resource_ = nullptr;   // Relinquish ownership from source
            cout << "    [ScopedUniqueHandle Move Assign] Previous freed, ownership transferred.\n";
        }
        return *this;
    }

    // Transparent Access Operators
    T& operator*() const { return *resource_; }
    T* operator->() const { return resource_; }

    // Utility Accessors
    [[nodiscard]] T* get() const noexcept { return resource_; }
    [[nodiscard]] explicit operator bool() const noexcept { return resource_ != nullptr; }
};

// =====================================================================================
// HELPER CLASS 3: NON-COPYABLE MIXIN BASE CLASS PATTERN (DERIVATION IDIOM)
// Demonstrates modern alternative to legacy C++ boost::noncopyable.
// =====================================================================================
class NonCopyableBase {
protected:
    NonCopyableBase() = default;
    ~NonCopyableBase() = default;

public:
    NonCopyableBase(const NonCopyableBase&) = delete;
    NonCopyableBase& operator=(const NonCopyableBase&) = delete;

    NonCopyableBase(NonCopyableBase&&) noexcept = default;
    NonCopyableBase& operator=(NonCopyableBase&&) noexcept = default;
};

class InheritedNonCopyableHandle : private NonCopyableBase {
private:
    ExclusiveResource* resource_;

public:
    explicit InheritedNonCopyableHandle(ExclusiveResource* res) : resource_(res) {}
    ~InheritedNonCopyableHandle() noexcept { delete resource_; }

    [[nodiscard]] ExclusiveResource* get() const noexcept { return resource_; }
};

// =====================================================================================
// FUNCTION OWNERSHIP CONTRACT DEMONSTRATIONS
// =====================================================================================

// Contract 1: Sink Function - Takes EXCLUSIVE Ownership (Caller must `std::move`)
void consumeResourceSink(ScopedUniqueHandle<ExclusiveResource> sinkHandle) {
    if (sinkHandle) {
        cout << "    [SINK FUNCTION] Received ownership of Resource ID: " << sinkHandle->getId() << "\n";
        sinkHandle->executePayload();
        cout << "    [SINK FUNCTION] Exiting function scope... Resource will be destroyed HERE.\n";
    }
} // `sinkHandle` destructor executes HERE!

// Contract 2: Borrowing Function - Non-Owning View (Caller retains ownership)
void observeResourceBorrowed(const ExclusiveResource* borrowedObserver) {
    if (borrowedObserver != nullptr) {
        cout << "    [BORROWED OBSERVER] Inspecting Resource ID: " << borrowedObserver->getId() 
             << " (Ownership retained by caller)\n";
        borrowedObserver->executePayload();
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for non-copyable RAII analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC NON-COPYABLE LIFECYCLE & COMPILE-TIME GUARD
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME COPY PREVENTION (`= delete`) ================\n";

    {
        cout << "  - Instantiating primary non-copyable handle `handleA`...\n";
        ScopedUniqueHandle<ExclusiveResource> handleA(new ExclusiveResource(userInputValue, "UniqueNodeA"));

        handleA->executePayload();

        // COMPILE-TIME GUARD PROOF:
        // Attempting to duplicate or pass by value will cause immediate compilation failure:
        // ScopedUniqueHandle<ExclusiveResource> handleCopy = handleA; // COMPILE ERROR: Copy Ctor is deleted!
        // handleCopy = handleA;                                      // COMPILE ERROR: Copy Assign is deleted!

        cout << "  - [COMPILE GUARANTEE]: Copy operations are strictly blocked by `= delete`.\n";
        cout << "  - Exiting `handleA` scope...\n";
    } // `handleA` destructor executes automatically HERE!

    cout << "  - Active Instances Post-Scope: " << ExclusiveResource::activeInstances << "\n";

    // =====================================================================================
    // 2. EXPLICIT OWNERSHIP TRANSFER (MOVE SEMANTICS)
    // =====================================================================================
    cout << "\n================ 2. EXPLICIT OWNERSHIP TRANSFER (MOVE SEMANTICS) ================\n";

    {
        cout << "  - Creating original owner `sourceHandle`...\n";
        ScopedUniqueHandle<ExclusiveResource> sourceHandle(new ExclusiveResource(userInputValue + 10, "MovableNode"));

        cout << "  - `sourceHandle` Target Address: " << static_cast<const void*>(sourceHandle.get()) << "\n";

        cout << "\n  - Transferring ownership to `targetHandle` via `std::move()`...\n";
        ScopedUniqueHandle<ExclusiveResource> targetHandle = std::move(sourceHandle);

        cout << "  - Post-Move `sourceHandle` State : " 
             << (sourceHandle ? "VALID" : "NULLPTR (Ownership Relinquished)") << "\n";
        cout << "  - Post-Move `targetHandle` Target: " 
             << static_cast<const void*>(targetHandle.get()) << " (Current Exclusive Owner)\n";

        targetHandle->executePayload();

        cout << "\n  - Exiting scope containing `targetHandle`...\n";
    } // `targetHandle` destructor frees resource HERE!

    cout << "  - Active Instances Post-Move Scope: " << ExclusiveResource::activeInstances << "\n";

    // =====================================================================================
    // 3. FUNCTION OWNERSHIP CONTRACTS (SINK vs BORROWING)
    // =====================================================================================
    cout << "\n================ 3. FUNCTION OWNERSHIP CONTRACTS ================\n";

    {
        cout << "  - Creating handle `workHandle`...\n";
        ScopedUniqueHandle<ExclusiveResource> workHandle(new ExclusiveResource(userInputValue + 20, "ContractNode"));

        // Pattern A: Borrowing Contract (Caller retains ownership)
        cout << "\n  - Pattern A: Invoking `observeResourceBorrowed(workHandle.get())`...\n";
        observeResourceBorrowed(workHandle.get());

        cout << "  - Caller STILL maintains ownership of Resource ID: " << workHandle->getId() << "\n";

        // Pattern B: Sink Contract (Caller transfers ownership into function)
        cout << "\n  - Pattern B: Invoking `consumeResourceSink(std::move(workHandle))`...\n";
        consumeResourceSink(std::move(workHandle));

        cout << "  - Post-Sink `workHandle` State: " 
             << (workHandle ? "VALID" : "NULLPTR (Ownership Transferred & Consumed)") << "\n";
    }

    cout << "  - Active Instances Post-Contract Scope: " << ExclusiveResource::activeInstances << "\n";

    // =====================================================================================
    // 4. STANDARD LIBRARY NON-COPYABLE RAII TYPES
    // =====================================================================================
    cout << "\n================ 4. STANDARD LIBRARY NON-COPYABLE RAII TYPES ================\n";

    // A. std::unique_ptr (Exclusive Memory Ownership)
    {
        cout << "  - A. `std::unique_ptr<T>` (Move-only dynamic memory handle):\n";
        auto stdUnique = std::make_unique<ExclusiveResource>(userInputValue + 30, "StdUniqueNode");
        // auto stdCopy = stdUnique; // COMPILE ERROR: std::unique_ptr copy ctor is deleted!
        stdUnique->executePayload();
    } // Auto-freed

    // B. std::ofstream (Exclusive System File Handle)
    {
        cout << "\n  - B. `std::ofstream` (Move-only file handle):\n";
        std::ofstream fileStream("non_copyable_test.txt", ios::out | ios::trunc);
        // std::ofstream copyStream = fileStream; // COMPILE ERROR: file streams are non-copyable!
        if (fileStream.is_open()) {
            fileStream << "Non-copyable file handle successfully managing system handle.\n";
            cout << "    Wrote telemetry to 'non_copyable_test.txt'\n";
        }
    } // Auto-closed

    // C. std::lock_guard (Exclusive Non-Copyable, Non-Movable Scope Lock)
    {
        cout << "\n  - C. `std::lock_guard<std::mutex>` (Scoped lock):\n";
        std::mutex mtx;
        {
            std::lock_guard<std::mutex> lock(mtx);
            // std::lock_guard<std::mutex> lockCopy = lock; // COMPILE ERROR: lock_guard is non-copyable!
            cout << "    Critical region safely protected under non-copyable std::lock_guard.\n";
        } // Auto-unlocked
    }

    cout << "\n  - Final Active Instances Count: " << ExclusiveResource::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ NON-COPYABLE RAII OBJECTS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Design Property       | Implementation Mechanism          | Primary Safety Benefit            |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Copy Suppression      | `Type(const Type&) = delete;`     | Blocks accidental duplicate copies|\n"
         << "| Copy Assignment Guard | `Type& operator=(const Type&)=del;`| Prevents double-free crashes      |\n"
         << "| Ownership Transfer    | `Type(Type&&) noexcept;`          | Enables safe move-only semantics  |\n"
         << "| Borrowing Contract    | Pass raw pointer (`const T*`/`T&`)| Allows temporary read inspection  |\n"
         << "| Sink Contract         | Pass move-only type by value      | Explicit ownership consumption    |\n"
         << "| Standard Memory       | `std::unique_ptr<T>`              | Single-owner heap memory handle   |\n"
         << "| Standard I/O Handles  | `std::ofstream`, `std::ifstream`  | Single-owner system file handle   |\n"
         << "| Standard Concurrency  | `std::lock_guard<std::mutex>`     | Scoped, non-copyable mutex lock   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}