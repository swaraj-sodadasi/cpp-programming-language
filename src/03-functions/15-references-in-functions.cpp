/*
 * =====================================================================================
 * CONCEPT        : References in Functions in C++ (Lvalue, Const, Return Ref, Rvalue)
 * DESCRIPTION    : Comprehensive implementation detailing reference usage in functions:
 *                  1. Pass by Lvalue Reference (`T&`)   : Direct binding to caller variables; 
 *                                                       enables in-place modification.
 *                  2. Pass by Const Reference (`const T&`): Fast zero-copy read-only access; 
 *                                                       binds to both lvalues and temporaries.
 *                  3. Returning References (`T&`)        : Allows function call expressions to act 
 *                                                       as modifiable lvalues (e.g., container access).
 *                  4. Dangling Reference Safety          : Explaining variable lifetimes and avoiding 
 *                                                       dangling references to popped stack frames.
 *                  5. Pass by Rvalue Reference (`T&&`)   : Enables resource move semantics and temporary 
 *                                                       ownership transfers without copying.
 *
 * TIME COMPLEXITY  : O(1) for reference binding and scalar parameter passing.
 * SPACE COMPLEXITY : O(1) stack allocation (references act as memory aliases/pointers under the hood).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <utility>

using namespace std;

// =====================================================================================
// 1. PASS BY LVALUE REFERENCE (`T&`)
// Modifies caller variables directly without making copies.
// =====================================================================================

void swapValues(int& x, int& y) {
    int temp = x;
    x = y;
    y = temp;
}

void scaleInPlace(int& value, int factor) {
    value *= factor;
}

// =====================================================================================
// 2. PASS BY CONST REFERENCE (`const T&`)
// Provides zero-copy read-only access. Binds to both persistent lvalues and temporary rvalues.
// =====================================================================================

void printLargeMessage(const string& message) {
    // message += " (modified)"; // COMPILER ERROR: Cannot modify through const reference
    cout << "  - [const string&] Memory Address: " << &message 
         << " | Value: \"" << message << "\"" << endl;
}

// =====================================================================================
// 3. RETURNING REFERENCES FROM FUNCTIONS (`T&` / `const T&`)
// Returns an alias to an existing object owned by caller/outer scope.
// =====================================================================================

int& getArrayElement(vector<int>& container, size_t index) {
    // SAFE: 'container' is owned by the caller and outlives this function frame
    return container[index];
}

const string& getLongerString(const string& str1, const string& str2) {
    // SAFE: Returns reference to one of the parameters owned by caller
    return (str1.length() >= str2.length()) ? str1 : str2;
}

// =====================================================================================
// 4. PASS BY RVALUE REFERENCE (`T&&`)
// Binds exclusively to temporary/expiring objects, enabling zero-copy move semantics.
// =====================================================================================

void processTemporaryData(string&& temporaryStr) {
    cout << "  - [string&&] Received temporary object at address: " << &temporaryStr << endl;
    string localOwner = std::move(temporaryStr); // Steal buffer resources without deep copying
    cout << "  - Moved temporary data into local string: \"" << localOwner << "\"" << endl;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int valA = 0;
    int valB = 0;
    string userText;

    // Dynamic input collection with stream flushing
    cout << "Enter first integer valA (e.g., 10): " << flush;
    if (!(cin >> valA)) {
        cout << "Invalid input. Defaulting valA to 10." << endl;
        valA = 10;
    }

    cout << "Enter second integer valB (e.g., 50): " << flush;
    if (!(cin >> valB)) {
        cout << "Invalid input. Defaulting valB to 50." << endl;
        valB = 50;
    }

    cout << "Enter a text string (e.g., Modern C++ References): " << flush;
    cin.ignore();
    getline(cin, userText);
    if (userText.empty()) {
        userText = "Modern C++ References";
    }

    // 1. DEMONSTRATING PASS BY LVALUE REFERENCE
    cout << "\n================ 1. PASS BY LVALUE REFERENCE (T&) ================" << endl;
    cout << "Before swapValues() : valA = " << valA << " | valB = " << valB << endl;
    swapValues(valA, valB);
    cout << "After swapValues()  : valA = " << valA << " | valB = " << valB << " (Mutated in caller)" << endl;

    cout << "\nBefore scaleInPlace(): valA = " << valA << endl;
    scaleInPlace(valA, 3);
    cout << "After scaleInPlace(3): valA = " << valA << endl;

    // 2. DEMONSTRATING PASS BY CONST REFERENCE
    cout << "\n================ 2. PASS BY CONST REFERENCE (const T&) ================" << endl;
    cout << "Passing lvalue variable to const reference:" << endl;
    printLargeMessage(userText);

    cout << "Passing temporary rvalue literal to const reference:" << endl;
    printLargeMessage("Temporary String Literal Example");

    // 3. DEMONSTRATING RETURNING REFERENCES FROM FUNCTIONS
    cout << "\n================ 3. RETURNING REFERENCES FROM FUNCTIONS ================" << endl;
    vector<int> numbers = {100, 200, 300, 400};
    cout << "Container before reference assignment : numbers[1] = " << numbers[1] << endl;

    // Function call expression acts as a modifiable lvalue!
    getArrayElement(numbers, 1) = 999;
    cout << "Container after reference assignment  : numbers[1] = " << numbers[1] << endl;

    string word1 = "Short";
    string word2 = "Much Longer Word";
    const string& longerRef = getLongerString(word1, word2);
    cout << "Longer word returned by const ref     : \"" << longerRef << "\"" << endl;

    // 4. DANGLING REFERENCE PITFALL WARNING
    cout << "\n================ 4. DANGLING REFERENCE SAFETY RULES ================" << endl;
    cout << "  CRITICAL RULE: NEVER return a reference (T& / const T&) to a function-local variable!" << endl;
    cout << "                 Local variables exist on the stack frame and are destroyed upon returning." << endl;
    cout << "                 Returning a reference to a local variable creates a DANGLING REFERENCE," << endl;
    cout << "                 leading to Undefined Behavior (UB) when accessed." << endl;

    // 5. DEMONSTRATING RVALUE REFERENCES & MOVE SEMANTICS
    cout << "\n================ 5. PASS BY RVALUE REFERENCE (T&&) ================" << endl;
    cout << "Passing std::move(userText) to rvalue reference function:" << endl;
    processTemporaryData(std::move(userText));

    cout << "\n================ REFERENCES IN FUNCTIONS SUMMARY ================" << endl;
    cout << "1. Lvalue Ref (`T&`)     : Allows direct mutation of arguments in caller scope." << endl;
    cout << "2. Const Ref (`const T&`): DEFAULT choice for objects/strings; zero-copy read-only access." << endl;
    cout << "3. Return Ref (`T&`)     : Enables functions to return modifiable aliases to outer objects." << endl;
    cout << "4. Rvalue Ref (`T&&`)   : Binds to expiring temporaries to enable move semantics." << endl;

    return 0;
}