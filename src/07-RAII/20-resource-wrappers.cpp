/*
 * =====================================================================================
 * CONCEPT        : Resource Wrappers in RAII (Resource Acquisition Is Initialization)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how to design, build, and use custom RAII Resource Wrappers in C++:
 *
 *                  1. Resource Encapsulation   : Wrapping unmanaged, raw handles (pointers, 
 *                                                 file descriptors, sockets, C-API handles) 
 *                                                 into stack-bound C++ abstractions.
 *                  2. Deterministic Release    : Automatic invocation of cleanup/deleter routines 
 *                                                 upon wrapper scope exit or stack unwinding.
 *                  3. Strict Move-Only Invariant: Prohibiting copy semantics (`= delete`) while 
 *                                                 enabling safe, zero-cost move semantics (`std::move`).
 *                  4. Complete Handle Interface: Overloading `operator*`, `operator->`, `explicit operator bool`,
 *                                                 and providing `.get()`, `.release()`, and `.reset()`.
 *                  5. Wrapping C-Style APIs     : Encapsulating pairs of C functions (e.g., `open`/`close`, 
 *                                                 `create`/`destroy`, `malloc`/`free`) safely.
 *                  6. Non-Pointer Handles      : Managing non-pointer stateful resources (sessions, 
 *                                                 hardware pins, transaction locks) via wrappers.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Access / Move / Reset / Release: O(1) constant time.
 * SPACE COMPLEXITY : Wrapper Overhead: Uniform 8-16 bytes (raw handle footprint).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <iomanip>
#include <limits>
#include <cstdio>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE PAYLOAD
// Logs lifecycle milestones to visually observe RAII wrapper operations in real time.
// =====================================================================================
class RawResourcePayload {
private:
    int id_;
    string description_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    RawResourcePayload(int id, string desc) : id_(id), description_(std::move(desc)) {
        ++activeCount;
        cout << "    [RAW RESOURCE CREATED]   ID: " << setw(3) << id_ << " (" << setw(20) << left << description_
             << ") at address " << static_cast<const void*>(this)
             << " | Active Count: " << activeCount << "\n";
    }

    ~RawResourcePayload() noexcept {
        --activeCount;
        cout << "    [RAW RESOURCE DESTROYED] ID: " << setw(3) << id_ << " (" << setw(20) << left << description_
             << ") at address " << static_cast<const void*>(this)
             << " | Active Count: " << activeCount << "\n";
    }

    void executeTask() const {
        cout << "    [PAYLOAD EXECUTION]      ID: " << id_ << " (" << description_ << ") processing...\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getDescription() const { return description_; }
};

// =====================================================================================
// HELPER 2: SIMULATED C-STYLE API FOR OPAQUE HANDLE DEMONSTRATION
// Simulates legacy C-library resource management (e.g., OS Handle, DB Connection, C-API).
// =====================================================================================
struct OpaqueCHandle {
    int handleId;
    char handleName[64];
};

using CHandle = OpaqueCHandle*;

// C-API Allocation/Deallocation pair
inline CHandle c_api_create_handle(int id, const char* name) {
    auto* h = new OpaqueCHandle();
    h->handleId = id;
    std::snprintf(h->handleName, sizeof(h->handleName), "%s", name);
    cout << "    [C-API CREATE]  Created raw C-Handle ID: " << h->handleId 
         << " ('" << h->handleName << "') at " << static_cast<const void*>(h) << "\n";
    return h;
}

inline void c_api_destroy_handle(CHandle handle) noexcept {
    if (handle != nullptr) {
        cout << "    [C-API DESTROY] Freeing raw C-Handle ID: " << handle->handleId 
             << " ('" << handle->handleName << "') at " << static_cast<const void*>(handle) << "\n";
        delete handle;
    }
}

// =====================================================================================
// HELPER CLASS 3: GENERIC RAII RESOURCE WRAPPER (`ResourceWrapper<T, Deleter>`)
// Production-grade implementation of a generic move-only RAII resource wrapper.
// =====================================================================================
template <typename T, typename Deleter = std::default_delete<T>>
class ResourceWrapper {
private:
    T* handle_;
    Deleter deleter_;

public:
    // 1. Constructor: Explicit acquisition
    explicit ResourceWrapper(T* handle = nullptr, Deleter deleter = Deleter()) noexcept
        : handle_(handle), deleter_(std::move(deleter)) {
        cout << "    [ResourceWrapper Ctor] Encapsulated handle at: " 
             << static_cast<const void*>(handle_) << "\n";
    }

    // 2. Destructor: Deterministic cleanup via stored deleter
    ~ResourceWrapper() noexcept {
        reset(); // Delegates to safe reset logic
    }

    // =================================================================================
    // RULE OF 5: COPY SUPPRESSION & MOVE SEMANTICS
    // =================================================================================

    // Prohibit copy constructor and copy assignment (Enforce single ownership invariant)
    ResourceWrapper(const ResourceWrapper&) = delete;
    ResourceWrapper& operator=(const ResourceWrapper&) = delete;

    // Enable move constructor (Transfer handle and disarm source)
    ResourceWrapper(ResourceWrapper&& other) noexcept
        : handle_(other.handle_), deleter_(std::move(other.deleter_)) {
        other.handle_ = nullptr; // Disarm source handle to prevent double-free
        cout << "    [ResourceWrapper Move Ctor] Resource handle ownership transferred.\n";
    }

    // Enable move assignment operator
    ResourceWrapper& operator=(ResourceWrapper&& other) noexcept {
        if (this != &other) { // Self-assignment check
            reset(); // Release currently held resource
            handle_ = other.handle_;
            deleter_ = std::move(other.deleter_);
            other.handle_ = nullptr; // Disarm source handle
            cout << "    [ResourceWrapper Move Assign] Existing freed, new handle ownership transferred.\n";
        }
        return *this;
    }

    // =================================================================================
    // SMART HANDLE INTERFACE METHODS
    // =================================================================================

    // Resets current handle with a new pointer, invoking deleter on previous resource
    void reset(T* newHandle = nullptr) noexcept {
        if (handle_ != newHandle) {
            T* oldHandle = handle_;
            handle_ = newHandle;
            if (oldHandle != nullptr) {
                cout << "    [ResourceWrapper reset] Invoking deleter for old handle: " 
                     << static_cast<const void*>(oldHandle) << "\n";
                deleter_(oldHandle);
            }
        }
    }

    // Relinquishes ownership of the handle without destroying it (Disarms wrapper)
    [[nodiscard]] T* release() noexcept {
        T* temp = handle_;
        handle_ = nullptr;
        cout << "    [ResourceWrapper release] Relinquished ownership of handle: " 
             << static_cast<const void*>(temp) << "\n";
        return temp;
    }

    // Transparent Access Operators & State Query
    [[nodiscard]] T* get() const noexcept { return handle_; }
    [[nodiscard]] explicit operator bool() const noexcept { return handle_ != nullptr; }

    T& operator*() const { return *handle_; }
    T* operator->() const { return handle_; }
};

// Custom Functor Deleter for C-Style API Handle Wrapper
struct CHandleDeleter {
    void operator()(CHandle handle) const noexcept {
        c_api_destroy_handle(handle);
    }
};

// Type alias for C-API Handle RAII Wrapper
using ScopedCHandle = ResourceWrapper<OpaqueCHandle, CHandleDeleter>;

// =====================================================================================
// HELPER CLASS 4: NON-POINTER STATEFUL RESOURCE WRAPPER (`ScopedSessionWrapper`)
// Demonstrates RAII wrapping for non-pointer resource invariants (e.g., Sessions/Locks).
// =====================================================================================
class ScopedSessionWrapper {
private:
    int sessionId_;
    bool isActive_;

public:
    explicit ScopedSessionWrapper(int sessionId) : sessionId_(sessionId), isActive_(true) {
        cout << "    [SESSION OPEN]  Session ID: " << sessionId_ << " acquired and active.\n";
    }

    ~ScopedSessionWrapper() noexcept {
        if (isActive_) {
            cout << "    [SESSION CLOSE] Session ID: " << sessionId_ << " closed automatically on scope exit.\n";
            isActive_ = false;
        }
    }

    // Non-copyable
    ScopedSessionWrapper(const ScopedSessionWrapper&) = delete;
    ScopedSessionWrapper& operator=(const ScopedSessionWrapper&) = delete;

    // Move-only
    ScopedSessionWrapper(ScopedSessionWrapper&& other) noexcept 
        : sessionId_(other.sessionId_), isActive_(other.isActive_) {
        other.isActive_ = false; // Disarm source
        cout << "    [SESSION MOVE]  Session ID: " << sessionId_ << " transferred to new owner.\n";
    }

    ScopedSessionWrapper& operator=(ScopedSessionWrapper&& other) noexcept {
        if (this != &other) {
            if (isActive_) {
                cout << "    [SESSION CLOSE] Closing old Session ID: " << sessionId_ << " before move assign.\n";
            }
            sessionId_ = other.sessionId_;
            isActive_ = other.isActive_;
            other.isActive_ = false;
        }
        return *this;
    }

    void executeOperation(const string& opName) const {
        if (isActive_) {
            cout << "    [SESSION WORK]  Session ID: " << sessionId_ << " executing: '" << opName << "'\n";
        }
    }

    [[nodiscard]] int getSessionId() const noexcept { return sessionId_; }
    [[nodiscard]] bool isActive() const noexcept { return isActive_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for resource wrapper analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC RESOURCE WRAPPER LIFECYCLE
    // =====================================================================================
    cout << "\n================ 1. BASIC RESOURCE WRAPPER ANATOMY ================\n";

    {
        cout << "  - Instantiating `ResourceWrapper<RawResourcePayload>` in block scope...\n";
        ResourceWrapper<RawResourcePayload> wrapperA(
            new RawResourcePayload(userInputValue, "BasicWrapperPayload")
        );

        // Transparent invocation via operator->
        wrapperA->executeTask();

        cout << "  - Exiting block scope (Notice automatic destructor execution):\n";
    } // Wrapper destructor fires HERE automatically!

    cout << "  - Active Instances Post-Basic Scope: " << RawResourcePayload::activeCount << "\n";

    // =====================================================================================
    // 2. SMART HANDLE INTERFACE MECHANICS (.get(), .release(), .reset())
    // =====================================================================================
    cout << "\n================ 2. SMART HANDLE INTERFACE MECHANICS ================\n";

    {
        cout << "  - Instantiating wrapper for handle interface testing...\n";
        ResourceWrapper<RawResourcePayload> testWrapper(
            new RawResourcePayload(userInputValue + 10, "InterfaceTestNode")
        );

        cout << "\n  - Testing `.reset()`: Replacing payload with a new object...\n";
        testWrapper.reset(new RawResourcePayload(userInputValue + 11, "ReplacementNode")); // Frees Node 10, acquires Node 11

        cout << "\n  - Testing `.release()`: Relinquishing ownership to raw pointer...\n";
        RawResourcePayload* rawExtracted = testWrapper.release(); // Disarms wrapper

        cout << "    * testWrapper valid check post-release: " 
             << (testWrapper ? "VALID" : "DISARMED / NULLPTR") << "\n";

        cout << "    * Manually deleting extracted raw payload...\n";
        delete rawExtracted; // Manual cleanup required because wrapper was disarmed!
    }

    cout << "  - Active Instances Post-Interface Scope: " << RawResourcePayload::activeCount << "\n";

    // =====================================================================================
    // 3. WRAPPING C-STYLE API HANDLES (`ScopedCHandle`)
    // =====================================================================================
    cout << "\n================ 3. WRAPPING C-STYLE API HANDLES ================\n";

    {
        cout << "  - Creating raw C-handle via `c_api_create_handle` wrapped in `ScopedCHandle`...\n";
        ScopedCHandle cWrapper(c_api_create_handle(userInputValue + 20, "LegacySystemHandle"));

        cout << "    Accessing C-handle fields via wrapper: ID=" << cWrapper->handleId 
             << ", Name='" << cWrapper->handleName << "'\n";

        cout << "  - Exiting C-handle scope (Wrapper invokes `CHandleDeleter` / `c_api_destroy_handle`):\n";
    } // C-API destroy handle called automatically HERE!

    // =====================================================================================
    // 4. MOVE SEMANTICS & EXPLICIT OWNERSHIP TRANSFER
    // =====================================================================================
    cout << "\n================ 4. MOVE SEMANTICS & OWNERSHIP TRANSFER ================\n";

    {
        cout << "  - Creating source wrapper `sourceWrapper`...\n";
        ResourceWrapper<RawResourcePayload> sourceWrapper(
            new RawResourcePayload(userInputValue + 30, "MovableWrapperNode")
        );

        cout << "  - Initial sourceWrapper address: " 
             << static_cast<const void*>(sourceWrapper.get()) << "\n";

        cout << "\n  - Transferring ownership to `targetWrapper` via `std::move()`...\n";
        ResourceWrapper<RawResourcePayload> targetWrapper = std::move(sourceWrapper);

        cout << "    * Post-Move `sourceWrapper` State : " 
             << (sourceWrapper ? "VALID" : "DISARMED / NULLPTR") << "\n";
        cout << "    * Post-Move `targetWrapper` Address: " 
             << static_cast<const void*>(targetWrapper.get()) << " (Current Exclusive Owner)\n";

        targetWrapper->executeTask();

        cout << "\n  - Leaving scope containing `targetWrapper`...\n";
    }

    cout << "  - Active Instances Post-Move Scope: " << RawResourcePayload::activeCount << "\n";

    // =====================================================================================
    // 5. NON-POINTER STATEFUL RESOURCE WRAPPERS (`ScopedSessionWrapper`)
    // =====================================================================================
    cout << "\n================ 5. NON-POINTER STATEFUL RESOURCE WRAPPERS ================\n";

    {
        cout << "  - Opening session via non-pointer `ScopedSessionWrapper`...\n";
        ScopedSessionWrapper sessionA(userInputValue + 40);
        sessionA.executeOperation("Process Transaction Batch A");

        cout << "\n  - Transferring session ownership via `std::move()`...\n";
        ScopedSessionWrapper sessionB = std::move(sessionA);
        sessionB.executeOperation("Process Transaction Batch B");

        cout << "  - Leaving session scope...\n";
    } // Session closed automatically HERE!

    cout << "\n  - Final Active Instances Count: " << RawResourcePayload::activeCount << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RESOURCE WRAPPERS IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Resource Wrapper Feature| Implementation Mechanism        | Primary Operational Safety Trait  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Acquisition Ctor      | `explicit Wrapper(Handle h)`      | Binds raw handle directly to scope|\n"
         << "| Release Dtor          | `~Wrapper() { deleter_(h_); }`    | Guarantees leak-free auto release |\n"
         << "| Copy Suppression      | `Wrapper(const Wrapper&) = delete`| Prevents double-free crash bugs   |\n"
         << "| Move Semantics        | `Wrapper(Wrapper&&) noexcept`     | Zero-cost ownership transfer      |\n"
         << "| Interface `.get()`    | Returns underlying raw handle     | Enables interoperability with C-APIs|\n"
         << "| Interface `.release()`| Relinquishes handle ownership     | Disarms wrapper without deleting  |\n"
         << "| Interface `.reset()`  | Frees old handle, acquires new ptr| Safe in-place handle re-binding   |\n"
         << "| Custom Deleters       | Policy Functors / Function Pointers| Adapts wrapper to any C-style API |\n"
         << "| Non-Pointer Wrappers  | Encapsulate integer IDs / flags   | Extends RAII to non-memory state  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}