/*
 * =====================================================================================
 * CONCEPT        : Destructor Behavior During Exceptions in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how destructors behave when exceptions propagate during stack unwinding:
 *
 *                  1. Stack Unwinding Mechanics   : Unconditional, LIFO execution of destructors
 *                                                   for all fully constructed stack objects when
 *                                                   an exception is thrown.
 *                  2. Detecting Unwinding         : Using `std::uncaught_exceptions()` to detect
 *                                                   if a destructor is executing due to stack unwinding.
 *                  3. Double Exception Hazard     : Why allowing an exception to escape a destructor
 *                                                   during stack unwinding triggers `std::terminate()`.
 *                  4. The `noexcept` Contract     : Implicit `noexcept` destructors in C++11+ and
 *                                                   defensive error handling inside destructors.
 *                  5. Two-Phase Cleanup Pattern   : Separating throwing cleanup operations (e.g., `.close()`)
 *                                                   from safe, non-throwing destructor fallback cleanup.
 *
 * TIME COMPLEXITY  : Destructor execution during unwinding: O(1) per stack object.
 * SPACE COMPLEXITY : Memory Footprint: Uniform 8-16 bytes per RAII handle.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <exception>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: UNWINDING INSPECTOR
// Inspects `std::uncaught_exceptions()` during destruction to distinguish normal exit
// from exception-induced stack unwinding.
// =====================================================================================
class UnwindingInspector {
private:
    int id_;
    string label_;
    int initialUncaughtCount_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    UnwindingInspector(int id, string label)
        : id_(id), label_(std::move(label)), initialUncaughtCount_(std::uncaught_exceptions()) {
        ++activeCount;
        cout << "    [CTOR] Acquiring UnwindingInspector ID: " << setw(3) << id_ << " (" << setw(20) << left << label_
             << ") | Active: " << activeCount << "\n";
    }

    ~UnwindingInspector() noexcept {
        --activeCount;
        int currentUncaughtCount = std::uncaught_exceptions();

        cout << "    [DTOR] Releasing UnwindingInspector ID: " << setw(3) << id_ << " (" << setw(20) << left << label_
             << ") | Active: " << activeCount;

        // Detecting if destructor was triggered by an active exception unwinding stack
        if (currentUncaughtCount > initialUncaughtCount_) {
            cout << " | [REASON: STACK UNWINDING (Active Exceptions: " << currentUncaughtCount << ")]\n";
        } else {
            cout << " | [REASON: NORMAL SCOPE EXIT]\n";
        }
    }

    void executeWork() const {
        cout << "    [WORK] UnwindingInspector ID: " << id_ << " (" << label_ << ") performing work.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
};

// =====================================================================================
// HELPER CLASS 2: SAFE DESTRUCTOR HANDLE
// Demonstrates defensive programming inside destructors to handle internal errors safely.
// =====================================================================================
class SafeDestructorResource {
private:
    int resourceId_;
    bool simulateReleaseFailure_;

public:
    SafeDestructorResource(int id, bool simulateFailure)
        : resourceId_(id), simulateReleaseFailure_(simulateFailure) {
        cout << "    [SafeDestructor Ctor] Acquired Resource ID: " << resourceId_ << "\n";
    }

    // Destructors are implicitly noexcept in modern C++.
    // ALWAYS catch internal exceptions to prevent std::terminate()!
    ~SafeDestructorResource() noexcept {
        cout << "    [SafeDestructor Dtor] Initiating cleanup for Resource ID: " << resourceId_ << "...\n";
        try {
            if (simulateReleaseFailure_) {
                throw std::runtime_error("OS flush failure during handle closure!");
            }
            cout << "    [SafeDestructor Dtor] Cleanup completed successfully.\n";
        } catch (const std::exception& e) {
            // CRITICAL RULE: Log/handle error locally; NEVER allow exception to escape!
            cout << "    [SafeDestructor Dtor CATCH] Exception caught inside destructor: \"" << e.what() << "\"\n";
            cout << "    [SAFETY GUARANTEE] Exception suppressed to prevent `std::terminate()`.\n";
        }
    }

    SafeDestructorResource(const SafeDestructorResource&) = delete;
    SafeDestructorResource& operator=(const SafeDestructorResource&) = delete;
};

// =====================================================================================
// HELPER CLASS 3: TWO-PHASE CLEANUP PATTERN
// Allows users to explicitly call `.close()` (which can throw) while providing
// a safe, non-throwing destructor fallback for RAII scope safety.
// =====================================================================================
class TwoPhaseFileHandle {
private:
    string filename_;
    bool isOpen_;
    bool isFlushed_;

public:
    explicit TwoPhaseFileHandle(string filename)
        : filename_(std::move(filename)), isOpen_(true), isFlushed_(false) {
        cout << "    [TWO-PHASE INIT] Opened file handle for '" << filename_ << "'\n";
    }

    // Phase 1: Explicit throwing cleanup method (User can catch I/O errors directly)
    void close() {
        if (!isOpen_) return;

        cout << "    [EXPLICIT CLOSE] Attempting explicit flush & close for '" << filename_ << "'...\n";
        // Simulate an I/O flushing exception
        isFlushed_ = true;
        isOpen_ = false;
        cout << "    [EXPLICIT CLOSE] File '" << filename_ << "' closed cleanly via explicit call.\n";
    }

    // Phase 2: Implicit non-throwing destructor fallback
    ~TwoPhaseFileHandle() noexcept {
        if (isOpen_) {
            cout << "    [DTOR FALLBACK] Object exiting scope without explicit close. Performing silent cleanup...\n";
            isOpen_ = false;
            // Silent fallback cleanup never throws!
        }
    }

    void writeData(const string& data) {
        if (isOpen_) {
            cout << "    [WRITE] Wrote data to '" << filename_ << "': \"" << data << "\"\n";
        }
    }

    TwoPhaseFileHandle(const TwoPhaseFileHandle&) = delete;
    TwoPhaseFileHandle& operator=(const TwoPhaseFileHandle&) = delete;
};

// =====================================================================================
// DEMONSTRATION FUNCTIONS
// =====================================================================================

void triggerStackUnwindingPipeline(int baseId) {
    cout << "  - Entering `triggerStackUnwindingPipeline()` function scope...\n";
    UnwindingInspector outerObj(baseId, "OuterStackNode");

    {
        cout << "    - Entering inner nested block scope...\n";
        UnwindingInspector innerObj1(baseId + 1, "InnerStackNode1");
        UnwindingInspector innerObj2(baseId + 2, "InnerStackNode2");

        cout << "    - Throwing exception from inner scope...\n";
        throw std::runtime_error("Critical error in processing pipeline!");

        // innerObj2, innerObj1, and outerObj destructors will execute during unwinding in LIFO order
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for destructor behavior testing (e.g., 900): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 900." << endl;
        userInputValue = 900;
    }

    // =====================================================================================
    // 1. NORMAL SCOPE EXIT VS EXCEPTION STACK UNWINDING
    // =====================================================================================
    cout << "\n================ 1. NORMAL DESTRUCTION VS STACK UNWINDING ================\n";

    // Normal destruction demonstration
    {
        cout << "  - Case A: Executing normal scope exit...\n";
        UnwindingInspector normalObj(userInputValue, "NormalScopeNode");
        normalObj.executeWork();
        cout << "  - Exiting normal scope...\n";
    } // Destructor runs with std::uncaught_exceptions() == 0

    // Exception stack unwinding demonstration
    cout << "\n  - Case B: Executing exception-induced stack unwinding...\n";
    try {
        triggerStackUnwindingPipeline(userInputValue + 10);
    } catch (const std::exception& e) {
        cout << "  - [CATCH BLOCK] Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Trackers Post-Unwinding: " << UnwindingInspector::activeCount 
         << " (RAII GUARANTEE: Zero leaks during stack unwinding!)\n";

    // =====================================================================================
    // 2. DEFENSIVE DESTRUCTOR ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 2. DEFENSIVE DESTRUCTOR ERROR HANDLING ================\n";

    {
        cout << "  - Creating `SafeDestructorResource` with simulated release failure...\n";
        SafeDestructorResource failingRes(userInputValue + 20, true);

        cout << "  - Exiting scope containing failing resource...\n";
    } // Destructor catches internal exception locally

    cout << "  - [SAFETY GUARANTEE]: Program execution continued normally past destructor error.\n";

    // =====================================================================================
    // 3. TWO-PHASE CLEANUP PATTERN
    // =====================================================================================
    cout << "\n================ 3. TWO-PHASE CLEANUP PATTERN ================\n";

    // Usage Pattern A: Explicit closing when error checking is desired
    {
        cout << "  - Usage A: Explicit `.close()` call...\n";
        TwoPhaseFileHandle fileA("explicit_file.txt");
        fileA.writeData("Payload A");
        fileA.close(); // Explicit close
    }

    // Usage Pattern B: Automatic implicit fallback via RAII on scope exit/exception
    {
        cout << "\n  - Usage B: Implicit RAII destructor fallback...\n";
        TwoPhaseFileHandle fileB("implicit_file.txt");
        fileB.writeData("Payload B");
        cout << "  - Exiting scope without explicit close call...\n";
    } // Destructor fallback executes safely

    // =====================================================================================
    // 4. THE DOUBLE EXCEPTION HAZARD & std::terminate()
    // =====================================================================================
    cout << "\n================ 4. THE DOUBLE EXCEPTION HAZARD ================\n";
    cout << "  - RULE: If a destructor allows an exception to escape WHILE stack unwinding\n";
    cout << "          is ALREADY in progress due to another active exception, C++ calls\n";
    cout << "          `std::terminate()` immediately!\n";
    cout << "  - WHY: C++ cannot handle two active unhandled exceptions simultaneously.\n";
    cout << "  - SOLUTION: Destructors MUST be `noexcept` and catch all internal errors.\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DESTRUCTOR BEHAVIOR DURING EXCEPTIONS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Scenario / Rule       | Destructor Behavior Mechanics     | Primary Safety Requirement        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Normal Scope Exit     | Runs in LIFO order on `{}` exit   | Releases resource cleanly         |\n"
         << "| Stack Unwinding       | Runs unconditionally during throw | Guarantees leak-free stack cleanup|\n"
         << "| `uncaught_exceptions` | Increments during stack unwind    | Detects if exception is active    |\n"
         << "| Double Exception      | Throwing during active unwind     | FATAL: Triggers `std::terminate()`|\n"
         << "| `noexcept` Contract   | Implicitly `noexcept` in C++11+   | Destructors MUST NOT throw        |\n"
         << "| Two-Phase Cleanup     | Explicit `.close()` + Dtor fallback| Allows error checking + RAII safety|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}