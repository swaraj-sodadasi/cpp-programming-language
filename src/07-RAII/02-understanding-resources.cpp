/*
 * =====================================================================================
 * CONCEPT        : Understanding Resources in RAII (Resource Acquisition Is Initialization)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  what constitutes a "Resource" in software engineering, how RAII 
 *                  encapsulates diverse resource types, and how deterministic destruction 
 *                  prevents resource leaks:
 *
 *                  1. What is a "Resource"?       : Any finite system entity requiring explicit 
 *                                                   acquisition and mandatory release (Heap memory, 
 *                                                   file descriptors, mutex locks, network sockets).
 *                  2. Memory Resources            : Managing dynamic heap memory allocations via RAII.
 *                  3. File & I/O Resources        : Encapsulating operating system file handles and streams.
 *                  4. Synchronization Resources   : Managing thread concurrency locks (`std::mutex`).
 *                  5. Ownership & Resource Move   : Safe resource transfer via Move Semantics (Rule of 5).
 *                  6. Standard Library Resources  : Idiomatic C++ wrappers (`std::unique_ptr`, 
 *                                                   `std::lock_guard`, `std::fstream`).
 *
 * TIME COMPLEXITY  : Acquisition / Release / Move / Access: O(1) constant time.
 * SPACE COMPLEXITY : Resource wrapper overhead: Uniform 8-16 bytes (raw handle footprint).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <fstream>
#include <mutex>
#include <string>
#include <utility>
#include <limits>
#include <cstddef>
#include <stdexcept>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: TRACKED MEMORY RESOURCE
// Demonstrates heap allocation tracking for RAII memory management.
// =====================================================================================
class MemoryBlock {
private:
    int id_;
    size_t sizeBytes_;

public:
    static inline int activeAllocations = 0; // C++17 inline static tracker

    MemoryBlock(int id, size_t sizeBytes) : id_(id), sizeBytes_(sizeBytes) {
        ++activeAllocations;
        cout << "    [MEMORY ACQUIRED] Heap Block ID: " << id_ << " (" << sizeBytes_ 
             << " bytes) at " << static_cast<const void*>(this) 
             << " | Active Blocks: " << activeAllocations << "\n";
    }

    ~MemoryBlock() {
        --activeAllocations;
        cout << "    [MEMORY RELEASED] Heap Block ID: " << id_ << " (" << sizeBytes_ 
             << " bytes) at " << static_cast<const void*>(this) 
             << " | Active Blocks: " << activeAllocations << "\n";
    }

    void processData() const {
        cout << "    [MEMORY PROCESSING] Processing data inside Heap Block ID: " << id_ << "\n";
    }

    [[nodiscard]] int getId() const { return id_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM RAII MEMORY RESOURCE WRAPPER (`ScopedBuffer<T>`)
// Encapsulates a dynamic heap array resource.
// =====================================================================================
template <typename T>
class ScopedBuffer {
private:
    T* buffer_;
    size_t count_;

public:
    // Acquire resource in constructor
    ScopedBuffer(size_t count, const T& initialVal) : buffer_(new T[count]), count_(count) {
        for (size_t i = 0; i < count_; ++i) {
            buffer_[i] = initialVal;
        }
        cout << "    [ScopedBuffer Ctor] Allocated dynamic buffer of " << count_ 
             << " items at " << static_cast<const void*>(buffer_) << "\n";
    }

    // Release resource in destructor
    ~ScopedBuffer() {
        cout << "    [ScopedBuffer Dtor] Freeing dynamic buffer at " 
             << static_cast<const void*>(buffer_) << "\n";
        delete[] buffer_; // Deterministic array deallocation
    }

    // Non-copyable (Prevent duplicate ownership / double-free)
    ScopedBuffer(const ScopedBuffer&) = delete;
    ScopedBuffer& operator=(const ScopedBuffer&) = delete;

    // Movable (Transfer ownership)
    ScopedBuffer(ScopedBuffer&& other) noexcept 
        : buffer_(other.buffer_), count_(other.count_) {
        other.buffer_ = nullptr;
        other.count_ = 0;
        cout << "    [ScopedBuffer Move Ctor] Buffer resource ownership transferred.\n";
    }

    ScopedBuffer& operator=(ScopedBuffer&& other) noexcept {
        if (this != &other) {
            delete[] buffer_; // Release existing resource
            buffer_ = other.buffer_;
            count_ = other.count_;
            other.buffer_ = nullptr;
            other.count_ = 0;
            cout << "    [ScopedBuffer Move Assign] Existing freed, new ownership transferred.\n";
        }
        return *this;
    }

    T& operator[](size_t index) { return buffer_[index]; }
    const T& operator[](size_t index) const { return buffer_[index]; }
    [[nodiscard]] size_t size() const { return count_; }
    [[nodiscard]] explicit operator bool() const { return buffer_ != nullptr; }
};

// =====================================================================================
// HELPER CLASS 3: CUSTOM RAII FILE / HANDLE RESOURCE WRAPPER (`ScopedFileHandle`)
// Encapsulates an operating system file descriptor / stream resource.
// =====================================================================================
class ScopedFileHandle {
private:
    string filename_;
    ofstream fileStream_;

public:
    // Acquire resource in constructor
    explicit ScopedFileHandle(const string& filename)
        : filename_(filename), fileStream_(filename, ios::out | ios::trunc) {
        if (!fileStream_.is_open()) {
            throw runtime_error("Failed to open/acquire file resource: " + filename_);
        }
        cout << "    [FILE RESOURCE ACQUIRED] File Handle created for '" << filename_ << "'\n";
    }

    // Release resource in destructor
    ~ScopedFileHandle() {
        if (fileStream_.is_open()) {
            fileStream_.flush();
            fileStream_.close(); // Flushes and closes system file handle
            cout << "    [FILE RESOURCE RELEASED] File Handle closed for '" << filename_ << "'\n";
        }
    }

    void writeLine(const string& text) {
        if (fileStream_.is_open()) {
            fileStream_ << text << "\n";
            cout << "    [FILE I/O] Wrote line to '" << filename_ << "': \"" << text << "\"\n";
        }
    }

    // Non-copyable resource handle
    ScopedFileHandle(const ScopedFileHandle&) = delete;
    ScopedFileHandle& operator=(const ScopedFileHandle&) = delete;
};

// =====================================================================================
// HELPER CLASS 4: CUSTOM RAII SYNCHRONIZATION RESOURCE WRAPPER (`ScopedLockGuard`)
// Encapsulates a thread concurrency lock resource.
// =====================================================================================
class ScopedLockGuard {
private:
    mutex& mutexRef_;

public:
    // Acquire lock resource in constructor
    explicit ScopedLockGuard(mutex& mtx) : mutexRef_(mtx) {
        mutexRef_.lock(); // Block until mutex resource acquired
        cout << "    [MUTEX RESOURCE LOCKED] Synchronization lock acquired on thread context.\n";
    }

    // Release lock resource in destructor
    ~ScopedLockGuard() {
        mutexRef_.unlock(); // Guaranteed unlock on scope exit
        cout << "    [MUTEX RESOURCE UNLOCKED] Synchronization lock released.\n";
    }

    // Non-copyable and non-movable lock scope
    ScopedLockGuard(const ScopedLockGuard&) = delete;
    ScopedLockGuard& operator=(const ScopedLockGuard&) = delete;
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for resource analysis (e.g., 200): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 200." << endl;
        userInputValue = 200;
    }

    // =====================================================================================
    // 1. UNDERSTANDING RESOURCES: WHAT IS A RESOURCE?
    // =====================================================================================
    cout << "\n================ 1. WHAT IS A RESOURCE IN RAII? ================\n";
    cout << "  - A software resource is any system entity with limited availability that must be:\n";
    cout << "    1. ACQUIRED before use (Constructor role).\n";
    cout << "    2. RELEASED after use (Destructor role).\n";
    cout << "  - Examples: Heap Memory, File Descriptors, Mutex Locks, Sockets, DB Connections.\n";

    // =====================================================================================
    // 2. RESOURCE TYPE 1: HEAP MEMORY RESOURCES
    // =====================================================================================
    cout << "\n================ 2. RESOURCE TYPE 1: HEAP MEMORY ================\n";

    {
        cout << "  - Entering local block scope for memory resource test...\n";
        ScopedBuffer<int> heapBuffer(4, userInputValue);

        cout << "  - Accessing items inside RAII heap buffer: [ ";
        for (size_t i = 0; i < heapBuffer.size(); ++i) {
            cout << heapBuffer[i] << " ";
        }
        cout << "]\n";

        cout << "  - Exiting scope (ScopedBuffer destructor will free heap memory automatically):\n";
    } // Heap memory freed HERE automatically!

    // =====================================================================================
    // 3. RESOURCE TYPE 2: FILE AND SYSTEM HANDLES
    // =====================================================================================
    cout << "\n================ 3. RESOURCE TYPE 2: FILE SYSTEM HANDLES ================\n";

    try {
        cout << "  - Opening file resource using `ScopedFileHandle`...\n";
        ScopedFileHandle logFile("raii_resource_log.txt");

        logFile.writeLine("Log Entry 1: Resource acquired successfully.");
        logFile.writeLine("Log Entry 2: Processing operational workload...");

        cout << "  - Exiting scope (File descriptor will be flushed and closed automatically):\n";
    } catch (const exception& e) {
        cout << "  - Error: " << e.what() << "\n";
    } // File closed HERE automatically!

    // =====================================================================================
    // 4. RESOURCE TYPE 3: SYNCHRONIZATION & CONCURRENCY LOCKS
    // =====================================================================================
    cout << "\n================ 4. RESOURCE TYPE 3: SYNCHRONIZATION MUTEX LOCKS ================\n";

    mutex sharedResourceMutex;

    {
        cout << "  - Entering critical section... Acquiring mutex lock resource...\n";
        ScopedLockGuard lock(sharedResourceMutex);

        cout << "  - Executing thread-safe critical region work...\n";

        cout << "  - Exiting critical section block (Mutex unlocked automatically):\n";
    } // Mutex unlocked HERE automatically!

    // =====================================================================================
    // 5. RESOURCE OWNERSHIP TRANSFER (MOVE SEMANTICS)
    // =====================================================================================
    cout << "\n================ 5. RESOURCE OWNERSHIP TRANSFER (MOVE SEMANTICS) ================\n";

    {
        cout << "  - Creating initial buffer owner `bufA`...\n";
        ScopedBuffer<int> bufA(3, 777);

        cout << "  - Transferring buffer resource from `bufA` to `bufB` via `std::move()`...\n";
        ScopedBuffer<int> bufB = std::move(bufA);

        cout << "  - `bufA` valid check : " << (bufA ? "VALID" : "EMPTY (Ownership Relinquished)") << "\n";
        cout << "  - `bufB` valid check : " << (bufB ? "VALID (Current Resource Owner)" : "EMPTY") << "\n";

        cout << "  - Exiting scope (Only `bufB` will release the underlying heap memory once):\n";
    }

    // =====================================================================================
    // 6. STANDARD LIBRARY RAII RESOURCE ABSTRACTIONS
    // =====================================================================================
    cout << "\n================ 6. STANDARD LIBRARY RAII RESOURCE ABSTRACTIONS ================\n";

    // Memory Resource Abstraction
    {
        cout << "  - Standard Memory Resource (`std::unique_ptr`):\n";
        auto smartMem = std::make_unique<MemoryBlock>(userInputValue + 1, 1024);
        smartMem->processData();
    } // Freed automatically

    // Mutex Resource Abstraction
    {
        cout << "\n  - Standard Mutex Lock Resource (`std::lock_guard`):\n";
        std::lock_guard<std::mutex> stdLock(sharedResourceMutex);
        cout << "    Standard lock_guard acquired and managing mutex safety.\n";
    } // Unlocked automatically

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ UNDERSTANDING RESOURCES IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Resource Category     | Acquisition Mechanics | Release Mechanics     | Idiomatic RAII Wrapper            |\n"
         << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Dynamic Heap Memory   | `new` / `malloc`      | `delete` / `free`     | `std::unique_ptr<T>`, `vector`    |\n"
         << "| File Handles & Streams| `fopen` / `open`      | `fclose` / `close`    | `std::fstream`, `ScopedFile`      |\n"
         << "| Concurrency Mutex     | `mtx.lock()`          | `mtx.unlock()`        | `std::lock_guard<std::mutex>`     |\n"
         << "| Network Sockets       | `socket()` / `connect`| `close()` / `shutdown`| Custom Socket RAII Wrapper        |\n"
         << "| OS GUI / Graphics     | `CreateGDIObject()`   | `DeleteObject()`      | Custom OS Handle RAII Wrapper     |\n"
         << "| Database Connections  | `db.connect()`        | `db.disconnect()`     | Custom DB Connection RAII Wrapper |\n"
         << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n";

    return 0;
}