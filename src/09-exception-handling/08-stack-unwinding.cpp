/*
 * =====================================================================================
 * CONCEPT        : Stack Unwinding under Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the exact mechanics, sequencing, guarantees, and safety constraints 
 *                  of Stack Unwinding during exception propagation:
 *
 *                  1. Stack Unwinding Definition & Scope Popping:
 *                     - When an exception is thrown, the C++ runtime pops stack frames 
 *                       one by one from the throw site up to the matching `catch` handler.
 *
 *                  2. LIFO Destructor Execution Guarantee :
 *                     - All fully constructed automatic (stack) objects in each popped 
 *                       frame are destructed in exact REVERSE order of construction (LIFO).
 *
 *                  3. Active Unwinding Inspection (`std::uncaught_exceptions`) :
 *                     - C++17 feature allowing destructors to inspect if they are being 
 *                       called due to an ongoing exception stack unwind.
 *
 *                  4. The Cardinal Rule of Destructors (No Exceptions During Unwind) :
 *                     - Destructors are implicitly `noexcept`. If a destructor throws 
 *                       an exception while stack unwinding is ALREADY in progress, 
 *                       the C++ runtime calls `std::terminate()` immediately!
 *
 *                  5. Partial Construction vs. Stack Unwinding :
 *                     - If an exception occurs inside a constructor, member sub-objects 
 *                       constructed so far are destructed in reverse order before 
 *                       unwinding propagates to the caller.
 *
 * TIME COMPLEXITY  : Stack Frame Popper / Destructor Invocation : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Storage & Unwind Table Metadata   : ABI-managed runtime memory.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>

using namespace std;

// =====================================================================================
// 1. RAII STACK TRACKER CLASS
// Traces constructor and destructor call sequences to demonstrate LIFO destruction.
// =====================================================================================
class StackScopeTracer {
private:
    string objectLabel_;
    int scopeDepth_;

public:
    StackScopeTracer(string label, int depth)
        : objectLabel_(std::move(label)), scopeDepth_(depth) {
        cout << "      [CTOR] Constructing '" << objectLabel_ 
             << "' at Stack Depth " << scopeDepth_ << "\n";
    }

    ~StackScopeTracer() noexcept {
        // Inspect active uncaught exceptions (C++17)
        int uncaughtCount = std::uncaught_exceptions();
        cout << "      [DTOR] Destructing  '" << objectLabel_ 
             << "' at Stack Depth " << scopeDepth_ 
             << " | Active Uncaught Exceptions: " << uncaughtCount << "\n";
    }
};

// =====================================================================================
// 2. RESOURCE GUARD DEMONSTRATING RAII LEAK-FREE CLEANUP DURING UNWINDING
// =====================================================================================
class ManagedResourceGuard {
private:
    int resourceId_;
    int* heapBuffer_;

public:
    explicit ManagedResourceGuard(int id)
        : resourceId_(id), heapBuffer_(new int[100]()) {
        cout << "    [RAII Resource Ctor] Acquired heap buffer for Guard ID #" << resourceId_ << "\n";
    }

    ~ManagedResourceGuard() noexcept {
        delete[] heapBuffer_; // Guaranteed memory release during stack unwinding
        cout << "    [RAII Resource Dtor] Freed heap buffer for Guard ID #" << resourceId_ 
             << " (Clean Unwind)\n";
    }
};

// =====================================================================================
// 3. CLASS DEMONSTRATING `std::uncaught_exceptions()` SCOPE DETECTION
// =====================================================================================
class UnwindDetector {
private:
    string sessionName_;
    int initialUncaughtCount_;

public:
    explicit UnwindDetector(string name)
        : sessionName_(std::move(name)), initialUncaughtCount_(std::uncaught_exceptions()) {
        cout << "    [UnwindDetector Ctor] Session '" << sessionName_ << "' initialized.\n";
    }

    ~UnwindDetector() noexcept {
        // If current count > initial count, scope exit is caused by an exception throw
        if (std::uncaught_exceptions() > initialUncaughtCount_) {
            cout << "    [UnwindDetector Dtor] Scope exit for '" << sessionName_ 
                 << "' triggered by STACK UNWINDING!\n";
        } else {
            cout << "    [UnwindDetector Dtor] Scope exit for '" << sessionName_ 
                 << "' completed NORMALLY.\n";
        }
    }
};

// =====================================================================================
// 4. NESTED FUNCTION CALL STACK FOR MULTI-FRAME UNWINDING
// =====================================================================================

// Deepest Stack Frame (Level 3)
void stackFrameLevel3(int triggerValue) {
    cout << "\n  ---> ENTERING stackFrameLevel3 (Depth 3)...\n";
    StackScopeTracer tracer3A("Level3_Object_A", 3);
    ManagedResourceGuard resource3(300);
    StackScopeTracer tracer3B("Level3_Object_B", 3);

    if (triggerValue > 0) {
        cout << "    [THROW SITE] Throwing `std::runtime_error` from Depth 3!\n";
        throw std::runtime_error("Critical failure triggered in stackFrameLevel3");
    }

    cout << "  <--- EXITING stackFrameLevel3 normally.\n";
}

// Intermediate Stack Frame (Level 2)
void stackFrameLevel2(int triggerValue) {
    cout << "\n  ---> ENTERING stackFrameLevel2 (Depth 2)...\n";
    StackScopeTracer tracer2A("Level2_Object_A", 2);
    UnwindDetector detector2("Level2_Session");
    StackScopeTracer tracer2B("Level2_Object_B", 2);

    stackFrameLevel3(triggerValue);

    cout << "  <--- EXITING stackFrameLevel2 normally.\n";
}

// Top Stack Frame (Level 1)
void stackFrameLevel1(int triggerValue) {
    cout << "\n  ---> ENTERING stackFrameLevel1 (Depth 1)...\n";
    StackScopeTracer tracer1A("Level1_Object_A", 1);
    ManagedResourceGuard resource1(100);

    stackFrameLevel2(triggerValue);

    cout << "  <--- EXITING stackFrameLevel1 normally.\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Stack Unwinding analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MULTI-FRAME STACK UNWINDING DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. MULTI-FRAME STACK UNWINDING (LIFO DESTRUCTION) ================\n";

    try {
        cout << "  - Initiating nested function call chain: Level1 -> Level2 -> Level3...\n";
        stackFrameLevel1(userInputValue);
    } catch (const std::exception& ex) {
        cout << "\n  =========================================================================\n";
        cout << "  [CATCH HANDLER REACHED IN MAIN]:\n";
        cout << "    * Exception Message: \"" << ex.what() << "\"\n";
        cout << "  =========================================================================\n";
        cout << "  Notice how all stack frames (Level 3 -> Level 2 -> Level 1) were popped,\n"
             << "  and local RAII objects were destructed in exact REVERSE order of construction!\n";
    }

    // =====================================================================================
    // 2. NORMAL SCOPE EXIT VS STACK UNWINDING COMPARISON
    // =====================================================================================
    cout << "\n================ 2. NORMAL SCOPE EXIT VS STACK UNWINDING ================\n";

    cout << "  - Scenario A: Executing function chain WITHOUT throwing exceptions...\n";
    try {
        stackFrameLevel1(0); // Trigger value = 0 (No exception thrown)
    } catch (const std::exception& ex) {
        cout << "  [UNREACHABLE] " << ex.what() << "\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ STACK UNWINDING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Metric      | Stack Unwinding Rule              | Architectural Guarantee           |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Destruction Sequence  | LIFO (Last-In, First-Out)         | Exact reverse order of Ctor calls |\n"
         << "| Resource Management   | RAII Cleanups Executed            | Prevents memory & socket leaks    |\n"
         << "| Uncaught Inspector    | `std::uncaught_exceptions()` > 0  | Detects active exception unwinding|\n"
         << "| Destructor Exception  | MUST NOT THROW (`noexcept`)       | Throwing during unwind = terminate|\n"
         << "| Partial Construction  | Sub-objects destructed in reverse | Destructs ready members on Ctor fail|\n"
         << "| Scope Popping         | Frame-by-frame unwinding          | Traverses stack to matching catch |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}