/*
 * =====================================================================================
 * CONCEPT        : RAII in Standard Library (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the primary Resource Acquisition Is Initialization (RAII) abstractions 
 *                  provided natively by the C++ Standard Library:
 *
 *                  1. Dynamic Memory RAII        : `std::unique_ptr`, `std::shared_ptr`, and `std::weak_ptr`
 *                                                  for automatic, leak-free heap lifecycle management.
 *                  2. Container & String RAII     : `std::vector` and `std::string` for self-managing, 
 *                                                  dynamically resizable heap buffers.
 *                  3. File & Stream I/O RAII      : `std::ofstream`, `std::ifstream`, and `std::stringstream` 
 *                                                  for automatic file descriptor flushing & closure.
 *                  4. Concurrency Synchronization : `std::lock_guard`, `std::unique_lock`, `std::shared_lock`, 
 *                                                  and `std::scoped_lock` for exception-safe mutex locking.
 *                  5. Thread Execution RAII       : `std::jthread` (C++20) / RAII thread handles 
 *                                                  for automatic thread join/detach on scope exit.
 *
 * TIME COMPLEXITY  : Acquisition / Release per abstraction: O(1) constant time (except vector resize O(N)).
 * SPACE COMPLEXITY : Footprint matches underlying handle size (8-24 bytes per standard wrapper).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS: MANAGED RESOURCE WITH ACTIVE INSTANCE TRACKING
// Logs lifecycle milestones to visually observe STL RAII abstractions in real-time.
// =====================================================================================
class StdRaiiTracker {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static variable

    StdRaiiTracker(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [STL TRACKER CREATED]   ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~StdRaiiTracker() noexcept {
        --activeInstances;
        cout << "    [STL TRACKER DESTROYED] ID: " << setw(3) << id_ << " (" << setw(20) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    void executePayload() const {
        cout << "    [PAYLOAD EXECUTED]      Tracker ID: " << id_ << " (" << label_ << ") performing work.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS FOR THREAD RAII (SCOPED THREAD JOINER FOR PRE-C++20 COMPATIBILITY)
// Demonstrates RAII wrapping around thread execution handles.
// =====================================================================================
class ScopedThreadJoiner {
private:
    thread threadHandle_;

public:
    explicit ScopedThreadJoiner(thread t) : threadHandle_(std::move(t)) {
        if (!threadHandle_.joinable()) {
            throw invalid_argument("No joinable thread provided to ScopedThreadJoiner!");
        }
    }

    ~ScopedThreadJoiner() noexcept {
        if (threadHandle_.joinable()) {
            cout << "    [ScopedThreadJoiner Dtor] Automatically joining worker thread on scope exit...\n";
            threadHandle_.join();
        }
    }

    ScopedThreadJoiner(const ScopedThreadJoiner&) = delete;
    ScopedThreadJoiner& operator=(const ScopedThreadJoiner&) = delete;

    ScopedThreadJoiner(ScopedThreadJoiner&&) noexcept = default;
    ScopedThreadJoiner& operator=(ScopedThreadJoiner&&) noexcept = default;
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for standard library RAII testing (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. SMART POINTERS (DYNAMIC MEMORY MANAGEMENT RAII)
    // =====================================================================================
    cout << "\n================ 1. DYNAMIC MEMORY RAII (`unique_ptr`, `shared_ptr`, `weak_ptr`) ================\n";

    // A. Exclusive Ownership (`std::unique_ptr`)
    {
        cout << "  - A. Demonstrating `std::unique_ptr` (Exclusive Heap RAII)...\n";
        auto uniqueNode = std::make_unique<StdRaiiTracker>(userInputValue, "UniqueNode");
        uniqueNode->executePayload();

        cout << "  - Exiting `unique_ptr` block scope...\n";
    } // Memory automatically freed HERE!

    cout << "  - Active Instances Post-Unique Scope: " << StdRaiiTracker::activeInstances << "\n";

    // B. Shared & Weak Ownership (`std::shared_ptr` & `std::weak_ptr`)
    {
        cout << "\n  - B. Demonstrating `std::shared_ptr` and `std::weak_ptr` (Shared Ref-Counted RAII)...\n";
        std::weak_ptr<StdRaiiTracker> weakObserver;

        {
            std::shared_ptr<StdRaiiTracker> sharedOwner1 = std::make_shared<StdRaiiTracker>(userInputValue + 1, "SharedNode");
            weakObserver = sharedOwner1;

            cout << "    * Ref Count (1 owner)  = " << sharedOwner1.use_count() << "\n";

            {
                std::shared_ptr<StdRaiiTracker> sharedOwner2 = sharedOwner1;
                cout << "    * Ref Count (2 owners) = " << sharedOwner1.use_count() << "\n";
            } // sharedOwner2 leaves scope

            cout << "    * Ref Count post inner scope = " << sharedOwner1.use_count() << "\n";
            cout << "  - Exiting outer shared scope...\n";
        } // sharedOwner1 leaves scope -> Ref Count reaches 0 -> Memory freed HERE!

        cout << "  - `weakObserver.expired()` check: " 
             << (weakObserver.expired() ? "TRUE (Safely detected target destruction!)" : "FALSE") << "\n";
    }

    cout << "  - Active Instances Post-Shared Scope: " << StdRaiiTracker::activeInstances << "\n";

    // =====================================================================================
    // 2. CONTAINERS & STRINGS (HEAP BUFFER MANAGEMENT RAII)
    // =====================================================================================
    cout << "\n================ 2. CONTAINER & STRING BUFFER RAII (`std::vector`, `std::string`) ================\n";

    {
        cout << "  - Demonstrating `std::vector` managing dynamic heap allocations...\n";
        std::vector<StdRaiiTracker> trackerVector;
        trackerVector.reserve(2);

        cout << "    Emplacing elements inside std::vector...\n";
        trackerVector.emplace_back(userInputValue + 10, "VectorNode1");
        trackerVector.emplace_back(userInputValue + 11, "VectorNode2");

        for (const auto& item : trackerVector) {
            item.executePayload();
        }

        cout << "  - Exiting vector block scope...\n";
    } // Vector destructor deallocates buffer and calls element destructors automatically HERE!

    cout << "  - Active Instances Post-Vector Scope: " << StdRaiiTracker::activeInstances << "\n";

    // =====================================================================================
    // 3. FILE & STREAM I/O (SYSTEM HANDLE RAII)
    // =====================================================================================
    cout << "\n================ 3. FILE & STREAM I/O RAII (`std::ofstream`, `std::fstream`) ================\n";

    {
        cout << "  - Opening file stream via `std::ofstream`...\n";
        std::ofstream logFile("std_raii_output.txt", ios::out | ios::trunc);

        if (logFile.is_open()) {
            logFile << "Base ID: " << userInputValue << "\n";
            logFile << "C++ Standard Library streams automatically flush and close file handles on destruction.\n";
            cout << "    Wrote telemetry data to 'std_raii_output.txt'\n";
        }

        cout << "  - Exiting std::ofstream block scope...\n";
    } // std::ofstream destructor flushes and closes system file handle automatically HERE!

    // =====================================================================================
    // 4. CONCURRENCY SYNCHRONIZATION (MUTEX LOCK RAII)
    // =====================================================================================
    cout << "\n================ 4. CONCURRENCY SYNCHRONIZATION RAII (`lock_guard`, `scoped_lock`) ================\n";

    std::mutex mtxA;
    std::mutex mtxB;
    std::shared_mutex sharedMtx;

    // A. Single Mutex Scoped Lock (`std::lock_guard`)
    {
        cout << "  - A. `std::lock_guard<std::mutex>` (Scoped Exclusive Lock):\n";
        std::lock_guard<std::mutex> lock(mtxA);
        cout << "    Critical section safely protected under std::lock_guard.\n";
    } // Mutex unlocked automatically HERE!

    // B. Flexible Lock (`std::unique_lock`)
    {
        cout << "\n  - B. `std::unique_lock<std::mutex>` (Flexible Scoped Lock):\n";
        std::unique_lock<std::mutex> flexLock(mtxA);
        cout << "    Critical section 1 executing...\n";

        flexLock.unlock(); // Manual temporary unlock
        cout << "    [UNLOCKED] Non-critical background work executing...\n";

        flexLock.lock(); // Manual re-lock
        cout << "    [RELOCKED] Critical section 2 executing...\n";
    } // Mutex unlocked automatically HERE!

    // C. Shared Read Lock (`std::shared_lock`)
    {
        cout << "\n  - C. `std::shared_lock<std::shared_mutex>` (Concurrent Shared Read Lock):\n";
        std::shared_lock<std::shared_mutex> readLock(sharedMtx);
        cout << "    Thread-safe non-exclusive read operation executing...\n";
    } // Read lock released automatically HERE!

    // D. Multi-Mutex Deadlock-Free Lock (`std::scoped_lock` - C++17)
    {
        cout << "\n  - D. `std::scoped_lock<std::mutex, std::mutex>` (Deadlock-Free Multi-Lock):\n";
        std::scoped_lock<std::mutex, std::mutex> multiLock(mtxA, mtxB);
        cout << "    Mutually exclusive access to Mutex A and Mutex B acquired simultaneously.\n";
    } // Both mutexes unlocked simultaneously HERE!

    // =====================================================================================
    // 5. THREAD EXECUTION RAII (`ScopedThreadJoiner`)
    // =====================================================================================
    cout << "\n================ 5. THREAD EXECUTION RAII ================\n";

    {
        cout << "  - Spawning asynchronous worker thread inside `ScopedThreadJoiner`...\n";
        int targetId = userInputValue + 20;

        ScopedThreadJoiner threadGuard(std::thread([targetId]() {
            cout << "    [BACKGROUND THREAD] Worker thread ID " << targetId << " executing asynchronously...\n";
        }));

        cout << "  - Main thread continuing execution... Leaving thread guard scope...\n";
    } // ScopedThreadJoiner destructor automatically calls .join() HERE!

    cout << "  - Worker thread joined cleanly. Final Active Instances: " << StdRaiiTracker::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RAII IN STANDARD LIBRARY SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Standard Library Type | Managed Resource / System Handle  | Primary RAII Release Behavior     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| `std::unique_ptr<T>`  | Dynamic Heap Memory (Exclusive)   | Automatically calls `delete`      |\n"
         << "| `std::shared_ptr<T>`  | Dynamic Heap Memory (Shared)      | Frees heap memory when ref-count=0|\n"
         << "| `std::weak_ptr<T>`    | Non-owning reference to shared_ptr| Observes without extending lifetime|\n"
         << "| `std::vector<T>`      | Dynamic Contiguous Heap Array     | Frees array buffer & destructs elements|\n"
         << "| `std::string`         | Dynamic Char Heap Buffer          | Deallocates string character buffer|\n"
         << "| `std::ofstream`       | OS Output File Descriptor Handle  | Flushes buffer and calls `close()`|\n"
         << "| `std::ifstream`       | OS Input File Descriptor Handle   | Releases system file handle       |\n"
         << "| `std::lock_guard`     | Concurrency Mutex Lock Scope      | Automatically calls `unlock()`    |\n"
         << "| `std::unique_lock`    | Flexible Mutex Lock Scope         | Deferred/manual lock + auto unlock|\n"
         << "| `std::scoped_lock`    | Multiple Concurrency Mutexes      | Atomic deadlock-free multi-unlock |\n"
         << "| `std::jthread`(C++20) | OS Execution Thread Handle        | Automatically calls `join()` on exit|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}