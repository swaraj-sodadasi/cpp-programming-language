/*
 * =====================================================================================
 * CONCEPT        : Control Flow and Scope in C++
 * DESCRIPTION    : Comprehensive implementation explaining variable scope, lifetime, 
 *                  and control flow mechanics in modern C++:
 *                  1. Global vs Local Scope    : Lifetime and visibility of global variables 
 *                                                vs local block-scoped variables.
 *                  2. Block & Nested Scopes    : Lifetimes bound to block braces {}, variable 
 *                                                shadowing, and explicit scope resolution (::).
 *                  3. C++17 Init-Statements    : Scoping variables strictly inside if/switch 
 *                                                condition statements.
 *                  4. Static Local Scope       : Static variable lifetime persisting across function 
 *                                                invocations within function scope.
 *                  5. RAII & Scope Unwinding   : Destructor execution order when control flow 
 *                                                exits scope via return, break, or continue.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Proportional to the number of loop iterations N.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation for local variables.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>

using namespace std;

// 1. GLOBAL SCOPE VARIABLE
int globalCounter = 100;

// 2. NAMESPACE SCOPE
namespace AppConfig {
    constexpr int MAX_THRESHOLD = 50;
}

// RAII HELPER CLASS TO VISUALIZE SCOPE LIFETIME & DESTRUCTION ORDER
class ScopeTracker {
private:
    string scopeName_;

public:
    explicit ScopeTracker(string name) : scopeName_(std::move(name)) {
        cout << "  [RAII Construct] Entered scope : " << scopeName_ << endl;
    }

    ~ScopeTracker() {
        cout << "  [RAII Destruct ] Exited scope  : " << scopeName_ << endl;
    }
};

/**
 * @brief Demonstrates static local variable scope persistence.
 */
void demonstrateStaticScope() {
    // Static local variable: initialized once, persists lifetime across function calls
    static int persistentCallCount = 0;
    ++persistentCallCount;
    cout << "  [Static Scope] Function invocation count: " << persistentCallCount << endl;
}

/**
 * @brief Demonstrates block scoping, variable shadowing, and C++17 init-statements.
 * @param inputVal User provided dynamic integer input.
 */
void demonstrateBlockAndShadowing(int inputVal) {
    cout << "\n================ 1. BLOCK SCOPE & VARIABLE SHADOWING ================" << endl;

    int scopeVal = 10; // Outer local scope variable
    cout << "Outer Scope 'scopeVal' initial value : " << scopeVal << endl;

    {
        ScopeTracker innerBlockTracker("Inner Block Alpha");
        
        // Variable Shadowing: Inner 'scopeVal' hides outer 'scopeVal'
        int scopeVal = 99; 
        cout << "  Inner Scope 'scopeVal' (Shadowing) : " << scopeVal << endl;
        cout << "  Global 'globalCounter' (via ::)    : " << ::globalCounter << endl;
        cout << "  Namespace Config 'MAX_THRESHOLD'   : " << AppConfig::MAX_THRESHOLD << endl;
    } // Inner scope ends here; inner scopeVal and innerBlockTracker are destroyed

    cout << "Outer Scope 'scopeVal' after block   : " << scopeVal << " (Unchanged)" << endl;

    cout << "\n================ 2. C++17 CONTROL FLOW INIT-STATEMENTS ================" << endl;
    // C++17 if-statement with initializer: 'computedVal' is scoped ONLY to the if/else branches
    if (int computedVal = inputVal * 2; computedVal > AppConfig::MAX_THRESHOLD) {
        cout << "  [if branch] 'computedVal' (" << computedVal 
             << ") exceeded threshold (" << AppConfig::MAX_THRESHOLD << ")." << endl;
    } else {
        cout << "  [else branch] 'computedVal' (" << computedVal 
             << ") is within threshold (" << AppConfig::MAX_THRESHOLD << ")." << endl;
    }
    // 'computedVal' is no longer accessible outside the if-else control structure
}

/**
 * @brief Demonstrates scope unwinding and RAII cleanup during loop control flow (break/continue).
 * @param iterations Maximum iterations to execute.
 * @param breakPoint Iteration threshold to trigger break.
 */
void demonstrateLoopControlAndUnwinding(int iterations, int breakPoint) {
    cout << "\n================ 3. CONTROL FLOW & SCOPE UNWINDING ================" << endl;
    cout << "Executing loop with RAII trackers (Break trigger at iteration " << breakPoint << "):" << endl;

    for (int i = 1; i <= iterations; ++i) {
        cout << "\n--- Loop Iteration " << i << " Start ---" << endl;
        ScopeTracker iterationTracker("Loop Iteration " + to_string(i));

        if (i == breakPoint) {
            cout << "  [Break Triggered] Exiting loop early at iteration " << i << "..." << endl;
            // Destructor for iterationTracker will automatically be invoked BEFORE break exits loop
            break; 
        }

        cout << "  Processing iteration " << i << " body..." << endl;
    } // Local iteration objects unwound cleanly at loop boundary or break exit
}

int main() {
    int userInputVal = 0;
    int loopCount = 0;
    int breakAt = 0;

    // Dynamic input collection with explicit stream flushing
    cout << "Enter an integer input for threshold evaluation (e.g., 30): " << flush;
    if (!(cin >> userInputVal)) {
        cout << "Invalid input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter total loop iteration count (1 - 5): " << flush;
    if (!(cin >> loopCount) || loopCount <= 0 || loopCount > 10) {
        cout << "Invalid loop count. Setting default to 4." << endl;
        loopCount = 4;
    }

    cout << "Enter iteration step to trigger break (1 - " << loopCount << "): " << flush;
    if (!(cin >> breakAt) || breakAt <= 0) {
        cout << "Invalid break point. Setting default to 2." << endl;
        breakAt = 2;
    }

    // 1. Demonstrate Block Scope, Shadowing, and C++17 Init-Statements
    demonstrateBlockAndShadowing(userInputVal);

    // 2. Demonstrate Static Local Scope Persistence
    cout << "\n================ 4. STATIC LOCAL SCOPE PERSISTENCE ================" << endl;
    demonstrateStaticScope();
    demonstrateStaticScope();
    demonstrateStaticScope();

    // 3. Demonstrate RAII Scope Unwinding during Loop Control Flow
    demonstrateLoopControlAndUnwinding(loopCount, breakAt);

    cout << "\n================ SCOPE & CONTROL FLOW SUMMARY ================" << endl;
    cout << "1. Global Scope   : Lifetime = Entire program execution; accessible globally." << endl;
    cout << "2. Block Scope    : Lifetime = Bound between enclosing braces {}; automatic destruction." << endl;
    cout << "3. Static Local   : Scope = Local to function; Lifetime = Entire program execution." << endl;
    cout << "4. Control Flow   : Exiting a scope via break/return/exception unwinds stack and calls destructors." << endl;

    return 0;
}