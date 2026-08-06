/*
 * =====================================================================================
 * CONCEPT        : Storage Duration in C++ (Automatic, Static, Dynamic, Thread-Local)
 * DESCRIPTION    : Comprehensive implementation explaining the 4 standard C++ storage durations:
 *                  1. Automatic Storage Duration   : Local stack variables created at block 
 *                                                    entry and destroyed upon scope exit.
 *                  2. Static Storage Duration      : Global or static local variables initialized 
 *                                                    once and persisting until program termination.
 *                  3. Dynamic Storage Duration     : Heap memory allocated at runtime (managed via 
 *                                                    RAII smart pointers like std::unique_ptr).
 *                  4. Thread-Local Storage Duration: Variables declared with 'thread_local' whose 
 *                                                    lifetime is bound to thread execution lifetime.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear time to dynamically allocate, populate, 
 *                    and process N elements across storage durations.
 * SPACE COMPLEXITY : Best Case: O(N) — Dynamic heap storage for N elements plus O(1) stack/static memory.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <memory>
#include <thread>

using namespace std;

// 1. STATIC STORAGE DURATION (GLOBAL SCOPE)
// Allocated when the program starts and deallocated when the program terminates.
static int globalStaticCounter = 0;

// 2. THREAD-LOCAL STORAGE DURATION
// Each thread gets its own distinct instance initialized independently.
thread_local int threadLocalValue = 100;

/**
 * @brief Demonstrates Automatic vs Static Local Storage Duration.
 * @param dynamicIncrement Step value provided dynamically by user input.
 */
void demonstrateLocalDurations(int dynamicIncrement) {
    // AUTOMATIC STORAGE DURATION (Stack allocation, re-created on each function invocation)
    int automaticVar = 0;

    // STATIC LOCAL STORAGE DURATION (Persists state across invocations, initialized once)
    static int staticLocalVar = 0;

    automaticVar += dynamicIncrement;
    staticLocalVar += dynamicIncrement;
    globalStaticCounter += dynamicIncrement;

    cout << "  |- Automatic Local Variable (re-initialized each call) : " << automaticVar << endl;
    cout << "  |- Static Local Variable    (persists across calls)    : " << staticLocalVar << endl;
    cout << "  |- Global Static Counter    (program lifetime duration): " << globalStaticCounter << endl;
}

/**
 * @brief Demonstrates Thread-Local Storage Duration across separate worker threads.
 * @param threadId Unique identifier for the thread.
 * @param offset Value to modify thread-local storage.
 */
void demonstrateThreadLocalDuration(int threadId, int offset) {
    threadLocalValue += offset;
    cout << "  |- [Thread " << threadId << "] thread_local Value (Isolated Instance): " 
         << threadLocalValue << endl;
}

int main() {
    int userIncrement = 0;
    size_t heapElementCount = 0;

    // 3. DYNAMIC INPUT COLLECTION WITH EXPLICIT STREAM FLUSHING
    cout << "Enter an increment step for local counter demonstrations: " << flush;
    if (!(cin >> userIncrement)) {
        cout << "Invalid increment input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter number of elements for dynamic heap allocation: " << flush;
    if (!(cin >> heapElementCount) || heapElementCount == 0) {
        cout << "Invalid count provided. Program terminated." << endl;
        return 0;
    }

    // 4. AUTOMATIC VS STATIC STORAGE DURATION DEMONSTRATION
    cout << "\n================ 1. AUTOMATIC VS STATIC STORAGE DURATION ================" << endl;
    cout << "First Invocation of demonstrateLocalDurations():" << endl;
    demonstrateLocalDurations(userIncrement);

    cout << "\nSecond Invocation of demonstrateLocalDurations():" << endl;
    demonstrateLocalDurations(userIncrement);

    // 5. DYNAMIC STORAGE DURATION DEMONSTRATION (HEAP MEMORY WITH RAII)
    cout << "\n================ 2. DYNAMIC STORAGE DURATION (HEAP) ================" << endl;
    {
        // Heap memory allocated dynamically via std::make_unique (RAII guarantees automatic deallocation)
        auto dynamicVectorPtr = make_unique<vector<int>>(heapElementCount);

        cout << "Allocated dynamic heap vector holding " << heapElementCount << " elements." << endl;
        for (size_t i = 0; i < heapElementCount; ++i) {
            (*dynamicVectorPtr)[i] = static_cast<int>(i + 1) * userIncrement;
        }

        cout << "Dynamic Vector Elements: ";
        for (const auto& elem : *dynamicVectorPtr) {
            cout << elem << " ";
        }
        cout << endl;
        cout << "Leaving dynamic block scope... Heap memory will be automatically released via RAII." << endl;
    } // Dynamic heap memory allocated for dynamicVectorPtr is freed automatically here

    // 6. THREAD-LOCAL STORAGE DURATION DEMONSTRATION
    cout << "\n================ 3. THREAD-LOCAL STORAGE DURATION ================" << endl;
    cout << "Main Thread Initial threadLocalValue: " << threadLocalValue << endl;

    // Spawn two distinct threads to modify thread_local storage independently
    thread t1(demonstrateThreadLocalDuration, 1, 10);
    thread t2(demonstrateThreadLocalDuration, 2, 50);

    // Join threads to main thread execution
    t1.join();
    t2.join();

    cout << "Main Thread Final threadLocalValue (Unmodified by child threads): " << threadLocalValue << endl;

    return 0;
}