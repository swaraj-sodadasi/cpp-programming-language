/*
 * =====================================================================================
 * CONCEPT        : Scope-Based Resource Management in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  Scope-Based Resource Management—the fundamental core of RAII where
 *                  the acquisition and release of resources are strictly bound to the
 *                  lexical scope of stack variables:
 *
 *                  1. Lexical Scope Binding      : Tying resource lifetime (memory, files, locks) 
 *                                                   directly to block scopes `{ ... }`.
 *                  2. Control Flow Immunity       : Guaranteed cleanup regardless of early `return`, 
 *                                                   `break`, `continue`, or conditional exits.
 *                  3. LIFO Nested Scope Teardown  : Strict Last-In, First-Out destruction order 
 *                                                   across nested block scopes.
 *                  4. Scope-Bound Non-Memory Handles: Managing mutex locks and file handles via 
 *                                                   scope-enforced lifetimes.
 *                  5. Scope-Guard Pattern        : Generic scope exit cleanup via custom `ScopeGuard` 
 *                                                   lambdas.
 *                  6. Exception Scope Unwinding   : Unconditional scope cleanup when exceptions 
 *                                                   propagate up the call stack.
 *
 * TIME COMPLEXITY  : Acquisition / Release / Scope Exit: O(1) constant time.
 * SPACE COMPLEXITY : Scope Overhead: Uniform 8-16 bytes per scoped stack wrapper.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <mutex>
#include <functional>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: SCOPED RESOURCE TRACKER
// Logs entry and exit of block scopes to visually verify deterministic lifetime rules.
// =====================================================================================
class ScopedResourceNode {
private:
    int id_;
    string label_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    ScopedResourceNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeCount;
        cout << "    [SCOPE ENTRY  (Ctor)] Resource ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") acquired at " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    ~ScopedResourceNode() noexcept {
        --activeCount;
        cout << "    [SCOPE EXIT   (Dtor)] Resource ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") released at " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    // Disable copy semantics to keep lifetime tracking unambiguous
    ScopedResourceNode(const ScopedResourceNode&) = delete;
    ScopedResourceNode& operator=(const ScopedResourceNode&) = delete;

    // Enable move semantics
    ScopedResourceNode(ScopedResourceNode&& other) noexcept 
        : id_(other.id_), label_(std::move(other.label_) + "_Moved") {
        other.id_ = -1;
    }

    ScopedResourceNode& operator=(ScopedResourceNode&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            label_ = std::move(other.label_) + "_Moved";
            other.id_ = -1;
        }
        return *this;
    }

    void executeWork() const {
        if (id_ != -1) {
            cout << "    [INSIDE SCOPE (Work)] Resource ID: " << id_ << " (" << label_ << ") processing...\n";
        }
    }

    [[nodiscard]] int getId() const { return id_; }
};

// =====================================================================================
// HELPER CLASS 2: GENERIC SCOPE GUARD (SCOPE-BOUND CLEANUP PATTERN)
// Executes arbitrary cleanup actions upon scope exit (e.g., C-API cleanup, state resets).
// =====================================================================================
class GenericScopeGuard {
private:
    std::function<void()> cleanupTask_;
    bool active_;

public:
    explicit GenericScopeGuard(std::function<void()> task) 
        : cleanupTask_(std::move(task)), active_(true) {
        cout << "    [SCOPE GUARD INIT]    Scope cleanup callback registered.\n";
    }

    ~GenericScopeGuard() noexcept {
        if (active_ && cleanupTask_) {
            try {
                cout << "    [SCOPE GUARD EXECUTE] Executing registered cleanup on scope exit...\n";
                cleanupTask_();
            } catch (...) {
                // Destructors must never allow exceptions to escape!
            }
        }
    }

    // Dismiss the guard if cleanup should be bypassed (e.g., on transaction commit)
    void dismiss() noexcept {
        active_ = false;
        cout << "    [SCOPE GUARD DISMISSED] Cleanup callback disarmed.\n";
    }

    // Non-copyable, non-movable scope invariant
    GenericScopeGuard(const GenericScopeGuard&) = delete;
    GenericScopeGuard& operator=(const GenericScopeGuard&) = delete;
};

// =====================================================================================
// HELPER CLASS 3: SCOPED FILE LOGGING SIMULATOR
// Demonstrates non-memory resource management tied to stack scope.
// =====================================================================================
class ScopedFileLogger {
private:
    string filename_;
    bool isOpen_;

public:
    explicit ScopedFileLogger(string filename) 
        : filename_(std::move(filename)), isOpen_(true) {
        cout << "    [FILE OPEN  (Ctor)] Handle opened for '" << filename_ << "'\n";
    }

    ~ScopedFileLogger() noexcept {
        if (isOpen_) {
            cout << "    [FILE CLOSE (Dtor)] Flushed and closed handle for '" << filename_ << "'\n";
            isOpen_ = false;
        }
    }

    void logMessage(const string& msg) const {
        if (isOpen_) {
            cout << "    [FILE LOG         ] [" << filename_ << "] " << msg << "\n";
        }
    }

    ScopedFileLogger(const ScopedFileLogger&) = delete;
    ScopedFileLogger& operator=(const ScopedFileLogger&) = delete;
};

// =====================================================================================
// HELPER FUNCTIONS FOR CONTROL FLOW & EXCEPTION TESTS
// =====================================================================================

// Demonstrates that early returns inside function scopes do NOT bypass RAII cleanup
void earlyReturnScopeDemo(bool triggerEarlyReturn, int baseId) {
    cout << "  - Entering `earlyReturnScopeDemo()` function scope...\n";
    ScopedResourceNode funcScopedRes(baseId, "FuncScopedRes");
    funcScopedRes.executeWork();

    if (triggerEarlyReturn) {
        cout << "  - [EARLY RETURN TRIGGERED] Executing `return;` statement prematurely...\n";
        return; // Destructor for funcScopedRes executes automatically HERE!
    }

    cout << "  - Reached normal end of function body.\n";
}

// Demonstrates exception unwinding across function block scopes
void exceptionUnwindingScopeDemo(int baseId) {
    cout << "  - Entering `exceptionUnwindingScopeDemo()`...\n";
    ScopedResourceNode outerRes(baseId, "OuterScopeRes");

    {
        cout << "    - Entering inner nested block scope...\n";
        ScopedResourceNode innerRes(baseId + 1, "InnerScopeRes");

        cout << "    - [THROW EXCEPTION] Simulating error in inner scope...\n";
        throw runtime_error("Critical error in nested scope!");

        // innerRes destructor executes HERE during stack unwinding
    }

    // outerRes destructor executes HERE as exception continues unwinding
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for scope-based resource management analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC LEXICAL BLOCK SCOPE (`{ ... }`)
    // =====================================================================================
    cout << "\n================ 1. BASIC LEXICAL BLOCK SCOPE MANAGEMENT ================\n";

    cout << "  - Active Resources Before Scope Entry: " << ScopedResourceNode::activeCount << "\n";

    {
        cout << "  - Entering explicit block scope `{ ... }`...\n";
        ScopedResourceNode scopedNode(userInputValue, "BlockScopeNode");
        scopedNode.executeWork();

        cout << "  - Reached closing brace `}` of block scope...\n";
    } // `scopedNode` destructor executes automatically HERE!

    cout << "  - Active Resources Post Scope Exit: " << ScopedResourceNode::activeCount 
         << " (RESOURCE RELEASED AUTOMATICALLY)\n";

    // =====================================================================================
    // 2. CONTROL FLOW IMMUNITY (EARLY RETURN / BREAK GUARANTEES)
    // =====================================================================================
    cout << "\n================ 2. CONTROL FLOW IMMUNITY (EARLY RETURNS) ================\n";

    cout << "  - Invoking demo function with early return enabled...\n";
    earlyReturnScopeDemo(true, userInputValue + 10);

    cout << "  - Active Resources Post Early Return: " << ScopedResourceNode::activeCount 
         << " (RAII GUARANTEE: Zero leaks on early exit!)\n";

    // =====================================================================================
    // 3. NESTED LEXICAL SCOPES & LIFO TEARDOWN ORDER
    // =====================================================================================
    cout << "\n================ 3. NESTED SCOPES & LIFO TEARDOWN ORDER ================\n";

    {
        cout << "  - Outer Scope Entry...\n";
        ScopedResourceNode outer(userInputValue + 20, "OuterResource");

        {
            cout << "    - Middle Scope Entry...\n";
            ScopedResourceNode middle(userInputValue + 21, "MiddleResource");

            {
                cout << "      - Inner Scope Entry...\n";
                ScopedResourceNode inner(userInputValue + 22, "InnerResource");

                cout << "      - Exiting Inner Scope...\n";
            } // inner destroyed

            cout << "    - Exiting Middle Scope...\n";
        } // middle destroyed

        cout << "  - Exiting Outer Scope...\n";
    } // outer destroyed

    cout << "  - Active Resources Post Nested Scopes: " << ScopedResourceNode::activeCount << "\n";

    // =====================================================================================
    // 4. NON-MEMORY SCOPE MANAGEMENT (FILE HANDLES & MUTEX LOCKS)
    // =====================================================================================
    cout << "\n================ 4. SCOPE-BOUND NON-MEMORY RESOURCES ================\n";

    // A. Scope-bound File Handle
    {
        ScopedFileLogger logger("system_audit.log");
        logger.logMessage("Starting transaction auditing...");
        logger.logMessage("Writing encrypted telemetry payload...");

        cout << "  - Leaving logger block scope...\n";
    } // File automatically flushed & closed HERE!

    // B. Scope-bound Concurrency Lock (`std::lock_guard`)
    mutex systemMutex;
    {
        cout << "\n  - Acquiring `std::lock_guard<std::mutex>` bound to block scope...\n";
        std::lock_guard<std::mutex> lock(systemMutex); // Locks mutex on Ctor
        (void)lock; // Suppress unused variable warning

        cout << "    Critical region executing thread-safe state mutations...\n";

        cout << "  - Leaving critical section scope...\n";
    } // Mutex unlocked automatically on Dtor HERE!

    // =====================================================================================
    // 5. SCOPE GUARD PATTERN (GENERIC EXIT CLEANUP)
    // =====================================================================================
    cout << "\n================ 5. SCOPE GUARD PATTERN (CUSTOM EXIT CALLBACKS) ================\n";

    {
        cout << "  - Registering scope guard for temporary system configuration override...\n";
        bool configModified = true;
        cout << "    System state: Config modified = " << (configModified ? "TRUE" : "FALSE") << "\n";

        GenericScopeGuard guard([&configModified]() {
            configModified = false;
            cout << "    [SCOPE GUARD CALLBACK] Reverted system configuration override to FALSE.\n";
        });

        cout << "  - Performing operations with modified state...\n";
        cout << "  - Exiting block scope containing guard...\n";
    } // GenericScopeGuard destructor runs callback HERE!

    // =====================================================================================
    // 6. EXCEPTION UNWINDING ACROSS SCOPES
    // =====================================================================================
    cout << "\n================ 6. EXCEPTION UNWINDING ACROSS SCOPES ================\n";

    try {
        exceptionUnwindingScopeDemo(userInputValue + 30);
    } catch (const std::exception& e) {
        cout << "  - [CATCH BLOCK EXECUTED] Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Resources Post Exception Unwind: " << ScopedResourceNode::activeCount 
         << " (RAII GUARANTEE: Scope cleanup completed during unwind!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ SCOPE-BASED RESOURCE MANAGEMENT SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Scope Mechanism       | Syntax / Implementation Pattern   | Primary Operational Safety Trait  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Basic Lexical Scope   | `{ RAIIObject obj; }`             | Auto-cleanup at closing brace `}` |\n"
         << "| Control Flow Immunity | Early `return`, `break`, `continue`| Zero leaks regardless of exit path|\n"
         << "| Nested Scope Order    | Nested `{ { { } } }` blocks       | Strict LIFO teardown sequence     |\n"
         << "| Scoped File Handles   | `ScopedFileLogger logger(file);`  | Guaranteed flush/close on exit    |\n"
         << "| Scoped Mutex Locks    | `std::lock_guard<std::mutex> lock`| Unlocks mutex on critical exit    |\n"
         << "| ScopeGuard Pattern    | `GenericScopeGuard guard(lambda);`| Custom cleanup logic on scope exit|\n"
         << "| Exception Unwinding   | `throw runtime_error(...);`       | Cleans stack frame scopes on error|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}