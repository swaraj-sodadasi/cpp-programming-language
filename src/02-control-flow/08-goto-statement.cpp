/*
 * =====================================================================================
 * CONCEPT        : The 'goto' Statement in C++ (Unconditional Branching & Labeled Jumps)
 * DESCRIPTION    : Comprehensive implementation explaining the C++ goto mechanism:
 *                  1. Basic Unconditional Jump : Transferring execution to a labeled statement 
 *                                                within the same function scope.
 *                  2. Multi-Level Loop Exit    : Cleanly breaking out of deeply nested loops 
 *                                                where standard 'break' only exits one level.
 *                  3. Backward Jumps (Loops)   : Implementing iteration using labeled jumps 
 *                                                (educational demonstration; standard loops preferred).
 *                  4. Forward Error Handling   : Jumping forward to a cleanup/error section.
 *                  5. Scope Rules & Limits     : Demonstrating rules regarding variable initialization 
 *                                                and function scope boundaries.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct unconditional jump evaluated at compile-time/runtime.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack storage for tracking control flow variables.
 * =====================================================================================
 */

#include <iostream>

using namespace std;

int main() {
    int targetVal = 0;
    int maxRows = 0;
    int maxCols = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter target matrix value to search (e.g., 8, 12, 18): " << flush;
    if (!(cin >> targetVal)) {
        cout << "Invalid input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter matrix rows count (1 - 5): " << flush;
    if (!(cin >> maxRows) || maxRows <= 0) {
        cout << "Invalid rows input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter matrix columns count (1 - 5): " << flush;
    if (!(cin >> maxCols) || maxCols <= 0) {
        cout << "Invalid columns input. Program terminated." << endl;
        return 0;
    }

    // 2. BACKWARD JUMP DEMONSTRATION (SIMULATING A LOOP WITH GOTO)
    cout << "\n================ 1. BACKWARD JUMP (GOTO LOOP) ================" << endl;
    cout << "Counting down using backward goto jump:" << endl;
    int counter = 3;

COUNTDOWN_LABEL: // Labeled statement target for backward jump
    if (counter > 0) {
        cout << "  Counter: " << counter << endl;
        --counter;
        goto COUNTDOWN_LABEL; // Jumps backward to COUNTDOWN_LABEL
    }
    cout << "  Countdown completed!" << endl;

    // 3. MULTI-LEVEL NESTED LOOP EXIT (PRIMARY ACCEPTED USE CASE)
    cout << "\n================ 2. MULTI-LEVEL NESTED LOOP EXIT ================" << endl;
    cout << "Searching dynamic " << maxRows << "x" << maxCols << " matrix for target value " << targetVal << ":" << endl;

    bool valueFound = false;
    int foundRow = -1;
    int foundCol = -1;

    for (int r = 1; r <= maxRows; ++r) {
        for (int c = 1; c <= maxCols; ++c) {
            int cellValue = r * c * 2; // Dynamic cell value calculation
            cout << "  Checking cell (" << r << ", " << c << ") = " << cellValue;

            if (cellValue == targetVal) {
                cout << " -> TARGET MATCH FOUND!" << endl;
                foundRow = r;
                foundCol = c;
                valueFound = true;
                // Standard 'break' would only exit the inner loop (c loop).
                // 'goto' cleanly escapes BOTH nested loops immediately.
                goto MATRIX_SEARCH_EXIT;
            }
            cout << endl;
        }
    }

MATRIX_SEARCH_EXIT: // Target label outside the nested loop hierarchy
    if (valueFound) {
        cout << "  Result: Target " << targetVal << " located at matrix position (" 
             << foundRow << ", " << foundCol << ")." << endl;
    } else {
        cout << "  Result: Target " << targetVal << " was not found in the matrix." << endl;
    }

    // 4. FORWARD JUMP FOR ERROR HANDLING / CLEANUP PATTERN
    cout << "\n================ 3. FORWARD JUMP FOR ERROR HANDLING ================" << endl;
    cout << "Simulating multi-step resource validation..." << endl;

    if (targetVal < 0) {
        cout << "  [Error Detected] Negative target value is invalid!" << endl;
        goto ERROR_CLEANUP_LABEL; // Forward jump directly to cleanup section
    }

    cout << "  Step 1: Resource allocation successful." << endl;
    cout << "  Step 2: Operational processing completed normally." << endl;
    goto NORMAL_EXIT_LABEL;

ERROR_CLEANUP_LABEL: // Forward jump target
    cout << "  [Cleanup Handler] Performing resource teardown due to error." << endl;

NORMAL_EXIT_LABEL:
    cout << "  Execution completed successfully." << endl;

    // 5. COMPILER RULES & BEST PRACTICES SUMMARY
    cout << "\n================ 4. GOTO RULES & RESTRICTIONS ================" << endl;
    cout << "1. Scope Limit   : Jumps can ONLY occur within the same function scope." << endl;
    cout << "2. Init Bypass   : Cannot jump OVER a variable initialization within the same scope." << endl;
    cout << "3. Best Practice : Prefer structured loops (for, while) and functions over goto," << endl;
    cout << "                   except for multi-level loop breakouts or C-style cleanup routines." << endl;

    return 0;
}