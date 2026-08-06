/*
 * =====================================================================================
 * CONCEPT        : Function Call Stack in C++
 * DESCRIPTION    : Comprehensive implementation explaining call stack mechanics:
 *                  1. Activation Records / Stack Frames : Stack memory allocated when a 
 *                                                         function is called (parameters, 
 *                                                         local variables, return address).
 *                  2. LIFO Execution Behavior           : Last-In, First-Out frame allocation;
 *                                                         functions push frames on entry and 
 *                                                         pop frames on return.
 *                  3. Memory Address Progression        : Demonstrating stack growth direction 
 *                                                         by printing variable addresses across 
 *                                                         nested calls.
 *                  4. Call Stack Depth                  : Visualizing recursive call stack frame 
 *                                                         accumulation and unwinding.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear execution across call stack frames.
 * SPACE COMPLEXITY : Best Case: O(N) — Stack frame allocation on the execution stack.
 * =====================================================================================
 */

#include <iostream>
#include <cstdint>
#include <string>

using namespace std;

// =====================================================================================
// 1. NESTED CALL STACK CHAIN DEMONSTRATION
// Main -> Level A -> Level B -> Level C
// =====================================================================================

void levelC(int paramC, const uintptr_t parentAddr) {
    int localVarC = 300;
    uintptr_t currentAddr = reinterpret_cast<uintptr_t>(&localVarC);

    cout << "    [PUSH] Entering levelC()\n";
    cout << "           - Parameter 'paramC'     : " << paramC << "\n";
    cout << "           - Local Var Address      : 0x" << hex << currentAddr << dec << "\n";
    
    // Demonstrate stack growth direction
    if (currentAddr < parentAddr) {
        cout << "           - Stack Offset           : -" << (parentAddr - currentAddr) 
             << " bytes (Stack growing DOWNWARDS in memory)\n";
    } else {
        cout << "           - Stack Offset           : +" << (currentAddr - parentAddr) 
             << " bytes (Stack growing UPWARDS in memory)\n";
    }

    cout << "    [POP ] Exiting levelC() (Frame destroyed)\n";
}

void levelB(int paramB, const uintptr_t parentAddr) {
    int localVarB = 200;
    uintptr_t currentAddr = reinterpret_cast<uintptr_t>(&localVarB);

    cout << "  [PUSH] Entering levelB()\n";
    cout << "         - Parameter 'paramB'       : " << paramB << "\n";
    cout << "         - Local Var Address        : 0x" << hex << currentAddr << dec << "\n";

    // Call nested function Level C
    levelC(paramB + 10, currentAddr);

    cout << "  [POP ] Exiting levelB() (Frame destroyed)\n";
}

void levelA(int paramA, const uintptr_t mainAddr) {
    int localVarA = 100;
    uintptr_t currentAddr = reinterpret_cast<uintptr_t>(&localVarA);

    cout << "[PUSH] Entering levelA()\n";
    cout << "       - Parameter 'paramA'         : " << paramA << "\n";
    cout << "       - Local Var Address          : 0x" << hex << currentAddr << dec << "\n";

    // Call nested function Level B
    levelB(paramA + 10, currentAddr);

    cout << "[POP ] Exiting levelA() (Frame destroyed)\n";
}

/**
 * @brief Section 1: Demonstrates chained function call stack pushes and pops.
 */
void demonstrateNestedCallStack(int baseValue) {
    cout << "\n================ 1. NESTED CALL STACK EXECUTION ================" << endl;

    int mainLocalVar = 50;
    uintptr_t mainAddr = reinterpret_cast<uintptr_t>(&mainLocalVar);

    cout << "[MAIN FRAME] 'mainLocalVar' Address : 0x" << hex << mainAddr << dec << endl;
    cout << "Triggering nested call chain: levelA() -> levelB() -> levelC()\n" << endl;

    levelA(baseValue, mainAddr);

    cout << "\nReturned safely to main frame." << endl;
}

// =====================================================================================
// 2. RECURSIVE CALL STACK VISUALIZATION
// =====================================================================================

/**
 * @brief Section 2: Visualizes recursive stack frame accumulation and unwinding.
 */
int recursiveStackAnalysis(int currentDepth, int targetDepth, uintptr_t previousAddr = 0) {
    int localFrameMarker = currentDepth * 111;
    uintptr_t currentAddr = reinterpret_cast<uintptr_t>(&localFrameMarker);

    string indent(static_cast<size_t>((currentDepth - 1) * 3), ' ');

    cout << indent << "-> [PUSH Frame #" << currentDepth << "] Frame Addr: 0x" 
         << hex << currentAddr << dec;

    if (previousAddr != 0) {
        intptr_t diff = static_cast<intptr_t>(previousAddr) - static_cast<intptr_t>(currentAddr);
        cout << " (Delta: " << diff << " bytes)";
    }
    cout << endl;

    // BASE CASE: Stop pushing frames onto stack
    if (currentDepth >= targetDepth) {
        cout << indent << "   [Base Case Reached] Reached target depth of " << targetDepth 
             << ". Beginning stack unwinding (POPs)..." << endl;
        cout << indent << "<- [POP  Frame #" << currentDepth << "] Returning marker value: " 
             << localFrameMarker << endl;
        return localFrameMarker;
    }

    // RECURSIVE STEP: Push next frame
    int childResult = recursiveStackAnalysis(currentDepth + 1, targetDepth, currentAddr);
    int totalResult = localFrameMarker + childResult;

    cout << indent << "<- [POP  Frame #" << currentDepth << "] Unwound frame. Aggregate sum: " 
         << totalResult << endl;

    return totalResult;
}

int main() {
    int targetCallDepth = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter target recursive call stack depth (2 to 5): " << flush;
    if (!(cin >> targetCallDepth) || targetCallDepth < 1 || targetCallDepth > 10) {
        cout << "Invalid depth input provided. Defaulting target depth to 4." << endl;
        targetCallDepth = 4;
    }

    // 1. DEMONSTRATE NESTED CALL STACK EXECUTION
    demonstrateNestedCallStack(10);

    // 2. DEMONSTRATE RECURSIVE CALL STACK ACCUMULATION & UNWINDING
    cout << "\n================ 2. RECURSIVE CALL STACK ACCUMULATION ================" << endl;
    cout << "Executing recursive call stack up to depth " << targetCallDepth << ":\n" << endl;

    int totalStackSum = recursiveStackAnalysis(1, targetCallDepth);

    cout << "\nFinal result accumulated from unwound stack frames: " << totalStackSum << endl;

    cout << "\n================ CALL STACK FUNDAMENTALS SUMMARY ================" << endl;
    cout << "1. Stack Frame Content : Contains parameters, local variables, return address, and saved registers." << endl;
    cout << "2. Execution Order     : Functions follow LIFO (Last-In, First-Out) stack order." << endl;
    cout << "3. Stack Growth        : On most architectures (e.g., x86_64), stack memory grows downwards." << endl;
    cout << "4. Stack Overflow      : Exceeding available stack memory space (e.g., infinite recursion) crashes the process." << endl;

    return 0;
}