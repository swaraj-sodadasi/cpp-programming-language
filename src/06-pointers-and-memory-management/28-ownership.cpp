/*
 * =====================================================================================
 * CONCEPT        : Ownership in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the core architectural concept of "Ownership" in C++ memory management:
 *
 *                  1. What is Ownership?         : Defining entity responsibility for the lifecycle, 
 *                                                  cleanup, and destruction of dynamic heap memory.
 *                  2. The Problem: Unclear Ownership: Hazards of raw pointers (`T*`), ambiguity 
 *                                                  over who calls `delete`, leaks, and UAF.
 *                  3. Exclusive Ownership (`unique_ptr`): Exactly one owner at any time. Non-copyable,
 *                                                  move-only semantics via `std::move`.
 *                  4. Shared Ownership (`shared_ptr`)  : Multiple co-owners sharing resource lifecycle.
 *                                                  Automated reference counting control blocks.
 *                  5. Non-Owning Observers (`weak_ptr`): Weak observer referencing `shared_ptr` memory
 *                                                  without extending lifetime; breaks circular refs.
 *                  6. Ownership Function Contracts     : Transferring ownership (sink functions), 
 *                                                  borrowing (const refs / raw observer ptrs).
 *
 * TIME COMPLEXITY  : Allocation / Move / Ref Count / Access: O(1) constant time.
 * SPACE COMPLEXITY : `unique_ptr`: 8 bytes (raw pointer size).
 *                    `shared_ptr` / `weak_ptr`: 16 bytes (pointer + control block pointer).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <utility>
#include <limits>
#include <string>

using namespace std;

// =====================================================================================
// HELPER CLASS FOR LIFECYCLE AND OWNERSHIP TRACKING
// =====================================================================================
class Resource {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    Resource(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE CREATED]  ID: " << id_ << " (" << label_ 
             << ") | Address: " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    ~Resource() {
        --activeInstances;
        cout << "    [RESOURCE DESTROYED] ID: " << id_ << " (" << label_ 
             << ") | Address: " << static_cast<const void*>(this) 
             << " | Remaining Active: " << activeInstances << "\n";
    }

    void performWork() const {
        cout << "    [RESOURCE BUSY]     Executing operation on ID: " << id_ 
             << " (" << label_ << ")\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER FUNCTIONS DEMONSTRATING OWNERSHIP FUNCTION CONTRACTS
// =====================================================================================

// Contract 1: Sink Function - Takes EXCLUSIVE OWNERSHIP (Caller loses ownership)
void consumeResource(std::unique_ptr<Resource> resource) {
    cout << "    [SINK FUNCTION] Received ownership of resource ID: " << resource->getId() << "\n";
    resource->performWork();
    cout << "    [SINK FUNCTION] Exiting scope... resource will be destroyed HERE.\n";
} // `resource` goes out of scope and frees heap memory HERE!

// Contract 2: Non-Owning Observer - Borrowing read-only access (Caller retains ownership)
void observeResource(const Resource* rawObserverPtr) {
    if (rawObserverPtr != nullptr) {
        cout << "    [OBSERVER FUNCTION] Borrowing non-owning view of ID: " 
             << rawObserverPtr->getId() << " (Caller maintains ownership)\n";
        rawObserverPtr->performWork();
    }
}

// Contract 3: Factory Function - TRANSFERS NEW OWNERSHIP to the caller
std::unique_ptr<Resource> createResourceFactory(int id, const string& label) {
    cout << "    [FACTORY] Creating new resource and relinquishing ownership to caller...\n";
    return std::make_unique<Resource>(id, label);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer ID for ownership lifecycle tracking (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. THE PROBLEM: UNCLEAR OWNERSHIP WITH RAW POINTERS
    // =====================================================================================
    cout << "\n================ 1. UNCLEAR OWNERSHIP WITH RAW POINTERS ================\n";

    cout << "  - Allocating dynamic heap object via raw pointer `Resource* rawPtr = new Resource(...)`...\n";
    Resource* ambiguityPtr = new Resource(userInputValue, "AmbiguousRawPtr");

    cout << "  - [UNCLEAR OWNERSHIP HAZARD]: Raw pointers convey NO explicit ownership semantics.\n";
    cout << "    * Who is responsible for deleting `ambiguityPtr`?\n";
    cout << "    * Is a function receiving `ambiguityPtr` supposed to delete it or just read it?\n";
    cout << "    * Forgetting `delete ambiguityPtr` leaks memory; deleting twice causes double-free crash.\n";

    // Manually clearing raw pointer ambiguity
    delete ambiguityPtr;
    ambiguityPtr = nullptr;
    cout << "  - Raw pointer manually deleted and set to `nullptr`.\n";

    // =====================================================================================
    // 2. EXCLUSIVE OWNERSHIP (`std::unique_ptr` & `std::move`)
    // =====================================================================================
    cout << "\n================ 2. EXCLUSIVE OWNERSHIP (`std::unique_ptr`) ================\n";

    cout << "  - Creating exclusive owner `ownerA` using `std::make_unique`...\n";
    std::unique_ptr<Resource> ownerA = std::make_unique<Resource>(userInputValue + 1, "ExclusiveResource");

    cout << "  - `ownerA` address held: " << static_cast<const void*>(ownerA.get()) << "\n";

    // EXCLUSIVE OWNERSHIP RULE: Cannot copy a std::unique_ptr!
    // std::unique_ptr<Resource> ownerB = ownerA; // COMPILE ERROR: Copy constructor deleted!

    cout << "\n  - [OWNERSHIP TRANSFER]: Moving ownership from `ownerA` to `ownerB` via `std::move()`...\n";
    std::unique_ptr<Resource> ownerB = std::move(ownerA); // Explicit ownership transfer

    cout << "  - Post-Move `ownerA` state : " 
         << (ownerA == nullptr ? "NULLPTR (Relinquished Ownership)" : "VALID") << "\n";
    cout << "  - Post-Move `ownerB` state : " 
         << static_cast<const void*>(ownerB.get()) << " (Now Exclusive Owner)\n";

    cout << "\n  - Passing `ownerB` into `consumeResource()` (Sink pattern transfer)...\n";
    consumeResource(std::move(ownerB)); // Transfer ownership into function

    cout << "  - Post-Sink `ownerB` state : " 
         << (ownerB == nullptr ? "NULLPTR (Resource Destructed inside Sink)" : "VALID") << "\n";

    // =====================================================================================
    // 3. SHARED OWNERSHIP (`std::shared_ptr` & REFERENCE COUNTING)
    // =====================================================================================
    cout << "\n================ 3. SHARED OWNERSHIP (`std::shared_ptr`) ================\n";

    std::shared_ptr<Resource> share1;

    {
        cout << "  - Creating first shared owner `share1` via `std::make_shared`...\n";
        share1 = std::make_shared<Resource>(userInputValue + 2, "SharedResource");

        cout << "    * `share1` Use Count = " << share1.use_count() << "\n";

        {
            cout << "  - Entering nested scope... creating co-owner `share2 = share1`...\n";
            std::shared_ptr<Resource> share2 = share1; // Shares ownership, increments reference count

            cout << "    * `share1` Use Count = " << share1.use_count() << "\n";
            cout << "    * `share2` Use Count = " << share2.use_count() << "\n";

            {
                std::shared_ptr<Resource> share3 = share2; // Third co-owner
                cout << "    * Added `share3` -> Current Use Count = " << share1.use_count() << "\n";
            } // share3 leaves scope, decrements ref count

            cout << "  - Exited `share3` scope -> Current Use Count = " << share1.use_count() << "\n";
        } // share2 leaves scope, decrements ref count

        cout << "  - Exited `share2` scope -> Current Use Count = " << share1.use_count() << "\n";
        cout << "  - [SHARED OWNERSHIP GUARANTEE]: Resource remains alive as long as Use Count > 0.\n";
    }

    cout << "  - Leaving outer block... `share1` goes out of scope and triggers destructor...\n";
    share1.reset(); // Relinquish final reference explicitly for demonstration

    // =====================================================================================
    // 4. NON-OWNING OBSERVERS (`std::weak_ptr` & RAW OBSERVER POINTERS)
    // =====================================================================================
    cout << "\n================ 4. NON-OWNING OBSERVERS (`std::weak_ptr`) ================\n";

    std::weak_ptr<Resource> weakObserver;

    {
        cout << "  - Creating `sharedOwner` and assigning non-owning observer `weakObserver`...\n";
        std::shared_ptr<Resource> sharedOwner = std::make_shared<Resource>(userInputValue + 3, "ObservedResource");
        weakObserver = sharedOwner; // Non-owning observation

        cout << "    * `sharedOwner` Strong Ref Count = " << sharedOwner.use_count() << "\n";
        cout << "    * `weakObserver.expired()`       = " << (weakObserver.expired() ? "TRUE" : "FALSE") << "\n";

        // Accessing weak_ptr data safely requires locking it into a temporary shared_ptr
        if (auto lockedShared = weakObserver.lock()) {
            cout << "    * Successfully locked `weakObserver` -> Resource ID: " << lockedShared->getId() << "\n";
            cout << "    * Temp Use Count during lock    = " << lockedShared.use_count() << "\n";
        }
        cout << "  - Exiting scope containing `sharedOwner`...\n";
    } // `sharedOwner` destroyed HERE!

    cout << "  - Post-Destruction `weakObserver.expired()` check: " 
         << (weakObserver.expired() ? "TRUE (Resource Safely Detected as Destroyed!)" : "FALSE") << "\n";

    if (auto failedLock = weakObserver.lock()) {
        failedLock->performWork();
    } else {
        cout << "  - [SAFE WEAK_PTR]: Lock failed because observed resource no longer exists (Zero dangling bugs!).\n";
    }

    // =====================================================================================
    // 5. OWNERSHIP FUNCTION CONTRACTS (FACTORY & BORROWING)
    // =====================================================================================
    cout << "\n================ 5. OWNERSHIP FUNCTION CONTRACTS ================\n";

    // A. Factory Function receiving ownership
    std::unique_ptr<Resource> factoryRes = createResourceFactory(userInputValue + 4, "FactoryResource");

    // B. Non-Owning Borrowing inspection via raw pointer
    cout << "  - Passing `factoryRes.get()` into `observeResource()` for borrowing...\n";
    observeResource(factoryRes.get());

    cout << "  - Caller STILL owns resource ID: " << factoryRes->getId() 
         << " after borrowing inspection.\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ OWNERSHIP SEMANTICS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Pointer / Smart Type  | Ownership Model   | Copyable / Move   | Primary Lifecycle Responsibility  |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Raw Pointer (`T*`)    | Unclear / Ambiguous| Copyable & Move   | Manual (`delete`) - Prone to leaks|\n"
         << "| `std::unique_ptr<T>`  | Exclusive (1 Owner)| Move-Only         | Automatic RAII single-owner free  |\n"
         << "| `std::shared_ptr<T>`  | Shared (N Owners) | Copyable & Move   | Freed when reference count == 0   |\n"
         << "| `std::weak_ptr<T>`    | Non-Owning Observer| Copyable & Move  | Non-owning view; prevents cycles  |\n"
         << "| Raw Observer (`const T*`)| Non-Owning Borrow | Copyable & Move  | Read-only view; NO deletion right |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}