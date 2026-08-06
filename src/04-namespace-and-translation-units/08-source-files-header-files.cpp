/*
 * =====================================================================================
 * CONCEPT        : Source Files (.cpp) and Header Files (.h / .hpp) in C++
 * DESCRIPTION    : Comprehensive implementation demonstrating the separation of 
 *                  declarations and definitions across Header and Source files:
 *                  1. Header Files (.h/.hpp)   : Declarations, class interfaces, function 
 *                                                prototypes, and header guards.
 *                  2. Header Guards            : `#pragma once` or `#ifndef` guards preventing 
 *                                                multiple inclusion errors (ODR).
 *                  3. Source Files (.cpp)      : Implementation of declared functions/classes, 
 *                                                enforcing compilation isolation.
 *                  4. Standard vs Local Include: `#include <system>` vs `#include "local.h"`.
 *                  5. Header File Safety       : Avoiding `using namespace` in headers, inline 
 *                                                definitions, and template code handling.
 *
 * TIME COMPLEXITY  : Best Case: O(1) scalar execution.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <utility>

using namespace std;

// =====================================================================================
// SECTION 1: SIMULATED HEADER FILE -> "Calculator.h"
// In a real multi-file project, this block resides in a separate `Calculator.h` or `Calculator.hpp` file.
// =====================================================================================

#ifndef CALCULATOR_H
#define CALCULATOR_H

// HEADER BEST PRACTICE 1: Do NOT put `using namespace std;` in header files!
// Always use fully qualified names (`std::string`, `std::vector`) to avoid polluting global namespace of inclusion sites.

class Calculator {
private:
    double lastResult_;
    std::string calculatorName_;

public:
    // Constructor declaration
    explicit Calculator(std::string name);

    // Function declarations (Prototypes)
    double add(double a, double b);
    double multiply(double a, double b);

    // Inline function definition inside header (Safe for ODR because it is inline/defined inside class body)
    double getLastResult() const {
        return lastResult_;
    }

    [[nodiscard]] const std::string& getName() const {
        return calculatorName_;
    }
};

// Standalone function prototype declaration in header
void printHeaderInfo(const std::string& msg);

#endif // CALCULATOR_H

// =====================================================================================
// SECTION 2: SIMULATED SOURCE FILE -> "Calculator.cpp"
// In a real multi-file project, this block resides in `Calculator.cpp` and begins with `#include "Calculator.h"`.
// =====================================================================================

// #include "Calculator.h" // In a multi-file setup, source file includes its corresponding header

// Implementation of Calculator constructor
Calculator::Calculator(std::string name)
    : lastResult_(0.0), calculatorName_(std::move(name)) {
    std::cout << "  - [Calculator.cpp] Initialized " << calculatorName_ << " instance.\n";
}

// Implementation of add method
double Calculator::add(double a, double b) {
    lastResult_ = a + b;
    return lastResult_;
}

// Implementation of multiply method
double Calculator::multiply(double a, double b) {
    lastResult_ = a * b;
    return lastResult_;
}

// Implementation of standalone function
void printHeaderInfo(const std::string& msg) {
    std::cout << "  - [Calculator.cpp] Executing standalone header function: " << msg << "\n";
}

// =====================================================================================
// SECTION 3: MAIN SOURCE FILE -> "main.cpp"
// Consumes header declarations and orchestrates application execution.
// =====================================================================================

int main() {
    double num1 = 0.0;
    double num2 = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter first operand number (e.g., 15.5): " << flush;
    if (!(cin >> num1)) {
        cout << "Invalid input. Defaulting first operand to 15.5." << endl;
        num1 = 15.5;
    }

    cout << "Enter second operand number (e.g., 4.5): " << flush;
    if (!(cin >> num2)) {
        cout << "Invalid input. Defaulting second operand to 4.5." << endl;
        num2 = 4.5;
    }

    // 1. HEADER FILE GUARDS & DECLARATION DEMO
    cout << "\n================ 1. HEADER FILES (.h / .hpp) ================" << endl;
    cout << "  - Purpose: Contains class declarations, function prototypes, macro definitions,\n"
         << "             and template specifications.\n";
    cout << "  - Header Guards: `#ifndef CALCULATOR_H` / `#define CALCULATOR_H` / `#endif` or\n"
         << "                   `#pragma once` prevents redefinition errors when included multiple times.\n";

    // 2. SOURCE FILE IMPLEMENTATION DEMO
    cout << "\n================ 2. SOURCE FILES (.cpp) ================" << endl;
    cout << "  - Purpose: Implements function and class method bodies defined in headers.\n";
    cout << "  - Compilation: Each .cpp file compiles independently into an object file (.o / .obj).\n";

    Calculator calc("ScientificCalc_v1");

    printHeaderInfo("Header/Source separation active");

    double sum = calc.add(num1, num2);
    cout << "  - calc.add(" << num1 << ", " << num2 << ")      = " << fixed << setprecision(2) << sum << endl;

    double prod = calc.multiply(num1, num2);
    cout << "  - calc.multiply(" << num1 << ", " << num2 << ") = " << fixed << setprecision(2) << prod << endl;

    cout << "  - calc.getLastResult()      = " << calc.getLastResult() << endl;

    // 3. INCLUDE DIRECTIVES & COMPILATION PIPELINE
    cout << "\n================ 3. INCLUDE DIRECTIVES & LINKING ================" << endl;
    cout << "  - `#include <header>` : Searches standard library system directories.\n";
    cout << "  - `#include \"header.h\"`: Searches local project directory first, then system paths.\n";
    cout << "  - Linking Phase: Linker combines object files (`main.o`, `Calculator.o`) into binary.\n";

    cout << "\n================ SOURCE vs HEADER FILES SUMMARY ================" << endl;
    cout << "1. Declarations in `.h`   : Put prototypes, interfaces, templates, and constants in headers." << endl;
    cout << "2. Definitions in `.cpp`  : Put non-inline function/method implementations in source files." << endl;
    cout << "3. Header Guards Always   : Use `#pragma once` or `#ifndef` guards in every header file." << endl;
    cout << "4. No `using` in Headers : Never put `using namespace std;` in header files at global scope." << endl;
    cout << "5. Template Exception    : Template class/function definitions must remain inside header files." << endl;

    return 0;
}