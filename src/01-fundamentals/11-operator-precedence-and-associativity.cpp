/*
 * =====================================================================================
 * CONCEPT        : Operator Precedence and Associativity in C++
 * DESCRIPTION    : Comprehensive implementation demonstrating how C++ evaluates complex expressions:
 *                  1. Operator Precedence  : Dictates which operator binds first when operators of 
 *                                            different priority levels are present (e.g., * before +).
 *                  2. Associativity        : Dictates the evaluation direction (Left-to-Right vs Right-to-Left)
 *                                            when operators share the exact same precedence level.
 *                  3. Gotchas & Pitfalls   : Bitwise vs Relational precedence (e.g., & vs ==),
 *                                            Shift vs Arithmetic precedence (e.g., << vs +).
 *                  4. Parentheses '()'     : Explicitly overriding default precedence and associativity rules.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct arithmetic and logical evaluation of dynamic scalar inputs.
 * SPACE COMPLEXITY : Best Case: O(1) — Memory footprint bounded by primitive stack variables.
 * =====================================================================================
 */

#include <iostream>

using namespace std;

int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter integer value for a: " << flush;
    if (!(cin >> a)) {
        cout << "Invalid input for a. Program terminated." << endl;
        return 0;
    }

    cout << "Enter integer value for b: " << flush;
    if (!(cin >> b)) {
        cout << "Invalid input for b. Program terminated." << endl;
        return 0;
    }

    cout << "Enter integer value for c: " << flush;
    if (!(cin >> c)) {
        cout << "Invalid input for c. Program terminated." << endl;
        return 0;
    }

    cout << "Enter integer value for d: " << flush;
    if (!(cin >> d)) {
        cout << "Invalid input for d. Program terminated." << endl;
        return 0;
    }

    // 2. OPERATOR PRECEDENCE (DIFFERENT PRECEDENCE LEVELS)
    cout << "\n================ 1. OPERATOR PRECEDENCE ================" << endl;
    
    // Arithmetic Precedence: '*' and '/' have higher precedence than '+' and '-'
    // Expression: a + b * c  ==> Evaluates as: a + (b * c)
    int precedenceArithDefault = a + b * c;
    int precedenceArithOverride = (a + b) * c;
    
    cout << "a + b * c   (Default: * before +)   -> Result: " << precedenceArithDefault << endl;
    cout << "(a + b) * c (Override with ())     -> Result: " << precedenceArithOverride << endl;

    // Logical Precedence: '&&' (Logical AND) has higher precedence than '||' (Logical OR)
    // Expression: a || b && c ==> Evaluates as: a || (b && c)
    bool precedenceLogicalDefault = (a != 0) || ((b != 0) && (c != 0));
    bool precedenceLogicalOverride = ((a != 0) || (b != 0)) && (c != 0);

    cout << boolalpha;
    cout << "a || b && c (Default: && before ||) -> Result: " << precedenceLogicalDefault << endl;
    cout << "(a || b) && c (Override with ())   -> Result: " << precedenceLogicalOverride << endl;

    // 3. OPERATOR ASSOCIATIVITY (SAME PRECEDENCE LEVEL)
    cout << "\n================ 2. OPERATOR ASSOCIATIVITY ================" << endl;

    // Left-to-Right Associativity: Arithmetic operators (+, -, *, /) group Left-to-Right
    // Expression: a - b - c ==> Evaluates as: (a - b) - c
    int assocLeftToRightDefault = a - b - c;
    int assocLeftToRightOverride = a - (b - c);

    cout << "a - b - c   (Left-to-Right: (a-b)-c) -> Result: " << assocLeftToRightDefault << endl;
    cout << "a - (b - c) (Right-Grouped Override) -> Result: " << assocLeftToRightOverride << endl;

    // Right-to-Left Associativity: Assignment (=) groups Right-to-Left
    // Expression: var1 = var2 = var3 ==> Evaluates as: var1 = (var2 = var3)
    int var1 = a, var2 = b, var3 = c;
    var1 = var2 = var3; // Assigns var3 to var2, then var2's new value to var1
    cout << "Assignment Chaining (v1 = v2 = v3)   -> v1: " << var1 << ", v2: " << var2 << ", v3: " << var3 << endl;

    // Right-to-Left Associativity: Conditional / Ternary Operator (?:)
    // Expression: a ? b : c ? d : 0 ==> Evaluates as: a ? b : (c ? d : 0)
    int assocTernaryDefault = (a > 0) ? b : ((c > 0) ? d : 0);
    cout << "Ternary Chaining (a ? b : c ? d : 0) -> Result: " << assocTernaryDefault << endl;

    // 4. COMMON PRECEDENCE GOTCHAS IN C++
    cout << "\n================ 3. COMMON PRECEDENCE GOTCHAS ================" << endl;

    // GOTCHA 1: Bitwise operators (&, |, ^) have LOWER precedence than Relational operators (==, !=, <, >)
    // Expression: a & b == c ==> Binds as: a & (b == c) [NOT (a & b) == c]
    // Parentheses added explicitly below to avoid compiler warnings (-Wparentheses) while demonstrating behavior
    int bitwiseRelationalDefault = a & (b == c); 
    int bitwiseRelationalCorrect = (a & b) == c;

    cout << "a & (b == c) (Default: == before &) -> Result: " << bitwiseRelationalDefault << endl;
    cout << "(a & b) == c (Intended Bitwise Eq) -> Result: " << bitwiseRelationalCorrect << endl;

    // GOTCHA 2: Bit Shift operators (<<, >>) have LOWER precedence than Arithmetic (+, -)
    // Expression: a << b + c ==> Binds as: a << (b + c) [NOT (a << b) + c]
    int shiftArithDefault = a << (b + c);
    int shiftArithOverride = (a << b) + c;

    cout << "a << (b + c) (Default: + before <<) -> Result: " << shiftArithDefault << endl;
    cout << "(a << b) + c (Intended Shift First) -> Result: " << shiftArithOverride << endl;

    // 5. SUMMARY OF KEY PRECEDENCE LEVELS (HIGH TO LOW)
    cout << "\n================ 4. C++ PRECEDENCE HIERARCHY SUMMARY ================" << endl;
    cout << "1. Primary / Postfix : Scope ::, Subscript [], Function (), Postfix ++/--" << endl;
    cout << "2. Unary             : Prefix ++/--, Unary +/-, Logical !, Bitwise ~, Deref *" << endl;
    cout << "3. Multiplicative    : *, /, %" << endl;
    cout << "4. Additive          : +, -" << endl;
    cout << "5. Bitwise Shift     : <<, >>" << endl;
    cout << "6. Relational        : <, <=, >, >=" << endl;
    cout << "7. Equality          : ==, !=" << endl;
    cout << "8. Bitwise AND/XOR/OR: &, ^, |" << endl;
    cout << "9. Logical AND/OR    : &&, ||" << endl;
    cout << "10. Ternary / Assign : ?:, =, +=, -= (Right-to-Left Associativity)" << endl;
    cout << "11. Comma            : ," << endl;

    return 0;
}