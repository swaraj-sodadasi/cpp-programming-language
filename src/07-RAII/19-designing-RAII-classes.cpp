/*
 * =====================================================================================
 * CONCEPT        : Designing RAII Classes (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architectural design workflow, design patterns, and C++ idioms
 *                  for building robust, reusable RAII classes:
 *
 *                  1. Encapsulation & Invariants  : Enforcing valid resource state via constructors 
 *                                                   (`explicit`) and prohibiting invalid post-conditions.
 *                  2. Rule of 0 vs Rule of 5      : Choosing Rule of 0 (composition via standard RAII) 
 *                                                   vs Rule of 5 (custom raw handle wrapper).
 *                  3. Custom RAII Class Anatomy   : Implementing explicit acquisition, `noexcept` destructor 
 *                                                   cleanup, `= delete` copies, and `noexcept` move operations.
 *                  4. Smart Handle Interface      : Overloading `operator*`, `operator->`, `explicit operator bool`, 
 *                                                   and providing `.get()`, `.release()`, and `.reset()`.
 *                  5. Policy / Custom Deleters    : Designing flexible RAII classes with customizable 
 *                                                   deleter policies (Functors / Lambdas).
 *                  6. Production Verification     : Testing lifecycle invariants, move semantics, 
 *                                                   and exception-safe cleanup.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Move / Access / Release / Reset: O(1) constant time.
 * SPACE COMPLEXITY : Handle Footprint: Uniform 8-16 bytes depending on stored pointer/deleter policy.
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
// HELPER CLASS 1: MANAGED RESOURCE ENTITY
// Simulates a low-level system handle or dynamic resource to track RAII class lifecycles.
// =====================================================================================
class ResourceEntity {
private:
    int id_;
    string label_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    ResourceEntity(int id, string label) : id_(id), label_(std::move(label)) {
        if (id <= 0) {
            throw std::invalid_argument("Resource acquisition error: ID must be positive!");
        }
        ++activeCount;
        cout << "    [RESOURCE ACQUIRED] ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    ~ResourceEntity() noexcept {
        --activeCount;
        cout << "    [RESOURCE FREED]    ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    void execute() const {
        cout << "    [PAYLOAD EXECUTED]  Resource ID: " << id_ << " (" << label_ << ") running task.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// DESIGN PATTERN 1: CUSTOM DELETER POLICY
// Strategy functor for policy-driven RAII design.
// =====================================================================================
struct CustomResourceDeleter {
    void operator()(ResourceEntity* res) const noexcept {
        if (res != nullptr) {
            cout << "    [CUSTOM DELETER FUNCTOR] Executing custom release routine for ID: " 
                 << res->getId() << "\n";
            delete res;
        }
    }
};

// =====================================================================================
// DESIGN PATTERN 2: CUSTOM RAII HANDLE CLASS (RULE OF 5 DESIGN)
// Comprehensive anatomy of a custom move-only RAII handle with full interface features.
// =====================================================================================
template <typename T, typename Deleter = std::default_delete<T>>
class CustomRaiiHandle {
private:
    T* ptr_;
    Deleter deleter_;

public:
    // 1. Constructor: Explicit acquisition enforcing invariants
    explicit CustomRaiiHandle(T* ptr = nullptr, Deleter deleter = Deleter()) noexcept
        : ptr_(ptr), deleter_(std::move(deleter)) {
        cout << "    [CustomRaiiHandle Ctor] Bound resource address: " 
             << static_cast<const void*>(ptr_) << "\n";
    }

    // 2. Destructor: Deterministic, non-throwing release
    ~CustomRaiiHandle() noexcept {
        reset(); // Reusable reset logic
    }

    // =================================================================================
    // RULE OF 5: COPY SUPPRESSION & MOVE SEMANTICS
    // =================================================================================
    
    // Prohibit copy operations to enforce exclusive single ownership
    CustomRaiiHandle(const CustomRaiiHandle&) = delete;
    CustomRaiiHandle& operator=(const CustomRaiiHandle&) = delete;

    // Enable move constructor (transfers pointer and disarms source)
    CustomRaiiHandle(CustomRaiiHandle&& other) noexcept
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr; // Disarm source handle
        cout << "    [CustomRaiiHandle Move Ctor] Ownership transferred cleanly.\n";
    }

    // Enable move assignment operator
    CustomRaiiHandle& operator=(CustomRaiiHandle&& other) noexcept {
        if (this != &other) {
            reset(); // Release currently held resource
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr; // Disarm source handle
            cout << "    [CustomRaiiHandle Move Assign] Existing freed, ownership transferred.\n";
        }
        return *this;
    }

    // =================================================================================
    // SMART HANDLE INTERFACE METHODS
    // =================================================================================

    // Resets current handle with a new resource pointer
    void reset(T* newPtr = nullptr) noexcept {
        if (ptr_ != newPtr) {
            T* oldPtr = ptr_;
            ptr_ = newPtr;
            if (oldPtr != nullptr) {
                cout << "    [CustomRaiiHandle reset] Releasing old address: " 
                     << static_cast<const void*>(oldPtr) << "\n";
                deleter_(oldPtr);
            }
        }
    }

    // Relinquishes ownership without deleting the underlying resource (Disarms handle)
    [[nodiscard]] T* release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        cout << "    [CustomRaiiHandle release] Relinquished ownership of address: " 
             << static_cast<const void*>(temp) << "\n";
        return temp;
    }

    // Transparent Accessors
    [[nodiscard]] T* get() const noexcept { return ptr_; }
    [[nodiscard]] explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
};

// =====================================================================================
// DESIGN PATTERN 3: RULE OF 0 DESIGN (COMPOSITION VIA STANDARD RAII)
// Idiomatic C++ approach: Compose domain classes using std::unique_ptr / STL RAII wrappers.
// No custom destructor, copy/move constructors, or assignment operators required!
// =====================================================================================
class RuleOfZeroManager {
private:
    std::unique_ptr<ResourceEntity> primaryRes_;
    std::unique_ptr<ResourceEntity> secondaryRes_;

public:
    // Constructor enforces class invariants
    RuleOfZeroManager(int id1, int id2)
        : primaryRes_(std::make_unique<ResourceEntity>(id1, "RuleOfZero_Primary")),
          secondaryRes_(std::make_unique<ResourceEntity>(id2, "RuleOfZero_Secondary")) {
        cout << "    [RuleOfZeroManager Ctor] Both RAII sub-objects initialized cleanly.\n";
    }

    // Rule of 0: Compiler auto-generates correct destructor and move operations!
    // Copy operations are automatically deleted because std::unique_ptr is move-only.

    void processAll() const {
        if (primaryRes_) primaryRes_->execute();
        if (secondaryRes_) secondaryRes_->execute();
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for RAII class design analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. DESIGNING WITH RULE OF 5 (CUSTOM RAII HANDLE ANATOMY)
    // =====================================================================================
    cout << "\n================ 1. DESIGNING WITH RULE OF 5 (CUSTOM RAII HANDLE) ================\n";

    {
        cout << "  - Creating `CustomRaiiHandle` bound to raw allocation...\n";
        CustomRaiiHandle<ResourceEntity> handleA(new ResourceEntity(userInputValue, "CustomHandleA"));

        // Transparent usage via operator->
        handleA->execute();

        cout << "\n  - Demonstrating ownership move `handleB = std::move(handleA)`...\n";
        CustomRaiiHandle<ResourceEntity> handleB = std::move(handleA);

        cout << "    * handleA valid check : " << (handleA ? "VALID" : "DISARMED / NULLPTR") << "\n";
        cout << "    * handleB valid check : " << (handleB ? "VALID (Current Owner)" : "NULLPTR") << "\n";

        cout << "  - Leaving scope (handleB destructor releases resource automatically):\n";
    } // handleB destructor executes HERE

    cout << "  - Active Instances Post-Rule of 5 Scope: " << ResourceEntity::activeCount << "\n";

    // =====================================================================================
    // 2. SMART HANDLE INTERFACE OPERATIONS (.get(), .release(), .reset())
    // =====================================================================================
    cout << "\n================ 2. SMART HANDLE INTERFACE OPERATIONS ================\n";

    {
        cout << "  - Testing `.reset()` operation...\n";
        CustomRaiiHandle<ResourceEntity> resetHandle(new ResourceEntity(userInputValue + 10, "ResetNode1"));
        
        cout << "  - Replacing resource via `.reset(new ResourceEntity)`...\n";
        resetHandle.reset(new ResourceEntity(userInputValue + 11, "ResetNode2")); // Frees Node1, acquires Node2

        cout << "\n  - Testing `.release()` operation...\n";
        ResourceEntity* rawExtracted = resetHandle.release(); // Relinquishes handle, returns raw pointer

        cout << "    * resetHandle valid check post-release: " << (resetHandle ? "VALID" : "DISARMED") << "\n";
        cout << "    * Manually deleting extracted raw resource...\n";
        delete rawExtracted; // Manual cleanup required since ownership was released!
    }

    cout << "  - Active Instances Post-Interface Scope: " << ResourceEntity::activeCount << "\n";

    // =====================================================================================
    // 3. POLICY-DRIVEN DESIGN WITH CUSTOM DELETERS
    // =====================================================================================
    cout << "\n================ 3. POLICY-DRIVEN DESIGN WITH CUSTOM DELETERS ================\n";

    {
        cout << "  - Instantiating `CustomRaiiHandle` with `CustomResourceDeleter` policy...\n";
        CustomRaiiHandle<ResourceEntity, CustomResourceDeleter> policyHandle(
            new ResourceEntity(userInputValue + 20, "PolicyNode")
        );

        policyHandle->execute();
        cout << "  - Leaving scope (Invokes custom deleter functor instead of default delete):\n";
    }

    cout << "  - Active Instances Post-Policy Scope: " << ResourceEntity::activeCount << "\n";

    // =====================================================================================
    // 4. DESIGNING WITH RULE OF 0 (COMPOSITION USING STANDARD RAII)
    // =====================================================================================
    cout << "\n================ 4. DESIGNING WITH RULE OF 0 (RAII COMPOSITION) ================\n";

    {
        cout << "  - Instantiating `RuleOfZeroManager` (Composes std::unique_ptr members)...\n";
        RuleOfZeroManager zeroMgr(userInputValue + 30, userInputValue + 31);
        zeroMgr.processAll();

        cout << "\n  - Moving `RuleOfZeroManager` via compiler-generated move ctor...\n";
        RuleOfZeroManager movedMgr = std::move(zeroMgr);
        movedMgr.processAll();

        cout << "  - Leaving scope (Compiler auto-destructs std::unique_ptr members in reverse order):\n";
    }

    cout << "  - Active Instances Post-Rule of 0 Scope: " << ResourceEntity::activeCount << "\n";

    // =====================================================================================
    // 5. CONSTRUCTOR INVARIANT ENFORCEMENT & EXCEPTION ROLLBACK
    // =====================================================================================
    cout << "\n================ 5. INVARIANT ENFORCEMENT & EXCEPTION SAFETY ================\n";

    try {
        cout << "  - Attempting to construct RAII object with invalid ID (-99)...\n";
        CustomRaiiHandle<ResourceEntity> invalidHandle(new ResourceEntity(-99, "InvalidNode"));
    } catch (const std::exception& e) {
        cout << "  - [INVARIANT CAUGHT] Exception thrown during constructor: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post-Invariant Failure: " << ResourceEntity::activeCount 
         << " (ZERO LEAKS: Invariants preserved!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DESIGNING RAII CLASSES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Design Pillar / Rule  | Implementation Mechanism          | Primary Architectural Benefit     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Rule of 0             | Compose standard RAII members     | Zero manual memory management     |\n"
         << "| Rule of 5             | Custom Dtor, Deleted Copy, Move   | Low-level custom resource handle  |\n"
         << "| Invariant Enforcement | Validate inputs in Constructor    | Prevents invalid object states    |\n"
         << "| `noexcept` Destructor | Never throw exceptions in Dtor    | Safe stack unwinding & crash prevention|\n"
         << "| `.release()` Interface| Disarm pointer, return raw handle | Relinquishes ownership to caller  |\n"
         << "| `.reset()` Interface  | Release old ptr, acquire new ptr  | Reusable handle state updates     |\n"
         << "| Policy-Driven Deleter | Template parameter `Deleter`      | Flexible custom release strategies|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}