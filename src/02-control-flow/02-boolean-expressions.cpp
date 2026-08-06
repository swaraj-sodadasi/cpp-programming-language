/*
 * =====================================================================================
 * CONCEPT        : Boolean Expressions in C++
 * DESCRIPTION    : Comprehensive implementation explaining C++ Boolean expressions:
 *                  1. Relational / Comparison Operators : Expressions producing bool values 
 *                                                         (==, !=, <, >, <=, >=).
 *                  2. Logical Operators                 : Combining Boolean expressions using 
 *                                                         AND (&&), OR (||), and NOT (!).
 *                  3. Short-Circuit Evaluation          : Guaranteed evaluation order and early 
 *                                                         exit behavior of logical operations.
 *                  4. De Morgan's Laws                  : Demonstrating Boolean equivalence rules 
 *                                                         !(A && B) == (!A || !B) programmatically.
 *                  5. Contextual Implicit Conversion    : How integral types, pointers, and 
 *                                                         objects evaluate to true/false in 
 *                                                         Boolean contexts.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct constant-time scalar logic evaluations.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation for test variables.
 * =====================================================================================
 */

#include <iostream>

using namespace std;

/**
 * @brief Helper function to demonstrate short-circuit side effects.
 * @param sideEffectOccurred Reference parameter modified if the function gets evaluated.
 * @param returnValue Value returned by the function.
 * @return Returns returnValue.
 */
bool evaluateWithSideEffect(bool& sideEffectOccurred, bool returnValue) {
    sideEffectOccurred = true;
    return returnValue;
}

int main() {
    int inputValA = 0;
    int inputValB = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter first integer value (inputValA): " << flush;
    if (!(cin >> inputValA)) {
        cout << "Invalid input for inputValA. Program terminated." << endl;
        return 0;
    }

    cout << "Enter second integer value (inputValB): " << flush;
    if (!(cin >> inputValB)) {
        cout << "Invalid input for inputValB. Program terminated." << endl;
        return 0;
    }

    // Enable text-based boolean output ("true" / "false")
    cout << boolalpha;

    // 2. RELATIONAL / COMPARISON EXPRESSIONS
    cout << "\n================ 1. RELATIONAL / COMPARISON EXPRESSIONS ================" << endl;
    bool isEqual      = (inputValA == inputValB);
    bool isNotEqual   = (inputValA != inputValB);
    bool isGreater    = (inputValA > inputValB);
    bool isLess       = (inputValA < inputValB);
    bool isGreaterEq  = (inputValA >= inputValB);
    bool isLessEq     = (inputValA <= inputValB);

    cout << "Expression: (inputValA == inputValB)  -> " << isEqual << endl;
    cout << "Expression: (inputValA != inputValB)  -> " << isNotEqual << endl;
    cout << "Expression: (inputValA > inputValB)   -> " << isGreater << endl;
    cout << "Expression: (inputValA < inputValB)   -> " << isLess << endl;
    cout << "Expression: (inputValA >= inputValB)  -> " << isGreaterEq << endl;
    cout << "Expression: (inputValA <= inputValB)  -> " << isLessEq << endl;

    // 3. LOGICAL COMPOUND EXPRESSIONS (&&, ||, !)
    cout << "\n================ 2. LOGICAL COMPOUND EXPRESSIONS ================" << endl;
    bool condA = (inputValA > 0);  // Is inputValA positive?
    bool condB = (inputValB > 0);  // Is inputValB positive?

    bool logicalAnd = condA && condB;
    bool logicalOr  = condA || condB;
    bool logicalNot = !condA;

    cout << "Condition A (inputValA > 0)          : " << condA << endl;
    cout << "Condition B (inputValB > 0)          : " << condB << endl;
    cout << "Logical AND (condA && condB)         : " << logicalAnd << endl;
    cout << "Logical OR  (condA || condB)         : " << logicalOr << endl;
    cout << "Logical NOT (!condA)                 : " << logicalNot << endl;

    // 4. SHORT-CIRCUIT EVALUATION DEMONSTRATION
    cout << "\n================ 3. SHORT-CIRCUIT EVALUATION ================" << endl;
    bool sideEffectFlag1 = false;
    bool sideEffectFlag2 = false;

    // In (false && ...), the right-hand side is NEVER evaluated because result is guaranteed false
    bool shortCircuitAnd = (false && evaluateWithSideEffect(sideEffectFlag1, true));
    
    // In (true || ...), the right-hand side is NEVER evaluated because result is guaranteed true
    bool shortCircuitOr  = (true || evaluateWithSideEffect(sideEffectFlag2, true));

    cout << "Short-Circuit AND (false && fn())     -> Result: " << shortCircuitAnd << endl;
    cout << "  |- Was RHS function evaluated?     -> " << (sideEffectFlag1 ? "Yes" : "No (Short-Circuited)") << endl;

    cout << "Short-Circuit OR  (true || fn())      -> Result: " << shortCircuitOr << endl;
    cout << "  |- Was RHS function evaluated?     -> " << (sideEffectFlag2 ? "Yes" : "No (Short-Circuited)") << endl;

    // 5. DE MORGAN'S LAWS VERIFICATION
    cout << "\n================ 4. DE MORGAN'S LAWS EQUIVALENCE ================" << endl;
    // Law 1: !(A && B) == (!A || !B)
    bool leftSide1  = !(condA && condB);
    bool rightSide1 = (!condA || !condB);

    // Law 2: !(A || B) == (!A && !B)
    bool leftSide2  = !(condA || condB);
    bool rightSide2 = (!condA && !condB);

    cout << "De Morgan Law 1: !(A && B) == (!A || !B)" << endl;
    cout << "  |- !(A && B)                       : " << leftSide1 << endl;
    cout << "  |- (!A || !B)                      : " << rightSide1 << endl;
    cout << "  |- Statements are Equivalent?      : " << (leftSide1 == rightSide1) << endl;

    cout << "De Morgan Law 2: !(A || B) == (!A && !B)" << endl;
    cout << "  |- !(A || B)                       : " << leftSide2 << endl;
    cout << "  |- (!A && !B)                      : " << rightSide2 << endl;
    cout << "  |- Statements are Equivalent?      : " << (leftSide2 == rightSide2) << endl;

    // 6. CONTEXTUAL IMPLICIT CONVERSION TO BOOL (TRUTHINESS)
    cout << "\n================ 5. CONTEXTUAL BOOLEAN CONVERSIONS ================" << endl;
    int nonZeroInt = inputValA;
    int zeroInt = 0;
    int* nullPtr = nullptr;
    int* validPtr = &inputValA;

    cout << "Non-zero int (" << nonZeroInt << ") implicitly evaluates to : " << static_cast<bool>(nonZeroInt) << endl;
    cout << "Zero int (0) implicitly evaluates to     : " << static_cast<bool>(zeroInt) << endl;
    cout << "Null pointer (nullptr) evaluates to      : " << static_cast<bool>(nullPtr) << endl;
    cout << "Valid pointer address evaluates to       : " << static_cast<bool>(validPtr) << endl;

    return 0;
}