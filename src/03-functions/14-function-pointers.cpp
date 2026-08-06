/*
 * =====================================================================================
 * CONCEPT        : Function Pointers in C++
 * DESCRIPTION    : Comprehensive implementation detailing function pointers:
 *                  1. Basic Syntax & Declaration : Memory address of functions and invocation syntax.
 *                  2. Type Aliases (`using`)      : Simplifying complex function pointer signatures.
 *                  3. Callbacks / Strategy        : Passing behavior/functions as arguments to other functions.
 *                  4. Jump Tables / Arrays        : Storing function pointers in arrays for O(1) dynamic dispatch.
 *                  5. Modern C++ (`std::function`): Comparing raw function pointers with type-erased wrappers.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct/Indirect function call through instruction pointer.
 * SPACE COMPLEXITY : Best Case: O(1) — Stores pointer memory address (typically 8 bytes on 64-bit systems).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <functional>
#include <iomanip>

using namespace std;

// =====================================================================================
// TARGET FUNCTIONS FOR POINTER BINDING
// =====================================================================================

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide(int a, int b) {
    return (b != 0) ? (a / b) : 0;
}

// =====================================================================================
// 1. TYPE ALIASING FOR FUNCTION POINTERS
// Syntax: using AliasName = ReturnType (*)(ParamTypes...);
// =====================================================================================
using MathOperation = int (*)(int, int);

// =====================================================================================
// 2. HIGHER-ORDER FUNCTION (CALLBACK DEMONSTRATION)
// Accepts a function pointer as a parameter to execute customizable behavior.
// =====================================================================================
void executeOperation(int x, int y, MathOperation operation, const string& opName) {
    // Calling function via pointer
    int result = operation(x, y); 
    cout << "  - [Callback Execution] " << opName << "(" << x << ", " << y << ") = " << result << endl;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int inputA = 0;
    int inputB = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter first operand integer (e.g., 20): " << flush;
    if (!(cin >> inputA)) {
        cout << "Invalid input. Defaulting to 20." << endl;
        inputA = 20;
    }

    cout << "Enter second operand integer (e.g., 5): " << flush;
    if (!(cin >> inputB)) {
        cout << "Invalid input. Defaulting to 5." << endl;
        inputB = 5;
    }

    // 1. BASIC FUNCTION POINTER SYNTAX & INVOCATION
    cout << "\n================ 1. BASIC FUNCTION POINTER SYNTAX ================" << endl;
    
    // Explicit syntax: ReturnType (*pointerName)(ParamTypes...)
    int (*rawFuncPtr)(int, int) = &add; // '&' is optional; 'rawFuncPtr = add;' is also valid

    cout << "Function 'add' resides at instruction address : " << reinterpret_cast<void*>(rawFuncPtr) << endl;
    
    // Invocation through pointer (both dereferencing (*funcPtr)() and direct calling funcPtr() work)
    int sumResult1 = (*rawFuncPtr)(inputA, inputB); // Explicit dereference call
    int sumResult2 = rawFuncPtr(inputA, inputB);   // Implicit dereference call

    cout << "  - Explicit (*rawFuncPtr)(" << inputA << ", " << inputB << ") = " << sumResult1 << endl;
    cout << "  - Direct rawFuncPtr(" << inputA << ", " << inputB << ")       = " << sumResult2 << endl;

    // Reassigning pointer to point to a different function with matching signature
    rawFuncPtr = subtract;
    cout << "\nReassigned rawFuncPtr to 'subtract' address : " << reinterpret_cast<void*>(rawFuncPtr) << endl;
    cout << "  - rawFuncPtr(" << inputA << ", " << inputB << ") = " << rawFuncPtr(inputA, inputB) << endl;

    // 2. CLEANER SYNTAX USING TYPE ALIASES (`using`)
    cout << "\n================ 2. TYPE ALIASING (`using MathOperation`) ================" << endl;
    MathOperation cleanPtr = multiply;
    cout << "  - cleanPtr (multiply) -> " << inputA << " * " << inputB << " = " << cleanPtr(inputA, inputB) << endl;

    // 3. PASSING FUNCTION POINTERS AS CALLBACKS
    cout << "\n================ 3. FUNCTION POINTERS AS CALLBACKS ================" << endl;
    executeOperation(inputA, inputB, add, "Add");
    executeOperation(inputA, inputB, subtract, "Subtract");
    executeOperation(inputA, inputB, multiply, "Multiply");
    executeOperation(inputA, inputB, divide, "Divide");

    // 4. JUMP TABLE / ARRAY OF FUNCTION POINTERS
    cout << "\n================ 4. JUMP TABLES (ARRAY OF FUNCTION POINTERS) ================" << endl;
    
    // Array of function pointers enabling O(1) dynamic dispatch by index
    MathOperation jumpTable[] = {add, subtract, multiply, divide};
    const string opLabels[] = {"[0] Addition", "[1] Subtraction", "[2] Multiplication", "[3] Division"};

    for (size_t i = 0; i < 4; ++i) {
        int result = jumpTable[i](inputA, inputB); // Jump table lookup and invocation
        cout << "  - Jump Table " << left << setw(18) << opLabels[i] 
             << " -> Result = " << result << endl;
    }

    // 5. MODERN C++ ALTERNATIVE (`std::function`)
    cout << "\n================ 5. MODERN ALTERNATIVE (`std::function`) ================" << endl;
    
    // std::function offers type-erased support for function pointers, lambdas, and functors
    std::function<int(int, int)> modernOp = add;
    cout << "  - std::function wrapping 'add'      : " << modernOp(inputA, inputB) << endl;

    modernOp = [](int a, int b) { return (a * a) + (b * b); }; // Binds smoothly to a lambda
    cout << "  - std::function wrapping a Lambda   : " << modernOp(inputA, inputB) << endl;

    cout << "\n================ FUNCTION POINTERS SUMMARY ================" << endl;
    cout << "1. Nature       : Stores the memory address of executable instructions, not data." << endl;
    cout << "2. Signature    : Pointer declaration must match target return type and parameter types exactly." << endl;
    cout << "3. Use Cases    : Event callbacks, strategy design patterns, OS C-APIs, jump/dispatch tables." << endl;
    cout << "4. Type Aliases : Use `using Alias = ReturnType (*)(Params...)` to clean up complex syntax." << endl;
    cout << "5. Modern C++   : Prefer `std::function` or templates for flexibility with lambdas & member functions." << endl;

    return 0;
}