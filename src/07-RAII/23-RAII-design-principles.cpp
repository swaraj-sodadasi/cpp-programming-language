/*
 * =====================================================================================
 * CONCEPT        : RAII Design Principles (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architectural design principles governing the Resource Acquisition 
 *                  Is Initialization (RAII) idiom in Modern C++:
 *
 *                  1. Direct Lifetime Binding    : Binding resource lifecycle strictly to 
 *                                                   stack object construction and destruction.
 *                  2. Class Invariant Guarantee  : Validating state in constructor; throwing on 
 *                                                   failure so objects never exist in invalid states.
 *                  3. Single Ownership Invariant  : Prohibiting shallow copies (`= delete`) to prevent 
 *                                                   double-free bugs; providing `noexcept` move semantics.
 *                  4. Deterministic Cleanup      : `noexcept` destructors guaranteeing leak-free cleanup 
 *                                                   during normal exit or stack unwinding.
 *                  5. Interface Transparency      : Providing intuitive smart pointer interface 
 *                                                   (`operator->`, `operator*`, `explicit operator bool`, 
 *                                                   `.get()`, `.release()`, `.reset()`).
 *                  6. Composition (Rule of 0)     : Designing high-level domain abstractions by composing 
 *                                                   existing move-only RAII sub-objects.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Access / Move / Reset / Release: O(1) constant time.
 * SPACE COMPLEXITY : Wrapper Footprint: Uniform 8-16 bytes (raw pointer + policy size).
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
// HELPER CLASS 1: MANAGED SYSTEM RESOURCE ENTITY
// Tracks active allocations to visually demonstrate lifecycle state transitions.
// =====================================================================================
class SystemResourceEntity {
private:
    int id_;
    string descriptor_;

public:
    static inline int activeInstances = 0; // C++17 inline static instance counter

    SystemResourceEntity(int id, string descriptor) 
        : id_(id), descriptor_(std::move(descriptor)) {
        if (id <= 0) {
            throw std::invalid_argument("RAII Invariant Error: Resource ID must be strictly positive!");
        }
        ++activeInstances;
        cout << "    [RESOURCE ALLOCATED] ID: " << setw(3) << id_ << " (" << setw(20) << left << descriptor_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    ~SystemResourceEntity() noexcept {
        --activeInstances;
        cout << "    [RESOURCE DEALLOCATED] ID: " << setw(3) << id_ << " (" << setw(20) << left << descriptor_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    void execute() const {
        cout << "    [PAYLOAD EXECUTING]  Resource ID: " << id_ << " (" << descriptor_ << ") running task.\n";
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
    [[nodiscard]] const string& getDescriptor() const noexcept { return descriptor_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM RAII WRAPPER EMBODYING CORE DESIGN PRINCIPLES
// =====================================================================================
template <typename T, typename Deleter = std::default_delete<T>>
class CanonicalRaiiWrapper {
private:
    T* resource_;
    Deleter deleter_;

public:
    // Principle 1: Acquisition In Constructor with Invariant Enforcement
    explicit CanonicalRaiiWrapper(T* resource = nullptr, Deleter deleter = Deleter()) noexcept
        : resource_(resource), deleter_(std::move(deleter)) {
        cout << "    [CanonicalRaiiWrapper Ctor] Bound raw handle at: " 
             << static_cast<const void*>(resource_) << "\n";
    }

    // Principle 2: Deterministic Release in `noexcept` Destructor
    ~CanonicalRaiiWrapper() noexcept {
        reset(); // Delegates to safe reset routine
    }

    // Principle 3: Single Ownership Invariant (Delete Copy Operations)
    CanonicalRaiiWrapper(const CanonicalRaiiWrapper&) = delete;
    CanonicalRaiiWrapper& operator=(const CanonicalRaiiWrapper&) = delete;

    // Principle 4: Move Semantics for Ownership Transfer (Rule of 5)
    CanonicalRaiiWrapper(CanonicalRaiiWrapper&& other) noexcept
        : resource_(other.resource_), deleter_(std::move(other.deleter_)) {
        other.resource_ = nullptr; // Disarm source handle to prevent double-free
        cout << "    [CanonicalRaiiWrapper Move Ctor] Resource ownership transferred safely.\n";
    }

    CanonicalRaiiWrapper& operator=(CanonicalRaiiWrapper&& other) noexcept {
        if (this != &other) {
            reset(); // Release currently held resource
            resource_ = other.resource_;
            deleter_ = std::move(other.deleter_);
            other.resource_ = nullptr; // Disarm source handle
            cout << "    [CanonicalRaiiWrapper Move Assign] Previous freed, new ownership acquired.\n";
        }
        return *this;
    }

    // Principle 5: Interface Transparency Operations

    // Resets current handle with a new resource pointer
    void reset(T* newResource = nullptr) noexcept {
        if (resource_ != newResource) {
            T* oldResource = resource_;
            resource_ = newResource;
            if (oldResource != nullptr) {
                cout << "    [CanonicalRaiiWrapper reset] Freeing existing resource at: " 
                     << static_cast<const void*>(oldResource) << "\n";
                deleter_(oldResource);
            }
        }
    }

    // Relinquishes handle ownership without deleting the underlying resource (Disarms handle)
    [[nodiscard]] T* release() noexcept {
        T* temp = resource_;
        resource_ = nullptr;
        cout << "    [CanonicalRaiiWrapper release] Relinquished handle ownership for: " 
             << static_cast<const void*>(temp) << "\n";
        return temp;
    }

    // Accessors and Operators
    [[nodiscard]] T* get() const noexcept { return resource_; }
    [[nodiscard]] explicit operator bool() const noexcept { return resource_ != nullptr; }

    T& operator*() const { return *resource_; }
    T* operator->() const { return resource_; }
};

// =====================================================================================
// HELPER CLASS 3: COMPOSITE MANAGER DEMONSTRATING THE RULE OF 0
// Principle 6: Prefer composing move-only RAII members over manual handle management.
// =====================================================================================
class RuleOfZeroCompositeManager {
private:
    std::unique_ptr<SystemResourceEntity> primaryRes_;
    CanonicalRaiiWrapper<SystemResourceEntity> secondaryRes_;

public:
    RuleOfZeroCompositeManager(int id1, int id2)
        : primaryRes_(std::make_unique<SystemResourceEntity>(id1, "RuleOfZero_Primary")),
          secondaryRes_(new SystemResourceEntity(id2, "RuleOfZero_Secondary")) {
        cout << "    [RuleOfZeroComposite Ctor] Initialized composite RAII sub-objects.\n";
    }

    // Rule of 0: No manual Dtor, Copy Ctor, Move Ctor, or Assignment required!
    // Compiler auto-generates correct teardown and move semantics based on member types.

    void processAll() const {
        if (primaryRes_) primaryRes_->execute();
        if (secondaryRes_) secondaryRes_->execute();
    }
};

// Custom Deleter Policy Functor for Policy-Driven RAII Design
struct CustomPolicyDeleter {
    void operator()(SystemResourceEntity* ptr) const noexcept {
        if (ptr != nullptr) {
            cout << "    [CUSTOM POLICY DELETER] Executing special cleanup routine for ID: " 
                 << ptr->getId() << "\n";
            delete ptr;
        }
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for RAII design principles analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. PRINCIPLE 1: LIFETIME BINDING & DETERMINISTIC RELEASE
    // =====================================================================================
    cout << "\n================ 1. DIRECT LIFETIME BINDING & DETERMINISTIC RELEASE ================\n";

    {
        cout << "  - Entering local scope... Allocating `CanonicalRaiiWrapper`...\n";
        CanonicalRaiiWrapper<SystemResourceEntity> scopedHandle(
            new SystemResourceEntity(userInputValue, "ScopedResourceA")
        );

        // Interface transparency access via operator->
        scopedHandle->execute();

        cout << "  - Exiting local scope (Notice automatic destructor execution):\n";
    } // `scopedHandle` destructor fires HERE automatically!

    cout << "  - Active Instances Post-Scope: " << SystemResourceEntity::activeInstances << "\n";

    // =====================================================================================
    // 2. PRINCIPLE 2: CLASS INVARIANT ENFORCEMENT & EXCEPTION SAFETY
    // =====================================================================================
    cout << "\n================ 2. CLASS INVARIANT ENFORCEMENT & EXCEPTION SAFETY ================\n";

    try {
        cout << "  - Attempting to acquire resource with invalid ID (-50)...\n";
        CanonicalRaiiWrapper<SystemResourceEntity> invalidHandle(
            new SystemResourceEntity(-50, "InvalidResource")
        );
    } catch (const std::exception& e) {
        cout << "  - [INVARIANT ENFORCED] Exception caught during construction: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post-Invariant Failure: " << SystemResourceEntity::activeInstances 
         << " (RAII GUARANTEE: Zero objects exist in an invalid state!)\n";

    // =====================================================================================
    // 3. PRINCIPLE 3: SINGLE OWNERSHIP & MOVE SEMANTICS
    // =====================================================================================
    cout << "\n================ 3. SINGLE OWNERSHIP INVARIANT & MOVE SEMANTICS ================\n";

    {
        cout << "  - Creating primary handle `ownerA`...\n";
        CanonicalRaiiWrapper<SystemResourceEntity> ownerA(
            new SystemResourceEntity(userInputValue + 10, "MoveableResource")
        );

        // CanonicalRaiiWrapper<SystemResourceEntity> copyAttempt = ownerA; 
        // COMPILE ERROR: Copy constructor deleted to enforce single ownership!

        cout << "  - Transferring ownership from `ownerA` to `ownerB` via `std::move()`...\n";
        CanonicalRaiiWrapper<SystemResourceEntity> ownerB = std::move(ownerA);

        cout << "    * ownerA State : " << (ownerA ? "VALID" : "DISARMED / NULLPTR") << "\n";
        cout << "    * ownerB State : " << (ownerB ? "VALID (Current Owner)" : "NULLPTR") << "\n";

        ownerB->execute();

        cout << "  - Exiting move scope...\n";
    }

    cout << "  - Active Instances Post-Move Scope: " << SystemResourceEntity::activeInstances << "\n";

    // =====================================================================================
    // 4. PRINCIPLE 4: SMART HANDLE INTERFACE MECHANICS (.release() & .reset())
    // =====================================================================================
    cout << "\n================ 4. SMART HANDLE INTERFACE MECHANICS ================\n";

    {
        cout << "  - Testing `.reset()`: Replacing held handle with new resource...\n";
        CanonicalRaiiWrapper<SystemResourceEntity> handle(
            new SystemResourceEntity(userInputValue + 20, "ResetNode1")
        );

        handle.reset(new SystemResourceEntity(userInputValue + 21, "ResetNode2")); // Frees Node 20, acquires Node 21

        cout << "\n  - Testing `.release()`: Disarming wrapper without freeing resource...\n";
        SystemResourceEntity* rawExtracted = handle.release();

        cout << "    * handle valid check post-release: " << (handle ? "VALID" : "DISARMED") << "\n";
        cout << "    * Manually freeing raw extracted resource...\n";
        delete rawExtracted; // Manual cleanup required because handle was disarmed!
    }

    cout << "  - Active Instances Post-Interface Scope: " << SystemResourceEntity::activeInstances << "\n";

    // =====================================================================================
    // 5. PRINCIPLE 5: POLICY-DRIVEN DELETERS & RULE OF 0 COMPOSITION
    // =====================================================================================
    cout << "\n================ 5. POLICY DELETERS & RULE OF 0 COMPOSITION ================\n";

    // A. Custom Policy Deleter
    {
        cout << "  - Instantiating handle with `CustomPolicyDeleter`...\n";
        CanonicalRaiiWrapper<SystemResourceEntity, CustomPolicyDeleter> policyHandle(
            new SystemResourceEntity(userInputValue + 30, "PolicyResource")
        );
        policyHandle->execute();

        cout << "  - Exiting policy scope...\n";
    }

    // B. Rule of 0 Composite Class
    {
        cout << "\n  - Instantiating `RuleOfZeroCompositeManager`...\n";
        RuleOfZeroCompositeManager composite(userInputValue + 40, userInputValue + 41);
        composite.processAll();

        cout << "  - Exiting composite scope (Sub-objects auto-destruct in reverse order):\n";
    }

    cout << "\n  - Final Active Resource Instances Count: " << SystemResourceEntity::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RAII DESIGN PRINCIPLES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| RAII Design Principle | Technical Implementation          | Primary Safety & Architecture Goal|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| 1. Lifetime Binding   | Acquire in Ctor, Release in Dtor  | Guarantees auto leak-free exit    |\n"
         << "| 2. Invariant Safety   | Validate parameters in Ctor       | Prevents invalid object existence |\n"
         << "| 3. Single Ownership   | `Wrapper(const Wrapper&) = delete`| Blocks double-free crash hazards  |\n"
         << "| 4. Move Semantics     | `Wrapper(Wrapper&&) noexcept`     | Safe zero-cost handle transfer    |\n"
         << "| 5. Interface Access   | Overload `->`, `*`, `explicit bool`| Natural smart pointer usability   |\n"
         << "| 6. Handle Management  | Implement `.get()`, `.release()`, `.reset()`| Flexible resource state control   |\n"
         << "| 7. Policy Delegation  | Template parameter `Deleter`      | Extends RAII to any C-style API   |\n"
         << "| 8. Rule of 0          | Compose RAII sub-object members   | Eliminates manual destructor logic|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}