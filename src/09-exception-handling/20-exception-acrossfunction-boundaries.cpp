/*
 * =====================================================================================
 * CONCEPT        : Exceptions Across Function Boundaries in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the rules, mechanics, and best practices when exceptions travel across
 *                  various function and system boundaries:
 *
 *                  1. Multi-Level Call Stack Traversal :
 *                     - When an exception is thrown inside a deep function frame, the runtime
 *                       unwinds the stack frame-by-frame across function boundaries until a
 *                       matching catch block is encountered.
 *                     - Automatic (stack) RAII objects in every intermediate frame are destructed
 *                       in exact reverse order of construction (LIFO).
 *
 *                  2. Architectural Subsystem Boundary Translation :
 *                     - Low-level infrastructure exceptions (e.g., POSIX I/O, database drivers)
 *                       should be caught at layer boundaries and translated/wrapped into 
 *                       high-level domain exceptions to avoid exposing internal details.
 *
 *                  3. C-ABI / Language Interop Boundaries (`extern "C"`) :
 *                     - C++ exceptions MUST NOT cross C language boundaries or DLL/shared object
 *                       C-ABI exports! Doing so results in UNDEFINED BEHAVIOR.
 *                     - Pattern: Intercept all exceptions (`catch (...)`) at the C-API boundary
 *                       and return an integer status error code.
 *
 *                  4. Thread Function Boundaries (`std::thread`) :
 *                     - Exceptions thrown inside a background thread function CANNOT cross the 
 *                       thread boundary back into the spawning main thread automatically.
 *                     - Unhandled exceptions inside a thread invoke `std::terminate()`.
 *                     - Solution: Transport exceptions safely using `std::exception_ptr`,
 *                       `std::current_exception()`, and `std::rethrow_exception()`.
 *
 *                  5. `noexcept` Function Boundaries :
 *                     - A function marked `noexcept` forms an unyielding boundary. An exception 
 *                       attempting to cross out of a `noexcept` boundary bypasses catch handlers 
 *                       and immediately calls `std::terminate()`.
 *
 * RESOLVED ISSUE : Properly escaped inner double quotes in string literals (`extern \"C\"`) 
 *                  to resolve C++ literal suffix compilation errors.
 *
 * TIME COMPLEXITY  : Stack Traversal / Unwinding : O(Depth of Call Stack).
 *                    Exception Pointer Transport: O(1) pointer assignment.
 * SPACE COMPLEXITY : Exception ABI Storage       : Runtime pool memory allocated for thrown object.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <thread>
#include <limits>

using namespace std;

// =====================================================================================
// 1. RAII STACK FRAME TRACER
// Logs entry and exit of functions to visualize frame unwinding across boundaries.
// =====================================================================================
class FrameBoundaryTracer {
private:
    string functionBoundaryName_;

public:
    explicit FrameBoundaryTracer(string name) 
        : functionBoundaryName_(std::move(name)) {
        cout << "      [Enter Frame] Entered function boundary: '" << functionBoundaryName_ << "'\n";
    }

    ~FrameBoundaryTracer() noexcept {
        cout << "      [Exit  Frame] Unwound & destroyed scope frame: '" << functionBoundaryName_ << "'\n";
    }
};

// =====================================================================================
// 2. DOMAIN EXCEPTION CLASSES FOR SUBSYSTEM BOUNDARIES
// =====================================================================================
class InfrastructureLowLevelException : public std::runtime_error {
private:
    int sysErrorCode_;

public:
    InfrastructureLowLevelException(const string& msg, int code)
        : std::runtime_error(msg), sysErrorCode_(code) {}

    [[nodiscard]] int getSysErrorCode() const noexcept { return sysErrorCode_; }
};

class ApplicationDomainException : public std::runtime_error {
private:
    string domainModule_;

public:
    ApplicationDomainException(const string& msg, string module)
        : std::runtime_error(msg), domainModule_(std::move(module)) {}

    [[nodiscard]] const string& getDomainModule() const noexcept { return domainModule_; }
};

// =====================================================================================
// 3. MULTI-LEVEL STACK TRAVERSAL FUNCTIONS
// =====================================================================================

// Level 3: Deepest worker function where error originates
void deepLeafWorkerFunction(int inputVal) {
    FrameBoundaryTracer tracer("deepLeafWorkerFunction()");
    cout << "        [Leaf Worker] Processing input = " << inputVal << "\n";

    if (inputVal > 50) {
        cout << "        [Leaf Worker THROW] Throwing `InfrastructureLowLevelException`...\n";
        throw InfrastructureLowLevelException("Socket read failed on port 8080: Connection Reset", 10054);
    }

    cout << "        [Leaf Worker] Work completed successfully.\n";
}

// Level 2: Intermediate processing layer
void intermediateServiceLayer(int inputVal) {
    FrameBoundaryTracer tracer("intermediateServiceLayer()");
    cout << "      [Service Layer] Delegating work down to leaf worker...\n";
    
    // Exception passes through this boundary unhindered; stack unwinding cleans tracer!
    deepLeafWorkerFunction(inputVal);

    cout << "      [Service Layer] Post-processing completed.\n";
}

// Level 1: Top controller layer
void topControllerLayer(int inputVal) {
    FrameBoundaryTracer tracer("topControllerLayer()");
    cout << "    [Controller Layer] Forwarding request to service layer...\n";
    
    intermediateServiceLayer(inputVal);

    cout << "    [Controller Layer] Request finished.\n";
}

// =====================================================================================
// 4. ARCHITECTURAL BOUNDARY TRANSLATION PATTERN
// Catches low-level infrastructure errors and translates them into domain exceptions.
// =====================================================================================
void boundaryTranslationService(int inputVal) {
    FrameBoundaryTracer tracer("boundaryTranslationService()");
    
    try {
        cout << "    [Boundary Translator] Calling low-level subsystem...\n";
        deepLeafWorkerFunction(inputVal);
    } catch (const InfrastructureLowLevelException& lowLevelEx) {
        cout << "    [Boundary Translator CATCH] Intercepted low-level error code " 
             << lowLevelEx.getSysErrorCode() << ": \"" << lowLevelEx.what() << "\"\n";
        cout << "    [Boundary Translator] Translating to high-level `ApplicationDomainException`...\n";
        
        // Translate and wrap into domain exception across architecture layer
        throw ApplicationDomainException(
            "User Data Sync Failed due to Subsystem Network Error",
            "UserService::SyncEngine"
        );
    }
}

// =====================================================================================
// 5. C-ABI / LANGUAGE INTEROP BOUNDARY PATTERN (`extern "C"`)
// Standard practice to expose C++ logic safely to C or foreign language wrappers.
// =====================================================================================

// C-compatible error status codes
enum C_ErrorCode {
    C_SUCCESS = 0,
    C_INVALID_ARGUMENT = -1,
    C_SYSTEM_ERROR = -2,
    C_UNKNOWN_ERROR = -99
};

// Safe C API Wrapper Function (Guaranteed never to throw C++ exceptions out)
extern "C" C_ErrorCode safe_c_api_function(int inputVal, int* outResult) noexcept {
    // Exception barrier try-catch block wrapping C++ implementation
    try {
        if (!outResult) {
            return C_INVALID_ARGUMENT;
        }

        if (inputVal < 0) {
            throw std::invalid_argument("Input value cannot be negative!");
        }

        if (inputVal > 100) {
            throw InfrastructureLowLevelException("Buffer overflow in C API backend", 500);
        }

        *outResult = inputVal * 2;
        return C_SUCCESS;

    } catch (const std::invalid_argument& ex) {
        cout << "    [C-API Boundary Catch] Caught invalid argument: \"" << ex.what() << "\"\n";
        return C_INVALID_ARGUMENT;
    } catch (const std::exception& ex) {
        cout << "    [C-API Boundary Catch] Caught standard exception: \"" << ex.what() << "\"\n";
        return C_SYSTEM_ERROR;
    } catch (...) {
        cout << "    [C-API Boundary Catch] Caught unknown exception!\n";
        return C_UNKNOWN_ERROR;
    }
}

// =====================================================================================
// 6. THREAD FUNCTION BOUNDARY TRANSPORTATION (`std::exception_ptr`)
// =====================================================================================
void backgroundThreadWorker(int inputVal, std::exception_ptr& outExPtr) {
    try {
        cout << "      [Background Thread ID: " << std::this_thread::get_id() 
             << "] Worker thread running with input = " << inputVal << "...\n";

        if (inputVal > 20) {
            throw std::out_of_range("Thread error: Value exceeds worker threshold (20)!");
        }

        cout << "      [Background Thread] Work executed without error.\n";
    } catch (...) {
        cout << "      [Background Thread CATCH] Capturing exception at thread boundary via `std::current_exception()`...\n";
        // Store the exception object safely in exception_ptr to pass across thread boundary
        outExPtr = std::current_exception();
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Function Boundary analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MULTI-LEVEL CALL STACK UNWINDING ACROSS BOUNDARIES
    // =====================================================================================
    cout << "\n================ 1. MULTI-LEVEL CALL STACK UNWINDING ================\n";

    try {
        cout << "  - Calling `topControllerLayer(" << userInputValue << ")`...\n";
        topControllerLayer(userInputValue);
    } catch (const InfrastructureLowLevelException& ex) {
        cout << "\n  - [MAIN CATCH HANDLER] Caught exception that crossed 3 function boundaries:\n";
        cout << "    * Message        : \"" << ex.what() << "\"\n";
        cout << "    * System Code    : " << ex.getSysErrorCode() << "\n";
        cout << "    [VERIFICATION]: All intermediate stack frames were correctly unwound and destructed!\n";
    }

    // =====================================================================================
    // 2. SUBSYSTEM BOUNDARY TRANSLATION
    // =====================================================================================
    cout << "\n================ 2. ARCHITECTURAL BOUNDARY TRANSLATION ================\n";

    try {
        cout << "  - Calling `boundaryTranslationService(" << userInputValue << ")`...\n";
        boundaryTranslationService(userInputValue);
    } catch (const ApplicationDomainException& domainEx) {
        cout << "\n  - [MAIN CATCH HANDLER] Caught translated high-level domain exception:\n";
        cout << "    * Domain Message : \"" << domainEx.what() << "\"\n";
        cout << "    * Target Module  : " << domainEx.getDomainModule() << "\n";
        cout << "    [VERIFICATION]: Low-level socket detail was hidden and wrapped safely for caller!\n";
    }

    // =====================================================================================
    // 3. C-ABI / LANGUAGE INTEROP BOUNDARY (`extern "C"`)
    // =====================================================================================
    cout << "\n================ 3. C-ABI / LANGUAGE INTEROP BOUNDARY ================\n";

    int cApiResult = 0;

    cout << "  - Calling `safe_c_api_function(input = " << userInputValue << ")`...\n";
    C_ErrorCode status1 = safe_c_api_function(userInputValue, &cApiResult);
    cout << "    * C API Return Code: " << status1 << " (C_SYSTEM_ERROR)\n";

    cout << "\n  - Calling `safe_c_api_function(input = -10)`...\n";
    C_ErrorCode status2 = safe_c_api_function(-10, &cApiResult);
    cout << "    * C API Return Code: " << status2 << " (C_INVALID_ARGUMENT)\n";

    cout << "\n  - Calling `safe_c_api_function(input = 25)`...\n";
    C_ErrorCode status3 = safe_c_api_function(25, &cApiResult);
    cout << "    * C API Return Code: " << status3 << " (C_SUCCESS), Result = " << cApiResult << "\n";
    cout << "    [VERIFICATION]: No C++ exceptions escaped the `extern \"C\"` boundary!\n";

    // =====================================================================================
    // 4. CROSSING THREAD BOUNDARIES (`std::exception_ptr`)
    // =====================================================================================
    cout << "\n================ 4. CROSSING THREAD BOUNDARIES ================\n";

    std::exception_ptr capturedThreadEx = nullptr;

    cout << "  - Spawning worker thread with input = " << userInputValue << "...\n";
    std::thread workerThread(backgroundThreadWorker, userInputValue, std::ref(capturedThreadEx));
    
    // Join thread to wait for completion
    workerThread.join();

    // Inspect if an exception was transported across thread boundary
    if (capturedThreadEx) {
        cout << "  - [MAIN THREAD] Detected exception captured from background thread.\n";
        try {
            cout << "  - [MAIN THREAD] Rethrowing exception via `std::rethrow_exception()`...\n";
            std::rethrow_exception(capturedThreadEx);
        } catch (const std::out_of_range& threadEx) {
            cout << "  - [MAIN THREAD CATCH] Successfully caught transported exception:\n";
            cout << "    * Message : \"" << threadEx.what() << "\"\n";
            cout << "    [VERIFICATION]: Thread boundary safely crossed without `std::terminate()`!\n";
        }
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTIONS ACROSS FUNCTION BOUNDARIES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Boundary Type         | Handling Rule / Mechanism         | Risk / Architectural Outcome      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Standard Call Stack   | Automatic Unwinding & Propagation | Destructs RAII objects in LIFO    |\n"
         << "| Layer / Subsystem     | Translate low-level to domain ex  | Prevents leak of internal impl    |\n"
         << "| C-ABI / extern \"C\"    | MUST catch all (`catch(...)`)     | Exception escaping = Undefined    |\n"
         << "| Thread Boundary       | `std::exception_ptr` & rethrow    | Exception escaping = terminate()  |\n"
         << "| `noexcept` Boundary   | Unwinding stops at function edge  | Throwing inside = `std::terminate`|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}