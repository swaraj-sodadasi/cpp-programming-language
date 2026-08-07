/*
 * =====================================================================================
 * CONCEPT        : Move-Only RAII Objects in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architecture, lifetime invariants, and usage patterns of move-only 
 *                  Resource Acquisition Is Initialization (RAII) abstractions:
 *
 *                  1. Move-Only Invariants       : Prohibiting copy construction and copy assignment 
 *                                                   (`= delete`) while enabling explicit, zero-cost 
 *                                                   ownership transfers via `noexcept` move operations.
 *                  2. Source Disarming Mechanics : Nullifying/invalidating source handles during move 
 *                                                   operations to guarantee single ownership and 
 *                                                   prevent double-free deallocation crashes.
 *                  3. Function Contracts         :
 *                     - Factory Functions        : Returning move-only RAII objects by value.
 *                     - Sink Functions           : Consuming exclusive ownership by value (`std::move`).
 *                     - Borrowing Functions      : Lending temporary non-owning observer access (`const T&`).
 *                  4. STL Container Integration  : Storing move-only RAII handles inside standard 
 *                                                   containers (`std::vector`) via `emplace_back` and `std::move`.
 *                  5. Standard Move-Only Types   : Demonstrating STL move-only abstractions (`std::unique_ptr`, 
 *                                                   `std::thread`, `std::ofstream`).
 *
 * TIME COMPLEXITY  : Construction / Destruction / Ownership Transfer (Move): O(1) constant time.
 * SPACE COMPLEXITY : Move-Only Wrapper Footprint: Uniform 8 bytes (raw pointer/handle size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <thread>
#include <fstream>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE WITH ACTIVE INSTANCE TRACKING
// Logs lifecycle milestones to visually demonstrate real-time ownership transfers.
// =====================================================================================
class ManagedTrackedResource {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static instance counter

    ManagedTrackedResource(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE CREATED]   ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    ~ManagedTrackedResource() noexcept {
        --activeInstances;
        cout << "    [RESOURCE DESTROYED] ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    void processWork() const {
        cout << "    [WORK EXECUTED]     Resource ID: " << id_ << " (" << label_ << ") running payload.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM MOVE-ONLY RAII WRAPPER (`MoveOnlyResourceHandle`)
// Production-grade implementation of a custom move-only RAII resource manager.
// =====================================================================================
class MoveOnlyResourceHandle {
private:
    ManagedTrackedResource* resource_;

public:
    // 1. Constructor: Acquires raw resource handle
    explicit MoveOnlyResourceHandle(ManagedTrackedResource* res = nullptr) noexcept 
        : resource_(res) {
        cout << "    [MoveOnlyHandle Ctor] Bound handle to resource: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    // 2. Destructor: Deterministic cleanup upon scope boundary exit
    ~MoveOnlyResourceHandle() noexcept {
        cout << "    [MoveOnlyHandle Dtor] Scope exit detected -> Freeing resource: " 
             << static_cast<const void*>(resource_) << "\n";
        delete resource_; // Safe delete (guaranteed no-op if resource_ is nullptr)
    }

    // =================================================================================
    // PROHIBIT COPY SEMANTICS (ENFORCE EXCLUSIVE SINGLE OWNERSHIP)
    // =================================================================================
    MoveOnlyResourceHandle(const MoveOnlyResourceHandle&) = delete;
    MoveOnlyResourceHandle& operator=(const MoveOnlyResourceHandle&) = delete;

    // =================================================================================
    // ENABLE MOVE SEMANTICS (ALLOW OWNERSHIP TRANSFER & SOURCE DISARMING)
    // =================================================================================
    
    // Move Constructor: Transfers resource pointer and nullifies source to disarm it
    MoveOnlyResourceHandle(MoveOnlyResourceHandle&& other) noexcept 
        : resource_(other.resource_) {
        other.resource_ = nullptr; // DISARM SOURCE: Relinquishes ownership to prevent double-free
        cout << "    [MoveOnlyHandle Move Ctor] Resource ownership transferred successfully.\n";
    }

    // Move Assignment Operator: Cleans up held resource, transfers handle, and disarms source
    MoveOnlyResourceHandle& operator=(MoveOnlyResourceHandle&& other) noexcept {
        if (this != &other) { // Self-assignment guard
            delete resource_;          // Step 1: Free currently held resource
            resource_ = other.resource_; // Step 2: Acquire target resource
            other.resource_ = nullptr;   // Step 3: Disarm source handle
            cout << "    [MoveOnlyHandle Move Assign] Existing resource freed, new ownership transferred.\n";
        }
        return *this;
    }

    // Transparent Access Operators
    ManagedTrackedResource& operator*() const { return *resource_; }
    ManagedTrackedResource* operator->() const { return resource_; }

    // Accessors and Utility State Inspection
    [[nodiscard]] ManagedTrackedResource* get() const noexcept { return resource_; }
    [[nodiscard]] explicit operator bool() const noexcept { return resource_ != nullptr; }
};

// =====================================================================================
// FUNCTION OWNERSHIP CONTRACT DEMONSTRATIONS
// =====================================================================================

// Contract 1: Factory Function - Generates and TRANSFERS NEW OWNERSHIP to caller
MoveOnlyResourceHandle createResourceFactory(int id, const string& label) {
    cout << "    [FACTORY FUNCTION] Instantiating resource and transferring ownership to caller...\n";
    return MoveOnlyResourceHandle(new ManagedTrackedResource(id, label)); // RVO / Move Return
}

// Contract 2: Sink Function - Consumes EXCLUSIVE OWNERSHIP by value (Caller MUST use std::move)
void consumeResourceSink(MoveOnlyResourceHandle sinkHandle) {
    if (sinkHandle) {
        cout << "    [SINK FUNCTION] Received ownership of Resource ID: " << sinkHandle->getId() << "\n";
        sinkHandle->processWork();
        cout << "    [SINK FUNCTION] Exiting scope... Resource will be destructed HERE.\n";
    }
} // `sinkHandle` destructor executes HERE!

// Contract 3: Borrowing Function - Non-Owning View (Caller retains ownership)
void observeBorrowedResource(const MoveOnlyResourceHandle& borrowedRef) {
    if (borrowedRef) {
        cout << "    [BORROWED OBSERVER] Inspecting Resource ID: " << borrowedRef->getId() 
             << " (Caller retains ownership)\n";
        borrowedRef->processWork();
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for move-only RAII testing (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC MOVE-ONLY LIFECYCLE & COMPILE-TIME COPY PREVENTION
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME COPY PREVENTION & BASIC LIFECYCLE ================\n";

    {
        cout << "  - Instantiating primary move-only handle `handleA`...\n";
        MoveOnlyResourceHandle handleA(new ManagedTrackedResource(userInputValue, "MoveOnlyNodeA"));

        handleA->processWork();

        // COMPILE-TIME SAFETY GUARD PROOF:
        // Attempting copy construction or copy assignment triggers a compilation error:
        // MoveOnlyResourceHandle handleCopy = handleA; // COMPILE ERROR: Copy constructor is deleted!
        // handleCopy = handleA;                       // COMPILE ERROR: Copy assignment operator is deleted!

        cout << "  - [COMPILE GUARANTEE]: Copy attempts are blocked at compile-time by `= delete`.\n";
        cout << "  - Exiting scope containing `handleA`...\n";
    } // `handleA` destructor executes automatically HERE!

    cout << "  - Active Instances Post-Scope: " << ManagedTrackedResource::activeInstances << "\n";

    // =====================================================================================
    // 2. EXPLICIT OWNERSHIP TRANSFER & SOURCE DISARMING MECHANICS
    // =====================================================================================
    cout << "\n================ 2. OWNERSHIP TRANSFER & SOURCE DISARMING ================\n";

    {
        cout << "  - Creating original owner `sourceHandle`...\n";
        MoveOnlyResourceHandle sourceHandle(new ManagedTrackedResource(userInputValue + 10, "MovableNodeB"));

        cout << "  - Initial `sourceHandle` State : " 
             << (sourceHandle ? "VALID (Holds Resource)" : "DISARMED") << "\n";
        cout << "  - Initial Target Memory Address: " 
             << static_cast<const void*>(sourceHandle.get()) << "\n";

        cout << "\n  - Transferring ownership to `targetHandle` via Move Constructor (`std::move`)...\n";
        MoveOnlyResourceHandle targetHandle = std::move(sourceHandle);

        cout << "\n  - Post-Move `sourceHandle` State : " 
             << (sourceHandle ? "VALID" : "DISARMED / NULLPTR (Ownership Relinquished)") << "\n";
        cout << "  - Post-Move `targetHandle` Target: " 
             << static_cast<const void*>(targetHandle.get()) << " (Current Exclusive Owner)\n";

        targetHandle->processWork();

        cout << "\n  - Re-assigning `targetHandle` using Move Assignment operator...\n";
        MoveOnlyResourceHandle freshHandle(new ManagedTrackedResource(userInputValue + 11, "ReplacementNodeC"));
        targetHandle = std::move(freshHandle); // Frees MovableNodeB, acquires ReplacementNodeC

        cout << "  - Exiting scope containing active handles...\n";
    } // Active handles destructed automatically HERE!

    cout << "  - Active Instances Post-Move Scope: " << ManagedTrackedResource::activeInstances << "\n";

    // =====================================================================================
    // 3. FUNCTION OWNERSHIP CONTRACTS (FACTORY, SINK, & BORROWING)
    // =====================================================================================
    cout << "\n================ 3. FUNCTION OWNERSHIP CONTRACTS ================\n";

    // Pattern A: Factory Contract (Returns move-only handle)
    cout << "  - Pattern A: Invoking `createResourceFactory()`...\n";
    MoveOnlyResourceHandle factoryHandle = createResourceFactory(userInputValue + 20, "FactoryNode");

    // Pattern B: Borrowing Contract (Lends non-owning reference)
    cout << "\n  - Pattern B: Borrowing handle via `observeBorrowedResource(factoryHandle)`...\n";
    observeBorrowedResource(factoryHandle);
    cout << "  - Caller STILL maintains ownership of Resource ID: " << factoryHandle->getId() << "\n";

    // Pattern C: Sink Contract (Consumes exclusive ownership via std::move)
    cout << "\n  - Pattern C: Passing handle into `consumeResourceSink(std::move(factoryHandle))`...\n";
    consumeResourceSink(std::move(factoryHandle));

    cout << "  - Post-Sink `factoryHandle` State: " 
         << (factoryHandle ? "VALID" : "DISARMED / NULLPTR (Ownership Transferred & Consumed)") << "\n";

    cout << "  - Active Instances Post-Contract Operations: " << ManagedTrackedResource::activeInstances << "\n";

    // =====================================================================================
    // 4. STL CONTAINER INTEGRATION FOR MOVE-ONLY RAII OBJECTS
    // =====================================================================================
    cout << "\n================ 4. STL CONTAINER INTEGRATION (`std::vector`) ================\n";

    {
        cout << "  - Creating `std::vector<MoveOnlyResourceHandle>`...\n";
        std::vector<MoveOnlyResourceHandle> handleVector;
        handleVector.reserve(2); // Pre-allocate buffer to prevent reallocation moves

        cout << "\n  - Method 1: Inserting element using `emplace_back()` (In-place construction):\n";
        handleVector.emplace_back(new ManagedTrackedResource(userInputValue + 30, "VectorEmplacedNode"));

        cout << "\n  - Method 2: Inserting element using `push_back(std::move(...))`:\n";
        MoveOnlyResourceHandle tempHandle(new ManagedTrackedResource(userInputValue + 31, "VectorMovedNode"));
        handleVector.push_back(std::move(tempHandle));

        cout << "\n  - Iterating and executing payload across move-only vector elements:\n";
        for (const auto& handle : handleVector) {
            handle->processWork();
        }

        cout << "\n  - Exiting vector scope (Vector destructor will free all move-only handles)...\n";
    } // All elements inside handleVector destructed automatically HERE!

    cout << "  - Active Instances Post-Vector Scope: " << ManagedTrackedResource::activeInstances << "\n";

    // =====================================================================================
    // 5. STANDARD LIBRARY BUILT-IN MOVE-ONLY RAII ABSTRACTIONS
    // =====================================================================================
    cout << "\n================ 5. STANDARD LIBRARY MOVE-ONLY RAII ABSTRACTIONS ================\n";

    // A. std::unique_ptr (Move-only dynamic memory handle)
    {
        cout << "  - A. `std::unique_ptr<T>` (Exclusive Move-Only Memory Handle):\n";
        auto uniquePtrA = std::make_unique<ManagedTrackedResource>(userInputValue + 40, "StdUniqueNode");
        
        // Ownership transfer via std::move
        std::unique_ptr<ManagedTrackedResource> uniquePtrB = std::move(uniquePtrA);
        uniquePtrB->processWork();
        cout << "    `uniquePtrA` is null: " << (uniquePtrA == nullptr ? "TRUE" : "FALSE") << "\n";
    } // Auto-freed

    // B. std::ofstream (Move-only system file handle)
    {
        cout << "\n  - B. `std::ofstream` (Exclusive Move-Only File Handle):\n";
        std::ofstream fileA("move_only_demo.txt", ios::out | ios::trunc);
        if (fileA.is_open()) {
            fileA << "Writing data using original file handle.\n";
        }

        // Move ownership of active file handle to fileB
        std::ofstream fileB = std::move(fileA);
        if (fileB.is_open()) {
            fileB << "Writing data using moved file handle.\n";
            cout << "    Successfully transferred OS file handle to `fileB` and wrote payload.\n";
        }
    } // Auto-closed

    // C. std::thread (Move-only OS thread handle)
    {
        cout << "\n  - C. `std::thread` (Exclusive Move-Only Thread Execution Handle):\n";
        int workerId = userInputValue + 50;
        
        std::thread threadA([workerId]() {
            cout << "    [BACKGROUND THREAD] Asynchronous worker thread " << workerId << " executing...\n";
        });

        // Transfer thread execution ownership to threadB
        std::thread threadB = std::move(threadA);
        
        if (threadB.joinable()) {
            threadB.join(); // Safely join moved thread
            cout << "    [MAIN THREAD] Joined moved worker thread successfully.\n";
        }
    }

    cout << "\n  - Final Active Resource Instances Count: " << ManagedTrackedResource::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MOVE-ONLY RAII OBJECTS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Move-Only Property    | Implementation Mechanics          | Operational Safety Trait          |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Copy Suppression      | `Type(const Type&) = delete;`     | Blocks accidental duplicate copies|\n"
         << "| Move Constructor      | `Type(Type&&) noexcept;`          | Transfers pointer & disarms source|\n"
         << "| Move Assignment       | `Type& operator=(Type&&) noexcept;`| Frees existing, transfers handle  |\n"
         << "| Source Disarming      | `other.resource_ = nullptr;`      | Prevents double-free crash bugs   |\n"
         << "| Sink Contract         | Pass move-only type by value      | Consumes exclusive ownership      |\n"
         << "| Borrowing Contract    | Pass by reference (`const Type&`) | Temporary read access; NO deletion|\n"
         << "| Container Storage     | `vector.push_back(std::move(p))`  | Enables efficient STL storage     |\n"
         << "| Standard Move-Only RAII| `unique_ptr`, `thread`, `ofstream` | Standardized single-owner handles |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}