/*
 * =====================================================================================
 * CONCEPT        : Storage Class Specifiers in C++
 * DESCRIPTION    : Comprehensive implementation showcasing all C++ storage class specifiers:
 *                  1. auto         : Historical storage class specifier for local variables 
 *                                     (repurposed in C++11 for automatic type deduction).
 *                  2. register     : Historical hint for register storage (deprecated C++11, 
 *                                     removed C++17; documented via comments for context).
 *                  3. static       : Grants static storage duration (persists across calls), 
 *                                     internal linkage (file scope), or shared class members.
 *                  4. extern       : Specifies external linkage for global variables or functions 
 *                                     defined across translation units.
 *                  5. mutable      : Permits modification of class data members even within 
 *                                     const member functions.
 *                  6. thread_local : Binds variable lifetime and state independently to each 
 *                                     executing thread.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time evaluation for storage specifier operations, 
 *                    class member mutations, and isolated thread execution.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack/data segment footprint for scalar inputs and threads.
 * =====================================================================================
 */

#include <iostream>
#include <thread>

using namespace std;

// 1. EXTERN STORAGE CLASS SPECIFIER (EXTERNAL LINKAGE DECLARATION)
// Declares variable that resides in global scope / translation unit
extern int globalExternValue;

// Global definition of extern variable
int globalExternValue = 1000;

// 2. CLASS SCOPE: STATIC AND MUTABLE SPECIFIERS
class CounterService {
private:
    // MUTABLE SPECIFIER: Allows modification even within 'const' member functions
    mutable size_t accessCounter{0};

public:
    // STATIC SPECIFIER: Single shared member variable across all class instances
    static inline size_t totalInstancesCreated{0};

    explicit CounterService() {
        ++totalInstancesCreated;
    }

    // Const member function modifying 'mutable' member variable
    void executeQuery(int payload) const {
        ++accessCounter; // Permitted because accessCounter is 'mutable'
        cout << "  |- Query Executed with Payload: " << payload 
             << " | Const Object Access Count: " << accessCounter << endl;
    }

    size_t getAccessCount() const noexcept {
        return accessCounter;
    }
};

// 3. THREAD_LOCAL STORAGE CLASS SPECIFIER
// Unique, isolated instance initialized independently for each executing thread
thread_local int threadSpecificData = 50;

/**
 * @brief Demonstrates static local storage class specifier behavior.
 * @param delta Dynamic user-defined increment step.
 */
void demonstrateStaticLocal(int delta) {
    // STATIC LOCAL: Initialized exactly once, retains value across function invocations
    static int persistentLocalCounter = 0;
    persistentLocalCounter += delta;

    cout << "  |- Persistent Static Local Counter Value: " << persistentLocalCounter << endl;
}

/**
 * @brief Worker function demonstrating thread_local storage specifier.
 * @param threadId Worker thread identifier.
 * @param inputVal Dynamic user step value.
 */
void demonstrateThreadLocal(int threadId, int inputVal) {
    threadSpecificData += inputVal * threadId;
    cout << "  |- [Thread " << threadId << "] thread_local Value: " << threadSpecificData << endl;
}

int main() {
    int userStep = 0;
    int dynamicPayload = 0;

    // Dynamic input collection with explicit stream flushing
    cout << "Enter dynamic increment step for static local counter: " << flush;
    if (!(cin >> userStep)) {
        cout << "Invalid step input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter dynamic payload value for mutable class test: " << flush;
    if (!(cin >> dynamicPayload)) {
        cout << "Invalid payload input. Program terminated." << endl;
        return 0;
    }

    // 4. AUTO & REGISTER SPECIFIERS (HISTORICAL CONTEXT VS MODERN USE)
    cout << "\n================ 1. AUTO & REGISTER SPECIFIERS ================" << endl;
    // Modern 'auto' performs compile-time type deduction (repurposed from C++11 automatic storage class)
    auto automaticDucedVar = userStep * 2; 
    cout << "Modern 'auto' Type-Duced Variable Value     : " << automaticDucedVar << endl;
    cout << "Note: 'register' specifier was deprecated in C++11 and unused/removed in C++17." << endl;

    // 5. EXTERN SPECIFIER DEMONSTRATION
    cout << "\n================ 2. EXTERN SPECIFIER ================" << endl;
    cout << "Accessed 'extern' Global Value (External Linkage): " << globalExternValue << endl;
    globalExternValue += userStep;
    cout << "Updated 'extern' Global Value                  : " << globalExternValue << endl;

    // 6. STATIC LOCAL SPECIFIER DEMONSTRATION
    cout << "\n================ 3. STATIC SPECIFIER (LOCAL SCOPE) ================" << endl;
    cout << "First Invocation of demonstrateStaticLocal():" << endl;
    demonstrateStaticLocal(userStep);

    cout << "Second Invocation of demonstrateStaticLocal():" << endl;
    demonstrateStaticLocal(userStep);

    // 7. MUTABLE & STATIC CLASS SPECIFIERS DEMONSTRATION
    cout << "\n================ 4. MUTABLE & STATIC SPECIFIERS (CLASS SCOPE) ================" << endl;
    const CounterService serviceA; // Const class instance
    const CounterService serviceB;

    cout << "Total CounterService Instances (Static Member): " << CounterService::totalInstancesCreated << endl;
    
    // Invoking const methods on a const object that mutates 'mutable' state
    serviceA.executeQuery(dynamicPayload);
    serviceA.executeQuery(dynamicPayload * 2);

    // 8. THREAD_LOCAL SPECIFIER DEMONSTRATION
    cout << "\n================ 5. THREAD_LOCAL SPECIFIER ================" << endl;
    cout << "Main Thread Initial thread_local Value : " << threadSpecificData << endl;

    thread t1(demonstrateThreadLocal, 1, userStep);
    thread t2(demonstrateThreadLocal, 2, userStep);

    t1.join();
    t2.join();

    cout << "Main Thread Final thread_local Value   : " << threadSpecificData 
         << " (Isolated from worker thread modifications)" << endl;

    return 0;
}