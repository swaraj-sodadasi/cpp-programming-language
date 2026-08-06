/*
 * =====================================================================================
 * CONCEPT        : Inline Functions in C++ (`inline`, Performance, Macros vs Inline)
 * DESCRIPTION    : Comprehensive implementation detailing C++ inline functions:
 *                  1. Concept & Mechanism    : Substituting function call sites directly with 
 *                                             the function body to eliminate function call 
 *                                             overhead (stack frame setup, register saving, branching).
 *                  2. Compiler Hint          : The 'inline' keyword is a request/hint to the compiler, 
 *                                             not an absolute command (compiler makes final decision).
 *                  3. Macro Pitfall Safety   : Replacing unsafe preprocessor macros (#define) with 
 *                                             type-safe, side-effect-free inline functions.
 *                  4. Implicit Inlining      : Member functions defined inside class declarations 
 *                                             and 'constexpr' functions are implicitly inline.
 *                  5. Trade-offs             : Reduced call overhead vs potential instruction cache 
 *                                             bloat if applied to large functions.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct instruction substitution at call site.
 * SPACE COMPLEXITY : Best Case: O(1) — Stack frame allocation eliminated for inlined execution.
 * =====================================================================================
 */

#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. UNSAFE MACRO VS TYPE-SAFE INLINE FUNCTION
// =====================================================================================

// Unsafe C-Style Preprocessor Macro (Susceptible to double-evaluation side effects)
#define UNSAFE_SQUARE(x) ((x) * (x))

// Explicit Inline Function (Type-safe, evaluates parameters exactly once)
inline int safeSquare(int x) {
    return x * x;
}

// Global state and helper function to safely demonstrate double-evaluation without unsequenced modification warnings
int g_sideEffectCounter = 0;

int getNextOperand() {
    return ++g_sideEffectCounter;
}

// Explicit Inline Function for mathematical calculation
inline double calculateCube(double val) {
    return val * val * val;
}

inline int findMax(int a, int b) {
    return (a > b) ? a : b;
}

// =====================================================================================
// 2. IMPLICIT INLINING IN CLASSES
// Member functions implemented INSIDE the class body are implicitly inline.
// =====================================================================================

class Vector2D {
private:
    double x_;
    double y_;

public:
    Vector2D(double x, double y) : x_(x), y_(y) {}

    // Implicitly Inline: Defined inside class declaration
    double getX() const { return x_; }
    double getY() const { return y_; }

    // Explicitly declared inline outside class declaration below
    void scale(double factor);
};

// Explicitly inlined member function implementation outside class declaration
inline void Vector2D::scale(double factor) {
    x_ *= factor;
    y_ *= factor;
}

// =====================================================================================
// 3. CONSTEXPR IMPLICIT INLINING (C++11/14+)
// Functions marked 'constexpr' are implicitly inline.
// =====================================================================================

constexpr double convertCelsiusToFahrenheit(double celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userNum = 0;
    double userTemp = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter an integer for square computation (e.g., 5): " << flush;
    if (!(cin >> userNum)) {
        cout << "Invalid integer input. Defaulting to 5." << endl;
        userNum = 5;
    }

    cout << "Enter a temperature in Celsius (e.g., 25.0): " << flush;
    if (!(cin >> userTemp)) {
        cout << "Invalid temperature input. Defaulting to 25.0." << endl;
        userTemp = 25.0;
    }

    // 1. DEMONSTRATING MACRO SIDE-EFFECT PITFALL VS INLINE SAFETY
    cout << "\n================ 1. MACRO SIDE-EFFECT BUG VS INLINE SAFETY ================" << endl;
    
    // Testing Macro with side-effect function call
    g_sideEffectCounter = userNum;
    cout << "Evaluating UNSAFE_SQUARE(getNextOperand()) macro with side-effect function:" << endl;
    cout << "  Initial counter = " << userNum << endl;
    // Macro expands to ((getNextOperand()) * (getNextOperand())), calling getNextOperand() TWICE!
    int macroResult = UNSAFE_SQUARE(getNextOperand());
    cout << "  Macro Result    = " << macroResult << " | Final counter = " << g_sideEffectCounter 
         << " (BUG: Side-effect function invoked twice due to text expansion!)" << endl;

    // Testing Inline Function with side-effect function call
    g_sideEffectCounter = userNum;
    cout << "\nEvaluating safeSquare(getNextOperand()) inline function with side-effect function:" << endl;
    cout << "  Initial counter = " << userNum << endl;
    // SAFE: getNextOperand() is evaluated exactly once before passing into safeSquare
    int inlineResult = safeSquare(getNextOperand());
    cout << "  Inline Result   = " << inlineResult << " | Final counter = " << g_sideEffectCounter 
         << " (CORRECT: Argument evaluated exactly once)" << endl;

    // 2. DEMONSTRATING BASIC INLINE HELPER FUNCTIONS
    cout << "\n================ 2. EXPLICIT INLINE CALCULATIONS ================" << endl;
    cout << "  Cube of " << userNum << " via calculateCube(): " << calculateCube(userNum) << endl;
    cout << "  Max between " << userNum << " and 10 via findMax(): " << findMax(userNum, 10) << endl;

    // 3. DEMONSTRATING CLASS MEMBER INLINING
    cout << "\n================ 3. IMPLICIT CLASS MEMBER INLINING ================" << endl;
    Vector2D point(3.0, 4.0);
    cout << "  Initial Vector2D : (" << point.getX() << ", " << point.getY() << ")" << endl;
    point.scale(2.5);
    cout << "  Scaled Vector2D  : (" << point.getX() << ", " << point.getY() << ")" << endl;

    // 4. DEMONSTRATING CONSTEXPR INLINING
    cout << "\n================ 4. CONSTEXPR IMPLICIT INLINING ================" << endl;
    double fahrenheit = convertCelsiusToFahrenheit(userTemp);
    cout << "  " << fixed << setprecision(1) << userTemp << " deg C -> " 
         << fahrenheit << " deg F (Evaluated seamlessly)" << endl;

    // 5. INLINE BENCHMARK SIMULATION
    cout << "\n================ 5. CALL OVERHEAD BENCHMARK SIMULATION ================" << endl;
    constexpr size_t ITERATIONS = 10000000;
    
    auto start = chrono::high_resolution_clock::now();
    uint64_t accumulatedSum = 0;
    for (size_t i = 0; i < ITERATIONS; ++i) {
        accumulatedSum += static_cast<uint64_t>(safeSquare(static_cast<int>(i % 100)));
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end - start;

    cout << "  Processed " << ITERATIONS << " inlined function calls in " 
         << fixed << setprecision(3) << elapsed.count() << " ms (Sum: " << accumulatedSum << ")" << endl;

    cout << "\n================ INLINE FUNCTIONS SUMMARY ================" << endl;
    cout << "1. Primary Purpose : Replaces function calls with body code to eliminate stack overhead." << endl;
    cout << "2. Compiler Hint   : 'inline' is a hint; compiler may ignore it for complex/large functions." << endl;
    cout << "3. Macro Alternative: Always prefer 'inline' over '#define' macros for type safety." << endl;
    cout << "4. Automatic Inline: Functions inside class definitions and 'constexpr' are automatically inline." << endl;
    cout << "5. Best Candidates : Short (1-3 lines), frequently called utility or accessor functions." << endl;

    return 0;
}