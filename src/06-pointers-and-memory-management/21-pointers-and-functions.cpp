/*
 * =====================================================================================
 * CONCEPT        : Pointers and Functions in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the relationship between pointers, dynamic memory, and functions:
 *
 *                  1. Pass-by-Pointer (`T*`)         : Mutating caller variables in-place via pointer parameters.
 *                  2. Const Parameter Contracts     : Read-only data (`const T*`) vs immutable addresses (`T* const`).
 *                  3. Returning Pointers            : Heap memory factory functions vs stack lifetime pitfalls.
 *                  4. Reference-to-Pointer (`T*&`)   : Cleanly rebinding caller pointers without double pointers (`T**`).
 *                  5. Function Pointers & Callbacks : Raw function pointers (`R(*)(Args)`) and modern `std::function`.
 *                  6. Member Function Pointers      : Invoking class member methods via pointer offset (`Class::*`).
 *                  7. Smart Pointers & Functions    : Passing and returning `std::unique_ptr` (Ownership transfer).
 *
 * TIME COMPLEXITY  : Function call / Pointer dereference / Invocation: O(1) constant time.
 * SPACE COMPLEXITY : Pointer argument size: Uniform 8 bytes (64-bit) / 4 bytes (32-bit).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <functional>
#include <limits>
#include <string>
#include <utility>

using namespace std;

// =====================================================================================
// HELPER CLASSES & FUNCTIONS FOR FUNCTION POINTER & PARAMETER DEMONSTRATIONS
// =====================================================================================

// Standard functions for function pointer callbacks
int addNumbers(int a, int b) {
    return a + b;
}

int multiplyNumbers(int a, int b) {
    return a * b;
}

// Function taking a raw C-style function pointer callback
void executeRawCallback(int x, int y, int (*callbackPtr)(int, int), const string& operationName) {
    if (callbackPtr != nullptr) {
        int result = callbackPtr(x, y);
        cout << "    [RAW CALLBACK - " << operationName << "] " << x << " and " << y << " = " << result << "\n";
    }
}

// Function taking a modern C++ std::function wrapper
void executeModernCallback(int x, int y, const std::function<int(int, int)>& func, const string& operationName) {
    if (func) {
        int result = func(x, y);
        cout << "    [MODERN CALLBACK - " << operationName << "] " << x << " and " << y << " = " << result << "\n";
    }
}

// Class for member function pointer demonstration
class CalculatorEngine {
public:
    int factor = 10;

    int computeAdd(int x) const {
        return x + factor;
    }

    int computeScale(int x) const {
        return x * factor;
    }
};

// Section 1: Pass-by-Pointer
void modifyViaPointer(int* targetPtr, int increment) {
    if (targetPtr != nullptr) {
        *targetPtr += increment; // Mutates original target memory directly
    }
}

// Section 2: Const Pointers in Arguments
void inspectDataReadOnly(const int* constDataPtr) {
    if (constDataPtr != nullptr) {
        cout << "    [READ-ONLY GUARANTEE] Inspected Value: " << *constDataPtr << "\n";
        // *constDataPtr += 10; // COMPILE ERROR: Cannot mutate value through pointer-to-const!
    }
}

void lockPointerAddress(int* const fixedAddrPtr, int newValue) {
    if (fixedAddrPtr != nullptr) {
        *fixedAddrPtr = newValue; // ALLOWED: Mutates target data
        // int dummy = 0;
        // fixedAddrPtr = &dummy; // COMPILE ERROR: Cannot rebind const pointer address!
    }
}

// Section 3: Returning Pointers
int* createHeapValue(int initialVal) {
    // Valid: Allocates persistent memory on the heap
    return new int(initialVal);
}

// Section 4: Reference-to-Pointer
void rebindCallerPointer(int*& ptrRef, int* newTarget) {
    ptrRef = newTarget; // Rebinds caller's actual single pointer variable directly!
}

// Section 7: Smart Pointers and Functions
std::unique_ptr<int> createSmartResource(int initialVal) {
    return std::make_unique<int>(initialVal); // Transferred ownership on return
}

void processAndConsumeSmartResource(std::unique_ptr<int> resource) {
    if (resource) {
        cout << "    [SMART CONSUMER] Processing owned resource value: " << *resource << "\n";
    }
} // `resource` goes out of scope and automatically frees heap memory HERE!

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for function and pointer analysis (e.g., 25): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting value to 25." << endl;
        userInputValue = 25;
    }

    int primaryVar = userInputValue;
    int secondaryVar = 1000;

    // =====================================================================================
    // 1. PASS-BY-VALUE VS PASS-BY-POINTER
    // =====================================================================================
    cout << "\n================ 1. PASS-BY-POINTER (IN-PLACE MUTATION) ================\n";

    cout << "  - Primary Variable Value Before Call : " << primaryVar << "\n";
    cout << "  - Primary Variable Memory Address   : " << static_cast<const void*>(&primaryVar) << "\n";

    modifyViaPointer(&primaryVar, 50);

    cout << "  - Primary Variable Value After Call  : " << primaryVar 
         << " (Mutated in-place via pointer dereference)\n";

    // =====================================================================================
    // 2. CONST POINTER QUALIFIERS IN FUNCTION PARAMETERS
    // =====================================================================================
    cout << "\n================ 2. CONST QUALIFIERS IN FUNCTION PARAMETERS ================\n";

    cout << "  - Demonstrating Read-Only Data Parameter (`const T*`):\n";
    inspectDataReadOnly(&primaryVar);

    cout << "  - Demonstrating Fixed-Address Parameter (`T* const`):\n";
    lockPointerAddress(&primaryVar, 200);
    cout << "    Value updated via fixed-address pointer: " << primaryVar << "\n";

    // =====================================================================================
    // 3. RETURNING POINTERS FROM FUNCTIONS (HEAP VS STACK)
    // =====================================================================================
    cout << "\n================ 3. RETURNING POINTERS FROM FUNCTIONS ================\n";

    int* heapPtr = createHeapValue(userInputValue * 4);

    cout << "  - Returned Heap Pointer Address      : " << static_cast<const void*>(heapPtr) << "\n";
    cout << "  - Returned Heap Pointer Value        : " << *heapPtr << "\n";
    cout << "  - [SAFETY RULE]: Heap pointers returned from functions MUST be freed by caller.\n";

    delete heapPtr;
    heapPtr = nullptr; // Disarm pointer

    cout << "  - Heap memory freed via `delete` and pointer disarmed to `nullptr`.\n";

    // =====================================================================================
    // 4. REBINDING CALLER POINTERS VIA REFERENCE-TO-POINTER (`T*&`)
    // =====================================================================================
    cout << "\n================ 4. REBINDING CALLER POINTERS VIA `T*&` ================\n";

    int* activePtr = &primaryVar;

    cout << "  - `activePtr` initially points to `primaryVar` Addr: " 
         << static_cast<const void*>(activePtr) << " | Value: " << *activePtr << "\n";

    // Rebind caller's activePtr directly to secondaryVar without double pointers (T**)
    rebindCallerPointer(activePtr, &secondaryVar);

    cout << "  - `activePtr` post `rebindCallerPointer` Addr    : " 
         << static_cast<const void*>(activePtr) << " | Value: " << *activePtr 
         << " (Rebound to `secondaryVar`!)\n";

    // =====================================================================================
    // 5. FUNCTION POINTERS & `std::function` CALLBACKS
    // =====================================================================================
    cout << "\n================ 5. FUNCTION POINTERS & `std::function` ================\n";

    // A. Raw C-Style Function Pointer
    int (*rawFuncPtr)(int, int) = &addNumbers;
    cout << "  - Invoking raw function pointer (`addNumbers`)     : " << rawFuncPtr(userInputValue, 10) << "\n";

    rawFuncPtr = &multiplyNumbers;
    cout << "  - Re-bound raw function pointer (`multiplyNumbers`): " << rawFuncPtr(userInputValue, 10) << "\n";

    // B. Callbacks with Raw Function Pointers
    cout << "\n  - Executing Raw Function Callbacks:\n";
    executeRawCallback(userInputValue, 5, &addNumbers, "Addition");
    executeRawCallback(userInputValue, 5, &multiplyNumbers, "Multiplication");

    // C. Modern std::function and Lambda Callbacks
    cout << "\n  - Executing Modern `std::function` & Lambda Callbacks:\n";
    executeModernCallback(userInputValue, 5, addNumbers, "Std::Function Add");

    // Lambda callback
    auto customSub = [](int a, int b) { return a - b; };
    executeModernCallback(userInputValue, 5, customSub, "Lambda Subtraction");

    // =====================================================================================
    // 6. CLASS MEMBER FUNCTION POINTERS (`Class::*`)
    // =====================================================================================
    cout << "\n================ 6. CLASS MEMBER FUNCTION POINTERS ================\n";

    CalculatorEngine calcObj;
    
    // Member Function Pointer Declaration Syntax
    int (CalculatorEngine::* memberFuncPtr)(int) const = nullptr;

    memberFuncPtr = &CalculatorEngine::computeAdd;
    int addResult = (calcObj.*memberFuncPtr)(userInputValue);
    cout << "  - Executed Member Function Pointer (`computeAdd`)  : " << addResult << "\n";

    memberFuncPtr = &CalculatorEngine::computeScale;
    int scaleResult = (calcObj.*memberFuncPtr)(userInputValue);
    cout << "  - Executed Member Function Pointer (`computeScale`): " << scaleResult << "\n";

    // =====================================================================================
    // 7. MODERN C++ RAII SMART POINTERS & FUNCTIONS
    // =====================================================================================
    cout << "\n================ 7. SMART POINTERS & FUNCTIONS ================\n";

    // Factory pattern with smart pointers
    std::unique_ptr<int> smartRes = createSmartResource(777);

    cout << "  - Factory Created `std::unique_ptr` Heap Addr     : " 
         << static_cast<const void*>(smartRes.get()) << " | Value: " << *smartRes << "\n";

    // Transferring ownership into a consumer function via std::move
    cout << "  - Transferring ownership into `processAndConsumeSmartResource()`...\n";
    processAndConsumeSmartResource(std::move(smartRes));

    cout << "  - Post-Transfer `smartRes` Null Check              : " 
         << (smartRes == nullptr ? "NULL (Ownership Transferred & Freed!)" : "VALID") << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ POINTERS AND FUNCTIONS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Function Interplay    | Syntax / Declaration Example      | Operational Trait & Memory Role   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Pass-by-Pointer       | `void func(int* ptr)`             | Modifies caller's memory in-place |\n"
         << "| Pointer-to-Const Param| `void func(const int* ptr)`       | Read-only access guarantee        |\n"
         << "| Const-Pointer Param   | `void func(int* const ptr)`       | Immutable pointer target address  |\n"
         << "| Return Heap Pointer   | `int* func() { return new int; }` | Returns persistent heap allocation|\n"
         << "| Reference-to-Pointer  | `void func(int*& ptrRef)`         | Rebinds caller's pointer directly |\n"
         << "| Raw Function Pointer  | `int (*ptr)(int, int)`            | Stores code segment function addr |\n"
         << "| Modern std::function  | `std::function<int(int, int)>`    | Callable wrapper (funcs/lambdas)  |\n"
         << "| Member Function Ptr   | `int (Class::*ptr)(int) const`    | Invokes method via object binding |\n"
         << "| Smart Pointer Factory | `unique_ptr<T> func()`            | Transfers RAII heap ownership     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}