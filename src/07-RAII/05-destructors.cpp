/*
 * =====================================================================================
 * CONCEPT        : Destructors in RAII (Resource Acquisition Is Initialization)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the critical role of destructors as the core release mechanism in RAII:
 *
 *                  1. Deterministic Scope Cleanup : Automatic, guaranteed destructor execution 
 *                                                   upon scope exit (block, return, exception).
 *                  2. LIFO Teardown Mechanics     : Stack objects and class members destructed in 
 *                                                   strict reverse order of construction.
 *                  3. `noexcept` Destructor Rule  : Why destructors must NEVER allow exceptions 
 *                                                   to escape (preventing `std::terminate`).
 *                  4. Virtual Destructors         : Ensuring proper polymorphic cleanup when 
 *                                                   deleting derived objects via base pointers.
 *                  5. Custom Non-Memory Releases  : Wrapping C-style handles (files, sockets, mutexes) 
 *                                                   with custom destructor cleanup routines.
 *                  6. Custom Smart Pointer Deleters: Tying custom destructors to `std::unique_ptr`.
 *
 * TIME COMPLEXITY  : Destructor Invocation / Resource Release: O(1) constant time.
 * SPACE COMPLEXITY : RAII Handle Footprint: Uniform 8-16 bytes (raw pointer/handle size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: LIFECYCLE & DESTRUCTION TRACKER
// Tracks active instances to visually verify clean destructor calls.
// =====================================================================================
class ResourceTracker {
private:
    int id_;
    string label_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    ResourceTracker(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeCount;
        cout << "    [ACQUIRED (Ctor)] Resource ID: " << id_ << " (" << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeCount << "\n";
    }

    ~ResourceTracker() noexcept {
        --activeCount;
        cout << "    [RELEASED (Dtor)] Resource ID: " << id_ << " (" << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Remaining Active: " << activeCount << "\n";
    }

    void execute() const {
        cout << "    [EXECUTE] Resource ID: " << id_ << " (" << label_ << ") performing action.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
};

// =====================================================================================
// HELPER CLASS 2: SAFE `noexcept` DESTRUCTOR HANDLING
// Demonstrates how destructors must handle internal errors without throwing exceptions.
// =====================================================================================
class SafeRaiiHandle {
private:
    int handleId_;
    bool simulateReleaseFailure_;

public:
    SafeRaiiHandle(int id, bool simulateFailure)
        : handleId_(id), simulateReleaseFailure_(simulateFailure) {
        cout << "    [SafeRaiiHandle Ctor] Opened system handle ID: " << handleId_ << "\n";
    }

    // Destructors are implicitly noexcept in C++11 and later.
    // ALWAYS catch internal release exceptions inside the destructor!
    ~SafeRaiiHandle() noexcept {
        try {
            cout << "    [SafeRaiiHandle Dtor] Attempting to close handle ID: " << handleId_ << "...\n";
            if (simulateReleaseFailure_) {
                throw std::runtime_error("OS system call failed during handle closure!");
            }
            cout << "    [SafeRaiiHandle Dtor] Handle ID: " << handleId_ << " closed successfully.\n";
        } catch (const std::exception& e) {
            // CRITICAL RAII RULE: Log or handle error locally; NEVER allow exception to escape!
            cout << "    [SafeRaiiHandle Dtor CATCH] Exception suppressed inside destructor: \"" 
                 << e.what() << "\"\n";
            cout << "    [SAFETY GUARANTEE] Prevented `std::terminate()` during stack unwinding.\n";
        }
    }

    // Rule of 5: Suppress copy operations, enable moves
    SafeRaiiHandle(const SafeRaiiHandle&) = delete;
    SafeRaiiHandle& operator=(const SafeRaiiHandle&) = delete;

    SafeRaiiHandle(SafeRaiiHandle&& other) noexcept 
        : handleId_(other.handleId_), simulateReleaseFailure_(other.simulateReleaseFailure_) {
        other.handleId_ = -1;
    }

    SafeRaiiHandle& operator=(SafeRaiiHandle&& other) noexcept {
        if (this != &other) {
            handleId_ = other.handleId_;
            simulateReleaseFailure_ = other.simulateReleaseFailure_;
            other.handleId_ = -1;
        }
        return *this;
    }
};

// =====================================================================================
// HELPER CLASSES 3: POLYMORPHIC INHERITANCE & VIRTUAL DESTRUCTORS
// =====================================================================================
class BaseRaiiResource {
public:
    BaseRaiiResource() {
        cout << "    [BaseRaiiResource Ctor] Base sub-object created.\n";
    }

    // CRITICAL: Virtual destructor ensures derived destructors execute when deleting via Base*
    virtual ~BaseRaiiResource() noexcept {
        cout << "    [BaseRaiiResource Dtor] Base sub-object destroyed.\n";
    }

    virtual void process() const = 0;
};

class DerivedRaiiResource : public BaseRaiiResource {
private:
    int* dynamicPayload_;

public:
    explicit DerivedRaiiResource(int val) : dynamicPayload_(new int(val)) {
        cout << "    [DerivedRaiiResource Ctor] Dynamic payload allocated (" << *dynamicPayload_ << ").\n";
    }

    ~DerivedRaiiResource() noexcept override {
        cout << "    [DerivedRaiiResource Dtor] Freeing dynamic payload...\n";
        delete dynamicPayload_;
        dynamicPayload_ = nullptr;
    }

    void process() const override {
        if (dynamicPayload_) {
            cout << "    [DerivedRaiiResource Process] Payload Value: " << *dynamicPayload_ << "\n";
        }
    }
};

// =====================================================================================
// HELPER FUNCTION: CUSTOM DELETER FOR C-STYLE HANDLES
// =====================================================================================
struct CStyleResource {
    int resourceId;
};

void freeCResource(CStyleResource* res) noexcept {
    if (res != nullptr) {
        cout << "    [CUSTOM DELETER FUNCTION] Releasing C-Style Resource ID: " << res->resourceId << "\n";
        delete res;
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for destructor mechanics analysis (e.g., 200): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 200." << endl;
        userInputValue = 200;
    }

    // =====================================================================================
    // 1. DETERMINISTIC SCOPE CLEANUP & LIFO ORDER
    // =====================================================================================
    cout << "\n================ 1. DETERMINISTIC CLEANUP & LIFO ORDER ================\n";

    {
        cout << "  - Entering local block scope...\n";
        ResourceTracker resA(userInputValue, "FirstCreated_ResA");
        ResourceTracker resB(userInputValue + 1, "SecondCreated_ResB");
        ResourceTracker resC(userInputValue + 2, "ThirdCreated_ResC");

        resA.execute();
        resB.execute();
        resC.execute();

        cout << "  - Exiting local block scope...\n";
        cout << "  - [NOTICE]: Destructors execute in LIFO order (ResC -> ResB -> ResA):\n";
    } // Local destructors run automatically in LIFO order HERE

    cout << "  - Active Tracker Instances Post-Scope: " << ResourceTracker::activeCount << "\n";

    // =====================================================================================
    // 2. THE `noexcept` DESTRUCTOR CONTRACT
    // =====================================================================================
    cout << "\n================ 2. `noexcept` DESTRUCTOR ERROR HANDLING ================\n";

    {
        cout << "  - Instantiating handle with simulated release failure...\n";
        SafeRaiiHandle failingHandle(userInputValue + 10, true);

        cout << "  - Exiting scope containing failing handle...\n";
    } // Destructor catches internal error locally and prevents application crash

    cout << "  - [SAFETY GUARANTEE]: Program continued execution normally past destructor error.\n";

    // =====================================================================================
    // 3. POLYMORPHIC CLEANUP & VIRTUAL DESTRUCTORS
    // =====================================================================================
    cout << "\n================ 3. VIRTUAL DESTRUCTORS IN INHERITANCE ================\n";

    {
        cout << "  - Allocating `DerivedRaiiResource` assigned to `BaseRaiiResource*`...\n";
        BaseRaiiResource* polyPtr = new DerivedRaiiResource(userInputValue + 20);
        polyPtr->process();

        cout << "  - Deleting polymorphic object via `delete polyPtr`...\n";
        // Virtual destructor guarantees Derived Dtor runs first, followed by Base Dtor
        delete polyPtr;
        polyPtr = nullptr;
    }

    cout << "  - [INHERITANCE GUARANTEE]: Virtual destructor prevented derived member leaks.\n";

    // =====================================================================================
    // 4. DESTRUCTORS DURING EXCEPTION STACK UNWINDING
    // =====================================================================================
    cout << "\n================ 4. DESTRUCTORS DURING EXCEPTION STACK UNWINDING ================\n";

    try {
        cout << "  - Entering try block...\n";
        ResourceTracker unwindingRes1(userInputValue + 30, "UnwindRes1");
        ResourceTracker unwindingRes2(userInputValue + 31, "UnwindRes2");

        cout << "  - Throwing exception during block execution...\n";
        throw std::runtime_error("Simulated execution failure");

        // Code here is bypassed, but stack unwinding forces unwindingRes2 & unwindingRes1 destructors!
    } catch (const std::exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Tracker Instances Post-Exception: " << ResourceTracker::activeCount 
         << " (RAII GUARANTEE: Zero leaks during unwinding!)\n";

    // =====================================================================================
    // 5. CUSTOM SMART POINTER DELETERS
    // =====================================================================================
    cout << "\n================ 5. CUSTOM DELETERS WITH `std::unique_ptr` ================\n";

    {
        cout << "  - Wrapping C-style resource in `std::unique_ptr` with custom deleter function...\n";
        std::unique_ptr<CStyleResource, decltype(&freeCResource)> customSmartPtr(
            new CStyleResource{userInputValue + 40}, 
            freeCResource
        );

        cout << "    Resource ID inside smart pointer: " << customSmartPtr->resourceId << "\n";
        cout << "  - Leaving scope (Smart pointer destructor invokes `freeCResource`)...\n";
    } // Custom deleter executed HERE automatically!

    cout << "  - Final Active Tracker Count: " << ResourceTracker::activeCount << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DESTRUCTORS IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Destructor Rule       | Implementation / Mechanics        | Primary Safety Guarantee          |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Deterministic Timing  | Triggered automatically at scope  | Eliminates manual `delete` calls  |\n"
         << "| Execution Order       | Strict LIFO (Last-In, First-Out)  | Teardown mirrors setup sequence   |\n"
         << "| `noexcept` Contract   | Catch & suppress errors internally| Prevents `std::terminate` crashes |\n"
         << "| Virtual Destructors   | `virtual ~Base() = default;`      | Prevents partial leaks in derived |\n"
         << "| Exception Unwinding   | Automatically runs during `throw` | Leak-free exception handling      |\n"
         << "| Custom Deleters       | `std::unique_ptr<T, DeleterFunc>` | Extends RAII to C-style APIs      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}