/*
 * =====================================================================================
 * CONCEPT        : Exceptions in Multithreading under Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how exceptions interact with multithreading execution boundaries:
 *
 *                  1. Unhandled Exception Rule in Threads :
 *                     - Exceptions thrown in a worker thread CANNOT directly cross frame 
 *                       boundaries into the parent/main thread.
 *                     - If an exception escapes a thread function uncaught, the runtime
 *                       immediately invokes `std::terminate()`.
 *
 *                  2. Manual Exception Transportation (`std::exception_ptr`) :
 *                     - `std::current_exception()` captures the active exception object into 
 *                       a thread-safe, reference-counted `std::exception_ptr`.
 *                     - `std::rethrow_exception()` rethrows the captured exception inside 
 *                       the main thread's scope for centralized handling.
 *
 *                  3. Automated Exception Propagation (`std::async` & `std::future`) :
 *                     - `std::async` and `std::packaged_task` capture thrown worker exceptions 
 *                       automatically into the shared task state.
 *                     - Calling `.get()` on the associated `std::future` automatically 
 *                       rethrows the worker thread exception in the retrieving thread.
 *
 *                  4. Parallel Multi-Thread Exception Aggregation :
 *                     - Thread-safe collection of multiple worker thread exceptions using 
 *                       mutex-guarded `std::vector<std::exception_ptr>`.
 *
 *                  5. RAII Thread Management (`ThreadGuard`) :
 *                     - Prevents `std::terminate()` caused by destructing unjoined `std::thread` 
 *                       objects when an exception throws in the main thread before `.join()`.
 *
 * TIME COMPLEXITY  : Exception Capture / Transport  : O(1) reference-counted pointer copy.
 *                    Future Exception Rethrow       : O(1) task state check & rethrow.
 * SPACE COMPLEXITY : Exception Shared State          : Runtime heap block managed by ABI.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <future>
#include <vector>
#include <mutex>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. CUSTOM EXCEPTION CLASSES FOR MULTITHREADED WORKERS
// =====================================================================================
class WorkerThreadException : public std::runtime_error {
private:
    int threadId_;

public:
    WorkerThreadException(const string& message, int threadId)
        : std::runtime_error(message), threadId_(threadId) {}

    [[nodiscard]] int getThreadId() const noexcept { return threadId_; }
};

// =====================================================================================
// 2. RAII THREAD GUARD (Guarantees join() on parent thread exception)
// =====================================================================================
class ThreadGuard {
private:
    std::thread& threadRef_;

public:
    explicit ThreadGuard(std::thread& t) noexcept : threadRef_(t) {}

    ~ThreadGuard() noexcept {
        if (threadRef_.joinable()) {
            cout << "    [ThreadGuard Dtor] Ensuring thread ID " << threadRef_.get_id() 
                 << " is joined on scope exit...\n";
            threadRef_.join();
        }
    }

    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
};

// =====================================================================================
// 3. DEMO 1: MANUAL EXCEPTION TRANSPORTATION USING std::exception_ptr
// =====================================================================================
void manualThreadWorker(int inputVal, int threadId, std::exception_ptr& outExPtr) {
    try {
        cout << "    [Worker Thread #" << threadId << "] Processing input value: " << inputVal << "...\n";
        
        if (inputVal > 50) {
            throw WorkerThreadException("Worker threshold exceeded (Input > 50)!", threadId);
        }

        cout << "    [Worker Thread #" << threadId << "] Processing succeeded.\n";
    } catch (...) {
        cout << "    [Worker Thread #" << threadId << " CATCH] Capturing exception via std::current_exception()...\n";
        outExPtr = std::current_exception(); // Store active exception safely
    }
}

// =====================================================================================
// 4. DEMO 2: AUTOMATED EXCEPTION PROPAGATION WITH std::async AND std::future
// =====================================================================================
int asyncThreadWorker(int numerator, int denominator) {
    cout << "    [Async Worker Thread] Calculating (" << numerator << " / " << denominator << ")...\n";
    
    if (denominator == 0) {
        throw std::invalid_argument("Async Worker Error: Division by zero attempted inside thread!");
    }

    return numerator / denominator;
}

// =====================================================================================
// 5. DEMO 3: AGGREGATING EXCEPTIONS FROM MULTIPLE PARALLEL WORKERS
// =====================================================================================
class ParallelWorkerPool {
private:
    std::mutex queueMutex_;
    std::vector<std::exception_ptr> capturedExceptions_;

public:
    void executeParallelTask(int taskId, int inputVal) {
        try {
            cout << "      [Parallel Task #" << taskId << "] Executing with input = " << inputVal << "...\n";
            
            if (inputVal % 2 != 0) {
                throw WorkerThreadException("Odd input rejected by Parallel Task #" + to_string(taskId), taskId);
            }

            cout << "      [Parallel Task #" << taskId << "] Task completed cleanly.\n";
        } catch (...) {
            std::lock_guard<std::mutex> lock(queueMutex_);
            capturedExceptions_.push_back(std::current_exception());
            cout << "      [Parallel Task #" << taskId << "] Exception thread-safely recorded in pool.\n";
        }
    }

    void evaluateParallelResults() {
        cout << "    [Main Thread] Evaluating " << capturedExceptions_.size() 
             << " captured parallel exceptions...\n";
        
        for (size_t i = 0; i < capturedExceptions_.size(); ++i) {
            try {
                if (capturedExceptions_[i]) {
                    std::rethrow_exception(capturedExceptions_[i]);
                }
            } catch (const WorkerThreadException& ex) {
                cout << "    * [Aggregated Exception #" << (i + 1) << "]: From Thread #" 
                     << ex.getThreadId() << " -> \"" << ex.what() << "\"\n";
            } catch (const std::exception& ex) {
                cout << "    * [Aggregated Exception #" << (i + 1) << "]: Standard Exception -> \"" 
                     << ex.what() << "\"\n";
            }
        }
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Multithreading Exception analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MANUAL EXCEPTION TRANSPORTATION WITH std::exception_ptr & std::thread
    // =====================================================================================
    cout << "\n================ 1. MANUAL TRANSPORTATION (`std::exception_ptr` & `std::thread`) ================\n";

    std::exception_ptr workerExPtr = nullptr;

    {
        cout << "  - Spawning `std::thread` with manual exception capture...\n";
        std::thread workerThread(manualThreadWorker, userInputValue, 1, std::ref(workerExPtr));
        ThreadGuard guard(workerThread); // Ensures thread is joined even if main throws
    } // ThreadGuard destructor joins thread HERE

    if (workerExPtr) {
        cout << "  - [Main Thread] Captured exception detected from worker thread.\n";
        try {
            cout << "  - [Main Thread] Rethrowing exception via `std::rethrow_exception()`...\n";
            std::rethrow_exception(workerExPtr);
        } catch (const WorkerThreadException& ex) {
            cout << "  - [MAIN CATCH HANDLER] Caught transported thread exception:\n";
            cout << "    * Thread ID : " << ex.getThreadId() << "\n";
            cout << "    * Message   : \"" << ex.what() << "\"\n";
            cout << "    [VERIFICATION]: Exception successfully crossed thread boundary without crashing!\n";
        }
    }

    // =====================================================================================
    // 2. AUTOMATED EXCEPTION PROPAGATION WITH std::async & std::future
    // =====================================================================================
    cout << "\n================ 2. AUTOMATED PROPAGATION (`std::async` & `std::future`) ================\n";

    // Scenario A: Successful Async Execution
    cout << "  - Scenario A: Executing valid async division (" << userInputValue << " / 2)...\n";
    std::future<int> validFuture = std::async(std::launch::async, asyncThreadWorker, userInputValue, 2);
    try {
        int result = validFuture.get(); // Retrieves value
        cout << "    * Async Result = " << result << "\n";
    } catch (const std::exception& ex) {
        cout << "    [UNREACHABLE]: " << ex.what() << "\n";
    }

    // Scenario B: Async Execution Throwing Division by Zero
    cout << "\n  - Scenario B: Executing invalid async division (" << userInputValue << " / 0)...\n";
    std::future<int> failingFuture = std::async(std::launch::async, asyncThreadWorker, userInputValue, 0);
    try {
        cout << "    * Main thread calling `failingFuture.get()`...\n";
        int result = failingFuture.get(); // Automatically rethrows worker exception HERE!
        cout << "    * Result = " << result << "\n";
    } catch (const std::invalid_argument& ex) {
        cout << "  - [MAIN CATCH HANDLER] Intercepted rethrown exception from `future.get()`:\n";
        cout << "    * Message: \"" << ex.what() << "\"\n";
        cout << "    [VERIFICATION]: `std::future::get()` automatically propagated worker exception!\n";
    }

    // =====================================================================================
    // 3. AGGREGATING EXCEPTIONS ACROSS PARALLEL WORKER THREADS
    // =====================================================================================
    cout << "\n================ 3. MULTI-THREAD PARALLEL EXCEPTION AGGREGATION ================\n";

    ParallelWorkerPool pool;
    std::vector<std::thread> threadPool;

    cout << "  - Launching 4 parallel worker threads into pool...\n";
    for (int i = 1; i <= 4; ++i) {
        int testInput = (i % 2 == 0) ? userInputValue : (userInputValue + 1); // Alternates even / odd
        threadPool.emplace_back(&ParallelWorkerPool::executeParallelTask, &pool, i, testInput);
    }

    // Join all parallel worker threads
    for (auto& t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Evaluate all collected worker exceptions
    pool.evaluateParallelResults();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MULTITHREADING EXCEPTION HANDLING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Multithreading Model  | Exception Mechanism               | Behavior & Architectural Guarantee|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Bare `std::thread`    | Uncaught Exception Escapes        | Immediately calls std::terminate()|\n"
         << "| `std::exception_ptr`  | `std::current_exception()`        | Thread-safe handle to exception   |\n"
         << "| Rethrow to Parent     | `std::rethrow_exception(exPtr)`   | Rethrows captured exception in main|\n"
         << "| `std::async` / Future | `future.get()`                    | Auto-rethrows exception on get()  |\n"
         << "| Parallel Aggregation  | `std::vector<std::exception_ptr>` | Mutex-guarded multi-error collection|\n"
         << "| RAII Thread Management| Custom `ThreadGuard` / `jthread`  | Auto-joins thread on exception    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}