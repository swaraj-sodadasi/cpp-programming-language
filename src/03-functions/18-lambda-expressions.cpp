/*
 * =====================================================================================
 * CONCEPT        : Lambda Expressions in C++ (Anatomy, Captures, Mutable, Generic)
 * DESCRIPTION    : Comprehensive implementation detailing C++ lambda expressions:
 *                  1. Basic Anatomy          : `[capture](parameters) mutable -> return_type { body }`
 *                  2. Capture Modes          : By-value `[=]`, By-reference `[&]`, explicit mix,
 *                                              and C++14 init-captures `[x = expr]`.
 *                  3. Mutable Lambdas        : Allowing modification of internal value-captured copies.
 *                  4. Generic Lambdas        : Using `auto` parameters (C++14) for polymorphic lambdas.
 *                  5. Storage & STL Usage    : Storing via `std::function`, converting captureless 
 *                                              lambdas to function pointers, and using in STL algorithms.
 *
 * TIME COMPLEXITY  : Best Case: O(1) for scalar execution; O(N log N) when used in STL algorithms.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation for compiler-generated closure objects.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>

using namespace std;

// =====================================================================================
// 1. BASIC ANATOMY & RETURN TYPE DEDUCTION
// =====================================================================================
void demonstrateBasicAnatomy() {
    cout << "\n================ 1. BASIC LAMBDA ANATOMY ================" << endl;

    // Minimal Lambda: [capture](parameters) { body }
    auto simpleGreeting = []() {
        cout << "  - [Basic Lambda] Executed no-argument, no-capture lambda." << endl;
    };
    simpleGreeting(); // Invoking the closure object

    // Trailing Return Type: [capture](parameters) -> ReturnType { body }
    auto computeProduct = [](double x, double y) -> double {
        return x * y;
    };
    cout << "  - Explicit return type product (3.5 * 2.0) = " << computeProduct(3.5, 2.0) << endl;
}

// =====================================================================================
// 2. CAPTURE CLAUSE MECHANICS ([=], [&], Mixed, Init-Capture)
// =====================================================================================
void demonstrateCaptureModes(int externalValue) {
    cout << "\n================ 2. CAPTURE CLAUSE MECHANICS ================" << endl;

    int factor = 5;
    int accumulator = 100;

    // A. Capture by Value [=] or [factor] (Read-only copy inside closure)
    auto multiplyByValue = [factor](int num) {
        return num * factor; // 'factor' is copied at lambda creation time
    };
    cout << "  A. Capture by Value   : " << externalValue << " * factor(" << factor << ") = " 
         << multiplyByValue(externalValue) << endl;

    // B. Capture by Reference [&] or [&accumulator] (Mutates outer variable)
    auto addToAccumulator = [&accumulator](int valueToAdd) {
        accumulator += valueToAdd; // Directly modifies caller's 'accumulator'
    };
    addToAccumulator(externalValue);
    cout << "  B. Capture by Ref     : Added " << externalValue << " to accumulator -> New Total = " 
         << accumulator << endl;

    // C. Mixed Explicit Captures ([=, &accumulator] or [&, factor])
    auto mixedCapture = [=, &accumulator](int multiplier) {
        // 'factor' and 'externalValue' captured by value; 'accumulator' captured by ref
        accumulator += (factor * multiplier);
    };
    mixedCapture(2);
    cout << "  C. Mixed Capture      : Updated accumulator after (factor * 2) -> New Total = " 
         << accumulator << endl;

    // D. C++14 Generalized Init-Capture (Move Semantics in Captures)
    auto uniquePtr = make_unique<string>("HeapResource");
    auto consumePtr = [ptr = std::move(uniquePtr)]() {
        cout << "  D. Move Init-Capture  : Managed resource = \"" << *ptr << "\"" << endl;
    };
    consumePtr(); // Resource moved into closure object
}

// =====================================================================================
// 3. MUTABLE LAMBDAS
// Value-captured variables are 'const' inside operator() by default.
// The 'mutable' keyword permits state modification of local value copies.
// =====================================================================================
void demonstrateMutableLambdas() {
    cout << "\n================ 3. MUTABLE LAMBDAS ================" << endl;

    int counter = 10;

    // Without 'mutable', `counter++` inside the lambda body causes a compiler error.
    auto statefulCounter = [counter]() mutable {
        ++counter; // Modifies the internal copy stored within the closure object
        cout << "  [Inside Mutable Lambda] Internal counter = " << counter << endl;
    };

    statefulCounter();
    statefulCounter();
    cout << "  [Outside Lambda Scope]  Original counter = " << counter 
         << " (Unchanged in outer scope)" << endl;
}

// =====================================================================================
// 4. GENERIC LAMBDAS (C++14 Polymorphic Auto Parameters)
// =====================================================================================
void demonstrateGenericLambdas() {
    cout << "\n================ 4. GENERIC LAMBDAS (`auto` Parameters) ================" << endl;

    // Generic lambda accepts arguments of any type supporting the `+` operator
    auto genericAdder = [](auto a, auto b) {
        return a + b;
    };

    cout << "  - Adding Integers : 10 + 20           = " << genericAdder(10, 20) << endl;
    cout << "  - Adding Doubles  : 3.14 + 2.71       = " << genericAdder(3.14, 2.71) << endl;
    cout << "  - Concatenating   : std::string + str = " 
         << genericAdder(string("Hello, "), string("Lambdas!")) << endl;
}

// =====================================================================================
// 5. STORAGE & STL ALGORITHM INTEGRATION
// =====================================================================================
void demonstrateSTLintegration(int threshold) {
    cout << "\n================ 5. STORAGE & STL ALGORITHM INTEGRATION ================" << endl;

    vector<int> numbers = {5, 12, 18, 3, 25, 8, 30, 14};

    // A. Storage in std::function (Type-erased wrapper)
    std::function<bool(int)> isAboveThreshold = [threshold](int num) {
        return num > threshold;
    };

    // B. Captureless Lambda to Raw Function Pointer Conversion
    int (*rawAddPtr)(int, int) = [](int a, int b) { return a + b; };
    cout << "  A. Captureless Lambda -> Function Pointer (10 + 5) = " << rawAddPtr(10, 5) << endl;

    // C. Using Lambda in std::count_if
    ptrdiff_t matchCount = count_if(numbers.begin(), numbers.end(), isAboveThreshold);
    cout << "  B. std::count_if  : Numbers strictly greater than " << threshold 
         << " = " << matchCount << endl;

    // D. Using Lambda in std::sort (Inline custom comparator)
    sort(numbers.begin(), numbers.end(), [](int a, int b) {
        return a > b; // Descending order
    });

    cout << "  C. Sorted Array   : ";
    for (int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userThreshold = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a threshold integer for STL lambda filter (e.g., 10): " << flush;
    if (!(cin >> userThreshold)) {
        cout << "Invalid input. Defaulting threshold to 10." << endl;
        userThreshold = 10;
    }

    // 1. BASIC ANATOMY
    demonstrateBasicAnatomy();

    // 2. CAPTURE MODES
    demonstrateCaptureModes(userThreshold);

    // 3. MUTABLE LAMBDAS
    demonstrateMutableLambdas();

    // 4. GENERIC LAMBDAS
    demonstrateGenericLambdas();

    // 5. STORAGE & STL INTEGRATION
    demonstrateSTLintegration(userThreshold);

    cout << "\n================ LAMBDA EXPRESSIONS SUMMARY ================" << endl;
    cout << "1. Syntax Structure : `[capture](parameters) mutable -> ReturnType { body }`." << endl;
    cout << "2. Capture Modes    : `[=]` (value copy), `[&]` (reference alias), `[x = expr]` (init/move)." << endl;
    cout << "3. Mutable Keyword  : Allows modifying value-captured internal copies inside closure body." << endl;
    cout << "4. Under The Hood   : Compilers translate lambdas into anonymous functor classes with `operator()`." << endl;
    cout << "5. Function Pointer : Captureless lambdas implicitly convert to standard C-style function pointers." << endl;

    return 0;
}