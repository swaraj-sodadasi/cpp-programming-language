/*
 * =====================================================================================
 * CONCEPT        : Managing Multiple Resources in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how Modern C++ handles multi-resource acquisition, lifecycle dependency,
 *                  and exception safety using the Resource Acquisition Is Initialization (RAII) idiom:
 *
 *                  1. The Multi-Resource Hazard   : Acquiring multiple raw resources sequentially in 
 *                                                   a raw constructor leads to leaks if intermediate 
 *                                                   allocations throw exceptions.
 *                  2. Member-Level RAII Composition: Structuring classes so that every resource member 
 *                                                   is an independent RAII object (`std::unique_ptr`, 
 *                                                   `std::ofstream`, lock guards).
 *                  3. Automatic Sub-Object Rollback: Utilizing C++ stack unwinding to automatically 
 *                                                   destruct previously initialized members if a 
 *                                                   subsequent member constructor throws.
 *                  4. Composite Multi-Handle Scope : Combining heterogeneous system handles 
 *                                                   (memory, mutex locks, file descriptors) into 
 *                                                   a single transactional scope wrapper.
 *                  5. Deadlock-Free Multi-Locking  : Acquiring multiple concurrency locks 
 *                                                   simultaneously via C++17 `std::scoped_lock`.
 *
 * TIME COMPLEXITY  : Acquisition / Release per resource: O(1) constant time.
 * SPACE COMPLEXITY : Composite Wrapper Footprint: Sum of individual RAII handle sizes (typically 8-24 bytes).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <mutex>
#include <fstream>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE WITH ACTIVE INSTANCE TRACKING
// Logs creation, payload processing, and destruction to trace multi-resource lifecycles.
// =====================================================================================
class ResourceNode {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    ResourceNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE ACQUIRED] ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    ~ResourceNode() noexcept {
        --activeInstances;
        cout << "    [RESOURCE FREED]    ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    void execute() const {
        cout << "    [PAYLOAD EXECUTED]  Resource ID: " << id_ << " (" << label_ << ") running task.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: ANTI-PATTERN DEMONSTRATION (RAW MULTI-RESOURCE HOLDER)
// Demonstrates how raw pointers cause severe memory leaks when intermediate acquisitions fail.
// =====================================================================================
class UnsafeRawMultiHolder {
private:
    ResourceNode* res1_;
    ResourceNode* res2_;
    ResourceNode* res3_;

public:
    UnsafeRawMultiHolder(int id1, int id2, int id3, bool failAtThird)
        : res1_(nullptr), res2_(nullptr), res3_(nullptr) {
        
        cout << "    [UNSAFE CTOR] Acquiring Resource 1...\n";
        res1_ = new ResourceNode(id1, "RawResource_1");

        cout << "    [UNSAFE CTOR] Acquiring Resource 2...\n";
        res2_ = new ResourceNode(id2, "RawResource_2");

        if (failAtThird) {
            cout << "    [UNSAFE CTOR EXCEPTION] Failure triggered before Resource 3 acquisition!\n";
            // HAZARD: Exception thrown here unwinds stack frame, BUT destructor ~UnsafeRawMultiHolder() 
            // is NEVER executed because object construction did NOT complete!
            // Result: res1_ and res2_ are PERMANENTLY LEAKED on the heap!
            throw runtime_error("Simulated failure in raw multi-resource constructor!");
        }

        cout << "    [UNSAFE CTOR] Acquiring Resource 3...\n";
        res3_ = new ResourceNode(id3, "RawResource_3");
    }

    ~UnsafeRawMultiHolder() noexcept {
        cout << "    [UNSAFE DTOR] Cleaning up raw resources...\n";
        delete res1_;
        delete res2_;
        delete res3_;
    }
};

// =====================================================================================
// HELPER CLASS 3: SAFE MULTI-RESOURCE HOLDER (MEMBER-LEVEL RAII COMPOSITION)
// Production-Grade Solution: Every member is an independent RAII smart pointer.
// =====================================================================================
class SafeRaiiMultiHolder {
private:
    std::unique_ptr<ResourceNode> primaryRes_;
    std::unique_ptr<ResourceNode> secondaryRes_;
    std::unique_ptr<ResourceNode> tertiaryRes_;

public:
    // Safe Acquisition: Even if third acquisition fails, C++ stack unwinding guarantees 
    // that primaryRes_ and secondaryRes_ destructors run automatically!
    SafeRaiiMultiHolder(int id1, int id2, int id3, bool failAtThird) {
        cout << "    [SAFE CTOR] Acquiring Primary Resource via std::make_unique...\n";
        primaryRes_ = std::make_unique<ResourceNode>(id1, "SafeResource_1");

        cout << "    [SAFE CTOR] Acquiring Secondary Resource via std::make_unique...\n";
        secondaryRes_ = std::make_unique<ResourceNode>(id2, "SafeResource_2");

        if (failAtThird) {
            cout << "    [SAFE CTOR EXCEPTION] Triggering simulated failure before Tertiary acquisition!\n";
            throw runtime_error("Simulated failure in safe multi-resource constructor!");
        }

        cout << "    [SAFE CTOR] Acquiring Tertiary Resource via std::make_unique...\n";
        tertiaryRes_ = std::make_unique<ResourceNode>(id3, "SafeResource_3");
        cout << "    [SAFE CTOR] All multiple resources acquired successfully.\n";
    }

    ~SafeRaiiMultiHolder() noexcept {
        cout << "    [SAFE DTOR] SafeRaiiMultiHolder container destructor body executing...\n";
    } // Member sub-objects primaryRes_, secondaryRes_, tertiaryRes_ destruct automatically in reverse order!

    void processAll() const {
        if (primaryRes_) primaryRes_->execute();
        if (secondaryRes_) secondaryRes_->execute();
        if (tertiaryRes_) tertiaryRes_->execute();
    }
};

// =====================================================================================
// HELPER CLASS 4: HETEROGENEOUS MULTI-SYSTEM-HANDLE TRANSACTION SCOPE
// Manages dynamic memory, thread concurrency mutex, and file I/O handles concurrently.
// =====================================================================================
class CompositeTransactionScope {
private:
    std::unique_ptr<ResourceNode> heapPayload_; // Resource 1: Dynamic Memory
    std::unique_lock<mutex> lockGuard_;          // Resource 2: Thread Mutex Lock
    ofstream logStream_;                         // Resource 3: File Handle

public:
    CompositeTransactionScope(int resId, mutex& mtx, const string& logFilename)
        : heapPayload_(std::make_unique<ResourceNode>(resId, "TransactionPayload")),
          lockGuard_(mtx), // Acquires lock immediately on constructor entry
          logStream_(logFilename, ios::out | ios::app) {
        
        if (!logStream_.is_open()) {
            throw runtime_error("Failed to acquire log file resource: " + logFilename);
        }

        logStream_ << "[TRANSACTION INIT] Transaction scope created for ID: " << resId << "\n";
        cout << "    [COMPOSITE SCOPE INIT] Memory, Mutex Lock, and File Handle acquired cleanly.\n";
    }

    ~CompositeTransactionScope() noexcept {
        if (logStream_.is_open()) {
            logStream_ << "[TRANSACTION COMMIT] Transaction scope exiting cleanly.\n";
            logStream_.flush();
            logStream_.close();
        }
        cout << "    [COMPOSITE SCOPE EXIT] File closed, lock released, and memory freed automatically.\n";
    }

    void executeTransaction(const string& logData) {
        if (heapPayload_) {
            heapPayload_->execute();
        }
        if (logStream_.is_open()) {
            logStream_ << "[TRANSACTION PAYLOAD] " << logData << "\n";
            cout << "    [TRANSACTION LOGGED] Wrote payload to transaction file stream.\n";
        }
    }

    // Non-copyable, non-movable scope container
    CompositeTransactionScope(const CompositeTransactionScope&) = delete;
    CompositeTransactionScope& operator=(const CompositeTransactionScope&) = delete;
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for multi-resource RAII testing (e.g., 700): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 700." << endl;
        userInputValue = 700;
    }

    // =====================================================================================
    // 1. THE MULTI-RESOURCE HAZARD (UNSAFE RAW POINTER ALLOCATION)
    // =====================================================================================
    cout << "\n================ 1. THE MULTI-RESOURCE HAZARD (RAW POINTER FAILURE) ================\n";

    cout << "  - Initial Active Resource Count: " << ResourceNode::activeInstances << "\n";

    try {
        cout << "  - Attempting multi-resource acquisition with raw pointers (failAtThird = true)...\n";
        UnsafeRawMultiHolder unsafeHolder(userInputValue, userInputValue + 1, userInputValue + 2, true);
    } catch (const std::exception& e) {
        cout << "  - Caught Expected Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - [HAZARD CONFIRMED] Active Resource Count Post Raw Exception = " 
         << ResourceNode::activeInstances << " (PERMANENT LEAK: Raw resources 1 & 2 leaked!)\n";

    // =====================================================================================
    // 2. SOLUTION: MEMBER-LEVEL RAII COMPOSITION (AUTOMATIC ROLLBACK)
    // =====================================================================================
    cout << "\n================ 2. MEMBER-LEVEL RAII COMPOSITION (AUTOMATIC ROLLBACK) ================\n";

    try {
        cout << "  - Attempting multi-resource acquisition with RAII smart members (failAtThird = true)...\n";
        SafeRaiiMultiHolder safeFailingHolder(userInputValue + 10, userInputValue + 11, userInputValue + 12, true);
    } catch (const std::exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - [RAII ROLLBACK GUARANTEE] Active Resource Count Post Safe Exception = " 
         << ResourceNode::activeInstances << " (ZERO LEAKS: Smart members auto-freed during unwind!)\n";

    cout << "\n  - Attempting multi-resource acquisition with RAII smart members (failAtThird = false)...\n";
    {
        SafeRaiiMultiHolder safeSuccessHolder(userInputValue + 20, userInputValue + 21, userInputValue + 22, false);
        safeSuccessHolder.processAll();
        cout << "  - Exiting scope containing successful safe holder...\n";
    } // All 3 resources destructed automatically HERE!

    cout << "  - Active Resource Count Post Normal Scope = " << ResourceNode::activeInstances << "\n";

    // =====================================================================================
    // 3. COMPOSITE HETEROGENEOUS MULTI-SYSTEM-HANDLE SCOPE
    // =====================================================================================
    cout << "\n================ 3. HETEROGENEOUS MULTI-SYSTEM-HANDLE SCOPE ================\n";

    mutex transactionMutex;

    {
        cout << "  - Entering transaction scope managing Memory, Mutex, and File handle concurrently...\n";
        CompositeTransactionScope txScope(userInputValue + 30, transactionMutex, "tx_audit.log");

        txScope.executeTransaction("Executing financial transaction batch update...");
        cout << "  - Exiting transaction scope...\n";
    } // File closed, mutex unlocked, and heap memory freed simultaneously HERE!

    // =====================================================================================
    // 4. ATOMIC DEADLOCK-FREE MULTI-MUTEX LOCKING (`std::scoped_lock`)
    // =====================================================================================
    cout << "\n================ 4. ATOMIC MULTI-RESOURCE LOCKING (`std::scoped_lock`) ================\n";

    mutex resourceMutexA;
    mutex resourceMutexB;
    mutex resourceMutexC;

    {
        cout << "  - Acquiring 3 distinct mutex resources simultaneously via C++17 `std::scoped_lock`...\n";
        
        // std::scoped_lock locks all 3 mutexes atomically using a deadlock-avoidance algorithm
        std::scoped_lock<mutex, mutex, mutex> multiLock(resourceMutexA, resourceMutexB, resourceMutexC);

        cout << "    [SCOPED_LOCK ACQUIRED] Mutexes A, B, and C locked safely without deadlock risk.\n";
        cout << "    Executing critical multi-resource concurrency workload...\n";

        cout << "  - Exiting multi-lock scope...\n";
    } // All 3 mutexes unlocked simultaneously on scope exit HERE!

    cout << "  - All multi-resource locks released successfully.\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MANAGING MULTIPLE RESOURCES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Multi-Resource Model  | Implementation Pattern            | Primary Operational Safety Trait  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Raw Multi-Allocation  | Raw pointers in single Ctor       | DANGEROUS: Leaks if Ctor throws   |\n"
         << "| Member RAII Composition| `std::unique_ptr<T> m1_, m2_;`     | SAFE: Auto-rollback on unwind     |\n"
         << "| Heterogeneous Composite| Memory + Mutex + File in 1 Scope | Unified transactional lifetime    |\n"
         << "| Multi-Mutex Lock      | `std::scoped_lock lock(m1, m2);`  | Deadlock-free atomic acquisition  |\n"
         << "| Tuple / Aggregation   | `std::tuple<RAII1, RAII2>`        | Grouped lifecycle encapsulation   |\n"
         << "| Exception Safety      | C++ Stack Unwinding               | Zero leaks across all resources   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}