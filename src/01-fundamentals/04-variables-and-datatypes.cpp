/*
 * =====================================================================================
 * CONCEPT        : Variables and Data Types in C++
 * DESCRIPTION    : Comprehensive implementation explaining fundamental C++ data types:
 *                  1. Primitive Types    : int, float, double, char, bool
 *                  2. Fixed-Width Types  : int32_t, int64_t, uint32_t, uint64_t (<cstdint>)
 *                  3. Automatic Deduction: auto keyword (compile-time type inference)
 *                  4. Scope & Type Bounds: Variable lifetime, sizeof, and numeric limits
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time evaluation for primitive variable initialization,
 *                    dynamic stream reading, and property reporting.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack memory footprint bounded by fundamental types.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <limits>
#include <cstdint>
#include <iomanip>
#include <typeinfo>

using namespace std;

/**
 * @brief Helper function to inspect and print metadata of a variable's data type.
 * @tparam T Generic data type parameter.
 * @param typeName Label for the data type being inspected.
 * @param val Dynamic value of the type.
 */
template <typename T>
void printTypeInfo(const string& typeName, const T& val) {
    cout << left << setw(18) << typeName
         << setw(10) << sizeof(T)
         << setw(24) << numeric_limits<T>::min()
         << setw(24) << numeric_limits<T>::max()
         << "Value: " << val << endl;
}

int main() {
    // 1. PRIMITIVE INTEGRAL & FLOATING TYPES INITIALIZATION
    int32_t userInt32{0};
    int64_t userInt64{0};
    float userFloat{0.0f};
    double userDouble{0.0};
    char userChar{' '};
    bool userBool{false};
    string userString{};

    // 2. DYNAMIC INPUT COLLECTION WITH EXPLICIT STREAM FLUSHING
    cout << "Enter a 32-bit Integer (e.g., 42): " << flush;
    cin >> userInt32;

    cout << "Enter a 64-bit Integer (e.g., 9876543210): " << flush;
    cin >> userInt64;

    cout << "Enter a Float value (e.g., 3.14159): " << flush;
    cin >> userFloat;

    cout << "Enter a Double value (e.g., 2.718281828): " << flush;
    cin >> userDouble;

    cout << "Enter a single Character (e.g., X): " << flush;
    cin >> userChar;

    cout << "Enter a Boolean value (1 for true, 0 for false): " << flush;
    cin >> userBool;

    cout << "Enter a single-word String (e.g., ModernCPP): " << flush;
    cin >> userString;

    // 3. AUTO TYPE DEDUCTION (Compile-time type inference)
    auto deducedSum = userInt32 + userFloat; // Deduced to float (implicit promotion)
    auto deducedStr = userString;            // Deduced to std::string

    // 4. METADATA & BOUNDS REPORTING
    cout << "\n================ DATA TYPES METADATA & LIMITS ================" << endl;
    cout << left << setw(18) << "Data Type"
         << setw(10) << "Size(B)"
         << setw(24) << "Min Limit"
         << setw(24) << "Max Limit"
         << "Runtime Value" << endl;
    cout << string(88, '-') << endl;

    printTypeInfo("int32_t", userInt32);
    printTypeInfo("int64_t", userInt64);
    printTypeInfo("float", userFloat);
    printTypeInfo("double", userDouble);
    printTypeInfo("char", userChar);
    printTypeInfo("bool", userBool);

    // 5. AUTO DEDUCTION & SCOPE DEMONSTRATION
    cout << "\n================ TYPE DEDUCTION & VARIABLE SCOPE ================" << endl;
    cout << "auto deducedSum (int32 + float) -> Value: " << deducedSum 
         << " | Type: " << typeid(deducedSum).name() << endl;
    cout << "auto deducedStr (string copy)   -> Value: " << deducedStr 
         << " | Type: " << typeid(deducedStr).name() << endl;

    // Local Block Scope Demonstration
    {
        // Block-scoped constant variable (constexpr evaluated at compile-time)
        constexpr double PI = 3.141592653589793;
        const double calculatedArea = PI * userDouble * userDouble;
        cout << "\n[Local Scope Block]" << endl;
        cout << "  |- Constant PI      : " << PI << endl;
        cout << "  |- Calculated Area : " << calculatedArea << endl;
    } // Local variables PI and calculatedArea go out of scope and release stack memory here

    return 0;
}