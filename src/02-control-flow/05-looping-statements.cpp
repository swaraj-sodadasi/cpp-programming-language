/*
 * =====================================================================================
 * CONCEPT        : Looping Statements in C++ (while, do-while, for, Range-Based for, Nested Loops)
 * DESCRIPTION    : Comprehensive implementation explaining C++ iteration mechanics:
 *                  1. while Loop            : Entry-controlled loop; condition evaluated before 
 *                                             executing the loop body.
 *                  2. do-while Loop        : Exit-controlled loop; body executes at least once 
 *                                             before evaluating the condition.
 *                  3. Standard for Loop     : Counter-driven loop with explicit initialization, 
 *                                             condition check, and post-iteration update.
 *                  4. Range-Based for Loop  : Modern C++ syntax for clean element-by-element 
 *                                             traversal over containers.
 *                  5. Loop Control Jump     : 'break' (immediate loop termination) and 
 *                                             'continue' (skip remaining body iteration).
 *                  6. Nested Loops          : Multi-dimensional iteration (e.g., grid or matrix generation).
 *
 * TIME COMPLEXITY  : Best Case: O(N + M^2) — Linear traversal for single loops + quadratic time for nested loops.
 * SPACE COMPLEXITY : Best Case: O(N)      — Memory footprint proportional to dynamic vector storage.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

int main() {
    int maxIterations = 0;
    int skipValue = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter target loop count (e.g., 5 - 10): " << flush;
    if (!(cin >> maxIterations) || maxIterations <= 0) {
        cout << "Invalid iteration count. Program terminated." << endl;
        return 0;
    }

    cout << "Enter a value to skip using 'continue' (1 to " << maxIterations << "): " << flush;
    if (!(cin >> skipValue)) {
        cout << "Invalid skip value. Program terminated." << endl;
        return 0;
    }

    // 2. WHILE LOOP (ENTRY-CONTROLLED)
    cout << "\n================ 1. WHILE LOOP (ENTRY-CONTROLLED) ================" << endl;
    cout << "Executing while loop up to " << maxIterations << " iterations:" << endl;
    int whileCount = 1;
    cout << "  Progress: ";
    while (whileCount <= maxIterations) {
        cout << "[" << whileCount << "] ";
        ++whileCount;
    }
    cout << "\n  Loop finished. Final counter value: " << whileCount << " (Condition evaluated to false)" << endl;

    // 3. DO-WHILE LOOP (EXIT-CONTROLLED)
    cout << "\n================ 2. DO-WHILE LOOP (EXIT-CONTROLLED) ================" << endl;
    cout << "Executing do-while loop (Guaranteed at least one execution):" << endl;
    int doCount = 1;
    cout << "  Progress: ";
    do {
        cout << "[" << doCount << "] ";
        ++doCount;
    } while (doCount <= maxIterations);
    cout << "\n  Loop finished. Final counter value: " << doCount << endl;

    // Demonstrating that do-while executes at least once even if condition starts as false
    cout << "  Testing do-while with initial false condition (doCount = 999): ";
    int falseCondCounter = 999;
    do {
        cout << "(Executed once with value " << falseCondCounter << ") ";
    } while (falseCondCounter < maxIterations);
    cout << endl;

    // 4. STANDARD FOR LOOP WITH BREAK AND CONTINUE
    cout << "\n================ 3. FOR LOOP WITH BREAK & CONTINUE ================" << endl;
    cout << "Standard for loop with 'skip' (continue) and 'cap at 8' (break):" << endl;
    cout << "  Output: ";
    for (int i = 1; i <= maxIterations; ++i) {
        if (i == skipValue) {
            cout << "<Skipped " << i << "> ";
            continue; // Skips remaining statement in loop body
        }
        if (i > 8) {
            cout << "\n  [Break Triggered] Iteration exceeded 8. Exiting loop early.";
            break; // Immediately terminates for loop
        }
        cout << i << " ";
    }
    cout << endl;

    // 5. RANGE-BASED FOR LOOP (MODERN C++)
    cout << "\n================ 4. RANGE-BASED FOR LOOP ================" << endl;
    // Populate dynamic vector
    vector<int> numbers;
    numbers.reserve(static_cast<size_t>(maxIterations));
    for (int i = 1; i <= maxIterations; ++i) {
        numbers.push_back(i * 10);
    }

    cout << "Traversing std::vector<int> via Range-Based for loop (const reference):" << endl;
    cout << "  Elements: ";
    for (const auto& num : numbers) {
        cout << num << " ";
    }
    cout << endl;

    // Modifying elements in-place using non-const reference
    cout << "Updating vector elements in-place (num += 5)..." << endl;
    for (auto& num : numbers) {
        num += 5;
    }

    cout << "  Updated Elements: ";
    for (const auto& num : numbers) {
        cout << num << " ";
    }
    cout << endl;

    // 6. NESTED LOOPS
    cout << "\n================ 5. NESTED LOOPS (GRID / MATRIX) ================" << endl;
    int gridSize = (maxIterations > 4) ? 4 : maxIterations; // Limit grid size for clean display
    cout << "Generating a " << gridSize << "x" << gridSize << " multiplication table grid:" << endl;

    for (int row = 1; row <= gridSize; ++row) {
        cout << "  Row " << row << " | ";
        for (int col = 1; col <= gridSize; ++col) {
            cout << setw(4) << (row * col) << " ";
        }
        cout << endl;
    }

    return 0;
}