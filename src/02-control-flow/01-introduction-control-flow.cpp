/*
 * =====================================================================================
 * CONCEPT        : Introduction to Control Flow in C++ (Sequential, Selection, Iteration, Jump)
 * DESCRIPTION    : Comprehensive implementation explaining how control flow directs execution:
 *                  1. Sequential Execution : Default top-to-bottom statement execution.
 *                  2. Selection / Branching : Decision-making using 'if-else', 'switch-case', 
 *                                             and conditional (ternary) operators.
 *                  3. Iteration / Looping   : Repeated execution using 'while', 'do-while', 
 *                                             standard 'for', and range-based 'for' loops.
 *                  4. Jump Statements       : Altering loop/switch flow via 'break' and 'continue'.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear time complexity proportional to loop iteration count N.
 * SPACE COMPLEXITY : Best Case: O(N) — Memory footprint bounded by dynamically populated std::vector.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    int userScore = 0;
    int loopLimit = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter a test score integer (0 - 100): " << flush;
    if (!(cin >> userScore)) {
        cout << "Invalid score input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter number of loop iterations (1 - 10): " << flush;
    if (!(cin >> loopLimit) || loopLimit <= 0 || loopLimit > 10) {
        cout << "Invalid iteration count. Defaulting to 5." << endl;
        loopLimit = 5;
    }

    // 2. SEQUENTIAL EXECUTION (Top-to-Bottom Flow)
    cout << "\n================ 1. SEQUENTIAL EXECUTION ================" << endl;
    cout << "Step A: Variable initialization completed." << endl;
    cout << "Step B: Stream input captured successfully." << endl;
    cout << "Step C: Sequential execution proceeds line-by-line in order." << endl;

    // 3. SELECTION / BRANCHING CONTROL FLOW (if / else if / else, switch, ternary)
    cout << "\n================ 2. SELECTION / BRANCHING ================" << endl;
    
    // Conditional Branching using if - else if - else
    char gradeLetter = 'F';
    if (userScore >= 90) {
        gradeLetter = 'A';
        cout << "if-branch executed      : Score " << userScore << " qualifies for Grade A." << endl;
    } else if (userScore >= 80) {
        gradeLetter = 'B';
        cout << "else-if branch executed : Score " << userScore << " qualifies for Grade B." << endl;
    } else if (userScore >= 70) {
        gradeLetter = 'C';
        cout << "else-if branch executed : Score " << userScore << " qualifies for Grade C." << endl;
    } else {
        gradeLetter = 'F';
        cout << "else-branch executed    : Score " << userScore << " is below Grade C requirement." << endl;
    }

    // Decision Making using switch-case statement
    cout << "Switch-case evaluation  : Grade Letter '" << gradeLetter << "' evaluated -> ";
    switch (gradeLetter) {
        case 'A':
            cout << "Performance Status: Excellent!" << endl;
            break; // Exits switch block
        case 'B':
            cout << "Performance Status: Good Job!" << endl;
            break;
        case 'C':
            cout << "Performance Status: Satisfactory." << endl;
            break;
        default:
            cout << "Performance Status: Needs Improvement." << endl;
            break;
    }

    // Ternary Conditional Operator (Expression-level branching)
    string passStatus = (userScore >= 70) ? "PASSED" : "FAILED";
    cout << "Ternary Operator        : Result -> " << passStatus << endl;

    // 4. ITERATION / LOOPING CONTROL FLOW (while, do-while, for, range-for)
    cout << "\n================ 3. ITERATION / LOOPING ================" << endl;

    // A. while loop (Entry-controlled loop)
    cout << "A. while Loop (Entry-controlled)     : ";
    int counter = 1;
    while (counter <= loopLimit) {
        cout << counter << " ";
        ++counter;
    }
    cout << endl;

    // B. do-while loop (Exit-controlled loop; executes at least once)
    cout << "B. do-while Loop (Exit-controlled)  : ";
    int doCounter = 1;
    do {
        cout << doCounter << " ";
        ++doCounter;
    } while (doCounter <= loopLimit);
    cout << endl;

    // C. Standard for loop with jump statements ('continue' and 'break')
    cout << "C. for Loop (with break & continue)  : ";
    vector<int> collectedData;
    for (int i = 1; i <= loopLimit; ++i) {
        if (i == 2) {
            // 'continue' skips remaining body and jumps to loop increment
            continue; 
        }
        if (i > 8) {
            // 'break' immediately terminates the loop iteration
            break; 
        }
        cout << i << " ";
        collectedData.push_back(i * 10);
    }
    cout << "(Note: '2' skipped via continue)" << endl;

    // D. Range-based for loop (Modern C++ element-by-element iteration)
    cout << "D. Range-Based for Loop (Vector)     : ";
    for (const auto& val : collectedData) {
        cout << "[" << val << "] ";
    }
    cout << endl;

    return 0;
}