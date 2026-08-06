/*
 * =====================================================================================
 * CONCEPT        : Expressions in C++
 * DESCRIPTION    : Comprehensive implementation explaining C++ expression mechanics:
 *                  1. Fundamental & Complex Expressions: Arithmetic, Relational, Logical, 
 *                     Bitwise, Assignment, Comma, and Ternary expressions.
 *                  2. Operator Precedence & Associativity: Controlling evaluation order.
 *                  3. Type Conversions: Implicit type promotion vs explicit static_cast.
 *                  4. Modern Value Categories: Lvalues, Prvalues, and Xvalues inspected 
 *                     via decltype and standard type traits.
 *                  5. Constant Expressions: Compile-time expression evaluation using constexpr.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time execution for dynamic scalar expressions.
 * SPACE COMPLEXITY : Best Case: O(1) — Memory footprint bounded by primitive stack variables.
 * =====================================================================================
 */

#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <utility>

using namespace std;

/**
 * @brief Template utility to inspect modern C++ Value Categories (Lvalue, Prvalue, Xvalue).
 * @tparam T Type deduction parameter capturing expression value category.
 * @return String description of the evaluated expression category.
 */
template <typename T>
constexpr string_view getValueCategory() noexcept {
    if constexpr (is_lvalue_reference_v<T>) {
        return "Lvalue (Has identity, address accessible, cannot be moved)";
    } else if constexpr (is_rvalue_reference_v<T>) {
        return "Xvalue (eXpiring value: Has identity, move-eligible temporary)";
    } else {
        return "Prvalue (Pure rvalue: No identity, compute-only temporary literal/value)";
    }
}

/**
 * @brief Compile-time constant expression evaluator.
 * @param base Base multiplier operand.
 * @param exponent Integral power factor.
 * @return Evaluated exponentiation result at compile-time when passed constant expressions.
 */
constexpr double computeCompileTimePower(double base, int exponent) noexcept {
    double result = 1.0;
    for (int i = 0; i < exponent; ++i) {
        result *= base;
    }
    return result;
}

int main() {
    int operandA = 0;
    int operandB = 0;
    double operandC = 0.0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter integer operand A: " << flush;
    if (!(cin >> operandA)) {
        cout << "Invalid input for operand A. Terminating." << endl;
        return 0;
    }

    cout << "Enter integer operand B: " << flush;
    if (!(cin >> operandB)) {
        cout << "Invalid input for operand B. Terminating." << endl;
        return 0;
    }

    cout << "Enter floating-point operand C: " << flush;
    if (!(cin >> operandC)) {
        cout << "Invalid input for operand C. Terminating." << endl;
        return 0;
    }

    // 2. OPERATOR PRECEDENCE & ASSOCIATIVITY IN EXPRESSIONS
    cout << "\n================ 1. OPERATOR PRECEDENCE & ASSOCIATIVITY ================" << endl;
    // Multiplication holds higher precedence than addition (* evaluated before +)
    int defaultPrecedence = operandA + operandB * 2;
    // Parentheses explicitly override default precedence constraints
    int customPrecedence = (operandA + operandB) * 2;

    cout << "Expression: operandA + operandB * 2     -> Result: " << defaultPrecedence << endl;
    cout << "Expression: (operandA + operandB) * 2   -> Result: " << customPrecedence << endl;

    // 3. IMPLICIT PROMOTION & EXPLICIT TYPE CAST EXPRESSIONS
    cout << "\n================ 2. IMPLICIT PROMOTION & EXPLICIT CASTS ================" << endl;
    // Implicit type promotion: int operandA is implicitly promoted to double to match operandC
    auto implicitPromotedResult = operandA + operandC;
    // Explicit static_cast prevents integer truncation during division
    double explicitCastResult = (operandB != 0) ? static_cast<double>(operandA) / operandB : 0.0;

    cout << "Implicit Promotion (int + double)      -> Result: " << implicitPromotedResult 
         << " | Type: " << typeid(implicitPromotedResult).name() << endl;
    cout << "Explicit Cast (static_cast<double>(A)/B)-> Result: " << explicitCastResult << endl;

    // 4. LOGICAL & SHORT-CIRCUIT EVALUATION EXPRESSIONS
    cout << "\n================ 3. LOGICAL SHORT-CIRCUIT EXPRESSIONS ================" << endl;
    bool sideEffectTriggered = false;
    // Short-circuiting: If left operand evaluates to false, right operand expression is NEVER evaluated
    bool shortCircuitResult = (operandA > 1000) && (sideEffectTriggered = true);

    cout << "Short-Circuit Test ((A > 1000) && sideEffect) -> Result: " << boolalpha << shortCircuitResult << endl;
    cout << "Was Right-Hand Side Expression Evaluated?     -> " 
         << (sideEffectTriggered ? "Yes" : "No (Short-Circuited)") << endl;

    // 5. TERNARY & COMMA ASSIGNMENT EXPRESSIONS
    cout << "\n================ 4. TERNARY & COMMA EXPRESSIONS ================" << endl;
    // Conditional expression (Ternary operator)
    int maxVal = (operandA >= operandB) ? operandA : operandB;
    // Comma expression: Evaluates left expression, discards result, returns rightmost expression
    int commaExpressionResult = (operandA += 5, operandA * operandB);

    cout << "Ternary Max Expression (A >= B ? A : B) -> Result: " << maxVal << endl;
    cout << "Comma Expression (A += 5, A * B)         -> Result: " << commaExpressionResult 
         << " (Updated A: " << operandA << ")" << endl;

    // 6. MODERN C++ VALUE CATEGORIES (LVALUE, PRVALUE, XVALUE)
    cout << "\n================ 5. VALUE CATEGORIES (LVALUE / PRVALUE / XVALUE) ================" << endl;
    int stateVariable = operandA;

    // decltype((expr)) inspection preserves reference and value category semantics
    cout << "Expression 'stateVariable' (NamedVar)    -> " << getValueCategory<decltype((stateVariable))>() << endl;
    cout << "Expression 'stateVariable + 10' (Literal)-> " << getValueCategory<decltype((stateVariable + 10))>() << endl;
    cout << "Expression 'std::move(stateVariable)'    -> " << getValueCategory<decltype((std::move(stateVariable)))>() << endl;

    // 7. COMPILE-TIME CONSTANT EXPRESSIONS (constexpr)
    cout << "\n================ 6. COMPILE-TIME CONSTANT EXPRESSIONS ================" << endl;
    constexpr double compileTimePower = computeCompileTimePower(2.0, 10);
    cout << "Compile-Time Expression (constexpr 2.0^10) -> Result: " << compileTimePower << endl;

    return 0;
}