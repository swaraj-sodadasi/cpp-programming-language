/*
 * =====================================================================================
 * CONCEPT        : File and Lock Management in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how Modern C++ handles operating system handles (files and thread locks)
 *                  using the Resource Acquisition Is Initialization (RAII) idiom:
 *
 *                  1. File Handle Encapsulation   : Binding C-style `FILE*` handles and C++ 
 *                                                   `std::fstream` objects to stack scopes to 
 *                                                   guarantee flushing and closure.
 *                  2. Mutex Lock Encapsulation    : Managing thread synchronization primitives 
 *                                                   via RAII wrappers (`std::lock_guard`, 
 *                                                   `std::unique_lock`, `std::scoped_lock`).
 *                  3. Custom File RAII Wrapper    : Designing a move-only `RAIIFileHandle` wrapper 
 *                                                   for legacy C-style FILE descriptors.
 *                  4. Custom Lock RAII Wrapper    : Building a custom scope-bound mutex lock.
 *                  5. Deadlock & Multi-Lock Safety: Utilizing C++17 `std::scoped_lock` for 
 *                                                   deadlock-free multi-resource acquisition.
 *                  6. Combined Thread-Safe I/O   : Demonstrating exception-safe, thread-safe 
 *                                                   file writing during stack unwinding.
 *
 * TIME COMPLEXITY  : File Acquisition/Close: O(1) OS syscall | Lock/Unlock: O(1) atomic ops.
 * SPACE COMPLEXITY : Resource Wrapper Footprint: Uniform 8-16 bytes (pointer / reference size).
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <mutex>
#include <thread>
#include <utility>
#include <stdexcept>
#include <limits>
#include <vector>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: CUSTOM C-STYLE FILE RAII WRAPPER (`RAIIFileHandle`)
// Wraps raw C `FILE*` handles to guarantee `fclose` on scope exit.
// =====================================================================================
class RAIIFileHandle {
private:
    FILE* filePtr_;
    string filename_;

public:
    // Constructor: Acquires raw C-file descriptor
    RAIIFileHandle(const string& filename, const char* mode)
        : filePtr_(std::fopen(filename.c_str(), mode)), filename_(filename) {
        if (!filePtr_) {
            throw std::runtime_error("Failed to open file handle for: " + filename_);
        }
        cout << "    [FILE HANDLE OPENED] C-style FILE* handle acquired for '" << filename_ << "'\n";
    }

    // Destructor: Guarantees flush and close on scope exit
    ~RAIIFileHandle() noexcept {
        if (filePtr_) {
            std::fflush(filePtr_);
            std::fclose(filePtr_);
            cout << "    [FILE HANDLE CLOSED] C-style FILE* handle closed for '" << filename_ << "'\n";
        }
    }

    // Rule of 5: Move-only resource semantics (Non-copyable)
    RAIIFileHandle(const RAIIFileHandle&) = delete;
    RAIIFileHandle& operator=(const RAIIFileHandle&) = delete;

    RAIIFileHandle(RAIIFileHandle&& other) noexcept
        : filePtr_(other.filePtr_), filename_(std::move(other.filename_)) {
        other.filePtr_ = nullptr;
    }

    RAIIFileHandle& operator=(RAIIFileHandle&& other) noexcept {
        if (this != &other) {
            if (filePtr_) {
                std::fflush(filePtr_);
                std::fclose(filePtr_);
            }
            filePtr_ = other.filePtr_;
            filename_ = std::move(other.filename_);
            other.filePtr_ = nullptr;
        }
        return *this;
    }

    void writeString(const string& text) {
        if (filePtr_) {
            std::fputs(text.c_str(), filePtr_);
            cout << "    [C-FILE WRITE] Wrote " << text.length() << " bytes to '" << filename_ << "'\n";
        }
    }

    [[nodiscard]] FILE* get() const { return filePtr_; }
    [[nodiscard]] explicit operator bool() const { return filePtr_ != nullptr; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM MUTEX LOCK RAII WRAPPER (`RAIIMutexLock`)
// Encapsulates raw `std::mutex` to guarantee `unlock()` on scope exit.
// =====================================================================================
class RAIIMutexLock {
private:
    mutex& mutexRef_;
    bool isLocked_;

public:
    explicit RAIIMutexLock(mutex& mtx) : mutexRef_(mtx), isLocked_(true) {
        mutexRef_.lock();
        cout << "    [CUSTOM LOCK ACQUIRED] Mutex locked on thread scope entry.\n";
    }

    ~RAIIMutexLock() noexcept {
        if (isLocked_) {
            mutexRef_.unlock();
            cout << "    [CUSTOM LOCK RELEASED] Mutex unlocked on thread scope exit.\n";
        }
    }

    // Non-copyable, non-movable scope invariant
    RAIIMutexLock(const RAIIMutexLock&) = delete;
    RAIIMutexLock& operator=(const RAIIMutexLock&) = delete;
};

// =====================================================================================
// HELPER CLASS 3: THREAD-SAFE FILE LOGGER COMBINING FILES AND LOCKS
// Demonstrates dual RAII encapsulation of `std::ofstream` and `std::mutex`.
// =====================================================================================
class ThreadSafeFileLogger {
private:
    string filename_;
    ofstream logStream_;
    mutable mutex logMutex_;

public:
    explicit ThreadSafeFileLogger(const string& filename)
        : filename_(filename), logStream_(filename, ios::out | ios::trunc) {
        if (!logStream_.is_open()) {
            throw std::runtime_error("ThreadSafeFileLogger failed to open log file: " + filename_);
        }
        cout << "    [THREAD-SAFE LOGGER INIT] Opened '" << filename_ << "' for concurrent operations.\n";
    }

    ~ThreadSafeFileLogger() noexcept {
        if (logStream_.is_open()) {
            logStream_.flush();
            logStream_.close();
            cout << "    [THREAD-SAFE LOGGER CLOSE] Flushed and closed '" << filename_ << "'\n";
        }
    }

    void logMessage(int threadId, const string& message) {
        // RAII Lock Management: Lock is acquired on Ctor, released on Dtor
        std::lock_guard<mutex> lock(logMutex_);

        if (logStream_.is_open()) {
            logStream_ << "[Thread " << threadId << "] " << message << "\n";
            cout << "    [LOG WRITE] [Thread " << threadId << "] Logged: \"" << message << "\"\n";
        }
    }

    // Non-copyable handle
    ThreadSafeFileLogger(const ThreadSafeFileLogger&) = delete;
    ThreadSafeFileLogger& operator=(const ThreadSafeFileLogger&) = delete;
};

// Function demonstrating multi-resource locking using C++17 std::scoped_lock
void performMultiResourceTransaction(mutex& mtxA, mutex& mtxB, const string& transactionName) {
    cout << "  - Attempting multi-lock acquisition for transaction: '" << transactionName << "'...\n";

    // C++17 std::scoped_lock acquires multiple mutexes simultaneously without deadlock
    std::scoped_lock<mutex, mutex> multiLock(mtxA, mtxB);

    cout << "    [SCOPED_LOCK ACQUIRED] Both Mutex A and Mutex B acquired safely.\n";
    cout << "    Executing multi-resource transaction payload...\n";

    cout << "    Exiting transaction scope...\n";
} // Both mutexes automatically unlocked HERE simultaneously!

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for file & lock RAII testing (e.g., 500): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 500." << endl;
        userInputValue = 500;
    }

    // =====================================================================================
    // 1. FILE MANAGEMENT IN RAII (CUSTOM C-FILE & STD::FSTREAM)
    // =====================================================================================
    cout << "\n================ 1. FILE MANAGEMENT IN RAII ================\n";

    // A. Custom C-Style FILE* Handle Wrapper
    {
        cout << "  - Testing Custom C-Style File RAII Wrapper (`RAIIFileHandle`)...\n";
        RAIIFileHandle cFile("raii_c_output.txt", "w");
        cFile.writeString("Data line 1: RAII guarantees fclose call.\n");
        cFile.writeString("Data line 2: Custom C-handle wrapper execution.\n");

        cout << "  - Leaving C-file scope...\n";
    } // fclose invoked automatically HERE!

    // B. Standard Library C++ Stream RAII (`std::ofstream`)
    {
        cout << "\n  - Testing Standard C++ Stream RAII (`std::ofstream`)...\n";
        std::ofstream cppStream("raii_cpp_output.txt", ios::out | ios::trunc);
        if (cppStream.is_open()) {
            cppStream << "Base ID: " << userInputValue << "\n";
            cppStream << "Modern C++ std::fstream closes stream automatically on destruction.\n";
            cout << "    [STD::OFSTREAM WRITE] Data written to 'raii_cpp_output.txt'\n";
        }
        cout << "  - Leaving std::ofstream scope...\n";
    } // std::ofstream destructor closes file automatically HERE!

    // =====================================================================================
    // 2. LOCK MANAGEMENT IN RAII (`std::lock_guard` & `std::unique_lock`)
    // =====================================================================================
    cout << "\n================ 2. LOCK MANAGEMENT IN RAII ================\n";

    mutex sharedMutex;

    // A. Custom RAIIMutexLock
    {
        cout << "  - Testing Custom Mutex Lock Wrapper (`RAIIMutexLock`)...\n";
        RAIIMutexLock customLock(sharedMutex);
        cout << "    Critical section executing under custom lock guard...\n";
        cout << "  - Exiting custom lock scope...\n";
    } // Custom lock destructor unlocks mutex HERE!

    // B. Standard Library `std::lock_guard`
    {
        cout << "\n  - Testing `std::lock_guard<std::mutex>`...\n";
        std::lock_guard<mutex> stdLock(sharedMutex);
        (void)stdLock; // Suppress unused variable warning
        cout << "    Critical section executing under std::lock_guard...\n";
        cout << "  - Exiting std::lock_guard scope...\n";
    } // std::lock_guard destructor unlocks mutex HERE!

    // C. Standard Library `std::unique_lock` (Supports defer, manual unlock/relock)
    {
        cout << "\n  - Testing Flexible Lock RAII (`std::unique_lock<std::mutex>`)...\n";
        std::unique_lock<mutex> flexLock(sharedMutex);
        cout << "    Critical section 1 executing under std::unique_lock...\n";

        flexLock.unlock(); // Manual temporary unlock
        cout << "    [FLEXIBLE LOCK UNLOCKED] Non-critical work executing unlocked...\n";

        flexLock.lock(); // Manual re-lock
        cout << "    [FLEXIBLE LOCK RELOCKED] Critical section 2 executing...\n";

        cout << "  - Leaving std::unique_ptr / unique_lock scope...\n";
    } // std::unique_lock destructor checks and unlocks mutex automatically HERE!

    // =====================================================================================
    // 3. DEADLOCK AVOIDANCE WITH C++17 `std::scoped_lock`
    // =====================================================================================
    cout << "\n================ 3. MULTI-RESOURCE LOCKING (`std::scoped_lock`) ================\n";

    mutex mutexResourceA;
    mutex mutexResourceB;

    performMultiResourceTransaction(mutexResourceA, mutexResourceB, "Transaction_Alpha");

    // =====================================================================================
    // 4. COMBINED CONCURRENT THREAD-SAFE FILE LOGGING
    // =====================================================================================
    cout << "\n================ 4. COMBINED THREAD-SAFE FILE & LOCK MANAGEMENT ================\n";

    {
        ThreadSafeFileLogger logger("concurrent_system.log");

        // Spawn multiple concurrent worker threads
        vector<thread> workerThreads;
        workerThreads.reserve(3);

        for (int i = 1; i <= 3; ++i) {
            workerThreads.emplace_back([&logger, i, userInputValue]() {
                logger.logMessage(i, "Worker thread executed task ID: " + to_string(userInputValue + i));
            });
        }

        // Join threads safely
        for (auto& th : workerThreads) {
            if (th.joinable()) {
                th.join();
            }
        }

        cout << "  - All concurrent worker threads finished safely.\n";
        cout << "  - Exiting thread-safe logger scope...\n";
    } // ThreadSafeFileLogger destructor flushes and closes file HERE!

    // =====================================================================================
    // 5. EXCEPTION SAFETY DURING FILE AND LOCK OPERATIONS
    // =====================================================================================
    cout << "\n================ 5. EXCEPTION SAFETY IN FILE & LOCK MANAGEMENT ================\n";

    try {
        cout << "  - Entering scope with file handle and lock guard before exception...\n";
        std::lock_guard<mutex> exceptionLock(sharedMutex);
        (void)exceptionLock;

        RAIIFileHandle exceptionFile("raii_exception_log.txt", "w");
        exceptionFile.writeString("Writing telemetry prior to execution error...\n");

        cout << "  - Throwing runtime exception inside locked file processing scope...\n";
        throw runtime_error("Simulated I/O pipeline processing failure!");

        // Code here is bypassed, but stack unwinding forces exceptionFile and exceptionLock destructors!
    } catch (const std::exception& e) {
        cout << "  - [CATCH BLOCK EXECUTED] Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - [RAII GUARANTEE]: Mutex unlocked and file handle closed safely during exception unwind!\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ FILE & LOCK MANAGEMENT SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Resource Type         | Modern C++ RAII Management Class  | Primary Operational Safety Trait  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Legacy C File Handle  | `RAIIFileHandle` (Custom FILE*)   | Auto `fflush` & `fclose` on exit  |\n"
         << "| Standard File Stream  | `std::fstream` / `std::ofstream`  | Auto flush and close on dtor      |\n"
         << "| Single Mutex Lock     | `std::lock_guard<std::mutex>`     | Scoped lock; auto unlock on exit  |\n"
         << "| Flexible Mutex Lock   | `std::unique_lock<std::mutex>`    | Deferred, manual unlock, auto dtor|\n"
         << "| Multi-Mutex Lock      | `std::scoped_lock<M1, M2>`        | Simultaneous lock; deadlock-free  |\n"
         << "| Combined Thread I/O   | `ThreadSafeFileLogger`            | Mutex + File combined RAII safety |\n"
         << "| Exception Unwinding   | All File & Lock RAII Wrappers     | Unconditional cleanup on error    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}