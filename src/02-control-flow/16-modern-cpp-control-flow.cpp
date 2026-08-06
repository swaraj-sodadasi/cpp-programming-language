/*
 * =====================================================================================
 * CONCEPT        : Modern C++ Control Flow Features (C++17 / C++20 Portable)
 * DESCRIPTION    : Comprehensive implementation showcasing modern C++ control flow idioms:
 *                  1. Scoped Init-Statements    : C++17 'if (init; cond)' and 'switch (init; cond)'.
 *                  2. Scoped Range Iteration    : Confining dataset lifetime to loop execution scope.
 *                  3. Compile-Time Selection    : C++17 'if constexpr' for static type branching.
 *                  4. Contextual Evaluation     : Feature-safe 'constexpr' function evaluation.
 *                  5. Variant Pattern Matching  : C++17 type-safe branching using 'std::visit'.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear execution over dynamic collections.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <iomanip>
#include <type_traits>

using namespace std;

// HELPER VISITOR STRUCT FOR VARIANT PATTERN MATCHING (C++17)
template<class... Ts> 
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts> 
overloaded(Ts...) -> overloaded<Ts...>;

/**
 * @brief Demonstrates C++17 compile-time conditional evaluation ('if constexpr').
 */
template<typename T>
void processValueByStaticType(T item) {
    if constexpr (std::is_integral_v<T>) {
        cout << "  - [if constexpr] Integral branch compiled: " << item << " * 2 = " << (item * 2) << endl;
    } else if constexpr (std::is_floating_point_v<T>) {
        cout << "  - [if constexpr] Floating-point branch compiled: " << fixed << setprecision(2) << item << endl;
    } else {
        cout << "  - [if constexpr] Generic fallback branch compiled for non-numeric type." << endl;
    }
}

/**
 * @brief Demonstrates constexpr computation with feature-test safety across standards.
 */
constexpr int evaluateContextualComputation(int val) {
#if defined(__cpp_lib_is_constant_evaluated) && __cpp_lib_is_constant_evaluated >= 201811L
    if (std::is_constant_evaluated()) {
        return val * 10;
    }
#endif
    return val * 2;
}

int main() {
    int inputVal = 0;
    double inputFloat = 0.0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter an integer operand (e.g., 5 or -3): " << flush;
    if (!(cin >> inputVal)) {
        cout << "Invalid integer input. Defaulting to 5." << endl;
        inputVal = 5;
    }

    cout << "Enter a floating-point operand (e.g., 12.34): " << flush;
    if (!(cin >> inputFloat)) {
        cout << "Invalid float input. Defaulting to 12.34." << endl;
        inputFloat = 12.34;
    }

    // 2. C++17 SCOPED INIT-STATEMENTS (IF & SWITCH)
    cout << "\n================ 1. C++17 SCOPED INIT-STATEMENTS ================" << endl;
    
    // Variable 'scaledVal' is local exclusively to this if/else block
    if (int scaledVal = inputVal * 3; scaledVal > 10) {
        cout << "  - [if init] 'scaledVal' (" << scaledVal << ") > 10 (Branch evaluated true)." << endl;
    } else {
        cout << "  - [if init] 'scaledVal' (" << scaledVal << ") <= 10 (Branch evaluated false)." << endl;
    }

    // Variable 'modKey' is local exclusively to this switch block
    switch (int modKey = (inputVal < 0 ? -inputVal : inputVal) % 3; modKey) {
        case 0:
            cout << "  - [switch init] Remainder key is 0 (Evenly divisible by 3)." << endl;
            break;
        case 1:
            cout << "  - [switch init] Remainder key is 1." << endl;
            break;
        default:
            cout << "  - [switch init] Remainder key is 2." << endl;
            break;
    }

    // 3. SCOPED RANGE ITERATION (CONFINING CONTAINER LIFETIME)
    cout << "\n================ 2. SCOPED CONTAINER RANGE ITERATION ================" << endl;
    
    // Confining transient dataset lifetime strictly using block scope
    {
        auto dataset = vector<int>{inputVal, inputVal + 2, inputVal + 4};
        for (const auto& elem : dataset) {
            cout << "  - [Scoped Range-For] Traversed element: " << elem << endl;
        }
    } // 'dataset' is automatically cleaned up and out of scope here

    // 4. CONDITIONAL BRANCH EXECUTION
    cout << "\n================ 3. CONDITIONAL BRANCH EXECUTION ================" << endl;
    
    if (inputVal >= 0) {
        cout << "  - [Positive Branch] Non-negative value processed: " << inputVal << endl;
    } else {
        cout << "  - [Negative Branch] Negative value processed: " << inputVal << endl;
    }

    // 5. COMPILE-TIME CONTROL FLOW (if constexpr & constexpr functions)
    cout << "\n================ 4. COMPILE-TIME CONTROL FLOW ================" << endl;
    
    // C++17 'if constexpr' dispatching based on static type traits
    processValueByStaticType(inputVal);
    processValueByStaticType(inputFloat);

    // Feature-safe constexpr function evaluation
    constexpr int compileTimeResult = evaluateContextualComputation(5);
    int runtimeResult = evaluateContextualComputation(inputVal);

    cout << "  - [constexpr function] Compile-time evaluateContextualComputation(5) = " << compileTimeResult << endl;
    cout << "  - [constexpr function] Runtime evaluateContextualComputation(" << inputVal << ") = " << runtimeResult << endl;

    // 6. C++17 TYPE-SAFE PATTERN MATCHING (std::variant & std::visit)
    cout << "\n================ 5. TYPE-SAFE PATTERN MATCHING ================" << endl;
    
    using DynamicCommand = variant<int, double, string>;
    vector<DynamicCommand> commandQueue = {inputVal, inputFloat, string("Modern C++ Controls")};

    for (const auto& cmd : commandQueue) {
        visit(overloaded {
            [](int i) { cout << "  - [std::visit] Handled Integer Command: " << i << endl; },
            [](double d) { cout << "  - [std::visit] Handled Double Command: " << fixed << setprecision(2) << d << endl; },
            [](const string& s) { cout << "  - [std::visit] Handled String Command: \"" << s << "\"" << endl; }
        }, cmd);
    }

    cout << "\n================ MODERN C++ CONTROL FLOW SUMMARY ================" << endl;
    cout << "1. Scoped Inits   : 'if (init; cond)' and 'switch (init; cond)' limit variable scope to the control block." << endl;
    cout << "2. Scoped Loops   : Explicit `{ init; for (...) }` blocks keep transient collections strictly local." << endl;
    cout << "3. Metaprogramming: 'if constexpr' streamlines compile-time dispatching without overhead." << endl;
    cout << "4. Pattern Match  : 'std::variant' with 'std::visit' replaces fragile casting hierarchies." << endl;

    return 0;
}