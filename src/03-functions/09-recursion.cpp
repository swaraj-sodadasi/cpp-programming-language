/*
 * =====================================================================================
 * CONCEPT        : Recursion in C++ (Base Cases, Call Stack, Tail Recursion, Tree Recursion)
 * DESCRIPTION    : Comprehensive implementation explaining recursion principles:
 *                  1. Anatomy of Recursion   : Base case (stopping condition) vs Recursive step 
 *                                              (subproblem reduction).
 *                  2. Call Stack Unwinding   : Visualizing stack frame pushes and pops during execution.
 *                  3. Tail Recursion         : Structure where the recursive call is the final action,
 *                                              enabling Tail Call Optimization (TCO).
 *                  4. Tree (Multiple) Recursion: Branching recursive calls (e.g., Fibonacci).
 *                  5. Indirect / Mutual      : Functions calling each other recursively in a cycle.
 *
 * TIME COMPLEXITY  : Linear Recursion: O(N) | Tree Recursion (Fibonacci): O(2^N)
 * SPACE COMPLEXITY : Call Stack depth: O(N) for linear recursion; O(1) if optimized via TCO.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <cstdint>

using namespace std;

// Forward declarations for mutual/indirect recursion
bool isOdd(int n);
bool isEven(int n);

/**
 * @brief Section 1: Linear Recursion with Execution Trace
 * Demonstrates call stack pushes (winding) and pops (unwinding).
 */
uint64_t factorialWithTrace(int n, int depth = 0) {
    string indent(static_cast<size_t>(depth * 3), ' ');
    
    cout << indent << "-> Entering factorial(" << n << ") [Stack Frame Pushed]" << endl;

    // BASE CASE: Halts further recursive branching
    if (n <= 1) {
        cout << indent << "   [Base Case Reached] Returning 1" << endl;
        cout << indent << "<- Exiting factorial(" << n << ") [Stack Frame Popped]" << endl;
        return 1;
    }

    // RECURSIVE CASE: Reduces problem size (n - 1)
    uint64_t subResult = factorialWithTrace(n - 1, depth + 1);
    uint64_t result = static_cast<uint64_t>(n) * subResult;

    cout << indent << "<- Exiting factorial(" << n << ") = " << result << " [Stack Frame Popped]" << endl;
    return result;
}

/**
 * @brief Section 2: Tail Recursion
 * Accumulates result in parameter so no pending operations remain after return.
 * Compilers can optimize this into an iterative loop (Tail Call Optimization / TCO).
 */
uint64_t factorialTailRecursive(int n, uint64_t accumulator = 1) {
    // Base case
    if (n <= 1) {
        return accumulator;
    }
    // Tail Recursive Call: Nothing left to calculate after the return
    return factorialTailRecursive(n - 1, n * accumulator);
}

/**
 * @brief Section 3: Tree Recursion (Branching)
 * Makes multiple self-calls per frame, generating a recursive decision tree.
 */
int fibonacciTree(int n, int& callCounter) {
    ++callCounter; // Track total stack frames created
    if (n <= 0) return 0; // Base Case 1
    if (n == 1) return 1; // Base Case 2

    // Two recursive calls per node (Binary Tree)
    return fibonacciTree(n - 1, callCounter) + fibonacciTree(n - 2, callCounter);
}

/**
 * @brief Section 4: Indirect / Mutual Recursion
 * Function A calls Function B, and Function B calls Function A.
 */
bool isEven(int n) {
    if (n == 0) return true;  // Base case: 0 is even
    return isOdd(n - 1);       // Indirect call to isOdd
}

bool isOdd(int n) {
    if (n == 0) return false; // Base case: 0 is not odd
    return isEven(n - 1);      // Indirect call to isEven
}

int main() {
    int userN = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a positive integer for recursive demonstration (3 to 6): " << flush;
    if (!(cin >> userN) || userN < 1 || userN > 12) {
        cout << "Invalid or out-of-range input. Defaulting to 4." << endl;
        userN = 4;
    }

    // 1. DEMONSTRATING LINEAR RECURSION & STACK UNWINDING
    cout << "\n================ 1. CALL STACK WINDING & UNWINDING ================" << endl;
    cout << "Trace for factorialWithTrace(" << userN << "):" << endl;
    uint64_t traceResult = factorialWithTrace(userN);
    cout << "Final Computed Factorial Result: " << traceResult << endl;

    // 2. DEMONSTRATING TAIL RECURSION
    cout << "\n================ 2. TAIL RECURSION (TCO EFFICIENT) ================" << endl;
    uint64_t tailResult = factorialTailRecursive(userN);
    cout << "factorialTailRecursive(" << userN << ") = " << tailResult << endl;
    cout << "  |- Insight: Accumulator parameter carries state, leaving no work after call." << endl;

    // 3. DEMONSTRATING TREE RECURSION
    cout << "\n================ 3. TREE RECURSION (BRANCHING) ================" << endl;
    int totalCalls = 0;
    int fibResult = fibonacciTree(userN, totalCalls);
    cout << "fibonacci(" << userN << ") = " << fibResult << endl;
    cout << "Total function stack frames created: " << totalCalls << " calls" << endl;
    cout << "  |- Insight: Tree recursion grows exponentially O(2^N) without memoization." << endl;

    // 4. DEMONSTRATING MUTUAL / INDIRECT RECURSION
    cout << "\n================ 4. INDIRECT / MUTUAL RECURSION ================" << endl;
    bool evenCheck = isEven(userN);
    cout << "isEven(" << userN << ") evaluated via mutual recursion -> " 
         << (evenCheck ? "TRUE (Even)" : "FALSE (Odd)") << endl;

    // SUMMARY OF RECURSION RULES
    cout << "\n================ RECURSION FUNDAMENTALS SUMMARY ================" << endl;
    cout << "1. Base Case Required : Every recursive function MUST have at least one stopping condition." << endl;
    cout << "2. Progress Mandatory : Every call MUST reduce the problem toward the base case." << endl;
    cout << "3. Stack Risk         : Missing base cases or deep depth cause Stack Overflow errors." << endl;
    cout << "4. Tail Optimization  : Tail-recursive functions save memory if TCO is enabled in compiler." << endl;

    return 0;
}