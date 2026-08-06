/*
 * =====================================================================================
 * CONCEPT        : Function Parameters in C++ (Value, Reference, Const Ref, Pointer, Defaults)
 * DESCRIPTION    : Comprehensive implementation detailing function parameter mechanisms:
 *                  1. Pass by Value          : Creates local copies of arguments; isolates caller data.
 *                  2. Pass by Lvalue Ref (T&): Binds directly to caller variables; enables in-place mutation.
 *                  3. Pass by Const Ref     : Provides zero-copy read-only access for efficient object passing.
 *                  4. Pass by Pointer (T*)   : Allows explicit memory address passing and nullable parameters.
 *                  5. Default Parameters     : Fallback parameter values for trailing arguments.
 *                  6. Initializer Lists      : Flexibly passing variable-length element lists (std::initializer_list).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant-time parameter passing and scalar operations.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation for local variables.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <initializer_list>
#include <iomanip>

using namespace std;

// 1. PASS BY VALUE
// Creates an independent local copy. Changes inside function do NOT affect caller.
void demonstratePassByValue(int val) {
    val += 100;
    cout << "  [Inside passByValue] Local 'val' modified to: " << val << endl;
}

// 2. PASS BY LVALUE REFERENCE (T&)
// Binds to original variable. Changes directly mutate caller's variable.
void demonstratePassByReference(int& ref) {
    ref += 100;
    cout << "  [Inside passByReference] Caller variable mutated to: " << ref << endl;
}

// 3. PASS BY CONST REFERENCE (const T&)
// Fast zero-copy read-only access; prevents expensive object copies and accidental mutations.
void demonstratePassByConstReference(const string& str) {
    // str += " modified"; // ERROR: Cannot modify const reference
    cout << "  [Inside passByConstReference] Efficient read-only access: \"" << str << "\"" << endl;
}

// 4. PASS BY POINTER (T*)
// Accepts memory address; allows passing nullptr to represent optional parameters.
void demonstratePassByPointer(int* ptr) {
    if (ptr != nullptr) {
        *ptr *= 2;
        cout << "  [Inside passByPointer] Value at pointer address updated to: " << *ptr << endl;
    } else {
        cout << "  [Inside passByPointer] Parameter is nullptr (no action taken)." << endl;
    }
}

// 5. DEFAULT PARAMETERS
// Trailing parameters with fallback values if caller omits them.
void demonstrateDefaultParameters(const string& item, int quantity = 1, double unitPrice = 9.99) {
    double total = quantity * unitPrice;
    cout << "  Item: " << left << setw(10) << item 
         << " | Qty: " << quantity 
         << " | Total: $" << fixed << setprecision(2) << total << endl;
}

// 6. std::initializer_list FOR VARIABLE-LENGTH PARAMETERS
// Allows passing an arbitrary list of homogeneous elements enclosed in curly braces `{}`.
int computeSum(initializer_list<int> numbers) {
    int sum = 0;
    for (int num : numbers) {
        sum += num;
    }
    return sum;
}

int main() {
    int userNum = 0;
    string userText;

    // Dynamic input collection with stream flushing
    cout << "Enter an integer value (e.g., 25): " << flush;
    if (!(cin >> userNum)) {
        cout << "Invalid integer input. Defaulting to 25." << endl;
        userNum = 25;
    }

    cout << "Enter a string label (e.g., C++ Parameters): " << flush;
    cin.ignore(); // Flush newline left in input stream
    getline(cin, userText);
    if (userText.empty()) {
        userText = "C++ Parameters";
    }

    // 1. DEMONSTRATING PASS BY VALUE
    cout << "\n================ 1. PASS BY VALUE ================" << endl;
    int numValue = userNum;
    cout << "Before call : numValue = " << numValue << endl;
    demonstratePassByValue(numValue);
    cout << "After call  : numValue = " << numValue << " (Unchanged in caller scope)" << endl;

    // 2. DEMONSTRATING PASS BY REFERENCE
    cout << "\n================ 2. PASS BY REFERENCE (T&) ================" << endl;
    int numRef = userNum;
    cout << "Before call : numRef = " << numRef << endl;
    demonstratePassByReference(numRef);
    cout << "After call  : numRef = " << numRef << " (Mutated in caller scope)" << endl;

    // 3. DEMONSTRATING PASS BY CONST REFERENCE
    cout << "\n================ 3. PASS BY CONST REFERENCE (const T&) ================" << endl;
    demonstratePassByConstReference(userText);

    // 4. DEMONSTRATING PASS BY POINTER
    cout << "\n================ 4. PASS BY POINTER (T*) ================" << endl;
    int targetVal = userNum;
    cout << "Before call : targetVal = " << targetVal << endl;
    demonstratePassByPointer(&targetVal); // Passing memory address
    cout << "After call  : targetVal = " << targetVal << " (Mutated via dereferenced pointer)" << endl;
    
    // Demonstrating nullable pointer handling
    demonstratePassByPointer(nullptr);

    // 5. DEMONSTRATING DEFAULT PARAMETERS
    cout << "\n================ 5. DEFAULT PARAMETERS ================" << endl;
    cout << "A. Omitting trailing optional parameters:" << endl;
    demonstrateDefaultParameters("Widget");

    cout << "B. Overriding quantity, relying on default unit price:" << endl;
    demonstrateDefaultParameters("Gadget", 3);

    cout << "C. Explicitly providing all parameters:" << endl;
    demonstrateDefaultParameters("Tool", 5, 14.50);

    // 6. DEMONSTRATING INITIALIZER LIST PARAMETERS
    cout << "\n================ 6. INITIALIZER LIST PARAMETERS ================" << endl;
    int sumResult = computeSum({10, 20, 30, 40, 50});
    cout << "Sum of {10, 20, 30, 40, 50} via std::initializer_list = " << sumResult << endl;

    cout << "\n================ FUNCTION PARAMETERS SUMMARY ================" << endl;
    cout << "1. Pass by Value (T)      : Safe for primitives; isolates caller from mutations." << endl;
    cout << "2. Pass by Reference (T&) : Allows direct mutation of caller arguments." << endl;
    cout << "3. Pass by Const Ref (const T&) : Best for objects/strings; zero-copy & read-only." << endl;
    cout << "4. Pass by Pointer (T*)   : Enables passing memory addresses and optional/null arguments." << endl;
    cout << "5. Default Parameters     : Simplifies call sites by providing trailing fallback values." << endl;
    cout << "6. initializer_list<T>    : Enables passing variable-sized curly-brace argument sets." << endl;

    return 0;
}