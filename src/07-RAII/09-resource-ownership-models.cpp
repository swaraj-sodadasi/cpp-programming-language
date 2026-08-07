/*
 * =====================================================================================
 * CONCEPT        : Resource Ownership Models in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the primary resource ownership models implemented via the RAII idiom:
 *
 *                  1. Exclusive Ownership       : Exactly one RAII object owns the resource 
 *                                                 at any time (`std::unique_ptr`). Non-copyable, 
 *                                                 movable semantics.
 *                  2. Shared Ownership          : Multiple co-owners share resource lifecycle 
 *                                                 via reference counting (`std::shared_ptr`). 
 *                                                 Destruction occurs when active owners drop to 0.
 *                  3. Non-Owning Observers      : Observing managed resources without taking 
 *                                                 ownership or extending lifetime (`std::weak_ptr` 
 *                                                 and raw observer pointers `const T*`).
 *                  4. Ownership Transfer (Sink) : Explicit transfer of control using Move Semantics 
 *                                                 (`std::move`) into functions or containers.
 *                  5. Borrowing / Lending Views : Function contracts lending access via const 
 *                                                 references/pointers without transferring ownership.
 *
 * TIME COMPLEXITY  : Acquisition / Release / Move / Ref-Count Mutation: O(1) constant time.
 * SPACE COMPLEXITY : Exclusive Wrapper: 8 bytes (raw handle footprint).
 *                    Shared Wrapper   : 16 bytes (raw handle + control block pointer).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE WITH ACTIVE LIFECYCLE TRACKING
// Logs creation, access, and destruction to trace ownership transitions in real-time.
// =====================================================================================
class OwnedResource {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static variable

    OwnedResource(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE CREATED]   ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~OwnedResource() noexcept {
        --activeInstances;
        cout << "    [RESOURCE DESTROYED] ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
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
// HELPER CLASS 2: CUSTOM EXCLUSIVE RAII WRAPPER (`ExclusiveHandle<T>`)
// Encapsulates standard exclusive ownership semantics (Move-only RAII).
// =====================================================================================
template <typename T>
class ExclusiveHandle {
private:
    T* resource_;

public:
    explicit ExclusiveHandle(T* res = nullptr) : resource_(res) {
        cout << "    [ExclusiveHandle Ctor] Acquired exclusive ownership of: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    ~ExclusiveHandle() noexcept {
        cout << "    [ExclusiveHandle Dtor] Releasing exclusive resource: " 
             << static_cast<const void*>(resource_) << "\n";
        delete resource_;
    }

    // EXCLUSIVE RULE 1: Suppress Copy Semantics (Cannot have multiple exclusive owners)
    ExclusiveHandle(const ExclusiveHandle&) = delete;
    ExclusiveHandle& operator=(const ExclusiveHandle&) = delete;

    // EXCLUSIVE RULE 2: Enable Move Semantics (Transfer ownership explicitly)
    ExclusiveHandle(ExclusiveHandle&& other) noexcept : resource_(other.resource_) {
        other.resource_ = nullptr;
        cout << "    [ExclusiveHandle Move Ctor] Exclusive ownership transferred.\n";
    }

    ExclusiveHandle& operator=(ExclusiveHandle&& other) noexcept {
        if (this != &other) {
            delete resource_;          // Release existing resource
            resource_ = other.resource_; // Transfer new resource
            other.resource_ = nullptr;
            cout << "    [ExclusiveHandle Move Assign] Previous freed, ownership transferred.\n";
        }
        return *this;
    }

    T& operator*() const { return *resource_; }
    T* operator->() const { return resource_; }
    [[nodiscard]] T* get() const { return resource_; }
    [[nodiscard]] explicit operator bool() const { return resource_ != nullptr; }
};

// =====================================================================================
// HELPER FUNCTIONS FOR OWNERSHIP FUNCTION CONTRACTS
// =====================================================================================

// Contract A: Sink Function - Consumes EXCLUSIVE Ownership (Caller loses ownership)
void consumeOwnershipSink(std::unique_ptr<OwnedResource> res) {
    if (res) {
        cout << "    [SINK FUNCTION] Received ownership of Resource ID: " << res->getId() << "\n";
        res->executePayload();
        cout << "    [SINK FUNCTION] Exiting scope... Resource will be destroyed HERE.\n";
    }
} // `res` destructor runs HERE!

// Contract B: Borrowing Function - Non-Owning View (Caller maintains ownership)
void observeBorrowedResource(const OwnedResource* rawObserver) {
    if (rawObserver != nullptr) {
        cout << "    [BORROWED OBSERVER] Inspecting Resource ID: " << rawObserver->getId() 
             << " (Owner remains external)\n";
        rawObserver->executePayload();
    }
}

// Contract C: Factory Function - Generates and TRANSFERS NEW OWNERSHIP to caller
std::unique_ptr<OwnedResource> createResourceFactory(int id, const string& label) {
    cout << "    [FACTORY FUNCTION] Instantiating and transferring ownership to caller...\n";
    return std::make_unique<OwnedResource>(id, label);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for resource ownership testing (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. EXCLUSIVE OWNERSHIP MODEL (`std::unique_ptr` & Custom Wrapper)
    // =====================================================================================
    cout << "\n================ 1. EXCLUSIVE OWNERSHIP MODEL (`std::unique_ptr`) ================\n";

    {
        cout << "  - Creating exclusive owner `ownerA` via `std::make_unique`...\n";
        std::unique_ptr<OwnedResource> ownerA = std::make_unique<OwnedResource>(userInputValue, "ExclusiveNodeA");

        cout << "  - `ownerA` Target Address : " << static_cast<const void*>(ownerA.get()) << "\n";

        // EXCLUSIVE RULE: Copying is forbidden by design
        // auto ownerCopy = ownerA; // COMPILE ERROR: Copy constructor deleted!

        cout << "\n  - Transferring exclusive ownership from `ownerA` to `ownerB` via `std::move()`...\n";
        std::unique_ptr<OwnedResource> ownerB = std::move(ownerA);

        cout << "  - Post-Move `ownerA` State : " 
             << (ownerA == nullptr ? "NULLPTR (Ownership Relinquished)" : "VALID") << "\n";
        cout << "  - Post-Move `ownerB` Target: " 
             << static_cast<const void*>(ownerB.get()) << " (Current Exclusive Owner)\n";

        cout << "\n  - Exiting scope containing `ownerB`...\n";
    } // `ownerB` destructor frees resource HERE!

    cout << "  - Active Instances Post-Exclusive Scope: " << OwnedResource::activeInstances << "\n";

    // =====================================================================================
    // 2. SHARED OWNERSHIP MODEL (`std::shared_ptr` & REFERENCE COUNTING)
    // =====================================================================================
    cout << "\n================ 2. SHARED OWNERSHIP MODEL (`std::shared_ptr`) ================\n";

    std::shared_ptr<OwnedResource> outerShared;

    {
        cout << "  - Creating primary shared owner `outerShared`...\n";
        outerShared = std::make_shared<OwnedResource>(userInputValue + 10, "SharedNode");

        cout << "    * Initial Use Count = " << outerShared.use_count() << "\n";

        {
            cout << "  - Entering inner block scope... Creating co-owner `innerShared = outerShared`...\n";
            std::shared_ptr<OwnedResource> innerShared = outerShared; // Shares ownership

            cout << "    * Ref Count inside inner scope = " << outerShared.use_count() << "\n";

            {
                std::shared_ptr<OwnedResource> thirdShared = innerShared; // Third co-owner
                cout << "    * Ref Count with 3 co-owners    = " << outerShared.use_count() << "\n";
            } // thirdShared leaves scope, ref count decrements

            cout << "  - Exited `thirdShared` scope -> Ref Count = " << outerShared.use_count() << "\n";
        } // innerShared leaves scope, ref count decrements

        cout << "  - Exited `innerShared` scope -> Ref Count = " << outerShared.use_count() << "\n";
        cout << "  - [SHARED GUARANTEE]: Resource remains alive as long as Use Count > 0.\n";
    }

    cout << "  - Resetting `outerShared` (Ref Count drops to 0)...\n";
    outerShared.reset(); // Final reference released HERE!

    cout << "  - Active Instances Post-Shared Reset: " << OwnedResource::activeInstances << "\n";

    // =====================================================================================
    // 3. NON-OWNING OBSERVER MODEL (`std::weak_ptr` & RAW OBSERVER POINTERS)
    // =====================================================================================
    cout << "\n================ 3. NON-OWNING OBSERVER MODEL (`std::weak_ptr`) ================\n";

    std::weak_ptr<OwnedResource> weakObserver;

    {
        cout << "  - Creating `sharedOwner` and binding non-owning `weakObserver`...\n";
        std::shared_ptr<OwnedResource> sharedOwner = std::make_shared<OwnedResource>(userInputValue + 20, "ObservedNode");
        weakObserver = sharedOwner; // Observer reference (does NOT increment strong ref count)

        cout << "    * Strong Ref Count = " << sharedOwner.use_count() << "\n";
        cout << "    * `weakObserver.expired()` check = " << (weakObserver.expired() ? "TRUE" : "FALSE") << "\n";

        // Locking weak_ptr safely creates a temporary shared_ptr if resource exists
        if (auto lockedShared = weakObserver.lock()) {
            cout << "    * Lock Successful! Accessing Resource ID: " << lockedShared->getId() << "\n";
            cout << "    * Temp Ref Count during lock = " << lockedShared.use_count() << "\n";
        }

        cout << "  - Leaving scope containing `sharedOwner`...\n";
    } // `sharedOwner` destroyed HERE!

    cout << "  - Post-Destruction `weakObserver.expired()` check: " 
         << (weakObserver.expired() ? "TRUE (Safely detected target destruction!)" : "FALSE") << "\n";

    if (auto failedLock = weakObserver.lock()) {
        failedLock->executePayload();
    } else {
        cout << "  - [WEAK_PTR SAFETY]: Lock failed gracefully without dangling pointer crash.\n";
    }

    // =====================================================================================
    // 4. OWNERSHIP FUNCTION CONTRACTS (FACTORY, SINK, & BORROWING)
    // =====================================================================================
    cout << "\n================ 4. OWNERSHIP FUNCTION CONTRACTS ================\n";

    // Pattern A: Factory Contract
    std::unique_ptr<OwnedResource> factoryRes = createResourceFactory(userInputValue + 30, "FactoryNode");

    // Pattern B: Borrowing Contract
    cout << "\n  - Borrowing non-owning view via `observeBorrowedResource(factoryRes.get())`...\n";
    observeBorrowedResource(factoryRes.get());

    cout << "  - Caller STILL maintains ownership of Resource ID: " << factoryRes->getId() << "\n";

    // Pattern C: Sink Contract
    cout << "\n  - Transferring ownership into `consumeOwnershipSink(std::move(factoryRes))`...\n";
    consumeOwnershipSink(std::move(factoryRes));

    cout << "  - Post-Sink `factoryRes` State: " 
         << (factoryRes == nullptr ? "NULLPTR (Ownership Transferred)" : "VALID") << "\n";

    cout << "  - Active Instances Post-Sink: " << OwnedResource::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RESOURCE OWNERSHIP MODELS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Ownership Model       | Smart / RAII Type | Copy / Move Rules | Primary Lifecycle Responsibility  |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Exclusive Ownership   | `std::unique_ptr` | Move-Only         | Single owner auto-frees on exit   |\n"
         << "| Shared Ownership      | `std::shared_ptr` | Copyable & Move   | Ref-counted; freed when count == 0|\n"
         << "| Non-Owning Observer   | `std::weak_ptr`   | Copyable & Move   | Observes shared_ptr without owning|\n"
         << "| Non-Owning Borrowing  | `const T*` / `T&` | Copyable & Move   | Temporary read access; NO deletion|\n"
         << "| Ownership Transfer    | `std::move(uPtr)` | Move-Only         | Relinquishes handle to sink/target|\n"
         << "| Custom Move-Only RAII | `ExclusiveHandle` | Move-Only         | Custom wrapper managing raw handle|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}