/*
 * =====================================================================================
 * CONCEPT        : Best Practices in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the core industry-standard best practices for designing and utilizing 
 *                  Resource Acquisition Is Initialization (RAII) in Modern C++:
 *
 *                  1. Prefer Rule of 0 Over Rule of 5 : Compose classes using standard RAII 
 *                                                       sub-objects (`std::unique_ptr`, `std::vector`) 
 *                                                       to eliminate custom manual destructors.
 *                  2. Mark Destructors `noexcept`     : Ensure destructors never throw exceptions 
 *                                                       to prevent `std::terminate` during stack unwinding.
 *                  3. Mark Move Operations `noexcept` : Enable STL container optimizations 
 *                                                       (e.g., `std::vector` reallocation reallocation safety).
 *                  4. Declare Constructors `explicit`  : Prevent unintended implicit type conversions 
 *                                                       during resource instantiation.
 *                  5. Enforce Invariants in Ctor      : Validate parameters inside constructors; throw 
 *                                                       exceptions on failure so invalid objects never exist.
 *                  6. Use `[[nodiscard]]` on Factories: Force callers to capture returned RAII handles 
 *                                                       to prevent immediate unintended resource destruction.
 *                  7. Avoid Raw `new`/`delete`         : Utilize `std::make_unique` and `std::make_shared` 
 *                                                       for allocation and exception safety.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Access / Move: O(1) constant time.
 * SPACE COMPLEXITY : Handle Overhead: Uniform 8-16 bytes (raw pointer / smart pointer footprint).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE WITH LIFECYCLE TRACKING
// Tracks real-time active instances to verify leak-free RAII execution.
// =====================================================================================
class ManagedResource {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    ManagedResource(int id, string label) : id_(id), label_(std::move(label)) {
        if (id <= 0) {
            throw std::invalid_argument("Invariant Violated: Resource ID must be strictly positive!");
        }
        ++activeInstances;
        cout << "    [RESOURCE ALLOCATED] ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~ManagedResource() noexcept {
        --activeInstances;
        cout << "    [RESOURCE FREED]     ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    void performTask() const {
        cout << "    [TASK EXECUTED]      Resource ID: " << id_ << " (" << label_ << ") running payload.\n";
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
    [[nodiscard]] const string& getLabel() const noexcept { return label_; }
};

// =====================================================================================
// BEST PRACTICE 1: RULE OF 0 COMPOSITION OVER RULE OF 5
// Compose domain classes using standard RAII sub-objects so compiler auto-generates
// safe destructors, move operations, and copy deletions.
// =====================================================================================
class RuleOfZeroClass {
private:
    std::unique_ptr<ManagedResource> resource_;
    string metadata_;

public:
    // Best Practice: `explicit` constructor preventing implicit conversion
    explicit RuleOfZeroClass(int id, string metadata)
        : resource_(std::make_unique<ManagedResource>(id, "RuleOfZero_Node")),
          metadata_(std::move(metadata)) {
        cout << "    [RuleOfZero Ctor] Composed sub-objects initialized cleanly.\n";
    }

    // Rule of 0: No custom destructor, copy operations, or move operations defined!
    // Compiler automatically provides move-only semantics and safe cleanup.

    void execute() const {
        if (resource_) {
            resource_->performTask();
        }
    }
};

// =====================================================================================
// BEST PRACTICE 2 & 3: CUSTOM RAII HANDLE WITH `noexcept` MOVE & DESTRUCTOR
// When writing custom handles, explicitly mark move constructors and destructors `noexcept`.
// =====================================================================================
template <typename T>
class CustomRaiiHandle {
private:
    T* ptr_;

public:
    // Best Practice: Explicit constructor enforcing invariants
    explicit CustomRaiiHandle(T* ptr = nullptr) noexcept : ptr_(ptr) {
        cout << "    [CustomRaiiHandle Ctor] Bound raw resource at: " 
             << static_cast<const void*>(ptr_) << "\n";
    }

    // Best Practice: Destructors MUST be `noexcept` and handle internal errors defensively
    ~CustomRaiiHandle() noexcept {
        try {
            if (ptr_ != nullptr) {
                cout << "    [CustomRaiiHandle Dtor] Safely releasing resource at: " 
                     << static_cast<const void*>(ptr_) << "\n";
                delete ptr_;
            }
        } catch (...) {
            // CRITICAL BEST PRACTICE: Never allow exceptions to escape a destructor!
        }
    }

    // Best Practice: Prohibit Copy Operations for Exclusive Ownership (Single Ownership Invariant)
    CustomRaiiHandle(const CustomRaiiHandle&) = delete;
    CustomRaiiHandle& operator=(const CustomRaiiHandle&) = delete;

    // Best Practice: Mark Move operations `noexcept` to allow STL vector reallocation optimization
    CustomRaiiHandle(CustomRaiiHandle&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr; // Disarm source handle
        cout << "    [CustomRaiiHandle Move Ctor (noexcept)] Ownership transferred cleanly.\n";
    }

    CustomRaiiHandle& operator=(CustomRaiiHandle&& other) noexcept {
        if (this != &other) {
            delete ptr_;          // Release existing resource
            ptr_ = other.ptr_;    // Transfer handle
            other.ptr_ = nullptr; // Disarm source handle
            cout << "    [CustomRaiiHandle Move Assign (noexcept)] Existing freed, new ownership acquired.\n";
        }
        return *this;
    }

    // Best Practice: Use `[[nodiscard]]` on resource accessors and query methods
    [[nodiscard]] T* get() const noexcept { return ptr_; }
    [[nodiscard]] explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
};

// =====================================================================================
// BEST PRACTICE 6: FACTORY FUNCTIONS WITH `[[nodiscard]]`
// Prevent callers from ignoring returned RAII handles (which causes immediate destruction).
// =====================================================================================
[[nodiscard]] std::unique_ptr<ManagedResource> createManagedResourceFactory(int id, const string& tag) {
    cout << "    [FACTORY FUNCTION] Instantiating resource via `std::make_unique`...\n";
    return std::make_unique<ManagedResource>(id, tag);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for RAII best practices analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BEST PRACTICE 1: PREFER RULE OF 0 OVER RULE OF 5
    // =====================================================================================
    cout << "\n================ 1. PREFER RULE OF 0 OVER RULE OF 5 ================\n";

    {
        cout << "  - Instantiating `RuleOfZeroClass` (composed via std::unique_ptr)...\n";
        RuleOfZeroClass zeroObj(userInputValue, "RuleOfZeroMetadata");
        zeroObj.execute();

        cout << "  - Moving `RuleOfZeroClass` instance...\n";
        RuleOfZeroClass movedZeroObj = std::move(zeroObj);
        movedZeroObj.execute();

        cout << "  - Exiting Rule of 0 scope (Sub-objects auto-destruct without custom Dtor code):\n";
    }

    cout << "  - Active Instances Post Rule of 0 Scope: " << ManagedResource::activeInstances << "\n";

    // =====================================================================================
    // 2. BEST PRACTICE 2 & 3: `noexcept` MOVE OPERATIONS & STL VECTOR EFFICIENCY
    // =====================================================================================
    cout << "\n================ 2. `noexcept` MOVE OPERATIONS & STL EFFICIENCY ================\n";

    {
        cout << "  - Creating `std::vector<CustomRaiiHandle<ManagedResource>>`...\n";
        std::vector<CustomRaiiHandle<ManagedResource>> handleVector;

        cout << "  - Inserting custom RAII handles into vector...\n";
        handleVector.push_back(CustomRaiiHandle<ManagedResource>(
            new ManagedResource(userInputValue + 10, "VectorNode1")
        ));

        cout << "\n  - Triggering vector reallocation (Vector uses `noexcept` move constructors safely):\n";
        handleVector.push_back(CustomRaiiHandle<ManagedResource>(
            new ManagedResource(userInputValue + 11, "VectorNode2")
        ));

        for (const auto& handle : handleVector) {
            handle->performTask();
        }

        cout << "  - Exiting vector scope...\n";
    } // Vector elements auto-destructed HERE!

    cout << "  - Active Instances Post Vector Scope: " << ManagedResource::activeInstances << "\n";

    // =====================================================================================
    // 3. BEST PRACTICE 4 & 5: `explicit` CTORS & INVARIANT ENFORCEMENT
    // =====================================================================================
    cout << "\n================ 3. `explicit` CONSTRUCTORS & INVARIANT ENFORCEMENT ================\n";

    try {
        cout << "  - Attempting to construct RAII object with invalid ID (-99)...\n";
        // Constructor validates invariant and throws immediately:
        ManagedResource invalidRes(-99, "InvalidNode");
    } catch (const std::exception& e) {
        cout << "  - [INVARIANT CAUGHT] Exception caught during constructor: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post Invariant Check: " << ManagedResource::activeInstances 
         << " (BEST PRACTICE: Invalid objects NEVER exist!)\n";

    // =====================================================================================
    // 4. BEST PRACTICE 6: `[[nodiscard]]` FACTORY FUNCTIONS & `std::make_unique`
    // =====================================================================================
    cout << "\n================ 4. `[[nodiscard]]` FACTORIES & `std::make_unique` ================\n";

    {
        cout << "  - Calling `[[nodiscard]]` factory function `createManagedResourceFactory`...\n";
        auto smartHandle = createManagedResourceFactory(userInputValue + 20, "FactoryNode");
        smartHandle->performTask();

        // Demonstrating that ignoring [[nodiscard]] return triggers a compiler warning
        // createManagedResourceFactory(userInputValue + 21, "DiscardedNode"); // Compiler warning!

        cout << "  - Leaving factory handle block scope...\n";
    }

    cout << "\n  - Final Active Resource Instances Count: " << ManagedResource::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ BEST PRACTICES IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| RAII Best Practice    | Technical Rule / Syntax           | Primary Architectural Advantage   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Prefer Rule of 0      | Compose `unique_ptr` / containers | Eliminates manual destructor bugs |\n"
         << "| `noexcept` Destructors| Mark Dtor `noexcept`, catch errors| Prevents `std::terminate` crashes |\n"
         << "| `noexcept` Move Ctors | `Type(Type&&) noexcept;`          | Enables STL vector move efficiency|\n"
         << "| `explicit` Ctors      | `explicit Type(Param p);`         | Prevents unintended type coercion |\n"
         << "| Invariant Enforcement | Throw exceptions inside Ctor      | Guarantees valid object states    ||\n"
         << "| `[[nodiscard]]` Use   | `[[nodiscard]] Handle create();`  | Prevents dropping temp resources  |\n"
         << "| Avoid Raw `new`/`del` | `std::make_unique<T>()`           | Provides exception argument safety|\n"
         << "| Single Ownership      | `Type(const Type&) = delete;`     | Eliminates double-free crash bugs |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}