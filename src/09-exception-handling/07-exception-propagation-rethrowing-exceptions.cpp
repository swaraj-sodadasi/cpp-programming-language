/*
 * =====================================================================================
 * CONCEPT        : Exception Propagation and Rethrowing Exceptions in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how exceptions travel up call stacks, how to rethrow them safely, 
 *                  nest exceptions, and transport them across execution threads:
 *
 *                  1. Multi-Level Exception Propagation :
 *                     - When an exception is thrown, control immediately exits the current
 *                       scope and unwinds the stack frame-by-frame (LIFO destruction of 
 *                       RAII objects) until a matching `catch` handler is found.
 *
 *                  2. Rethrowing Semantics (`throw;` vs. `throw ex;`) :
 *                     - Bare `throw;` : Rethrows the EXACT active exception object intact, 
 *                       preserving its dynamic runtime type and virtual function dispatch.
 *                     - `throw ex;`   : Copies the caught exception parameter into a new throw. 
 *                       If caught by base class reference, this SLICES the derived exception object!
 *
 *                  3. Exception Chaining / Nesting (`std::nested_exception`) :
 *                     - Using `std::throw_with_nested` and `std::rethrow_if_nested` to wrap 
 *                       low-level infrastructure errors inside higher-level domain exceptions,
 *                       preserving the full root-cause diagnostic chain.
 *
 *                  4. Cross-Thread Exception Propagation (`std::exception_ptr`) :
 *                     - Capturing exceptions from worker threads or async tasks using 
 *                       `std::current_exception()` and transporting them to the main thread 
 *                       where they are rethrown via `std::rethrow_exception()`.
 *
 * TIME COMPLEXITY  : Normal Path (No Exceptions) : O(1) zero-cost unwind table lookup.
 *                    Propagation & Stack Unwind   : O(Call Stack Depth).
 * SPACE COMPLEXITY : Exception Metadata Footprint : ABI-managed exception pool storage.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>
#include <thread>

using namespace std;

// =====================================================================================
// 1. POLYMORPHIC EXCEPTION HIERARCHY
// Used to demonstrate object slicing during rethrows.
// =====================================================================================
class BaseAppException : public std::runtime_error {
public:
    explicit BaseAppException(const string& msg)
        : std::runtime_error(msg) {}

    [[nodiscard]] virtual string getDiagnosticLayer() const {
        return "BaseAppException [Layer: Application Core]";
    }
};

class DatabaseConnectionException : public BaseAppException {
private:
    int errorCode_;
    string serverIp_;

public:
    DatabaseConnectionException(const string& msg, int errorCode, string serverIp)
        : BaseAppException(msg), errorCode_(errorCode), serverIp_(std::move(serverIp)) {}

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }
    [[nodiscard]] const string& getServerIp() const noexcept { return serverIp_; }

    [[nodiscard]] string getDiagnosticLayer() const override {
        return "DatabaseConnectionException [Layer: Data Access, Code: " + 
               to_string(errorCode_) + ", Host: " + serverIp_ + "]";
    }
};

// =====================================================================================
// 2. STACK FRAME UNWINDING TRACER (RAII)
// Traces frame destruction as exceptions propagate up nested call stacks.
// =====================================================================================
class StackFrameTracer {
private:
    string frameName_;

public:
    explicit StackFrameTracer(string name) : frameName_(std::move(name)) {
        cout << "      [STACK FRAME CTOR] Entering scope frame: '" << frameName_ << "'\n";
    }

    ~StackFrameTracer() noexcept {
        cout << "      [STACK FRAME DTOR] Unwinding & cleaning scope frame: '" << frameName_ << "'\n";
    }
};

// =====================================================================================
// 3. MULTI-LEVEL PROPAGATION & RETHROW FUNCTIONS
// =====================================================================================

// Level 3 (Deepest Call Stack Frame)
void level3_lowLevelWorker(int inputVal) {
    StackFrameTracer frame("Level3_WorkerFrame");
    cout << "        [Level 3 Worker] Low-level operational failure with inputVal = " << inputVal << "\n";
    cout << "        [Level 3 Worker] Throwing `DatabaseConnectionException`...\n";
    throw DatabaseConnectionException("Socket connection refused on port 5432", 10061, "192.168.1.50");
}

// Level 2 (Intermediate Service Layer - Catches and Rethrows)
void level2_serviceLayer(int inputVal, bool useBareRethrow) {
    StackFrameTracer frame("Level2_ServiceFrame");
    try {
        cout << "      [Level 2 Service] Calling Level 3 low-level worker...\n";
        level3_lowLevelWorker(inputVal);
    } catch (const BaseAppException& caughtEx) {
        cout << "      [Level 2 Service] Intercepted exception as `const BaseAppException&`.\n";
        cout << "      [Level 2 Service] Diagnostic: " << caughtEx.getDiagnosticLayer() << "\n";

        if (useBareRethrow) {
            cout << "      [Level 2 Service] Executing bare `throw;` (Preserves dynamic type)...\n";
            throw; // CORRECT: Preserves dynamic type DatabaseConnectionException
        } else {
            cout << "      [Level 2 Service] Executing `throw caughtEx;` (Anti-pattern - Causes Slicing!)...\n";
            throw caughtEx; // SLICES derived exception attributes down to BaseAppException!
        }
    }
}

// Level 1 (Controller Layer)
void level1_controllerLayer(int inputVal, bool useBareRethrow) {
    StackFrameTracer frame("Level1_ControllerFrame");
    cout << "    [Level 1 Controller] Delegating request to Level 2 Service Layer...\n";
    level2_serviceLayer(inputVal, useBareRethrow);
}

// =====================================================================================
// 4. NESTED EXCEPTION PROPAGATION (`std::throw_with_nested` / `std::rethrow_if_nested`)
// =====================================================================================
void fetchUserDataLowLevel(int userId) {
    if (userId < 0) {
        throw std::invalid_argument("User ID cannot be negative!");
    }
    throw std::runtime_error("Database read timeout on shard_04");
}

void processUserOrderService(int userId) {
    try {
        fetchUserDataLowLevel(userId);
    } catch (...) {
        cout << "    [Service Layer] Intercepted low-level error. Nesting inside higher-level context...\n";
        // Wraps the current active exception inside a new exception
        std::throw_with_nested(std::runtime_error("Order Processing Failed for User ID: " + to_string(userId)));
    }
}

// Recursive helper to unpack and display all nested exception contexts
void printNestedExceptions(const std::exception& ex, int level = 0) {
    string indent(level * 2, ' ');
    cout << "    " << indent << "-> [Context Level " << level << "]: " << ex.what() << "\n";
    try {
        std::rethrow_if_nested(ex);
    } catch (const std::exception& nestedEx) {
        printNestedExceptions(nestedEx, level + 1);
    } catch (...) {
        cout << "    " << indent << "  -> [Context Level " << (level + 1) << "]: Unknown non-std exception\n";
    }
}

// =====================================================================================
// 5. CROSS-THREAD EXCEPTION PROPAGATION (`std::exception_ptr`)
// =====================================================================================
std::exception_ptr workerThreadTask(int value) {
    std::exception_ptr capturedPtr = nullptr;
    
    // Spawn background thread
    std::thread workerThread([value, &capturedPtr]() {
        try {
            cout << "      [Worker Thread ID: " << std::this_thread::get_id() 
                 << "] Processing value = " << value << "...\n";
            if (value > 50) {
                throw std::out_of_range("Thread Error: Value " + to_string(value) + " exceeds maximum limit (50)!");
            }
            cout << "      [Worker Thread] Task completed successfully.\n";
        } catch (...) {
            cout << "      [Worker Thread] Exception thrown! Capturing via `std::current_exception()`...\n";
            capturedPtr = std::current_exception(); // Captures exception object safely
        }
    });

    workerThread.join();
    return capturedPtr;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Propagation & Rethrow analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MULTI-LEVEL PROPAGATION & BARE `throw;` (TYPE PRESERVATION)
    // =====================================================================================
    cout << "\n================ 1. MULTI-LEVEL PROPAGATION & BARE `throw;` ================\n";

    try {
        cout << "  - Calling Level 1 Controller (using bare `throw;`)...\n";
        level1_controllerLayer(userInputValue, true); // true = bare throw
    } catch (const BaseAppException& mainEx) {
        cout << "\n  - [MAIN CATCH BLOCK REACHED] Caught propagated exception:\n";
        cout << "    * Message          : \"" << mainEx.what() << "\"\n";
        cout << "    * Polymorphic Layer: " << mainEx.getDiagnosticLayer() << "\n";
        cout << "    [RESULT]: Dynamic type `DatabaseConnectionException` was PRESERVED across 3 stack frames!\n";
    }

    // =====================================================================================
    // 2. SLICED RETHROW (`throw ex;`) ANTI-PATTERN
    // =====================================================================================
    cout << "\n================ 2. SLICED RETHROW (`throw ex;`) ANTI-PATTERN ================\n";

    try {
        cout << "  - Calling Level 1 Controller (using sliced `throw ex;`)...\n";
        level1_controllerLayer(userInputValue, false); // false = sliced throw
    } catch (const BaseAppException& mainEx) {
        cout << "\n  - [MAIN CATCH BLOCK REACHED] Caught propagated exception:\n";
        cout << "    * Message          : \"" << mainEx.what() << "\"\n";
        cout << "    * Polymorphic Layer: " << mainEx.getDiagnosticLayer() << "\n";
        cout << "    [RESULT]: Dynamic type was SLICED down to BaseAppException! Derived attributes were lost!\n";
    }

    // =====================================================================================
    // 3. EXCEPTION CHAINING / NESTING (`std::throw_with_nested`)
    // =====================================================================================
    cout << "\n================ 3. EXCEPTION NESTING (`std::throw_with_nested`) ================\n";

    try {
        cout << "  - Invoking service layer to trigger nested exception chaining...\n";
        processUserOrderService(userInputValue);
    } catch (const std::exception& topEx) {
        cout << "\n  - [MAIN CATCH BLOCK] Unpacking nested exception diagnostic hierarchy:\n";
        printNestedExceptions(topEx);
    }

    // =====================================================================================
    // 4. CROSS-THREAD EXCEPTION PROPAGATION (`std::exception_ptr`)
    // =====================================================================================
    cout << "\n================ 4. CROSS-THREAD EXCEPTION PROPAGATION ================\n";

    cout << "  - Launching worker thread task with value " << userInputValue << "...\n";
    std::exception_ptr threadExPtr = workerThreadTask(userInputValue);

    if (threadExPtr) {
        cout << "  - [MAIN THREAD] Received valid exception_ptr from worker thread.\n";
        try {
            cout << "  - [MAIN THREAD] Rethrowing captured exception via `std::rethrow_exception()`...\n";
            std::rethrow_exception(threadExPtr);
        } catch (const std::out_of_range& threadEx) {
            cout << "  - [MAIN THREAD CATCH SUCCESS] Intercepted thread exception: \"" 
                 << threadEx.what() << "\"\n";
        }
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION PROPAGATION & RETHROW SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Propagation Pattern   | C++ Implementation Syntax         | Architectural Behavior & Rule     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Stack Unwinding       | Unwinds scopes automatically      | Destructs RAII objects in LIFO    |\n"
         << "| Bare Rethrow          | `throw;`                          | Preserves dynamic polymorphic type|\n"
         << "| Sliced Rethrow        | `throw caughtEx;` (Anti-pattern)  | Slices derived object to base copy|\n"
         << "| Nested Exceptions     | `std::throw_with_nested(...)`     | Chains root cause with high-level |\n"
         << "| Unpacking Nested      | `std::rethrow_if_nested(ex)`      | Recursively inspects inner errors |\n"
         << "| Cross-Thread Capture  | `std::current_exception()`        | Stores active exception in ptr    |\n"
         << "| Cross-Thread Dispatch | `std::rethrow_exception(exPtr)`   | Rethrows stored ptr on any thread |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}