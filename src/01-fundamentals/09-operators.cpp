/*
 * =====================================================================================
 * CONCEPT        : Operators in C++ (Arithmetic, Relational, Logical, Bitwise, 
 *                  Assignment, Increment/Decrement, Ternary, Special Operators)
 * DESCRIPTION    : Comprehensive implementation showcasing all standard C++ operator categories:
 *                  1. Arithmetic Operators           : +, -, *, /, %
 *                  2. Relational / Comparison        : ==, !=, <, >, <=, >=
 *                  3. Logical Operators              : &&, ||, !
 *                  4. Bitwise Operators              : &, |, ^, ~, <<, >>
 *                  5. Increment / Decrement          : ++val, val++, --val, val--
 *                  6. Compound Assignment Operators  : =, +=, -=, *=, /=, %=, &=, |=, ^=
 *                  7. Ternary & Special Operators    : ?:, sizeof, typeid, comma operator
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct constant-time evaluation for dynamic scalar operations.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack memory allocation for dynamic primitive variables.
 * =====================================================================================
 */

#include <iostream>
#include <typeinfo>
#include <cstdint>
#include <bitset>

using namespace std;

int main() {
    int32_t numA = 0;
    int32_t numB = 0;
    int rawBoolInput = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter first integer operand (numA): " << flush;
    if (!(cin >> numA)) {
        cout << "Invalid input for numA. Program terminated." << endl;
        return 0;
    }

    cout << "Enter second integer operand (numB): " << flush;
    if (!(cin >> numB)) {
        cout << "Invalid input for numB. Program terminated." << endl;
        return 0;
    }

    cout << "Enter a boolean flag (1 for true, 0 for false): " << flush;
    if (!(cin >> rawBoolInput)) {
        cout << "Invalid input for boolean flag. Program terminated." << endl;
        return 0;
    }

    bool userFlag = static_cast<bool>(rawBoolInput);

    // 2. ARITHMETIC OPERATORS (+, -, *, /, %)
    cout << "\n================ 1. ARITHMETIC OPERATORS ================" << endl;
    cout << "Addition (numA + numB)       : " << (numA + numB) << endl;
    cout << "Subtraction (numA - numB)    : " << (numA - numB) << endl;
    cout << "Multiplication (numA * numB) : " << (numA * numB) << endl;
    
    // Safety check against divide-by-zero undefined behavior
    if (numB != 0) {
        cout << "Division (numA / numB)       : " << (numA / numB) << endl;
        cout << "Modulus (numA % numB)        : " << (numA % numB) << endl;
    } else {
        cout << "Division & Modulus by zero are undefined and skipped." << endl;
    }

    // 3. RELATIONAL / COMPARISON OPERATORS (==, !=, <, >, <=, >=)
    cout << "\n================ 2. RELATIONAL OPERATORS ================" << endl;
    cout << boolalpha;
    cout << "Equal to (numA == numB)      : " << (numA == numB) << endl;
    cout << "Not Equal to (numA != numB)  : " << (numA != numB) << endl;
    cout << "Greater Than (numA > numB)   : " << (numA > numB) << endl;
    cout << "Less Than (numA < numB)      : " << (numA < numB) << endl;
    cout << "Greater or Equal (numA >= B) : " << (numA >= numB) << endl;
    cout << "Less or Equal (numA <= numB) : " << (numA <= numB) << endl;

    // 4. LOGICAL OPERATORS (&&, ||, !)
    cout << "\n================ 3. LOGICAL OPERATORS ================" << endl;
    bool isPositiveA = (numA > 0);
    bool isPositiveB = (numB > 0);
    cout << "Logical AND ((numA > 0) && (numB > 0)) : " << (isPositiveA && isPositiveB) << endl;
    cout << "Logical OR  ((numA > 0) || (numB > 0)) : " << (isPositiveA || isPositiveB) << endl;
    cout << "Logical NOT (!(numA > 0))              : " << (!isPositiveA) << endl;

    // 5. BITWISE OPERATORS (&, |, ^, ~, <<, >>)
    cout << "\n================ 4. BITWISE OPERATORS ================" << endl;
    cout << "numA Binary Pattern          : " << bitset<8>(numA) << endl;
    cout << "numB Binary Pattern          : " << bitset<8>(numB) << endl;
    cout << "Bitwise AND (numA & numB)    : " << bitset<8>(numA & numB) << " (" << (numA & numB) << ")" << endl;
    cout << "Bitwise OR  (numA | numB)    : " << bitset<8>(numA | numB) << " (" << (numA | numB) << ")" << endl;
    cout << "Bitwise XOR (numA ^ numB)    : " << bitset<8>(numA ^ numB) << " (" << (numA ^ numB) << ")" << endl;
    cout << "Bitwise NOT (~numA)          : " << bitset<8>(~numA) << " (" << (~numA) << ")" << endl;
    cout << "Left Shift  (numA << 1)      : " << (numA << 1) << endl;
    cout << "Right Shift (numA >> 1)      : " << (numA >> 1) << endl;

    // 6. INCREMENT & DECREMENT OPERATORS (++val, val++, --val, val--)
    cout << "\n================ 5. INCREMENT & DECREMENT OPERATORS ================" << endl;
    int tempValue = numA;
    cout << "Initial tempValue            : " << tempValue << endl;
    cout << "Prefix Increment (++temp)    : " << ++tempValue << endl;

    // Separated evaluation and side effect to avoid sequence point warnings / undefined behavior
    int postIncOld = tempValue++;
    cout << "Postfix Increment (temp++)   : " << postIncOld << " (Value after evaluation: " << tempValue << ")" << endl;

    cout << "Prefix Decrement (--temp)    : " << --tempValue << endl;

    int postDecOld = tempValue--;
    cout << "Postfix Decrement (temp--)   : " << postDecOld << " (Value after evaluation: " << tempValue << ")" << endl;

    // 7. COMPOUND ASSIGNMENT OPERATORS (=, +=, -=, *=, /=, %=, &=, |=, ^=)
    cout << "\n================ 6. COMPOUND ASSIGNMENT OPERATORS ================" << endl;
    int accumulator = numA;
    cout << "accumulator = numA           : " << accumulator << endl;
    accumulator += numB;
    cout << "accumulator += numB          : " << accumulator << endl;
    accumulator -= numB;
    cout << "accumulator -= numB          : " << accumulator << endl;
    accumulator *= 2;
    cout << "accumulator *= 2             : " << accumulator << endl;
    if (numB != 0) {
        accumulator /= (numB < 0 ? -numB : numB); // Prevent divide-by-zero
        cout << "accumulator /= abs(numB)     : " << accumulator << endl;
    }

    // 8. TERNARY & SPECIAL OPERATORS (?, sizeof, typeid, comma operator)
    cout << "\n================ 7. TERNARY & SPECIAL OPERATORS ================" << endl;
    int maxOperand = (numA > numB) ? numA : numB;
    cout << "Ternary Max (numA, numB)     : " << maxOperand << endl;
    cout << "sizeof(numA)                 : " << sizeof(numA) << " Bytes" << endl;
    cout << "typeid(numA).name()          : " << typeid(numA).name() << endl;
    cout << "User Flag State              : " << (userFlag ? "Active" : "Inactive") << endl;

    // Comma operator evaluation (evaluates left operand, returns right operand result)
    int commaResult = (numA += 5, numA * 2);
    cout << "Comma Operator (a+=5, a*2)   : " << commaResult << " (numA updated to: " << numA << ")" << endl;

    return 0;
}