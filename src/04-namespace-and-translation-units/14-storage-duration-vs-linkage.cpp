/*
 * =====================================================================================
 * CONCEPT        : Storage Duration vs Linkage in C++
 * DESCRIPTION    : Comprehensive program explaining the orthogonal concepts of:
 *                  1. Storage Duration (Lifetime) : WHEN memory is allocated & destroyed.
 *                     - Automatic : Stack-allocated; lifetime tied to block scope.
 *                     - Static    : Program lifetime; allocated at launch, freed at exit.
 *                     - Thread    : Lifetime tied to thread execution (`thread_local`).
 *                     - Dynamic   : Heap-allocated; manually/RAII managed (`new`/`delete`).
 *
 *                  2. Linkage (Visibility)       : WHERE a symbol name can be referenced.
 *                     - No Linkage       : Visible ONLY within current block/function scope.
 *                     - Internal Linkage  : Visible ONLY within current Translation Unit (TU).
 *                     - External Linkage  : Visible across MULTIPLE Translation Units.
 *
 *                  3. Key Concept Matrix (The "static" Keyword Ambiguity):
 *                     - At file scope  : `static` alters LINKAGE (External -> Internal).
 *                     - At block scope : `static` alters DURATION (Automatic -> Static).
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime; linkage and storage duration are resolved by compiler/linker.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <thread>
#include <memory>

using namespace std;

// =====================================================================================
// 1. STATIC STORAGE DURATION + EXTERNAL LINKAGE
// Memory created at program startup, destroyed at shutdown.
// Symbol visible across all Translation Units in the program.
// =====================================================================================
int g_externalGlobal = 100; 

// =====================================================================================
// 2. STATIC STORAGE DURATION + INTERNAL LINKAGE
// Memory created at program startup, destroyed at shutdown.
// Symbol visible ONLY inside this single Translation Unit (.cpp file).
// =====================================================================================
static int g_internalStaticGlobal = 200;

namespace {
    // Anonymous namespace members also have Internal Linkage
    int g_anonymousNamespaceVar = 300;
}

// =====================================================================================
// 3. STORAGE DURATION VS LINKAGE DEMONSTRATION FUNCTIONS
// =====================================================================================

// A. STATIC STORAGE DURATION + NO LINKAGE
// Local static variable: Retains state across function calls, but invisible outside this function!
void demonstrateStaticLocal() {
    static int callCounter = 0; // Lifetime = Static (Program), Linkage = None
    callCounter++;
    cout << "  - [Static Local] 'callCounter' = " << callCounter 
         << " | Address: " << static_cast<const void*>(&callCounter) << endl;
}

// B. AUTOMATIC STORAGE DURATION + NO LINKAGE
// Stack variable: Allocated on function entry, popped on exit. Invisible outside block.
void demonstrateAutomaticLocal() {
    int autoVar = 10; // Lifetime = Automatic (Stack), Linkage = None
    autoVar++;
    cout << "  - [Automatic Local] 'autoVar' = " << autoVar 
         << " | Address: " << static_cast<const void*>(&autoVar) << endl;
}

// C. THREAD STORAGE DURATION + NO LINKAGE
// Thread local variable: Unique instance per thread, lives as long as the thread executes.
void demonstrateThreadLocal(int threadId) {
    thread_local int threadState = 1000; // Lifetime = Thread, Linkage = None
    threadState += threadId * 50;
    cout << "  - [Thread Local | Thread " << threadId << "] 'threadState' = " << threadState 
         << " | Address: " << static_cast<const void*>(&threadState) << endl;
}

// D. DYNAMIC STORAGE DURATION + NO LINKAGE
// Heap object: Lifetime managed explicitly or via RAII (smart pointers). Pointer has no linkage.
void demonstrateDynamicStorage() {
    // Smart pointer (auto local variable) managing dynamic heap memory
    auto dynamicInt = make_unique<int>(500); // Lifetime = Dynamic (Heap), Linkage = None
    cout << "  - [Dynamic Heap] Pointer target value = " << *dynamicInt 
         << " | Heap Address: " << static_cast<const void*>(dynamicInt.get()) << endl;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int iterationCount = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter number of function call iterations (e.g., 3): " << flush;
    if (!(cin >> iterationCount) || iterationCount <= 0) {
        cout << "Invalid input. Defaulting to 3 iterations." << endl;
        iterationCount = 3;
    }

    cout << "\n================ 1. STATIC VS AUTOMATIC LOCAL STORAGE ================" << endl;
    cout << "Executing " << iterationCount << " function invocations:\n";
    for (int i = 1; i <= iterationCount; ++i) {
        cout << "Iteration " << i << ":\n";
        demonstrateAutomaticLocal(); // Value resets every call (Automatic)
        demonstrateStaticLocal();    // Value persists across calls (Static)
    }

    cout << "\n================ 2. THREAD STORAGE DURATION (`thread_local`) ================" << endl;
    cout << "Spawning 2 independent threads to demonstrate per-thread state isolation:\n";
    thread t1(demonstrateThreadLocal, 1);
    thread t2(demonstrateThreadLocal, 2);
    t1.join();
    t2.join();

    cout << "\n================ 3. DYNAMIC STORAGE DURATION (HEAP / RAII) ================" << endl;
    demonstrateDynamicStorage();

    cout << "\n================ 4. GLOBAL SCOPE LINKAGE DEMONSTRATION ================" << endl;
    cout << "  - g_externalGlobal         (Static Duration, External Linkage) = " << g_externalGlobal << endl;
    cout << "  - g_internalStaticGlobal   (Static Duration, Internal Linkage) = " << g_internalStaticGlobal << endl;
    cout << "  - g_anonymousNamespaceVar  (Static Duration, Internal Linkage) = " << g_anonymousNamespaceVar << endl;

    cout << "\n================ MATRIX SUMMARY: STORAGE DURATION VS LINKAGE ================" << endl;
    cout << "+-----------------------+-------------------+------------------+-----------------------------------+\n"
         << "| Variable Declaration  | Storage Duration  | Linkage Type     | Lifetime & Visibility Scope       |\n"
         << "+-----------------------+-------------------+------------------+-----------------------------------+\n"
         << "| Global `int x`        | Static            | External         | Program lifetime; All TUs         |\n"
         << "| Global `static int x` | Static            | Internal         | Program lifetime; Current TU only |\n"
         << "| Local `int x`         | Automatic         | None             | Block lifetime; Block scope only  |\n"
         << "| Local `static int x`  | Static            | None             | Program lifetime; Block scope only|\n"
         << "| `thread_local int x`  | Thread            | None (if local)  | Thread lifetime; Block scope only |\n"
         << "| `new int / unique_ptr`| Dynamic           | None             | Manual/RAII; Managed via pointer  |\n"
         << "+-----------------------+-------------------+------------------+-----------------------------------+\n";

    return 0;
}