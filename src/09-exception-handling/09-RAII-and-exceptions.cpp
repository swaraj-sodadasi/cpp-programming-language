/*
 * =====================================================================================
 * CONCEPT        : RAII (Resource Acquisition Is Initialization) and Exceptions in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the interplay between RAII resource management and Exception Handling:
 *
 *                  1. RAII Core Principle :
 *                     - Binds resource lifecycle (heap memory, file descriptors, mutexes)
 *                       to object lifetime (Stack allocation).
 *                     - Constructor acquires resource; Destructor releases resource.
 *
 *                  2. Exception Safety via RAII :
 *                     - When an exception is thrown, stack unwinding automatically calls
 *                       destructors for all fully constructed local RAII objects.
 *                     - Guarantees ZERO resource leaks (memory, locks, file handles)
 *                       even when control flow is abruptly interrupted.
 *
 *                  3. Raw Pointers vs. RAII Smart Pointers :
 *                     - Raw pointers allocated with `new` LEAK when an exception bypasses `delete`.
 *                     - `std::unique_ptr` / `std::shared_ptr` guarantee teardown during unwind.
 *
 *                  4. Partial Construction & Member Cleanup :
 *                     - If an exception occurs inside a constructor body, the object's OWN
 *                       destructor will NOT run. However, all fully constructed member
 *                       sub-objects are destructed in REVERSE order of declaration.
 *
 *                  5. Destructor `noexcept` Rule :
 *                     - Destructors MUST NOT throw exceptions during stack unwinding. Doing so
 *                       triggers `std::terminate()` immediately.
 *
 * TIME COMPLEXITY  : Resource Acquisition / Release : O(1) constant time.
 *                    Stack Unwinding / Teardown    : O(Depth of Call Stack).
 * SPACE COMPLEXITY : RAII Wrapper Overhead         : Zero runtime footprint (Zero-cost abstraction).
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <memory>
#include <utility>
#include <limits>

using namespace std;

// Tracking active heap allocations globally for leak detection
static int g_activeAllocations = 0;

// =====================================================================================
// 1. HELPER MEMBER CLASS TO DEMONSTRATE PARTIAL CONSTRUCTION CLEANUP
// =====================================================================================
class ManagedMember {
private:
    string memberName_;

public:
    explicit ManagedMember(string name) : memberName_(std::move(name)) {
        cout << "      [Member Ctor] Initialized sub-object member: '" << memberName_ << "'\n";
    }

    ~ManagedMember() noexcept {
        cout << "      [Member Dtor] Cleaned up sub-object member: '" << memberName_ << "'\n";
    }
};

// =====================================================================================
// 2. CUSTOM RAII RESOURCE GUARD (MOCK FILE / SOCKET HANDLE)
// Encapsulates a raw resource to guarantee leak-free cleanup on exception unwinding.
// =====================================================================================
class NetworkSocketGuard {
private:
    int socketId_;
    int* buffer_;

public:
    explicit NetworkSocketGuard(int id)
        : socketId_(id), buffer_(new int[256]()) {
        ++g_activeAllocations;
        cout << "    [RAII Socket Guard Ctor] Opened Socket ID #" << socketId_ 
             << " (Buffer allocated at: " << static_cast<void*>(buffer_) << ")\n";
    }

    // RAII DESTRUCTOR: Guaranteed release during stack unwinding
    ~NetworkSocketGuard() noexcept {
        delete[] buffer_;
        --g_activeAllocations;
        cout << "    [RAII Socket Guard Dtor] Closed Socket ID #" << socketId_ 
             << " & freed heap buffer. (Active Allocations: " << g_activeAllocations << ")\n";
    }

    // Disable copy semantics to enforce unique ownership
    NetworkSocketGuard(const NetworkSocketGuard&) = delete;
    NetworkSocketGuard& operator=(const NetworkSocketGuard&) = delete;

    // Enable move semantics
    NetworkSocketGuard(NetworkSocketGuard&& other) noexcept
        : socketId_(other.socketId_), buffer_(other.buffer_) {
        other.buffer_ = nullptr;
        other.socketId_ = -1;
    }

    void transmitData(const string& payload) const {
        if (!buffer_) {
            throw std::runtime_error("Attempted to transmit over disarmed/moved socket!");
        }
        cout << "    [Socket Transmit] Socket #" << socketId_ << " sending: '" << payload << "'\n";
    }
};

// =====================================================================================
// 3. COMPARISON DEMONSTRATION: RAW POINTER LEAKS VS RAII SAFETY
// =====================================================================================
class ResourceLeakComparison {
public:
    // ANTI-PATTERN: Raw pointer memory leak when an exception occurs
    static void executeUnsafeRawPointer(bool throwException) {
        cout << "    -> Entering `executeUnsafeRawPointer()`...\n";
        int* rawArray = new int[500](); // Raw heap allocation
        ++g_activeAllocations;
        cout << "       [Raw Allocation] Allocated 500 ints at " << static_cast<void*>(rawArray) 
             << " (Active Allocations: " << g_activeAllocations << ")\n";

        if (throwException) {
            cout << "       [THROW] Exception occurs! Bypassing `delete[] rawArray`...\n";
            throw std::runtime_error("Network Timeout Error during Raw Processing!");
        }

        delete[] rawArray; // NEVER REACHED IF EXCEPTION THROWS!
        --g_activeAllocations;
    }

    // BEST PRACTICE: Smart Pointer RAII safety during exception stack unwinding
    static void executeSafeRAIIPointer(bool throwException) {
        cout << "    -> Entering `executeSafeRAIIPointer()`...\n";
        
        // Smart pointer manages memory life scope automatically
        auto safeArray = make_unique<int[]>(500);
        cout << "       [RAII Allocation] `std::unique_ptr` managing 500 ints.\n";

        if (throwException) {
            cout << "       [THROW] Exception occurs! Stack unwinding triggers unique_ptr destructor...\n";
            throw std::runtime_error("Network Timeout Error during RAII Processing!");
        }

        cout << "       [Normal Exit] Function finished normally.\n";
    } // safeArray destructor executes automatically HERE
};

// =====================================================================================
// 4. PARTIAL CONSTRUCTION DEMONSTRATION
// Demonstrates member sub-object cleanup when constructor body throws.
// =====================================================================================
class PartialConstructionClass {
private:
    ManagedMember member1_;
    ManagedMember member2_;

public:
    PartialConstructionClass(bool failConstruction)
        : member1_("Member_SubObject_1"),
          member2_("Member_SubObject_2") {
        
        cout << "    [Class Ctor Body] Member sub-objects fully built. Entering constructor body...\n";
        
        if (failConstruction) {
            cout << "    [Class Ctor Body] THROWING exception inside constructor body!\n";
            throw std::runtime_error("Constructor Initialization Failed!");
        }

        cout << "    [Class Ctor Body] Class fully constructed.\n";
    }

    ~PartialConstructionClass() noexcept {
        cout << "    [Class Dtor] THIS WILL NEVER RUN IF CTOR THROWS!\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for RAII and Exception analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. RAW POINTER MEMORY LEAK VS. RAII SAFETY
    // =====================================================================================
    cout << "\n================ 1. RAW POINTER LEAKS VS. RAII SAFETY ================\n";

    // Scenario A: Anti-Pattern (Raw Pointer Memory Leak)
    cout << "  - Scenario A: Demonstrating Raw Pointer Memory Leak on Exception:\n";
    try {
        ResourceLeakComparison::executeUnsafeRawPointer(true);
    } catch (const std::exception& ex) {
        cout << "    [Caught Exception]: \"" << ex.what() << "\"\n";
        cout << "    [MEMORY LEAK DETECTED] Active Allocations Remaining: " 
             << g_activeAllocations << " (Raw array was NEVER deleted!)\n";
    }

    // Scenario B: RAII Smart Pointer Cleanup
    cout << "\n  - Scenario B: Demonstrating RAII `std::unique_ptr` Safety on Exception:\n";
    try {
        ResourceLeakComparison::executeSafeRAIIPointer(true);
    } catch (const std::exception& ex) {
        cout << "    [Caught Exception]: \"" << ex.what() << "\"\n";
        cout << "    [ZERO LEAKS] Active Allocations Remaining: " 
             << g_activeAllocations << " (Smart pointer cleaned up memory!)\n";
    }

    // =====================================================================================
    // 2. CUSTOM RAII SCOPE GUARD IN ACTION
    // =====================================================================================
    cout << "\n================ 2. CUSTOM RAII SCOPE GUARD UNWINDING ================\n";

    try {
        cout << "  - Instantiating custom `NetworkSocketGuard` on stack...\n";
        NetworkSocketGuard socketGuard(userInputValue);
        
        socketGuard.transmitData("INIT_HANDSHAKE_PACKET");

        cout << "  - Simulating processing failure that triggers exception throw...\n";
        throw std::runtime_error("Connection Lost Mid-Transmission");

        socketGuard.transmitData("UNREACHABLE_PACKET");
    } catch (const std::exception& ex) {
        cout << "  - [Main Catch] Caught exception: \"" << ex.what() << "\"\n";
        cout << "    Notice above: NetworkSocketGuard destructor executed BEFORE entering this catch block!\n";
    }

    // =====================================================================================
    // 3. PARTIAL CONSTRUCTION & SUB-OBJECT CLEANUP
    // =====================================================================================
    cout << "\n================ 3. PARTIAL CONSTRUCTION & SUB-OBJECT UNWINDING ================\n";

    try {
        cout << "  - Attempting to construct `PartialConstructionClass` with fail flag = TRUE...\n";
        PartialConstructionClass partialObj(true);
    } catch (const std::exception& ex) {
        cout << "  - [Main Catch] Caught constructor exception: \"" << ex.what() << "\"\n";
        cout << "    [RAII GUARANTEE]: Member sub-objects (member2_ then member1_) were destructed in\n"
             << "    reverse order, even though the main class destructor was skipped!\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RAII & EXCEPTIONS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Resource Pattern      | Exception Behavior                | RAII Guarantee / Architectural Rule|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Raw Pointer (`new`)   | Leak! `delete` skipped on unwind  | Anti-pattern; avoid raw pointers  |\n"
         << "| `std::unique_ptr`     | Automatic Heap Teardown           | Zero-overhead memory safety       |\n"
         << "| Custom Scope Guard    | Destructor releases OS handles    | Encapsulates socket/file cleanup  |\n"
         << "| Ctor Exception        | Fully-built sub-objects destructed| Cleans ready members in reverse   |\n"
         << "| Dtor Exception Rule   | MUST BE `noexcept`                | Throwing in dtor = `std::terminate`|\n"
         << "| Scope Unwinding       | LIFO (Last-In, First-Out) Teardown| Deterministic stack resource cleanup|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}