/*
 * =====================================================================================
 * CONCEPT        : Basic Program Termination in C++
 * DESCRIPTION    : Comprehensive implementation explaining normal vs abnormal program termination:
 *                  1. Normal Exit (return from main) : Complete stack unwinding, destructor calls,
 *                                                       and std::atexit() cleanup execution.
 *                  2. std::exit(code)                 : Immediate termination; cleans static storage and
 *                                                       triggers std::atexit(), but SKIPS local stack destructors!
 *                  3. std::quick_exit(code)           : C++11 quick exit; invokes std::at_quick_exit()
 *                                                       handlers without running object destructors.
 *                  4. std::_Exit(code)                : C++11 immediate exit; terminates process without
 *                                                       invoking any handlers or destructors.
 *                  5. std::abort()                    : Abnormal termination generating a SIGABRT signal.
 *                  6. Standard Exit Codes             : EXIT_SUCCESS (0) vs EXIT_FAILURE (non-zero).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time evaluation for input processing and execution termination.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation for tracking objects.
 * =====================================================================================
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <utility> // For std::move

using namespace std;

// 1. ATEXIT AND AT_QUICK_EXIT CLEANUP HANDLERS
void standardAtExitHandler() {
    cout << "  [atexit Handler] Executed via std::atexit() registration." << endl;
}

void quickExitHandler() {
    cout << "  [at_quick_exit Handler] Executed via std::at_quick_exit() registration." << endl;
}

// 2. RAII CLASS TO TRACK STACK UNWINDING & DESTRUCTOR EXECUTION
class ScopedTracker {
private:
    string objectName;

public:
    // Fully qualified call to std::move prevents unqualified move warnings
    explicit ScopedTracker(string name) : objectName(std::move(name)) {
        cout << "  [RAII Constructed] " << objectName << " created on stack." << endl;
    }

    ~ScopedTracker() {
        cout << "  [RAII Destructed]  " << objectName << " destroyed (Stack Unwound)." << endl;
    }
};

int main() {
    // Register termination handlers with explicit std namespace qualification
    std::atexit(standardAtExitHandler);
    std::at_quick_exit(quickExitHandler);

    // Instantiating a stack object to observe if destructor runs under selected exit mode
    ScopedTracker mainScopeObject("MainLocalObject");

    int terminationChoice = 0;

    // 3. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "\n================ PROGRAM TERMINATION MECHANISMS ================" << endl;
    cout << "Select a Program Termination Mechanism to execute:" << endl;
    cout << "1. Normal Return (return 0 from main - Unwinds stack & runs atexit)" << endl;
    cout << "2. Standard Exit (std::exit - Runs atexit, SKIPS local destructors)" << endl;
    cout << "3. Quick Exit    (std::quick_exit - Runs at_quick_exit, SKIPS destructors)" << endl;
    cout << "4. Immediate Exit(std::_Exit - Bypasses all handlers & destructors)" << endl;
    cout << "5. Abnormal Abort(std::abort - Generates SIGABRT signal)" << endl;
    cout << "Enter choice [1-5]: " << flush;

    if (!(cin >> terminationChoice) || terminationChoice < 1 || terminationChoice > 5) {
        cout << "\nInvalid choice input. Defaulting to Normal Return (Choice 1)." << endl;
        terminationChoice = 1;
    }

    cout << "\nExecuting selected termination mode..." << endl;
    cout << string(60, '-') << endl;

    // Nested block scope object to demonstrate block-level unwinding
    {
        ScopedTracker blockScopeObject("BlockLocalObject");
        cout << "Inside local block scope..." << endl;

        switch (terminationChoice) {
            case 1:
                cout << "\n[Mode 1] Returning normally from main..." << endl;
                // Exiting block will unwind blockScopeObject, then exiting main unwinds mainScopeObject
                break; // Flows down to 'return EXIT_SUCCESS'

            case 2:
                cout << "\n[Mode 2] Invoking std::exit(EXIT_SUCCESS)..." << endl;
                // std::exit terminates immediately. Notice local stack destructors WILL NOT be called!
                std::exit(EXIT_SUCCESS);

            case 3:
                cout << "\n[Mode 3] Invoking std::quick_exit(EXIT_SUCCESS)..." << endl;
                // Runs std::at_quick_exit handlers only
                std::quick_exit(EXIT_SUCCESS);

            case 4:
                cout << "\n[Mode 4] Invoking std::_Exit(EXIT_SUCCESS)..." << endl;
                // Immediate termination, no handlers or destructors
                std::_Exit(EXIT_SUCCESS);

            case 5:
                cout << "\n[Mode 5] Invoking std::abort()..." << endl;
                // Triggers abnormal process termination
                std::abort();

            default:
                break;
        }
    } // blockScopeObject destructor is called here ONLY if returning normally (Choice 1)

    cout << "Exiting main() naturally..." << endl;
    return EXIT_SUCCESS; // Standard exit code 0 indicating successful execution
}