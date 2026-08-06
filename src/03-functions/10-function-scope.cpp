/*
 * =====================================================================================
 * CONCEPT        : Function Scope in C++ (Parameters, Local Variables, Shadowing, Static)
 * DESCRIPTION    : Comprehensive implementation explaining variable visibility and lifetime
 *                  within function boundaries in C++:
 *                  1. Parameter & Local Scope   : Parameters and local variables exist only
 *                                                within the executing function frame.
 *                  2. Nested Block Scopes       : Sub-scopes created via `{}` inside functions
 *                                                restricting variable visibility even further.
 *                  3. Variable Shadowing        : Inner block variables hiding outer function-level
 *                                                variables with identical names.
 *                  4. Static Local Scope        : Function-scoped visibility paired with program-long
 *                                                lifetime (retains state across invocations).
 *                  5. Scope vs Lifetime         : RAII automatic destruction when control leaves
 *                                                function or block scope boundaries.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct scope evaluations and function frame allocations.
 * SPACE COMPLEXITY : Best Case: O(1) — Stack frame storage managed automatically upon scope entry/exit.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>

using namespace std;

// Helper RAII class to demonstrate exact scope entry and exit boundaries
class ScopeTracker {
private:
    string name_;

public:
    explicit ScopeTracker(string name) : name_(std::move(name)) {
        cout << "    [RAII Entry] Constructing '" << name_ << "' in scope.\n";
    }

    ~ScopeTracker() {
        cout << "    [RAII Exit ] Destroying '" << name_ << "' (Leaving scope).\n";
    }

    void doWork() const {
        cout << "    [RAII Action] '" << name_ << "' performing work inside its scope.\n";
    }
};

/**
 * @brief Section 1: Basic Function Parameter and Local Variable Scope
 * Demonstrates how function parameters and local variables are bound strictly to the function execution frame.
 */
void demonstrateLocalAndParameterScope(int inputParam) {
    cout << "\n================ 1. LOCAL PARAMETER & VARIABLE SCOPE ================" << endl;
    cout << "Inside function: 'inputParam' = " << inputParam << " at address: " << &inputParam << endl;

    // Local variable scoped strictly to this function
    int functionLocalVar = inputParam * 10;
    cout << "Inside function: 'functionLocalVar' = " << functionLocalVar 
         << " at address: " << &functionLocalVar << endl;

    // Changes here affect only this stack frame
    functionLocalVar += 5;
    cout << "Inside function: Modified 'functionLocalVar' = " << functionLocalVar << endl;
}

/**
 * @brief Section 2: Nested Block Scope & Variable Shadowing
 * Demonstrates variable masking/shadowing within sub-blocks inside a function.
 */
void demonstrateNestedBlockAndShadowing(int baseVal) {
    cout << "\n================ 2. NESTED BLOCK SCOPE & SHADOWING ================" << endl;

    int scopeVal = baseVal; // Function-level scope variable
    cout << "Outer Function Scope : 'scopeVal' = " << scopeVal 
         << " at address: " << &scopeVal << endl;

    {
        cout << "  --> Entering Inner Block 1..." << endl;
        ScopeTracker tracker1("Block1_Object");

        // Shadowing: Inner 'scopeVal' hides/shadows outer 'scopeVal'
        int scopeVal = 999; 
        cout << "  Inside Inner Block 1 : 'scopeVal' (Shadowed) = " << scopeVal 
             << " at address: " << &scopeVal << endl;
        tracker1.doWork();
    } // tracker1 and inner scopeVal are destroyed here upon exiting block 1

    cout << "  <-- Exited Inner Block 1." << endl;
    cout << "Outer Function Scope : 'scopeVal' remains = " << scopeVal 
         << " at address: " << &scopeVal << endl;

    {
        cout << "  --> Entering Inner Block 2..." << endl;
        ScopeTracker tracker2("Block2_Object");
        // Accessing the outer scopeVal without shadowing
        scopeVal += 50;
        cout << "  Inside Inner Block 2 : Modified outer 'scopeVal' = " << scopeVal << endl;
    } // tracker2 destroyed here
}

/**
 * @brief Section 3: Static Local Scope vs Automatic Local Scope
 * Demonstrates function scope visibility combined with persistent program-lifetime storage.
 */
void demonstrateStaticLocalScope() {
    // Automatic local variable: re-initialized every time function is invoked
    int autoCounter = 0;

    // Static local variable: initialized ONCE, persists across function calls, but strictly scoped to this function
    static int staticCounter = 0;

    ++autoCounter;
    ++staticCounter;

    cout << "  [Invocation] autoCounter = " << autoCounter 
         << " | staticCounter = " << staticCounter 
         << " (Address: " << &staticCounter << ")" << endl;
}

int main() {
    int userVal = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a test integer value (e.g., 15): " << flush;
    if (!(cin >> userVal)) {
        cout << "Invalid input provided. Defaulting to 15." << endl;
        userVal = 15;
    }

    // 1. LOCAL PARAMETER & VARIABLE SCOPE
    demonstrateLocalAndParameterScope(userVal);

    // 2. NESTED BLOCK SCOPE & VARIABLE SHADOWING
    demonstrateNestedBlockAndShadowing(userVal);

    // 3. STATIC LOCAL VARIABLE SCOPE
    cout << "\n================ 3. STATIC LOCAL SCOPE (PERSISTENT STATE) ================" << endl;
    cout << "Calling demonstrateStaticLocalScope() 3 times consecutively:" << endl;
    demonstrateStaticLocalScope();
    demonstrateStaticLocalScope();
    demonstrateStaticLocalScope();

    cout << "\n================ FUNCTION SCOPE SUMMARY ================" << endl;
    cout << "1. Parameter Scope : Function parameters exist only within the function frame." << endl;
    cout << "2. Block Scope     : Enclosing `{}` braces restrict variable visibility and lifetime further." << endl;
    cout << "3. Shadowing       : Declaring a variable with the same name in an inner scope masks the outer variable." << endl;
    cout << "4. Static Local    : Visible ONLY inside the function, but persists in memory for the entire program lifetime." << endl;
    cout << "5. RAII Automatic  : Stack-allocated objects are automatically destroyed when execution leaves their scope." << endl;

    return 0;
}