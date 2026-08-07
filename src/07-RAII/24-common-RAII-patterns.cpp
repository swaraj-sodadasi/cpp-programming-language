/*
 * =====================================================================================
 * CONCEPT        : Common RAII Patterns in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the core architectural RAII patterns widely used in modern C++:
 *
 *                  1. Scope Guard Pattern          : Executes an arbitrary callback/lambda 
 *                                                    unconditionally or conditionally on scope exit.
 *                  2. Scoped Lock Pattern         : Binds synchronization primitives (mutexes) 
 *                                                    to block scopes to prevent deadlocks.
 *                  3. Scoped Handle / Wrapper     : Wraps raw handles (files, OS descriptors) 
 *                                                    for clean, leak-free teardown.
 *                  4. Rollback / Transaction Guard : Implements commit-or-rollback semantics 
 *                                                    for exception-safe state mutations.
 *                  5. Rule of 0 Composition        : Builds domain classes by composing standard 
 *                                                    RAII sub-objects without manual teardown.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Scope Exit: O(1) constant time.
 * SPACE COMPLEXITY : Pattern Overhead: Uniform 8-24 bytes per stack-bound guard.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <vector>

using namespace std;

// =====================================================================================
// HELPER CLASS: MANAGED LIFECYCLE ENTITY
// Logs resource state changes to trace execution across pattern demonstrations.
// =====================================================================================
class EntityNode {
private:
    int id_;
    string label_;

public:
    static inline int activeCount = 0; // C++17 inline static variable

    EntityNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeCount;
        cout << "    [ENTITY CREATED]   ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    ~EntityNode() noexcept {
        --activeCount;
        cout << "    [ENTITY DESTROYED] ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    void execute() const {
        cout << "    [ENTITY EXECUTING] ID: " << id_ << " (" << label_ << ") performing task.\n";
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
    [[nodiscard]] const string& getLabel() const noexcept { return label_; }
};

// =====================================================================================
// PATTERN 1: SCOPE GUARD PATTERN
// Runs a custom cleanup lambda/callback on scope exit. Supports explicit dismissal.
// =====================================================================================
class ScopeGuard {
private:
    std::function<void()> cleanupTask_;
    bool active_;

public:
    explicit ScopeGuard(std::function<void()> task) noexcept
        : cleanupTask_(std::move(task)), active_(true) {
        cout << "    [ScopeGuard Ctor] Registered scope exit callback.\n";
    }

    ~ScopeGuard() noexcept {
        if (active_ && cleanupTask_) {
            try {
                cout << "    [ScopeGuard Dtor] Executing registered callback on scope exit...\n";
                cleanupTask_();
            } catch (...) {
                // Destructors must never allow exceptions to escape!
            }
        }
    }

    // Disarms the guard (e.g., when a transaction completes successfully)
    void dismiss() noexcept {
        active_ = false;
        cout << "    [ScopeGuard DISARMED] Scope callback disarmed.\n";
    }

    // Non-copyable, non-movable scope invariant
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;
};

// =====================================================================================
// PATTERN 2: SCOPED LOCK PATTERN (CUSTOM IMPLEMENTATION)
// Binds mutex lock and unlock operations to lexical block scopes.
// =====================================================================================
class CustomScopedLock {
private:
    mutex& mutexRef_;

public:
    explicit CustomScopedLock(mutex& mtx) : mutexRef_(mtx) {
        mutexRef_.lock();
        cout << "    [CustomScopedLock Ctor] Mutex locked on critical section entry.\n";
    }

    ~CustomScopedLock() noexcept {
        mutexRef_.unlock();
        cout << "    [CustomScopedLock Dtor] Mutex unlocked on critical section exit.\n";
    }

    // Non-copyable, non-movable scope invariant
    CustomScopedLock(const CustomScopedLock&) = delete;
    CustomScopedLock& operator=(const CustomScopedLock&) = delete;
    CustomScopedLock(CustomScopedLock&&) = delete;
    CustomScopedLock& operator=(CustomScopedLock&&) = delete;
};

// =====================================================================================
// PATTERN 3: TRANSACTIONAL ROLLBACK GUARD PATTERN
// Automatically rolls back mutated state unless explicitly committed before scope exit.
// =====================================================================================
class RollbackGuard {
private:
    std::function<void()> rollbackAction_;
    bool committed_;

public:
    explicit RollbackGuard(std::function<void()> rollbackAction) noexcept
        : rollbackAction_(std::move(rollbackAction)), committed_(false) {
        cout << "    [RollbackGuard Ctor] Transaction active. Rollback action registered.\n";
    }

    ~RollbackGuard() noexcept {
        if (!committed_ && rollbackAction_) {
            try {
                cout << "    [RollbackGuard ROLLBACK] Exception/Failure detected! Executing rollback...\n";
                rollbackAction_();
            } catch (...) {
                // Suppress exceptions inside destructor
            }
        }
    }

    void commit() noexcept {
        committed_ = true;
        cout << "    [RollbackGuard COMMIT] Transaction committed successfully. Rollback disarmed.\n";
    }

    // Non-copyable
    RollbackGuard(const RollbackGuard&) = delete;
    RollbackGuard& operator=(const RollbackGuard&) = delete;
};

// =====================================================================================
// PATTERN 4: RULE OF 0 COMPOSITION PATTERN
// Composes high-level abstractions using standard RAII sub-objects without manual teardown.
// =====================================================================================
class CompositeRuleOfZeroDomain {
private:
    std::unique_ptr<EntityNode> primaryEntity_;
    std::vector<int> transactionData_;

public:
    CompositeRuleOfZeroDomain(int id, const string& label)
        : primaryEntity_(std::make_unique<EntityNode>(id, label)),
          transactionData_({10, 20, 30, 40}) {
        cout << "    [RuleOfZero Domain Ctor] Composed RAII members initialized.\n";
    }

    // Rule of 0: No custom Destructor, Copy, or Move operations needed!
    // Compiler auto-generates correct cleanup and move semantics safely.

    void process() const {
        if (primaryEntity_) {
            primaryEntity_->execute();
        }
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for common RAII patterns analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. PATTERN 1: SCOPE GUARD PATTERN
    // =====================================================================================
    cout << "\n================ 1. SCOPE GUARD PATTERN ================\n";

    {
        cout << "  - Entering scope with active ScopeGuard...\n";
        bool systemFlag = true;

        ScopeGuard exitGuard([&systemFlag]() {
            systemFlag = false;
            cout << "    [SCOPE GUARD CALLBACK] System flag reset to FALSE on scope exit.\n";
        });

        cout << "    Current system flag state: " << (systemFlag ? "TRUE" : "FALSE") << "\n";
        cout << "  - Leaving scope containing ScopeGuard...\n";
    } // ScopeGuard callback executes automatically HERE!

    // =====================================================================================
    // 2. PATTERN 2: SCOPED LOCK PATTERN
    // =====================================================================================
    cout << "\n================ 2. SCOPED LOCK PATTERN ================\n";

    mutex sharedMutex;

    {
        cout << "  - Entering critical section block scope...\n";
        CustomScopedLock lock(sharedMutex);

        cout << "    Executing thread-safe critical work under guard...\n";

        cout << "  - Leaving critical section block scope...\n";
    } // CustomScopedLock unlocks mutex automatically HERE!

    // =====================================================================================
    // 3. PATTERN 3: TRANSACTIONAL ROLLBACK GUARD PATTERN
    // =====================================================================================
    cout << "\n================ 3. TRANSACTIONAL ROLLBACK GUARD PATTERN ================\n";

    // Scenario A: Successful Transaction (Committed)
    cout << "  - Case A: Successful Transaction...\n";
    std::vector<int> databaseRecords = {1, 2, 3};

    {
        databaseRecords.push_back(4); // Temporary mutation
        RollbackGuard guard([&databaseRecords]() {
            databaseRecords.pop_back(); // Rollback mutation on failure
        });

        cout << "    Performing transaction operations...\n";
        guard.commit(); // Transaction succeeded, disarm rollback
    }
    cout << "    Post-Transaction Vector Size: " << databaseRecords.size() << " (Expected: 4)\n";

    // Scenario B: Failing Transaction (Rollback Triggered by Exception)
    cout << "\n  - Case B: Failing Transaction (Exception Triggered)...\n";
    try {
        databaseRecords.push_back(999); // Temporary mutation
        RollbackGuard guard([&databaseRecords]() {
            databaseRecords.pop_back(); // Rollback mutation
        });

        cout << "    Throwing exception midway through transaction pipeline...\n";
        throw std::runtime_error("Database write constraint violated!");

        guard.commit(); // Bypassed due to exception
    } catch (const std::exception& e) {
        cout << "  - Caught Expected Exception: \"" << e.what() << "\"\n";
    }

    cout << "    Post-Rollback Vector Size: " << databaseRecords.size() 
         << " (Expected: 4, value 999 rolled back!)\n";

    // =====================================================================================
    // 4. PATTERN 4: RULE OF 0 COMPOSITION PATTERN
    // =====================================================================================
    cout << "\n================ 4. RULE OF 0 COMPOSITION PATTERN ================\n";

    {
        cout << "  - Instantiating `CompositeRuleOfZeroDomain` object...\n";
        CompositeRuleOfZeroDomain domainObj(userInputValue, "RuleOfZeroNode");
        domainObj.process();

        cout << "  - Leaving scope (RAII sub-objects auto-destruct cleanly):\n";
    }

    cout << "  - Final Active Entity Count: " << EntityNode::activeCount << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ COMMON RAII PATTERNS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| RAII Pattern          | Core Implementation Technique     | Primary Architectural Benefit     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Scope Guard           | `ScopeGuard guard(lambda);`       | Guarantees arbitrary scope cleanup|\n"
         << "| Scoped Lock           | `CustomScopedLock lock(mutex);`   | Exception-safe deadlock prevention|\n"
         << "| Rollback Guard        | `RollbackGuard guard(rollbackFn);`| Provides strong rollback safety   |\n"
         << "| Scoped Handle         | `Wrapper wrapper(rawHandle);`     | Auto-frees C-style system handles |\n"
         << "| Rule of 0 Composition | Compose `unique_ptr` / STL containers| Eliminates manual memory code   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}