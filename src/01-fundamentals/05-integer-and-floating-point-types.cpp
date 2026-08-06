/*
 * =====================================================================================
 * CONCEPT        : Integer and Floating-Point Data Types in C++
 * DESCRIPTION    : Comprehensive exploration of integer and floating-point types:
 *                  1. Integer Types: Fixed-width (int8_t through int64_t, uint64_t) 
 *                     and standard types, signedness, bit sizes, min/max range limits.
 *                  2. Floating-Point Types: Single (float), double (double), and extended 
 *                     (long double) precision, decimal digits of precision, machine epsilon.
 *                  3. Floating-Point Mechanics: Safe relative epsilon comparison, IEEE 754 
 *                     special values (Infinity, NaN).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time evaluation for type property 
 *                    inspection, scalar arithmetic, and stream operations.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack memory footprint bounded by primitive scalar types.
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdint>
#include <cmath>
#include <string>

using namespace std;

/**
 * @brief Helper template to inspect and print integer type properties and ranges.
 * @tparam T Integer type parameter.
 * @param typeName Descriptive label of the integer type.
 * @param val Dynamic runtime value.
 */
template <typename T>
void printIntegerProperties(const string& typeName, T val) {
    cout << left << setw(18) << typeName
         << setw(10) << sizeof(T)
         << setw(10) << (numeric_limits<T>::is_signed ? "Signed" : "Unsigned")
         << setw(26) << numeric_limits<T>::min()
         << setw(26) << numeric_limits<T>::max()
         << "Value: " << +val << endl; // Unary + promotes int8_t/uint8_t to int for numerical output
}

/**
 * @brief Helper template to inspect floating-point precision, limits, and machine epsilon.
 * @tparam T Floating-point type parameter.
 * @param typeName Descriptive label of the floating-point type.
 * @param val Dynamic runtime value.
 */
template <typename T>
void printFloatingProperties(const string& typeName, T val) {
    cout << left << setw(14) << typeName
         << setw(10) << sizeof(T)
         << setw(12) << numeric_limits<T>::digits10
         << setw(20) << numeric_limits<T>::min()
         << setw(20) << numeric_limits<T>::max()
         << setw(20) << numeric_limits<T>::epsilon()
         << "Value: " << setprecision(numeric_limits<T>::digits10) << val << endl;
}

/**
 * @brief Compares two floating-point numbers safely using relative machine epsilon.
 * @tparam T Floating-point type parameter.
 * @param a First operand.
 * @param b Second operand.
 * @return True if equal within tolerance threshold, false otherwise.
 */
template <typename T>
constexpr bool areEqualEpsilon(T a, T b) noexcept {
    return abs(a - b) <= (numeric_limits<T>::epsilon() * max(abs(a), abs(b)));
}

int main() {
    // Dynamic runtime variables allocation
    int32_t userInt32{0};
    uint64_t userUint64{0};
    float userFloat{0.0f};
    double userDoubleA{0.0};
    double userDoubleB{0.0};

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter a Signed 32-bit Integer (e.g., -2147483640): " << flush;
    cin >> userInt32;

    cout << "Enter an Unsigned 64-bit Integer (e.g., 18446744073709551615): " << flush;
    cin >> userUint64;

    cout << "Enter a Single-Precision Float (e.g., 3.1415926): " << flush;
    cin >> userFloat;

    cout << "Enter Double-Precision Value A (e.g., 0.1): " << flush;
    cin >> userDoubleA;

    cout << "Enter Double-Precision Value B (e.g., 0.2): " << flush;
    cin >> userDoubleB;

    // 2. INTEGER TYPES & FIXED-WIDTH TYPES REPORT
    cout << "\n================ INTEGER TYPES & FIXED-WIDTH TYPES (<cstdint>) ================" << endl;
    cout << left << setw(18) << "Type Name"
         << setw(10) << "Size(B)"
         << setw(10) << "Sign"
         << setw(26) << "Min Limit"
         << setw(26) << "Max Limit"
         << "Runtime Value" << endl;
    cout << string(108, '-') << endl;

    printIntegerProperties("int8_t", static_cast<int8_t>(userInt32));
    printIntegerProperties("uint8_t", static_cast<uint8_t>(userInt32));
    printIntegerProperties("int16_t", static_cast<int16_t>(userInt32));
    printIntegerProperties("int32_t", userInt32);
    printIntegerProperties("int64_t", static_cast<int64_t>(userInt32));
    printIntegerProperties("uint64_t", userUint64);

    // 3. FLOATING-POINT TYPES REPORT
    cout << "\n================ FLOATING-POINT TYPES METADATA ================" << endl;
    cout << left << setw(14) << "Type Name"
         << setw(10) << "Size(B)"
         << setw(12) << "Precision"
         << setw(20) << "Min Positive"
         << setw(20) << "Max Limit"
         << setw(20) << "Epsilon"
         << "Runtime Value" << endl;
    cout << string(114, '-') << endl;

    printFloatingProperties("float", userFloat);
    printFloatingProperties("double", userDoubleA);
    printFloatingProperties("long double", static_cast<long double>(userDoubleA));

    // 4. FLOATING-POINT EPSILON COMPARISON & SPECIAL VALUES DEMO
    cout << "\n================ FLOATING-POINT MECHANICS & SPECIAL VALUES ================" << endl;
    
    double computedSum = userDoubleA + userDoubleB;
    cout << "User Double A          : " << setprecision(17) << userDoubleA << endl;
    cout << "User Double B          : " << setprecision(17) << userDoubleB << endl;
    cout << "Computed Sum (A + B)   : " << computedSum << endl;

    bool isEqual = areEqualEpsilon(computedSum, userDoubleA + userDoubleB);
    cout << "Epsilon Comparison Check: " << (isEqual ? "EQUAL (Within Machine Epsilon Tolerance)" : "NOT EQUAL") << endl;

    // IEEE 754 Special Values Demonstration
    double infVal = numeric_limits<double>::infinity();
    double nanVal = numeric_limits<double>::quiet_NaN();

    cout << "\nIEEE 754 Special Floating Values:" << endl;
    cout << "  |- Infinity Value : " << infVal << " | isinf() = " << (isinf(infVal) ? "true" : "false") << endl;
    cout << "  |- Quiet NaN      : " << nanVal << " | isnan() = " << (isnan(nanVal) ? "true" : "false") << endl;

    return 0;
}