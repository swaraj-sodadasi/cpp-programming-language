/*
 * =====================================================================================
 * CONCEPT        : Type Deduction in C++ (auto, decltype, decltype(auto), Template Deduction, CTAD)
 * DESCRIPTION    : Comprehensive implementation explaining C++ type deduction mechanisms:
 *                  1. auto Deduction          : Value, reference (auto&), const reference (const auto&),
 *                                               and pointer (auto*) deduction rules.
 *                  2. decltype Deduction      : Inspecting expression types without evaluation;
 *                                               distinguishing decltype(entity) vs decltype((expression)).
 *                  3. decltype(auto)          : Exact return type preservation (value vs reference).
 *                  4. Class Template Argument : Automatic deduction of template arguments from 
 *                     Deduction (CTAD)          constructor parameters (C++17 CTAD).
 *                  5. Function Template       : Standard template function parameter type deduction.
 *                     Deduction
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Type deduction occurs entirely at compile-time; runtime operations
 *                    and type trait inspections execute in constant time.
 * SPACE COMPLEXITY : Best Case: O(1) — Memory allocation is bounded by scalar variables and references.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <utility>
#include <typeinfo>
#include <type_traits>


using namespace std;

// Global variable used to demonstrate decltype(auto) return semantics
int globalVal = 100;

/**
 * @brief Returns value by copy using standard 'auto' deduction.
 * @return Prvalue int (copy of globalVal).
 */
auto getValueByCopy() {
    return globalVal;
}

/**
 * @brief Returns exact reference type using 'decltype(auto)'.
 * @return Lvalue reference int& bound to globalVal due to parenthesized expression (globalVal).
 */
decltype(auto) getValueByRef() {
    return (globalVal); // Parentheses turn 'globalVal' into an lvalue expression -> deduces int&
}

/**
 * @brief Function template utilizing template argument type deduction.
 * @tparam T Deduced automatically at compile-time based on argument type.
 * @param operand Generic input operand.
 * @return Doubled value of operand with deduced type.
 */
template <typename T>
auto multiplyByTwo(T operand) {
    return operand * 2;
}

int main() {
    int userInputInt = 0;
    double userInputDouble = 0.0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter an integer value: " << flush;
    if (!(cin >> userInputInt)) {
        cout << "Invalid integer input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter a floating-point value: " << flush;
    if (!(cin >> userInputDouble)) {
        cout << "Invalid floating-point input. Program terminated." << endl;
        return 0;
    }

    // 2. AUTO TYPE DEDUCTION MECHANICS
    cout << "\n================ 1. AUTO TYPE DEDUCTION ================" << endl;
    auto valCopy = userInputInt;                   // Deduced as int (value copy, strips const/ref)
    const auto constVal = userInputInt;            // Deduced as const int
    auto& refVal = userInputInt;                   // Deduced as int& (reference)
    const auto& constRefVal = userInputDouble;     // Deduced as const double&
    auto* ptrVal = &userInputInt;                  // Deduced as int* (pointer)

    cout << "auto valCopy         : " << valCopy << " | Type: " << typeid(valCopy).name() << endl;
    cout << "const auto constVal  : " << constVal << " | Type: " << typeid(constVal).name() << endl;
    cout << "auto& refVal         : " << refVal << " | Type: " << typeid(refVal).name() << endl;
    cout << "const auto& constRef : " << constRefVal << " | Type: " << typeid(constRefVal).name() << endl;
    cout << "auto* ptrVal         : " << *ptrVal << " | Type: " << typeid(ptrVal).name() << endl;

    // 3. DECLTYPE DEDUCTION (INSPECTING TYPES WITHOUT EVALUATING)
    cout << "\n================ 2. DECLTYPE DEDUCTION ================" << endl;
    // decltype(entity) yields declared type (int)
    decltype(userInputInt) declVar1 = 50; 
    
    // decltype((expression)) yields reference type (int&) for lvalue expressions
    decltype((userInputInt)) declVar2 = userInputInt; 

    cout << "decltype(userInputInt)   -> declVar1 = " << declVar1 << " | Is lvalue ref: " 
         << boolalpha << is_lvalue_reference_v<decltype(userInputInt)> << endl;
    cout << "decltype((userInputInt)) -> declVar2 = " << declVar2 << " | Is lvalue ref: " 
         << is_lvalue_reference_v<decltype((userInputInt))> << endl;

    // 4. DECLTYPE(AUTO) RETURN TYPE DEDUCTION
    cout << "\n================ 3. DECLTYPE(AUTO) DEDUCTION ================" << endl;
    auto copyResult = getValueByCopy();        // Receives value copy (int)
    decltype(auto) refResult = getValueByRef(); // Receives exact reference (int&)

    cout << "Initial globalVal              : " << globalVal << endl;
    refResult = userInputInt; // Modifies globalVal directly through the deduced int& reference
    cout << "Updated globalVal via refResult: " << globalVal << endl;
    cout << "copyResult (Unmodified Copy)   : " << copyResult << endl;

    // 5. CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD)
    cout << "\n================ 4. CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD) ================" << endl;
    // Constructor parameters deduce template arguments automatically without explicit <int, double>
    pair dynamicPair(userInputInt, userInputDouble); // Deduced as std::pair<int, double>
    vector dynamicVec = {userInputInt, userInputInt * 2, userInputInt * 3}; // Deduced as std::vector<int>

    cout << "CTAD pair<int, double>         : (" << dynamicPair.first << ", " << dynamicPair.second << ")" << endl;
    cout << "CTAD vector<int> size          : " << dynamicVec.size() << " elements" << endl;

    // 6. FUNCTION TEMPLATE TYPE DEDUCTION
    cout << "\n================ 5. FUNCTION TEMPLATE TYPE DEDUCTION ================" << endl;
    auto doubledInt = multiplyByTwo(userInputInt);
    auto doubledFloat = multiplyByTwo(userInputDouble);

    cout << "multiplyByTwo(int)             : " << doubledInt << " | Type: " << typeid(doubledInt).name() << endl;
    cout << "multiplyByTwo(double)          : " << doubledFloat << " | Type: " << typeid(doubledFloat).name() << endl;

    return 0;
}