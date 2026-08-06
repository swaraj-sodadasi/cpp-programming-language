/*
 * =====================================================================================
 * CONCEPT        : Storage Classes in C++ Functions
 * DESCRIPTION    : Comprehensive implementation explaining C++ storage class specifiers 
 *                  and storage duration modes:
 *                  1. Automatic Storage (`auto` / Local Variables) :
 *                     - Default for function-local variables; allocated on stack upon entering scope
 *                       and destroyed upon exiting scope.
 *                  2. Static Storage (`static`) :
 *                     - Persistent lifetime throughout program execution; initialized exactly once;
 *                       scoped locally inside the function.
 *                  3. External Storage (`extern`) :
 *                     - Declares a variable or function with external linkage (defined globally 
 *                       or in another translation unit).
 *                  4. Thread Local Storage (`thread_local`) :
 *                     - Allocated per thread; created when thread starts and destroyed when thread ends.
 *                  5. Register Storage (`register`) :
 *                     - Historical CPU register hint; deprecated in C++11 and unused specifier in C++17.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time memory access and scalar increments.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack/static/thread-local memory allocation.
 * =====================================================================================
 */

#include <iostream>
#include <thread>

using namespace std;

// =====================================================================================
// 1. EXTERNAL STORAGE CLASS (`extern`)
// Defines a global variable with external linkage accessible across functions/files.
// =====================================================================================
int g_systemExecutionCounter = 100; // Global definition

void demonstrateExternalStorage() {
    // Declarative reference to external global variable
    extern int g_systemExecutionCounter; 
    cout << "  [extern] Accessed global execution counter = " << g_systemExecutionCounter 
         << " at address: " << &g_systemExecutionCounter << endl;
    g_systemExecutionCounter += 10;
}

// =====================================================================================
// 2. STATIC STORAGE CLASS (`static`)
// Function-local static variables preserve state across multiple function invocations.
// =====================================================================================
void demonstrateStaticStorage() {
    // Automatic local variable (re-created on stack every call)
    int autoVar = 0;

    // Static local variable (initialized ONCE, stored in data segment, persists for program lifetime)
    static int staticVar = 0;

    ++autoVar;
    ++staticVar;

    cout << "  [static vs auto] autoVar (Stack): " << autoVar 
         << " | staticVar (Persistent Data Seg): " << staticVar 
         << " (Address: " << &staticVar << ")" << endl;
}

// =====================================================================================
// 3. THREAD LOCAL STORAGE CLASS (`thread_local`)
// Each executing thread maintains its own independent instance of the variable.
// =====================================================================================
thread_local int t_threadSpecificCounter = 0;

void threadWorkerTask(int threadId) {
    // Modify thread-specific instance
    t_threadSpecificCounter += threadId * 5;

    cout << "  [thread_local] Thread #" << threadId 
         << " | t_threadSpecificCounter = " << t_threadSpecificCounter 
         << " (Thread-unique Address: " << &t_threadSpecificCounter << ")" << endl;
}

void demonstrateThreadLocalStorage() {
    cout << "\n================ 3. THREAD LOCAL STORAGE (`thread_local`) ================" << endl;
    cout << "Spawning 2 separate threads to verify independent memory instances:" << endl;

    // Spawn thread 1
    std::thread t1(threadWorkerTask, 1);
    // Spawn thread 2
    std::thread t2(threadWorkerTask, 2);

    // Wait for threads to complete execution
    t1.join();
    t2.join();
}

// =====================================================================================
// 4. AUTOMATIC STORAGE CLASS (`auto`)
// Standard function-local variables with automatic lifetime tied to block scope `{}`.
// =====================================================================================
void demonstrateAutomaticStorage(int inputValue) {
    cout << "\n================ 1. AUTOMATIC STORAGE CLASS (`auto` / Local Scope) ================" << endl;
    
    // Automatic storage duration (allocated on function call stack)
    int localCalculation = inputValue * 2;
    cout << "  [auto] 'localCalculation' allocated on stack frame with value = " 
         << localCalculation << endl;

} // 'localCalculation' is automatically destroyed here upon returning

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a starting test integer (e.g., 5): " << flush;
    if (!(cin >> userInputValue)) {
        cout << "Invalid input. Defaulting value to 5." << endl;
        userInputValue = 5;
    }

    // 1. AUTOMATIC STORAGE DEMONSTRATION
    demonstrateAutomaticStorage(userInputValue);

    // 2. STATIC STORAGE DEMONSTRATION
    cout << "\n================ 2. STATIC STORAGE CLASS (`static`) ================" << endl;
    cout << "Invoking demonstrateStaticStorage() 3 times to observe persistent state:" << endl;
    demonstrateStaticStorage();
    demonstrateStaticStorage();
    demonstrateStaticStorage();

    // 3. THREAD LOCAL STORAGE DEMONSTRATION
    demonstrateThreadLocalStorage();

    // 4. EXTERNAL STORAGE DEMONSTRATION
    cout << "\n================ 4. EXTERNAL STORAGE CLASS (`extern`) ================" << endl;
    cout << "Before function call : g_systemExecutionCounter = " << g_systemExecutionCounter << endl;
    demonstrateExternalStorage();
    cout << "After function call  : g_systemExecutionCounter = " << g_systemExecutionCounter << endl;

    // 5. REGISTER STORAGE EXPLANATION (C++17 SPECIFICATION NOTE)
    cout << "\n================ 5. REGISTER STORAGE CLASS (`register` Note) ================" << endl;
    cout << "  - Historical Role : Suggested compiler store variable directly in CPU register." << endl;
    cout << "  - C++11 Status    : Deprecated specifier." << endl;
    cout << "  - C++17 Standard  : Unused keyword specifier; modern compilers auto-optimize register usage." << endl;

    cout << "\n================ STORAGE CLASSES SUMMARY ==================" << endl;
    cout << "1. Automatic (`auto`)    : Stack duration; exists only within block scope `{}`." << endl;
    cout << "2. Static (`static`)      : Program duration; retains local state across calls." << endl;
    cout << "3. External (`extern`)    : Program duration; provides global linkage across translation units." << endl;
    cout << "4. Thread (`thread_local`): Thread duration; unique independent instance per thread." << endl;

    return 0;
}