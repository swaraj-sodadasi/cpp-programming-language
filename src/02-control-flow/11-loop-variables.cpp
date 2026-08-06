/*
 * =====================================================================================
 * CONCEPT        : Loop Variables in C++ (Scope, Lifetime, Type Selection, Range Binding)
 * DESCRIPTION    : Comprehensive implementation explaining C++ loop variable mechanics:
 *                  1. Scope & Lifetime       : Local loop variables (for-loop scope) vs 
 *                                             outer-scoped variables (while/do-while persistence).
 *                  2. Range-Based Binding    : Binding loop variables by value (copies), 
 *                                             reference (& for in-place mutation), and 
 *                                             const reference (const & for read-only efficiency).
 *                  3. Type Selection & Safety: Signed vs unsigned (size_t) counters, avoiding 
 *                                             overflow/underflow bugs during decrements.
 *                  4. Dynamic Mutation       : Controlling iteration step sizes and modifying 
 *                                             counter states safely inside loop bodies.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear traversal over dynamic container elements.
 * SPACE COMPLEXITY : Best Case: O(N) — Stack/heap memory allocation proportional to dynamic vector size.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <cstddef>

using namespace std;

/**
 * @brief Demonstrates range-based for loop variable binding semantics.
 * @param numbers Target vector container.
 */
void demonstrateRangeLoopBinding(const vector<int>& numbers) {
    cout << "\n================ 2. RANGE-BASED FOR LOOP BINDING ================" << endl;

    // A. Binding by Value (Creates a local copy of each element per iteration)
    cout << "A. By Value (auto val)         : ";
    for (auto val : numbers) {
        val *= 2; // Modifies local copy ONLY; original vector remains unchanged
        cout << val << " ";
    }
    cout << "(Copies updated locally)" << endl;

    // B. Binding by Const Reference (Read-only efficiency, no copy overhead)
    cout << "B. By Const Ref (const auto& v): ";
    for (const auto& val : numbers) {
        cout << val << " "; // Fast read-only access
    }
    cout << "(Original values unchanged)" << endl;

    // C. Binding by Lvalue Reference (In-place mutation of container elements)
    vector<int> mutableCopy = numbers;
    cout << "C. By Reference (auto& val)    : ";
    for (auto& val : mutableCopy) {
        val += 5; // Modifies element directly in memory
        cout << val << " ";
    }
    cout << "(Elements updated in-place)" << endl;
}

int main() {
    int maxIterations = 0;
    int stepIncrement = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter dynamic loop iteration limit (1 - 10): " << flush;
    if (!(cin >> maxIterations) || maxIterations <= 0 || maxIterations > 10) {
        cout << "Invalid iteration limit. Defaulting to 5." << endl;
        maxIterations = 5;
    }

    cout << "Enter step increment value (1 - 3): " << flush;
    if (!(cin >> stepIncrement) || stepIncrement <= 0) {
        cout << "Invalid step increment. Defaulting to 1." << endl;
        stepIncrement = 1;
    }

    // 2. SCOPE AND LIFETIME OF LOOP VARIABLES
    cout << "\n================ 1. SCOPE & LIFETIME OF LOOP VARIABLES ================" << endl;
    
    // Pattern 1: Variable declared within for-loop initializer (Local Scope)
    cout << "A. For-Loop Local Scope Variable ('i'):" << endl;
    cout << "   Progress: ";
    for (int i = 1; i <= maxIterations; i += stepIncrement) {
        cout << "[" << i << "] ";
    }
    cout << endl;
    // Note: 'i' goes out of scope here and cannot be accessed outside the for loop block.

    // Pattern 2: Variable declared outside the loop (Outer Scope / Persistent Lifetime)
    cout << "B. Outer Scope Loop Variable ('outerCounter'):" << endl;
    int outerCounter = 1;
    cout << "   Progress: ";
    while (outerCounter <= maxIterations) {
        cout << "[" << outerCounter << "] ";
        outerCounter += stepIncrement;
    }
    cout << "\n   Value of 'outerCounter' AFTER loop termination: " << outerCounter 
         << " (Persists beyond loop scope)" << endl;

    // 3. RANGE-BASED LOOP VARIABLE BINDINGS
    vector<int> dataset;
    dataset.reserve(static_cast<size_t>(maxIterations));
    for (int i = 1; i <= maxIterations; ++i) {
        dataset.push_back(i * 10);
    }
    demonstrateRangeLoopBinding(dataset);

    // 4. LOOP VARIABLE TYPES: SIGNED VS UNSIGNED (size_t)
    cout << "\n================ 3. LOOP VARIABLE TYPE SELECTION ================" << endl;
    
    // Standard container indexing using unsigned size_t
    cout << "A. Unsigned Container Indexing (size_t idx):" << endl;
    cout << "   Vector contents: ";
    for (size_t idx = 0; idx < dataset.size(); ++idx) {
        cout << "vec[" << idx << "]=" << dataset[idx] << " ";
    }
    cout << endl;

    // Safe reverse traversal using signed integer to prevent unsigned underflow
    cout << "B. Safe Reverse Traversal (Signed int reverseIdx):" << endl;
    cout << "   Reverse contents: ";
    for (int reverseIdx = static_cast<int>(dataset.size()) - 1; reverseIdx >= 0; --reverseIdx) {
        cout << "vec[" << reverseIdx << "]=" << dataset[static_cast<size_t>(reverseIdx)] << " ";
    }
    cout << endl;

    // 5. MUTATING LOOP VARIABLES INSIDE LOOP BODY
    cout << "\n================ 4. DYNAMIC LOOP VARIABLE MUTATION ================" << endl;
    cout << "Advancing counter dynamically inside loop body based on condition:" << endl;
    for (int k = 1; k <= maxIterations; ++k) {
        cout << "  Start iteration k = " << k;
        if (k % 2 == 0) {
            cout << " (Even detected -> applying additional jump increment)";
            ++k; // Modifying loop variable directly inside the body
        }
        cout << " -> Final k for step = " << k << endl;
    }

    return 0;
}