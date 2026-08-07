/*
 * =====================================================================================
 * CONCEPT        : Relationship of RAII with Other C++ Topics
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how the Resource Acquisition Is Initialization (RAII) paradigm serves
 *                  as the foundational pillar intersecting with core Modern C++ topics:
 *
 *                  1. Move Semantics & Ownership : Enables zero-cost ownership transfers
 *                                                  (Move-Only RAII, Rule of 5 vs Rule of 0).
 *                  2. Exception Safety & Unwinding: Guarantees leak-free cleanup during exception
 *                                                  propagation (Basic & Strong Guarantees).
 *                  3. Concurrency & Multi-Threading: Binds thread synchronization (`std::lock_guard`,
 *                                                  `std::scoped_lock`) and thread lifecycles (`std::jthread`).
 *                  4. C-API & OS Handle Interop   : Encapsulates legacy raw handles (`FILE*`, sockets,
 *                                                  pointers) via custom deleters and scope wrappers.
 *                  5. Transactional Design Patterns: Implements Scope Guards and Rollback Guards to
 *                                                  maintain class/system state invariants.
 *                  6. STL Containers & Memory    : Powers automatic memory allocation, resizing, and
 *                                                  element destruction in `std::vector` and smart pointers.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Move / Lock / Scope Exit: O(1) constant time.
 * SPACE COMPLEXITY : RAII Handle Overhead: Uniform 8-24 bytes (raw pointer / smart pointer size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>
#include <iomanip>
#include <limits>
#include <cstdio>

using namespace std;

// =====================================================================================
// HELPER CLASS: LIFECYCLE TRACKER ENTITY
// Tracks instance lifetimes to visually observe RAII interactions across topics.
// =====================================================================================
class TopicEntityNode {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    TopicEntityNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [ENTITY ALLOCATED]   ID: " << setw(3) << id_ << " (" << setw(22) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~TopicEntityNode() noexcept {
        --activeInstances;
        cout << "    [ENTITY DEALLOCATED] ID: " << setw(3) << id_ << " (" << setw(22) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    // Disable copies to enforce unambiguous RAII lifecycle tracking
    TopicEntityNode(const TopicEntityNode&) = delete;
    TopicEntityNode& operator=(const TopicEntityNode&) = delete;

    // Move Construction & Move Assignment
    TopicEntityNode(TopicEntityNode&& other) noexcept 
        : id_(other.id_), label_(std::move(other.label_) + "_Moved") {
        other.id_ = -1;
    }

    TopicEntityNode& operator=(TopicEntityNode&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            label_ = std::move(other.label_) + "_Moved";
            other.id_ = -1;
        }
        return *this;
    }

    void executeWork() const {
        if (id_ != -1) {
            cout << "    [PAYLOAD EXECUTING]  ID: " << id_ << " (" << label_ << ") performing task.\n";
        }
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
    [[nodiscard]] const string& getLabel() const noexcept { return label_; }
};

// =====================================================================================
// TOPIC INTERSECTION 1: RAII & DESIGN PATTERNS (SCOPE GUARD)
// =====================================================================================
class GenericScopeGuard {
private:
    std::function<void()> cleanupTask_;
    bool active_;

public:
    explicit GenericScopeGuard(std::function<void()> task) noexcept
        : cleanupTask_(std::move(task)), active_(true) {}

    ~GenericScopeGuard() noexcept {
        if (active_ && cleanupTask_) {
            try {
                cleanupTask_();
            } catch (...) {
                // Suppress exceptions inside destructor to enforce noexcept contract
            }
        }
    }

    void dismiss() noexcept { active_ = false; }

    GenericScopeGuard(const GenericScopeGuard&) = delete;
    GenericScopeGuard& operator=(const GenericScopeGuard&) = delete;
};

// =====================================================================================
// TOPIC INTERSECTION 2: RAII & C-API / LEGACY HANDLES
// Simulated legacy C-API library handle
// =====================================================================================
struct LegacyCHandle {
    int handleId;
    char buffer[32];
};

inline LegacyCHandle* legacy_c_api_open(int id) {
    auto* h = new LegacyCHandle();
    h->handleId = id;
    std::snprintf(h->buffer, sizeof(h->buffer), "C_Handle_Buffer_%d", id);
    cout << "    [C-API OPEN]  Acquired legacy C-handle at address " << static_cast<const void*>(h) << "\n";
    return h;
}

inline void legacy_c_api_close(LegacyCHandle* h) noexcept {
    if (h != nullptr) {
        cout << "    [C-API CLOSE] Releasing legacy C-handle at address " << static_cast<const void*>(h) << "\n";
        delete h;
    }
}

// Custom RAII Wrapper for C-API Handles
class ScopedCHandleWrapper {
private:
    LegacyCHandle* rawHandle_;

public:
    explicit ScopedCHandleWrapper(LegacyCHandle* h = nullptr) noexcept : rawHandle_(h) {}

    ~ScopedCHandleWrapper() noexcept {
        legacy_c_api_close(rawHandle_);
    }

    ScopedCHandleWrapper(const ScopedCHandleWrapper&) = delete;
    ScopedCHandleWrapper& operator=(const ScopedCHandleWrapper&) = delete;

    ScopedCHandleWrapper(ScopedCHandleWrapper&& other) noexcept : rawHandle_(other.rawHandle_) {
        other.rawHandle_ = nullptr;
    }

    ScopedCHandleWrapper& operator=(ScopedCHandleWrapper&& other) noexcept {
        if (this != &other) {
            legacy_c_api_close(rawHandle_);
            rawHandle_ = other.rawHandle_;
            other.rawHandle_ = nullptr;
        }
        return *this;
    }

    [[nodiscard]] LegacyCHandle* get() const noexcept { return rawHandle_; }
    LegacyCHandle* operator->() const noexcept { return rawHandle_; }
};

// =====================================================================================
// TOPIC INTERSECTION 3: RAII & THREAD MANAGEMENT (SCOPED THREAD JOINER)
// =====================================================================================
class ScopedThreadHandle {
private:
    thread workerThread_;

public:
    explicit ScopedThreadHandle(thread t) : workerThread_(std::move(t)) {
        if (!workerThread_.joinable()) {
            throw invalid_argument("ScopedThreadHandle requires a joinable thread!");
        }
    }

    ~ScopedThreadHandle() noexcept {
        if (workerThread_.joinable()) {
            cout << "    [THREAD RAII DTOR] Joining worker thread automatically on scope exit...\n";
            workerThread_.join();
        }
    }

    ScopedThreadHandle(const ScopedThreadHandle&) = delete;
    ScopedThreadHandle& operator=(const ScopedThreadHandle&) = delete;

    ScopedThreadHandle(ScopedThreadHandle&&) noexcept = default;
    ScopedThreadHandle& operator=(ScopedThreadHandle&&) noexcept = default;
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for RAII cross-topic relationship analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. RAII & MOVE SEMANTICS / RULE OF 5 VS RULE OF 0
    // =====================================================================================
    cout << "\n================ 1. RAII & MOVE SEMANTICS (RULE OF 5 / RULE OF 0) ================\n";

    {
        cout << "  - Demonstrating Move Semantics transferring exclusive RAII resource ownership...\n";
        auto primaryOwner = std::make_unique<TopicEntityNode>(userInputValue, "ExclusiveNodeA");

        cout << "    Primary Owner Address: " << static_cast<const void*>(primaryOwner.get()) << "\n";

        cout << "  - Moving ownership to `targetOwner` via `std::move()`...\n";
        std::unique_ptr<TopicEntityNode> targetOwner = std::move(primaryOwner);

        cout << "    Post-Move Primary Owner : " << (primaryOwner == nullptr ? "NULLPTR (Ownership Relinquished)" : "VALID") << "\n";
        cout << "    Post-Move Target Owner  : " << static_cast<const void*>(targetOwner.get()) << " (Current Exclusive Owner)\n";

        targetOwner->executeWork();
        cout << "  - Exiting scope containing `targetOwner`...\n";
    } // Memory automatically freed HERE!

    cout << "  - Active Instances Post Move Scope: " << TopicEntityNode::activeInstances << "\n";

    // =====================================================================================
    // 2. RAII & EXCEPTION SAFETY / STACK UNWINDING
    // =====================================================================================
    cout << "\n================ 2. RAII & EXCEPTION SAFETY / STACK UNWINDING ================\n";

    try {
        cout << "  - Entering scope allocating multiple RAII stack objects...\n";
        auto node1 = std::make_unique<TopicEntityNode>(userInputValue + 10, "UnwindNode1");
        auto node2 = std::make_unique<TopicEntityNode>(userInputValue + 11, "UnwindNode2");

        node1->executeWork();
        node2->executeWork();

        cout << "  - Throwing runtime exception inside scope pipeline...\n";
        throw std::runtime_error("Simulated execution failure during batch processing!");

        // Unreachable code
    } catch (const std::exception& e) {
        cout << "  - [CATCH BLOCK EXECUTED] Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post Exception Unwind: " << TopicEntityNode::activeInstances 
         << " (RAII GUARANTEE: Zero leaks during stack unwinding!)\n";

    // =====================================================================================
    // 3. RAII & CONCURRENCY SYNCHRONIZATION (MUTEXES & THREADS)
    // =====================================================================================
    cout << "\n================ 3. RAII & CONCURRENCY SYNCHRONIZATION ================\n";

    mutex resourceMutex;

    {
        cout << "  - A. Concurrency Lock RAII (`std::lock_guard`):\n";
        {
            std::lock_guard<mutex> lock(resourceMutex);
            cout << "    Critical region safely protected under lock_guard.\n";
        } // Mutex unlocked automatically HERE!

        cout << "\n  - B. Execution Thread RAII (`ScopedThreadHandle`):\n";
        int targetId = userInputValue + 20;
        ScopedThreadHandle threadGuard(std::thread([targetId]() {
            cout << "    [ASYNC THREAD] Worker thread " << targetId << " executing in background...\n";
        }));

        cout << "  - Main thread continuing execution... Leaving thread guard scope...\n";
    } // ScopedThreadHandle destructor automatically calls .join() HERE!

    cout << "  - Worker thread joined cleanly.\n";

    // =====================================================================================
    // 4. RAII & LEGACY C-API HANDLES
    // =====================================================================================
    cout << "\n================ 4. RAII & C-API / LEGACY OS HANDLES ================\n";

    {
        cout << "  - Wrapping legacy C-API open/close handle pair inside `ScopedCHandleWrapper`...\n";
        ScopedCHandleWrapper cWrapper(legacy_c_api_open(userInputValue + 30));

        cout << "    Accessing raw C-handle buffer via wrapper: \"" << cWrapper->buffer << "\"\n";

        cout << "  - Exiting C-handle scope...\n";
    } // C-API close handle function called automatically HERE!

    // =====================================================================================
    // 5. RAII & DESIGN PATTERNS (SCOPE GUARD & TRANSACTION ROLLBACK)
    // =====================================================================================
    cout << "\n================ 5. RAII & DESIGN PATTERNS (SCOPE GUARD) ================\n";

    {
        cout << "  - Demonstrating Scope Guard Pattern for non-resource state cleanup...\n";
        bool systemStateActive = true;

        {
            GenericScopeGuard guard([&systemStateActive]() {
                systemStateActive = false;
                cout << "    [SCOPE GUARD CALLBACK] System state restored to INACTIVE on scope exit.\n";
            });

            cout << "    Current system state: " << (systemStateActive ? "ACTIVE" : "INACTIVE") << "\n";
            cout << "  - Leaving scope containing Scope Guard...\n";
        } // Scope Guard callback executes automatically HERE!
    }

    // =====================================================================================
    // 6. RAII & STL CONTAINER INTEGRATION
    // =====================================================================================
    cout << "\n================ 6. RAII & STL CONTAINER INTEGRATION ================\n";

    {
        cout << "  - Creating `std::vector<std::unique_ptr<TopicEntityNode>>`...\n";
        std::vector<std::unique_ptr<TopicEntityNode>> container;
        container.reserve(2);

        container.push_back(std::make_unique<TopicEntityNode>(userInputValue + 40, "VectorNode1"));
        container.push_back(std::make_unique<TopicEntityNode>(userInputValue + 41, "VectorNode2"));

        for (const auto& item : container) {
            item->executeWork();
        }

        cout << "  - Exiting container block scope...\n";
    } // Vector destructor deallocates buffer and calls smart pointer destructors automatically HERE!

    cout << "\n  - Final Active Resource Instances Count: " << TopicEntityNode::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RELATIONSHIP WITH OTHER TOPICS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Related C++ Topic     | Technical Intersect with RAII    | Primary Operational Benefit       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Move Semantics        | `std::move` transfers ownership   | Enables Move-Only exclusive RAII  |\n"
         << "| Rule of 0 / Rule of 5 | Directs class copy/move/dtor rules| Eliminates manual memory management|\n"
         << "| Exception Safety      | Unconditional stack unwinding     | Guarantees zero leaks on throw    |\n"
         << "| Concurrency (Locks)   | `std::lock_guard`, `scoped_lock`  | Exception-safe deadlock prevention|\n"
         << "| Concurrency (Threads) | `std::jthread`, Scoped Thread     | Auto `join()` / `detach()` on exit|\n"
         << "| Legacy C-APIs         | Wraps `FILE*`, sockets, raw handles| Bridges modern C++ with C libraries|\n"
         << "| Scope Guard Pattern   | Executes cleanup lambdas on exit  | Maintains non-resource invariants |\n"
         << "| STL Containers        | Destructors clear dynamic buffers | Automatic container memory cleanup|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}