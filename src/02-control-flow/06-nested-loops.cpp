/*
 * =====================================================================================
 * CONCEPT        : Nested Loops in C++ (Multi-Dimensional Iteration & Control Flow)
 * DESCRIPTION    : Comprehensive implementation explaining nested loop structures:
 *                  1. Outer vs Inner Mechanics   : Execution order (for every 1 outer step, 
 *                                                  inner completes all iterations).
 *                  2. Grid & Matrix Generation   : Processing 2D structures using nested 'for' loops.
 *                  3. Dynamic Inner Bounds       : Inner loop conditions dependent on outer 
 *                                                  loop variables (e.g., triangular patterns).
 *                  4. Control Flow Scope         : How 'break' and 'continue' behave inside nested 
 *                                                  loops, and how to break out of multiple levels.
 *                  5. Alternate Nested Structures: Combining nested 'while' and 'for' loops.
 *
 * TIME COMPLEXITY  : Best Case: O(N * M) — Bilinear time complexity where N = outer limit, M = inner limit.
 * SPACE COMPLEXITY : Best Case: O(N * M) — Dynamic memory allocation for 2D std::vector grid elements.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

int main() {
    int rows = 0;
    int cols = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter number of matrix/grid rows (1 - 8): " << flush;
    if (!(cin >> rows) || rows <= 0 || rows > 8) {
        cout << "Invalid row count provided. Program terminated." << endl;
        return 0;
    }

    cout << "Enter number of matrix/grid columns (1 - 8): " << flush;
    if (!(cin >> cols) || cols <= 0 || cols > 8) {
        cout << "Invalid column count provided. Program terminated." << endl;
        return 0;
    }

    // 2. BASIC 2D GRID / MULTIPLICATION TABLE
    cout << "\n================ 1. BASIC 2D GRID / MULTIPLICATION TABLE ================" << endl;
    cout << "Outer loop controls rows (1 to " << rows << "), Inner loop controls cols (1 to " << cols << "):\n" << endl;

    for (int r = 1; r <= rows; ++r) {
        cout << "Row " << r << " | ";
        for (int c = 1; c <= cols; ++c) {
            // Inner loop executes completely for every single iteration of outer loop
            cout << setw(4) << (r * c) << " ";
        }
        cout << endl; // Newline after completing inner loop
    }

    // 3. DYNAMIC INNER LOOP BOUNDS (TRIANGULAR PATTERNS)
    cout << "\n================ 2. DYNAMIC INNER LOOP BOUNDS ================" << endl;
    cout << "Inner loop condition (c <= r) depends directly on the outer loop counter:\n" << endl;

    for (int r = 1; r <= rows; ++r) {
        cout << "  Row " << r << " : ";
        for (int c = 1; c <= r; ++c) {
            cout << "* ";
        }
        cout << endl;
    }

    // 4. 2D VECTOR TRAVERSAL (NESTED RANGE-BASED FOR LOOPS)
    cout << "\n================ 3. 2D VECTOR / MATRIX TRAVERSAL ================" << endl;
    // Constructing a 2D vector with dynamic dimensions
    vector<vector<int>> matrix(static_cast<size_t>(rows), vector<int>(static_cast<size_t>(cols), 0));

    // Populating 2D vector using standard index-based nested loops
    for (size_t r = 0; r < matrix.size(); ++r) {
        for (size_t c = 0; c < matrix[r].size(); ++c) {
            matrix[r][c] = static_cast<int>((r + 1) * 10 + (c + 1));
        }
    }

    // Displaying 2D vector using range-based nested for loops
    cout << "Traversing 2D std::vector via range-based nested for loops:" << endl;
    for (const auto& rowVector : matrix) {
        cout << "  ";
        for (const auto& element : rowVector) {
            cout << "[" << setw(2) << element << "] ";
        }
        cout << endl;
    }

    // 5. CONTROL FLOW IN NESTED LOOPS ('break' AND 'continue')
    cout << "\n================ 4. BREAK & CONTINUE IN NESTED LOOPS ================" << endl;
    
    cout << "A. 'break' inside inner loop ONLY terminates the inner loop:" << endl;
    for (int r = 1; r <= rows; ++r) {
        cout << "  Outer Row " << r << ": ";
        for (int c = 1; c <= cols; ++c) {
            if (c > 3) {
                cout << "<Inner Break at c=" << c << ">";
                break; // Breaks out of inner loop ONLY
            }
            cout << "c=" << c << " ";
        }
        cout << endl;
    }

    cout << "\nB. Exiting BOTH outer and inner loops using a boolean flag:" << endl;
    bool stopAllLoops = false;
    for (int r = 1; r <= rows && !stopAllLoops; ++r) {
        cout << "  Outer Row " << r << ": ";
        for (int c = 1; c <= cols; ++c) {
            if (r == 2 && c == 2) {
                cout << "<Target Found at (2,2)! Terminating Multi-level Loop>";
                stopAllLoops = true;
                break; // Exit inner loop, flag will terminate outer loop
            }
            cout << "c=" << c << " ";
        }
        cout << endl;
    }

    // 6. NESTED WHILE LOOPS
    cout << "\n================ 5. NESTED WHILE LOOPS ================" << endl;
    int outerWhile = 1;
    while (outerWhile <= rows) {
        int innerWhile = 1;
        cout << "  Outer While " << outerWhile << " -> Inner: ";
        while (innerWhile <= cols) {
            cout << innerWhile << " ";
            ++innerWhile;
        }
        cout << endl;
        ++outerWhile;
    }

    return 0;
}