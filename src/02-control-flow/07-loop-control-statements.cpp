/*
 * =====================================================================================
 * CONCEPT        : Loop Control Statements in C++ (break, continue, goto, return)
 * DESCRIPTION    : Comprehensive implementation explaining C++ loop control statements:
 *                  1. break Statement    : Immediately terminates the innermost loop or switch 
 *                                          block and transfers execution to the following statement.
 *                  2. continue Statement : Skips the remainder of the current loop body iteration 
 *                                          and proceeds directly to the next loop evaluation/increment.
 *                  3. goto Statement     : Performs an unconditional jump to a named labeled 
 *                                          statement within the same function scope (used safely 
 *                                          here for exiting multi-level nested loops).
 *                  4. return Statement   : Immediately terminates the function execution and 
 *                                          returns control (and an optional value) to the caller.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear traversal over dynamic vector elements bounded by dynamic triggers.
 * SPACE COMPLEXITY : Best Case: O(N) — Stack/heap storage proportional to dynamic vector size.
 * =====================================================================================
 */

#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief Helper function demonstrating the 'return' statement as a loop exit mechanism.
 * @param numbers Target vector of integer data.
 * @param searchVal Value to locate.
 * @return True if target found, false if loop completes without finding value.
 */
bool findAndReturnEarly(const vector<int>& numbers, int searchVal) {
    cout << "  Searching for target (" << searchVal << ") using early return..." << endl;
    for (size_t i = 0; i < numbers.size(); ++i) {
        if (numbers[i] == searchVal) {
            cout << "  [return Triggered] Target found at index " << i << "! Exiting function early." << endl;
            return true; // Immediately exits function body
        }
        cout << "    |- Inspected index " << i << " (Value: " << numbers[i] << ")" << endl;
    }
    cout << "  [Loop Exhausted] Target not present in collection." << endl;
    return false;
}

int main() {
    int loopLimit = 0;
    int breakVal = 0;
    int skipVal = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter total loop iteration count (1 - 10): " << flush;
    if (!(cin >> loopLimit) || loopLimit <= 0 || loopLimit > 10) {
        cout << "Invalid limit provided. Program terminated." << endl;
        return 0;
    }

    cout << "Enter iteration value to 'skip' via continue (1 - " << loopLimit << "): " << flush;
    if (!(cin >> skipVal)) {
        cout << "Invalid skip input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter iteration value to 'terminate' loop via break (1 - " << loopLimit << "): " << flush;
    if (!(cin >> breakVal)) {
        cout << "Invalid break input. Program terminated." << endl;
        return 0;
    }

    // 2. CONTINUE STATEMENT DEMONSTRATION
    cout << "\n================ 1. CONTINUE STATEMENT ================" << endl;
    cout << "Loop running up to " << loopLimit << ", skipping value " << skipVal << ":" << endl;
    cout << "  Output: ";
    for (int i = 1; i <= loopLimit; ++i) {
        if (i == skipVal) {
            cout << "<SKIP " << i << "> ";
            continue; // Jumps directly to loop increment (++i), skipping code below
        }
        cout << i << " ";
    }
    cout << endl;

    // 3. BREAK STATEMENT DEMONSTRATION
    cout << "\n================ 2. BREAK STATEMENT ================" << endl;
    cout << "Loop running up to " << loopLimit << ", stopping early at value " << breakVal << ":" << endl;
    cout << "  Output: ";
    for (int i = 1; i <= loopLimit; ++i) {
        if (i == breakVal) {
            cout << "\n  [break Triggered] Reached value " << i << ". Terminating loop immediately.";
            break; // Exits loop block entirely
        }
        cout << i << " ";
    }
    cout << endl;

    // 4. GOTO STATEMENT (MULTI-LEVEL NESTED LOOP EXIT)
    cout << "\n================ 3. GOTO STATEMENT (NESTED LOOP EXIT) ================" << endl;
    cout << "Searching dynamic 2D grid for break condition (r * c == breakVal):" << endl;

    for (int r = 1; r <= 3; ++r) {
        for (int c = 1; c <= 3; ++c) {
            int product = r * c;
            cout << "  Grid cell (" << r << ", " << c << ") = " << product;
            if (product == breakVal) {
                cout << " -> Match! Invoking 'goto EXIT_NESTED_LOOPS'..." << endl;
                goto EXIT_NESTED_LOOPS; // Unconditional jump out of both inner & outer loops
            }
            cout << endl;
        }
    }

// Jump label target for goto
EXIT_NESTED_LOOPS:
    cout << "  Successfully escaped multi-level loops via labeled goto jump." << endl;

    // 5. RETURN STATEMENT DEMONSTRATION
    cout << "\n================ 4. RETURN STATEMENT ================" << endl;
    // Construct dynamic vector for search demonstration
    vector<int> numbersContainer;
    for (int i = 1; i <= loopLimit; ++i) {
        numbersContainer.push_back(i * 10);
    }

    int searchTarget = breakVal * 10;
    bool found = findAndReturnEarly(numbersContainer, searchTarget);
    cout << "  Search Outcome: " << (found ? "SUCCESS" : "FAILURE") << endl;

    return 0;
}