/*
 * =====================================================================================
 * CONCEPT        : Writing Exception-Safe Code in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the fundamental techniques, patterns, and guidelines for writing 
 *                  Exception-Safe Code in Modern C++:
 *
 *                  1. RAII (Resource Acquisition Is Initialization) :
 *                     - Encapsulating raw resources (heap memory, file handles, locks) inside
 *                       objects whose destructors release them automatically during stack unwinding.
 *                     - Eliminates manual resource cleanup and prevents memory/resource leaks.
 *
 *                  2. The Copy-and-Swap Idiom (Strong Exception Guarantee) :
 *                     - Performing all allocation and modification work on temporary local copies.
 *                     - Committing changes via zero-cost, `noexcept` swap operations.
 *                     - Guarantees commit-or-rollback semantics (original state preserved on failure).
 *
 *                  3. `noexcept` Boundaries & Guarantees :
 *                     - Marking destructors, move operations, and swap methods `noexcept` so 
 *                       that runtime operations (like container reallocations) can rely on them.
 *
 *                  4. Scope Guards for Multi-Step Rollbacks :
 *                     - Using `std::uncaught_exceptions()` to execute rollback logic if and only if
 *                       a scope exits due to stack unwinding.
 *
 *                  5. Exception-Safe Lock Acquisition :
 *                     - Utilizing RAII lock managers (`std::scoped_lock` / `std::lock_guard`) 
 *                       to ensure mutex locks are freed when an exception bypasses normal control flow.
 *
 * TIME COMPLEXITY  : Copy-and-Swap Operation : O(N) copy allocation + O(1) nothrow swap.
 *                    RAII Cleanup Overhead   : O(1) constant-time destructor dispatch.
 * SPACE COMPLEXITY : Staging Buffer Overhead : O(N) temporary space for strong guarantee buffers.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <memory>
#include <utility>
#include <vector>
#include <mutex>
#include <limits>
#include <cstddef>
#include <functional>

using namespace std;

// =====================================================================================
// 1. ANTI-PATTERN: EXCEPTION-UNSAFE CLASS
// Demonstrates how raw pointers and non-RAII designs leak resources and break invariants.
// =====================================================================================
class UnsafeBufferManager {
private:
    int* rawBuffer_{nullptr};
    size_t capacity_{0};

public:
    UnsafeBufferManager(size_t cap, int defaultVal)
        : rawBuffer_(new int[cap]), capacity_(cap) {
        for (size_t i = 0; i < capacity_; ++i) {
            rawBuffer_[i] = defaultVal;
        }
    }

    ~UnsafeBufferManager() noexcept {
        delete[] rawBuffer_;
    }

    // UNSAFE ASSIGNMENT OPERATOR:
    // If allocation for new memory fails, or if array copying throws, the original object
    // is deleted or left in a dangling, corrupted state!
    UnsafeBufferManager& operator=(const UnsafeBufferManager& other) {
        if (this != &other) {
            delete[] rawBuffer_; // 1. Deletes old state BEFORE acquiring new state!
            rawBuffer_ = nullptr;

            // 2. If an exception occurs during allocation or copying, object is CORRUPTED!
            rawBuffer_ = new int[other.capacity_]; 
            capacity_ = other.capacity_;
            for (size_t i = 0; i < capacity_; ++i) {
                rawBuffer_[i] = other.rawBuffer_[i];
            }
        }
        return *this;
    }

    void simulateFailureUpdate(size_t index, int val) {
        if (index >= capacity_) {
            throw std::out_of_range("UnsafeBuffer: Index out of range!");
        }
        rawBuffer_[index] = val;
    }

    void printBuffer() const {
        cout << "    [UnsafeBuffer]: { ";
        for (size_t i = 0; i < capacity_; ++i) {
            cout << rawBuffer_[i] << (i + 1 < capacity_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// =====================================================================================
// 2. BEST PRACTICE: EXCEPTION-SAFE CLASS USING RAII & COPY-AND-SWAP
// Provides Strong Exception Guarantee and zero-leak invariants.
// =====================================================================================
class SafeBufferManager {
private:
    std::unique_ptr<int[]> safeBuffer_;
    size_t capacity_;

public:
    // Constructor: RAII acquisition
    SafeBufferManager(size_t cap, int defaultVal)
        : safeBuffer_(std::make_unique<int[]>(cap)), capacity_(cap) {
        for (size_t i = 0; i < capacity_; ++i) {
            safeBuffer_[i] = defaultVal;
        }
    }

    // Copy Constructor
    SafeBufferManager(const SafeBufferManager& other)
        : safeBuffer_(std::make_unique<int[]>(other.capacity_)), capacity_(other.capacity_) {
        for (size_t i = 0; i < capacity_; ++i) {
            safeBuffer_[i] = other.safeBuffer_[i];
        }
    }

    // Move Constructor marked `noexcept`
    SafeBufferManager(SafeBufferManager&& other) noexcept
        : safeBuffer_(std::move(other.safeBuffer_)), capacity_(other.capacity_) {
        other.capacity_ = 0;
    }

    // Nothrow Swap function
    void swap(SafeBufferManager& other) noexcept {
        using std::swap;
        swap(safeBuffer_, other.safeBuffer_);
        swap(capacity_, other.capacity_);
    }

    // COPY-AND-SWAP ASSIGNMENT OPERATOR:
    // Provides Strong Exception Guarantee! 'other' is passed BY VALUE (copied locally).
    // If copying fails during caller parameter setup, this function is never entered.
    SafeBufferManager& operator=(SafeBufferManager other) noexcept {
        swap(other); // Nothrow swap exchanges state with local copy
        return *this;
    } // Local copy 'other' is destructed HERE, safely cleaning up old buffer!

    // Exception-safe update with Strong Guarantee (commit-or-rollback)
    void updateBatchStrong(const vector<int>& newVals, bool triggerFailure) {
        cout << "    [SafeBuffer] Staging updates in local temporary buffer...\n";

        // 1. Create temporary staging buffer
        auto tempStaging = std::make_unique<int[]>(capacity_);
        for (size_t i = 0; i < capacity_; ++i) {
            tempStaging[i] = safeBuffer_[i];
        }

        // 2. Apply modifications to staging buffer
        for (size_t i = 0; i < std::min(capacity_, newVals.size()); ++i) {
            tempStaging[i] = newVals[i];

            if (triggerFailure && i == 1) {
                cout << "    [SafeBuffer THROW] Exception encountered during update staging at index " << i << "!\n";
                throw std::runtime_error("Transactional update failed! Initiating full rollback.");
            }
        }

        // 3. Commit phase: Swap only when modifications succeed completely
        safeBuffer_.swap(tempStaging);
        cout << "    [SafeBuffer COMMIT] Transaction committed successfully via nothrow swap!\n";
    }

    void printBuffer() const {
        cout << "    [SafeBuffer]: { ";
        for (size_t i = 0; i < capacity_; ++i) {
            cout << safeBuffer_[i] << (i + 1 < capacity_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// Nothrow swap overload
void swap(SafeBufferManager& a, SafeBufferManager& b) noexcept {
    a.swap(b);
}

// =====================================================================================
// 3. SCOPE GUARD FOR TRANSACTIONAL ROLLBACKS (`ScopeGuard`)
// Uses C++17 `std::uncaught_exceptions()` to trigger rollbacks on exception unwind.
// =====================================================================================
class ScopeRollbackGuard {
private:
    std::function<void()> rollbackAction_;
    int initialUncaughtCount_;
    bool dismissed_{false};

public:
    explicit ScopeRollbackGuard(std::function<void()> action)
        : rollbackAction_(std::move(action)),
          initialUncaughtCount_(std::uncaught_exceptions()) {}

    ~ScopeRollbackGuard() noexcept {
        // Run rollback action ONLY IF scope exit was triggered by an exception throw
        if (!dismissed_ && std::uncaught_exceptions() > initialUncaughtCount_) {
            try {
                if (rollbackAction_) {
                    rollbackAction_();
                }
            } catch (...) {
                // Ignore exceptions in scope guard destructor to obey noexcept rule!
            }
        }
    }

    void dismiss() noexcept {
        dismissed_ = true;
    }
};

// =====================================================================================
// 4. THREAD-SAFE & EXCEPTION-SAFE MUTEX LOCK MANAGEMENT
// =====================================================================================
class ExceptionSafeLockManager {
private:
    std::mutex resourceMutex_;
    int sharedResource_{0};

public:
    void executeLockedOperation(int newVal, bool triggerFailure) {
        // std::scoped_lock (RAII) guarantees lock release even if exception is thrown!
        std::scoped_lock lock(resourceMutex_);
        cout << "    [LockManager] Acquired RAII mutex lock successfully.\n";

        sharedResource_ = newVal;

        if (triggerFailure) {
            cout << "    [LockManager THROW] Aborting operation inside locked region...\n";
            throw std::runtime_error("Operational exception inside locked critical section!");
        }

        cout << "    [LockManager] Operation completed. Releasing lock on normal exit.\n";
    } // Mutex is automatically unlocked HERE by scoped_lock destructor
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Exception-Safe Code analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    vector<int> updateData = {userInputValue + 10, userInputValue + 20, userInputValue + 30, userInputValue + 40};

    // =====================================================================================
    // 1. EXCEPTION-UNSAFE vs EXCEPTION-SAFE DESIGN COMPARISON
    // =====================================================================================
    cout << "\n================ 1. EXCEPTION-SAFE DESIGN & COPY-AND-SWAP ================\n";

    SafeBufferManager safeManager(4, userInputValue);
    cout << "  - Initial State:\n";
    safeManager.printBuffer();

    // Scenario A: Rollback on exception failure
    try {
        cout << "\n  - Scenario A: Executing transactional batch update (Failure Trigger = TRUE)...\n";
        safeManager.updateBatchStrong(updateData, true);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT EXCEPTION]: " << ex.what() << "\n";
        cout << "  - State After Exception (Rollback Guaranteed):\n";
        safeManager.printBuffer();
        cout << "    [RESULT]: Original buffer state was preserved 100% intact!\n";
    }

    // Scenario B: Successful commit
    try {
        cout << "\n  - Scenario B: Executing transactional batch update (Failure Trigger = FALSE)...\n";
        safeManager.updateBatchStrong(updateData, false);
        cout << "  - State After Successful Commit:\n";
        safeManager.printBuffer();
    } catch (const std::exception& ex) {
        cout << "  - [UNREACHABLE]: " << ex.what() << "\n";
    }

    // =====================================================================================
    // 2. SCOPE ROLLBACK GUARD DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 2. SCOPE ROLLBACK GUARD (`ScopeRollbackGuard`) ================\n";

    int simulatedDatabaseState = 500;
    cout << "  - Initial Database State Value: " << simulatedDatabaseState << "\n";

    try {
        cout << "  - Modifying database state with ScopeRollbackGuard protection...\n";
        simulatedDatabaseState = 999; // Tentative change

        // Define rollback action if scope unwinds due to exception
        ScopeRollbackGuard rollbackGuard([&simulatedDatabaseState]() {
            cout << "    [ScopeRollbackGuard EXECUTED] Reverting database state back to 500!\n";
            simulatedDatabaseState = 500;
        });

        cout << "  - Simulating external pipeline failure...\n";
        throw std::runtime_error("Pipeline Error: Failed to push transaction to secondary node");

        // rollbackGuard.dismiss(); // Unreachable due to throw
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT PIPELINE EXCEPTION]: " << ex.what() << "\n";
        cout << "  - Database State After Rollback: " << simulatedDatabaseState << "\n";
        cout << "    [RESULT]: ScopeRollbackGuard automatically reverted state on exception!\n";
    }

    // =====================================================================================
    // 3. EXCEPTION-SAFE MUTEX LOCKING
    // =====================================================================================
    cout << "\n================ 3. EXCEPTION-SAFE MUTEX LOCKING (`std::scoped_lock`) ================\n";

    ExceptionSafeLockManager lockManager;

    try {
        cout << "  - Executing locked operation with failure trigger = TRUE...\n";
        lockManager.executeLockedOperation(userInputValue + 50, true);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT LOCK EXCEPTION]: " << ex.what() << "\n";
        cout << "    [RESULT]: Lock manager automatically released mutex during stack unwinding!\n";
    }

    // Re-attempt operation to verify lock was properly released
    try {
        cout << "\n  - Re-attempting locked operation with failure trigger = FALSE...\n";
        lockManager.executeLockedOperation(userInputValue + 100, false);
        cout << "    [RESULT]: Lock re-acquired successfully! No deadlock occurred.\n";
    } catch (const std::exception& ex) {
        cout << "  - [UNREACHABLE]: " << ex.what() << "\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ WRITING EXCEPTION-SAFE CODE SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Technique / Pattern   | C++ Implementation Standard       | Architectural Benefit & Guarantee |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| RAII Resource Guards  | `std::unique_ptr`, Smart Pointers | Zero resource/memory leaks        |\n"
         << "| Copy-and-Swap Idiom   | `SafeClass& operator=(SafeClass)` | Strong Guarantee (Commit/Rollback)|\n"
         << "| `noexcept` Operations | `Move Ctor`, `Swap`, `Destructor` | Safe container reallocations      |\n"
         << "| Scope Rollback Guards | `std::uncaught_exceptions()` > 0  | Automatic multi-step rollbacks    |\n"
         << "| RAII Lock Managers    | `std::scoped_lock lock(mutex);`   | Prevents mutex deadlock/lock leaks|\n"
         << "| Never Throw in Dtor   | `~Class() noexcept`               | Prevents immediate std::terminate |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}