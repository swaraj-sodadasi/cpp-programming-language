/*
 * =====================================================================================
 * CONCEPT        : `static` and `extern` Keywords in C++
 * DESCRIPTION    : Comprehensive program demonstrating the mechanics of `static` and `extern`:
 *                  1. Global Scope Linkage:
 *                     - `static` at global scope  : Grants Internal Linkage (symbol private to current TU).
 *                     - `extern` variable declaration: Promises symbol definition exists elsewhere (External Linkage).
 *                  2. Local Scope Storage Duration:
 *                     - `static` inside functions : Grants Static Storage Duration (persists state across calls,
 *                                                  initialized exactly once, thread-safe in C++11+).
 *                  3. Class Scope (Shared Members):
 *                     - `static` data members     : Shared single instance across all class objects.
 *                     - `static` member functions : Callable without an object instance; no `this` pointer.
 *                  4. Language Linkage (`extern "C"`):
 *                     - Suppresses C++ name mangling for C ABI interop or dynamic exports.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime execution.
 * SPACE COMPLEXITY : Best Case: O(1) stack frame allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>

using namespace std;

// =====================================================================================
// 1. GLOBAL SCOPE: `static` (INTERNAL LINKAGE) vs `extern` (EXTERNAL LINKAGE)
// =====================================================================================

// Global static variable: Invisible to other Translation Units (Internal Linkage)
static int g_internalTuCounter = 100;

// Declaration of an external variable defined elsewhere (External Linkage)
extern int g_externalSharedState;

// Definition of the external variable
int g_externalSharedState = 500;

// =====================================================================================
// 2. CLASS SCOPE: `static` MEMBERS
// =====================================================================================

class SystemWorker {
private:
    string workerName_;
    
    // Shared class-level member (One copy for all instances)
    static int activeWorkerCount_;

public:
    explicit SystemWorker(string name) : workerName_(std::move(name)) {
        ++activeWorkerCount_;
        cout << "  - [Constructor] Spawned worker '" << workerName_ 
             << "'. Active Workers: " << activeWorkerCount_ << "\n";
    }

    ~SystemWorker() {
        --activeWorkerCount_;
        cout << "  - [Destructor ] Terminated worker '" << workerName_ 
             << "'. Remaining Workers: " << activeWorkerCount_ << "\n";
    }

    // Static member function (Can be called without creating an object instance)
    static int getActiveWorkerCount() {
        return activeWorkerCount_;
    }
};

// Out-of-class definition required for static data members (C++17 allows inline static)
int SystemWorker::activeWorkerCount_ = 0;

// =====================================================================================
// 3. FUNCTION SCOPE: `static` LOCAL VARIABLES
// =====================================================================================

void demonstrateStaticLocal(int incrementStep) {
    // Persistent local variable: Initialized ONCE on first call; retains value across calls
    static int callCounter = 0;
    
    callCounter += incrementStep;
    cout << "  - [Local Static] 'callCounter' updated to: " << callCounter 
         << " (Address: " << static_cast<const void*>(&callCounter) << ")\n";
}

// =====================================================================================
// 4. LANGUAGE LINKAGE: `extern "C"`
// =====================================================================================

extern "C" {
    // Prevents C++ name mangling for C library integration
    void c_style_legacy_subsystem(int statusSignal) {
        cout << "  - [extern \"C\"] Unmangled legacy C function executed with Signal Code: " 
             << statusSignal << "\n";
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userStepInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an increment step for local static test (e.g., 5): " << flush;
    if (!(cin >> userStepInput) || userStepInput <= 0) {
        cout << "Invalid input. Defaulting increment step to 5." << endl;
        userStepInput = 5;
    }

    // 1. GLOBAL SCOPE DEMONSTRATION
    cout << "\n================ 1. GLOBAL SCOPE (`static` vs `extern`) ================" << endl;
    cout << "  - Global 'static' (Internal Linkage, TU private) : g_internalTuCounter = " 
         << g_internalTuCounter << "\n";
    cout << "  - Global 'extern' (External Linkage, Cross-TU)   : g_externalSharedState = " 
         << g_externalSharedState << "\n";

    // 2. LOCAL SCOPE DEMONSTRATION
    cout << "\n================ 2. LOCAL FUNCTION SCOPE (`static`) ================" << endl;
    cout << "Calling 'demonstrateStaticLocal(" << userStepInput << ")' three times:\n";
    demonstrateStaticLocal(userStepInput);
    demonstrateStaticLocal(userStepInput);
    demonstrateStaticLocal(userStepInput);

    // 3. CLASS SCOPE DEMONSTRATION
    cout << "\n================ 3. CLASS SCOPE (`static` MEMBERS) ================" << endl;
    cout << "Initial Active Workers via static method: " << SystemWorker::getActiveWorkerCount() << "\n";
    
    {
        SystemWorker w1("AlphaWorker");
        SystemWorker w2("BetaWorker");
        cout << "Active Workers inside block scope: " << SystemWorker::getActiveWorkerCount() << "\n";
    } // w1 and w2 destroyed here

    cout << "Active Workers after block exit: " << SystemWorker::getActiveWorkerCount() << "\n";

    // 4. LANGUAGE LINKAGE DEMONSTRATION
    cout << "\n================ 4. LANGUAGE LINKAGE (`extern \"C\"`) ================" << endl;
    c_style_legacy_subsystem(200);

    // 5. SUMMARY COMPARISON
    cout << "\n================ `static` vs `extern` SUMMARY ================" << endl;
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Context / Context     | Storage Duration  | Linkage Type      | Key Effect / Primary Purpose      |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Global `static var`   | Static (Program)  | Internal          | Restricts symbol to current TU    |\n"
         << "| Global `extern var`   | Static (Program)  | External          | Declares global shared symbol     |\n"
         << "| Local `static var`    | Static (Program)  | None (Block)      | Persists state across function calls|\n"
         << "| Class `static var`    | Static (Program)  | External/Internal | Single instance shared by all objs|\n"
         << "| Class `static func`   | Static (Code)     | External/Internal | Callable without object instance  |\n"
         << "| `extern \"C\"`          | Static/Code       | Language Linkage  | Disables C++ name mangling (C ABI)|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}