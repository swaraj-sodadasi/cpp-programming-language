/*
 * =====================================================================================
 * CONCEPT        : Core Principles of RAII (Resource Acquisition Is Initialization)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the 5 fundamental architectural pillars of the RAII idiom:
 *
 *                  1. Resource Acquisition in Ctor: Resource allocation occurs strictly 
 *                                                   during object initialization.
 *                  2. Deterministic Release in Dtor: Automatic, leak-free release upon scope 
 *                                                   exit or stack unwinding.
 *                  3. Exception Safety Guarantee  : Stack unwinding ensures zero resource 
 *                                                   leaks during runtime exceptions.
 *                  4. Single/Explicit Ownership   : Preventing double-free hazards via Copy 
 *                                                   Deletion and Move Semantics (Rule of 5).
 *                  5. Transparent Access Mechanics: Overloading pointer operators (`*`, `->`) 
 *                                                   for intuitive resource usability.
 *
 * TIME COMPLEXITY  : Acquisition / Release / Move / Access: O(1) constant time.
 * SPACE COMPLEXITY : RAII Wrapper Footprint: Uniform 8 bytes (raw pointer size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <limits>
#include <mutex>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE TRACKER
// Logs creation, execution, and destruction to verify lifecycle encapsulation.
// =====================================================================================
class TrackedEntity {
private:
    int id_;
    string name_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    TrackedEntity(int id, string name) : id_(id), name_(std::move(name)) {
        ++activeCount;
        cout << "    [RESOURCE ACQUIRED]  TrackedEntity ID: " << id_ << " (" << name_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Count: " << activeCount << "\n";
    }

    ~TrackedEntity() noexcept {
        --activeCount;
        cout << "    [RESOURCE RELEASED]  TrackedEntity ID: " << id_ << " (" << name_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Count: " << activeCount << "\n";
    }

    void execute() const {
        cout << "    [RESOURCE EXECUTING] TrackedEntity ID: " << id_ << " (" << name_ << ") running payload.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getName() const { return name_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM GENERIC RAII WRAPPER (`CoreRAIIWrapper<T>`)
// Encapsulates the core technical pillars of RAII and Rule of 5 mechanics.
// =====================================================================================
template <typename T>
class CoreRAIIWrapper {
private:
    T* resource_;

public:
    // Pillar 1: Resource Acquisition Is Initialization (Ctor Acquisition)
    explicit CoreRAIIWrapper(T* resource = nullptr) : resource_(resource) {
        cout << "    [CoreRAIIWrapper Ctor] Bound resource pointer: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    // Pillar 2: Deterministic Scope Release (Dtor Release)
    ~CoreRAIIWrapper() noexcept {
        cout << "    [CoreRAIIWrapper Dtor] Scope boundary reached -> Deleting handle: " 
             << static_cast<const void*>(resource_) << "\n";
        delete resource_; // Safe delete (no-op if nullptr)
    }

    // Pillar 4A: Suppress Copy Semantics (Prevents double-free hazards)
    CoreRAIIWrapper(const CoreRAIIWrapper&) = delete;
    CoreRAIIWrapper& operator=(const CoreRAIIWrapper&) = delete;

    // Pillar 4B: Move Semantics for Clean Ownership Transfer (Rule of 5)
    CoreRAIIWrapper(CoreRAIIWrapper&& other) noexcept : resource_(other.resource_) {
        other.resource_ = nullptr; // Nullify source to transfer ownership
        cout << "    [CoreRAIIWrapper Move Ctor] Resource ownership transferred safely.\n";
    }

    CoreRAIIWrapper& operator=(CoreRAIIWrapper&& other) noexcept {
        if (this != &other) {
            delete resource_;          // Free currently held resource
            resource_ = other.resource_; // Transfer new resource
            other.resource_ = nullptr;
            cout << "    [CoreRAIIWrapper Move Assign] Existing freed, new ownership transferred.\n";
        }
        return *this;
    }

    // Pillar 5: Transparent Operator Access
    T& operator*() const { return *resource_; }
    T* operator->() const { return resource_; }

    [[nodiscard]] T* get() const { return resource_; }
    [[nodiscard]] explicit operator bool() const { return resource_ != nullptr; }
};

// =====================================================================================
// HELPER CLASS 3: NON-MEMORY RAII RESOURCE (SCOPED CONCURRENCY LOCK)
// Proves RAII applies to non-memory system state handles as well.
// =====================================================================================
class ScopedMutexLock {
private:
    mutex& mutexRef_;

public:
    explicit ScopedMutexLock(mutex& mtx) : mutexRef_(mtx) {
        mutexRef_.lock();
        cout << "    [MUTEX ACQUIRED] Lock engaged on entering critical region.\n";
    }

    ~ScopedMutexLock() noexcept {
        mutexRef_.unlock();
        cout << "    [MUTEX RELEASED] Lock disengaged upon leaving critical region.\n";
    }

    // Non-copyable, non-movable scope lock
    ScopedMutexLock(const ScopedMutexLock&) = delete;
    ScopedMutexLock& operator=(const ScopedMutexLock&) = delete;
};

// Helper function demonstrating exception safety
void exceptionUnwindingDemo(int baseId) {
    cout << "    [FUNCTION ENTER] Constructing RAII object inside exception-prone scope...\n";
    CoreRAIIWrapper<TrackedEntity> safeNode(new TrackedEntity(baseId, "UnwindProtectedNode"));
    
    safeNode->execute();

    cout << "    [EXCEPTION THROWN] Simulating runtime execution failure...\n";
    throw runtime_error("Critical error during pipeline processing!");

    // Code below is unreachable, but stack unwinding guarantees safeNode destructor runs!
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for core RAII principles analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. PILLAR 1 & 2: ACQUISITION IN CTOR & DETERMINISTIC SCOPE RELEASE IN DTOR
    // =====================================================================================
    cout << "\n================ 1. ACQUISITION IN CTOR & DETERMINISTIC SCOPE RELEASE ================\n";

    {
        cout << "  - Entering local block scope...\n";
        CoreRAIIWrapper<TrackedEntity> scopedObj(new TrackedEntity(userInputValue, "ScopedObject"));
        
        // Pillar 5: Transparent access via operator->
        scopedObj->execute();

        cout << "  - Exiting local block scope (Destructor will run immediately):\n";
    } // `scopedObj` destructor executes automatically HERE

    cout << "  - Active Instances Post-Scope Exit: " << TrackedEntity::activeCount << "\n";

    // =====================================================================================
    // 2. PILLAR 3: EXCEPTION SAFETY & AUTOMATIC STACK UNWINDING
    // =====================================================================================
    cout << "\n================ 2. EXCEPTION SAFETY & AUTOMATIC STACK UNWINDING ================\n";

    try {
        cout << "  - Executing `exceptionUnwindingDemo()` inside try block...\n";
        exceptionUnwindingDemo(userInputValue + 1);
    } catch (const exception& e) {
        cout << "  - [CATCH BLOCK EXECUTED] Caught Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post-Exception: " << TrackedEntity::activeCount 
         << " (RAII GUARANTEE: Zero memory leaks!)\n";

    // =====================================================================================
    // 3. PILLAR 4: SINGLE OWNERSHIP & MOVE SEMANTICS (RULE OF 5)
    // =====================================================================================
    cout << "\n================ 3. OWNERSHIP CONTROL & MOVE SEMANTICS ================\n";

    {
        cout << "  - Creating original owner `ownerA`...\n";
        CoreRAIIWrapper<TrackedEntity> ownerA(new TrackedEntity(userInputValue + 2, "MovableEntity"));

        cout << "  - Transferring resource ownership from `ownerA` to `ownerB` via `std::move()`...\n";
        CoreRAIIWrapper<TrackedEntity> ownerB = std::move(ownerA);

        cout << "  - `ownerA` valid check : " << (ownerA ? "VALID" : "NULLPTR (Ownership Relinquished)") << "\n";
        cout << "  - `ownerB` valid check : " << (ownerB ? "VALID (Current Resource Owner)" : "NULLPTR") << "\n";

        cout << "  - Exiting scope (Only `ownerB` will free the underlying resource once):\n";
    }

    cout << "  - Active Instances Post-Move Scope: " << TrackedEntity::activeCount << "\n";

    // =====================================================================================
    // 4. NON-MEMORY SYSTEM RESOURCES (SCOPED LOCKING)
    // =====================================================================================
    cout << "\n================ 4. NON-MEMORY SYSTEM RESOURCES (SCOPED MUTEX LOCK) ================\n";

    mutex sharedResourceMutex;

    {
        cout << "  - Entering critical section block...\n";
        ScopedMutexLock lock(sharedResourceMutex);

        cout << "  - Performing thread-safe critical region work...\n";

        cout << "  - Leaving critical section block...\n";
    } // ScopedMutexLock destructor unlocks mutex automatically HERE

    // =====================================================================================
    // 5. STANDARD LIBRARY IDIOMATIC RAII (`std::unique_ptr` & `std::lock_guard`)
    // =====================================================================================
    cout << "\n================ 5. STANDARD LIBRARY IDIOMATIC RAII ================\n";

    {
        cout << "  - Creating `std::unique_ptr<TrackedEntity>` using `std::make_unique`...\n";
        auto stdSmartPtr = std::make_unique<TrackedEntity>(userInputValue + 3, "StdUniqueNode");
        stdSmartPtr->execute();

        cout << "\n  - Creating `std::lock_guard<std::mutex>` for scoped lock management...\n";
        std::lock_guard<std::mutex> stdLock(sharedResourceMutex);
        cout << "    Critical section safely managed by std::lock_guard.\n";

        cout << "\n  - Exiting scope (Standard library destructors execute automatically):\n";
    }

    cout << "  - Final Active Instances Count: " << TrackedEntity::activeCount << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CORE PRINCIPLES OF RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Core RAII Principle   | Technical Implementation          | Primary Safety Benefit & Goal     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| 1. Acquisition Ctor   | `Wrapper(T* p) : res_(p) {}`      | Binds resource directly to scope  |\n"
         << "| 2. Release Dtor       | `~Wrapper() { delete res_; }`     | Guarantees automatic scope cleanup|\n"
         << "| 3. Exception Safety   | Relies on C++ Stack Unwinding     | Prevents memory/handle leaks      |\n"
         << "| 4. Copy Deletion      | `Wrapper(const Wrapper&) = delete`| Prevents double-free crash bugs   |\n"
         << "| 5. Move Semantics     | `Wrapper(Wrapper&&) noexcept`     | Safe ownership transfer (Rule of 5|\n"
         << "| 6. Operator Access    | Overload `operator->` & `*`       | Natural smart-pointer transparency|\n"
         << "| 7. General Utility    | Files, Mutex Locks, Sockets, Heap | Standardized resource management  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}