/*
 * =====================================================================================
 * CONCEPT        : Function Overloading in C++ (Static Polymorphism)
 * DESCRIPTION    : Comprehensive implementation explaining C++ function overloading:
 *                  1. Definition             : Multiple functions in the same scope sharing the same name
 *                                              but having distinct parameter lists (signatures).
 *                  2. Overload Resolution    : Compile-time selection of the best-matching candidate function
 *                                              via C++ Name Mangling.
 *                  3. Disambiguation Rules   : Overloading by parameter count, parameter types, 
 *                                              and parameter order.
 *                  4. Invalid Overloads      : Why return type ALONE is insufficient for overloading.
 *                  5. Const Overloading      : Overloading based on const reference vs non-const reference.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Compile-time overload resolution introduces zero runtime overhead.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. OVERLOADING BY PARAMETER TYPES
// The compiler selects the target function based on the data type of arguments passed.
// =====================================================================================

void displayData(int value) {
    cout << "  [Overload: int] Value = " << value << endl;
}

void displayData(double value) {
    cout << "  [Overload: double] Value = " << fixed << setprecision(2) << value << endl;
}

void displayData(const string& value) {
    cout << "  [Overload: const string&] Value = \"" << value << "\"" << endl;
}

// =====================================================================================
// 2. OVERLOADING BY PARAMETER COUNT
// Functions differ in the number of parameters they accept.
// =====================================================================================

int calculateSum(int a, int b) {
    cout << "  [Overload: 2 int params] " << a << " + " << b << " = ";
    return a + b;
}

int calculateSum(int a, int b, int c) {
    cout << "  [Overload: 3 int params] " << a << " + " << b << " + " << c << " = ";
    return a + b + c;
}

// =====================================================================================
// 3. OVERLOADING BY PARAMETER ORDER / SEQUENCE
// Differing parameter type positions allow distinct overloads.
// =====================================================================================

void printItem(int id, double price) {
    cout << "  [Order: (int, double)] ID: " << id << " | Price: $" << fixed << setprecision(2) << price << endl;
}

void printItem(double price, int id) {
    cout << "  [Order: (double, int)] Price: $" << fixed << setprecision(2) << price << " | ID: " << id << endl;
}

// =====================================================================================
// 4. OVERLOADING BY CONST REFERENCE (REF QUALIFIERS)
// Non-const references bind to modifiable lvalues; const references bind to temporaries/const objects.
// =====================================================================================

void processMessage(string& msg) {
    cout << "  [Overload: non-const string&] Modifiable reference: \"" << msg << "\"" << endl;
    msg += " (Modified)";
}

void processMessage(const string& msg) {
    cout << "  [Overload: const string&] Read-only / Temporary string: \"" << msg << "\"" << endl;
}

// =====================================================================================
// INVALID OVERLOAD EXAMPLE (COMMENTED OUT TO PREVENT COMPILER ERRORS)
// =====================================================================================
/*
 * int getValue() { return 10; }
 * double getValue() { return 10.5; } 
 * 
 * ERROR: Functions cannot be overloaded by Return Type alone!
 * Reason: Call site like `getValue();` makes it ambiguous which return type is expected.
 */

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInt = 0;
    double userDouble = 0.0;
    string userText;

    // Dynamic input collection with stream flushing
    cout << "Enter an integer value (e.g., 42): " << flush;
    if (!(cin >> userInt)) {
        cout << "Invalid integer. Defaulting to 42." << endl;
        userInt = 42;
    }

    cout << "Enter a floating-point value (e.g., 99.95): " << flush;
    if (!(cin >> userDouble)) {
        cout << "Invalid double. Defaulting to 99.95." << endl;
        userDouble = 99.95;
    }

    cout << "Enter a string label (e.g., Overloading): " << flush;
    cin.ignore();
    getline(cin, userText);
    if (userText.empty()) {
        userText = "Overloading";
    }

    // 1. DEMONSTRATING TYPE-BASED OVERLOAD RESOLUTION
    cout << "\n================ 1. OVERLOADING BY PARAMETER TYPE ================" << endl;
    displayData(userInt);      // Resolves to displayData(int)
    displayData(userDouble);   // Resolves to displayData(double)
    displayData(userText);     // Resolves to displayData(const string&)

    // 2. DEMONSTRATING PARAMETER COUNT OVERLOADS
    cout << "\n================ 2. OVERLOADING BY PARAMETER COUNT ================" << endl;
    cout << calculateSum(userInt, 10) << endl;              // Resolves to 2-param version
    cout << calculateSum(userInt, 10, 20) << endl;          // Resolves to 3-param version

    // 3. DEMONSTRATING PARAMETER ORDER OVERLOADS
    cout << "\n================ 3. OVERLOADING BY PARAMETER ORDER ================" << endl;
    printItem(userInt, userDouble);    // Resolves to (int, double)
    printItem(userDouble, userInt);    // Resolves to (double, int)

    // 4. DEMONSTRATING CONST REFERENCE OVERLOAD RESOLUTION
    cout << "\n================ 4. CONST VS NON-CONST REFERENCE OVERLOADING ================" << endl;
    string mutableMsg = userText;
    processMessage(mutableMsg);        // Binds to non-const reference overload (modifies in-place)
    cout << "  - After call: mutableMsg = \"" << mutableMsg << "\"" << endl;

    processMessage("Temporary String Literal"); // Binds to const reference overload

    cout << "\n================ FUNCTION OVERLOADING SUMMARY ================" << endl;
    cout << "1. Signature Matrix : Overloading relies on parameter COUNT, TYPES, or ORDER." << endl;
    cout << "2. Return Type Rule  : Return type alone CANNOT differentiate overloaded functions." << endl;
    cout << "3. Static Dispatch   : Overload matching occurs at COMPILE-TIME with ZERO runtime penalty." << endl;
    cout << "4. Ambiguity Danger  : Be cautious combining default arguments with overloading (causes ambiguity)." << endl;

    return 0;
}