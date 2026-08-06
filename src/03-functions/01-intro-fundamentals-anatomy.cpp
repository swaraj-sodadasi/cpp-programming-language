/*
 * =====================================================================================
 * CONCEPT        : Fundamentals and Anatomy of Functions in C++
 * DESCRIPTION    : Comprehensive implementation detailing function mechanics:
 *                  1. Function Anatomy         : Return type, function name, parameter list,
 *                                                function body, and return statement.
 *                  2. Prototype vs Definition  : Forward declarations allowing flexible code layout.
 *                  3. Parameter Passing Modes  : Pass-by-value, pass-by-reference (T&), and
 *                                                pass-by-const-reference (const T&).
 *                  4. Default Arguments        : Providing fallback values for trailing parameters.
 *                  5. Trailing Return Syntax   : Modern C++ trailing return type signature (`auto ... -> Type`).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time arithmetic operations and scalar parameter passing.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation for local variables.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. FUNCTION DECLARATIONS (PROTOTYPES)
// Tells the compiler about function signatures before their actual implementation.
// =====================================================================================

/**
 * @brief Calculates the area of a rectangle.
 * @param length Length dimension (passed by value).
 * @param width Width dimension (passed by value).
 * @return Computed area as double.
 */
double calculateArea(double length, double width);

/**
 * @brief Demonstrates parameter passing mechanisms: value vs reference.
 * @param valCopy Passed by value (modifications do NOT affect original caller variable).
 * @param valRef Passed by reference (modifications directly mutate caller variable).
 */
void demonstrateParameterPassing(int valCopy, int& valRef);

/**
 * @brief Displays a formatted message using default parameters.
 * @param message Primary string content to display (passed by const reference for efficiency).
 * @param prefix Decorative prefix string (Default: "[INFO]").
 * @param repeatCount Number of times to print (Default: 1).
 */
void printMessage(const string& message, const string& prefix = "[INFO]", int repeatCount = 1);

/**
 * @brief Demonstrates modern C++ trailing return type syntax (`auto ... -> ReturnType`).
 * @param a First side length.
 * @param b Second side length.
 * @return Hypotenuse length.
 */
auto computeHypotenuse(double a, double b) -> double;

// =====================================================================================
// MAIN FUNCTION (Program Entry Point)
// =====================================================================================
int main() {
    double inputLength = 0.0;
    double inputWidth = 0.0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter rectangle length (e.g., 5.5): " << flush;
    if (!(cin >> inputLength) || inputLength <= 0) {
        cout << "Invalid length input. Defaulting to 5.0." << endl;
        inputLength = 5.0;
    }

    cout << "Enter rectangle width (e.g., 3.2): " << flush;
    if (!(cin >> inputWidth) || inputWidth <= 0) {
        cout << "Invalid width input. Defaulting to 3.0." << endl;
        inputWidth = 3.0;
    }

    // 2. ANATOMY OF A FUNCTION CALL
    cout << "\n================ 1. BASIC FUNCTION ANATOMY ================" << endl;
    cout << "Calling 'calculateArea(" << inputLength << ", " << inputWidth << ")':" << endl;
    
    // Function Invocation / Call
    double areaResult = calculateArea(inputLength, inputWidth);
    cout << "  - Computed Area = " << fixed << setprecision(2) << areaResult << endl;

    // 3. PARAMETER PASSING MODES (VALUE VS REFERENCE)
    cout << "\n================ 2. PASS BY VALUE VS PASS BY REFERENCE ================" << endl;
    int originalValueA = 10;
    int originalValueB = 10;

    cout << "Before Call : originalValueA (value param) = " << originalValueA 
         << " | originalValueB (ref param) = " << originalValueB << endl;

    demonstrateParameterPassing(originalValueA, originalValueB);

    cout << "After Call  : originalValueA (Unchanged)   = " << originalValueA 
         << " | originalValueB (Mutated)   = " << originalValueB << endl;

    // 4. DEFAULT ARGUMENTS
    cout << "\n================ 3. DEFAULT ARGUMENTS ================" << endl;
    cout << "A. Supplying all arguments:" << endl;
    printMessage("System online", "[SYSTEM]", 2);

    cout << "\nB. Omitting optional trailing arguments (using default values):" << endl;
    printMessage("Task completed successfully");

    // 5. MODERN TRAILING RETURN TYPE SYNTAX
    cout << "\n================ 4. MODERN C++ TRAILING RETURN SYNTAX ================" << endl;
    double sideA = 3.0;
    double sideB = 4.0;
    double hypotenuse = computeHypotenuse(sideA, sideB);

    cout << "computeHypotenuse(" << sideA << ", " << sideB << ") -> " << hypotenuse << endl;

    cout << "\n================ FUNCTION ANATOMY SUMMARY ================" << endl;
    cout << "1. Return Type : Specifies the type of data sent back to caller (or 'void')." << endl;
    cout << "2. Identifier  : Unique name following C++ identifier rules." << endl;
    cout << "3. Parameters  : Input variable declarations enclosed in parentheses `()`." << endl;
    cout << "4. Function Body: Executable statements enclosed in braces `{}`." << endl;
    cout << "5. Return Stmt : Transmits output value back and exits function execution." << endl;

    return 0;
}

// =====================================================================================
// FUNCTION DEFINITIONS (IMPLEMENTATIONS)
// =====================================================================================

/*
 * ANATOMY BREAKDOWN:
 * [Return Type] [Function Name] ( [Parameter List] ) {
 *     // [Function Body]
 *     return [Expression];
 * }
 */
double calculateArea(double length, double width) {
    // Local scope variable inside function body
    double area = length * width; 
    return area; // Transmits calculated value back to caller
}

void demonstrateParameterPassing(int valCopy, int& valRef) {
    cout << "  [Inside Function] Mutating both parameters internally..." << endl;
    valCopy += 50; // Modifies local copy only
    valRef += 50;  // Modifies original variable directly via lvalue reference
}

void printMessage(const string& message, const string& prefix, int repeatCount) {
    for (int i = 0; i < repeatCount; ++i) {
        cout << "  " << prefix << " " << message << endl;
    }
}

auto computeHypotenuse(double a, double b) -> double {
    return std::sqrt((a * a) + (b * b));
}