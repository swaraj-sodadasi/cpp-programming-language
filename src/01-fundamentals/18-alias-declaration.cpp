/*
 * =====================================================================================
 * CONCEPT        : Alias Declaration in C++ ('using' Syntax vs 'typedef')
 * DESCRIPTION    : Comprehensive implementation explaining modern C++ type aliases:
 *                  1. Basic Type Alias     : Creating readable names for standard library types 
 *                                            (e.g., using IntVector = std::vector<int>).
 *                  2. Alias Templates      : Parameterized type aliases (template <typename T> using ...)
 *                                            which are not directly possible with legacy 'typedef'.
 *                  3. Function Pointer Alias: Clean, clear syntax for function pointer declarations.
 *                  4. Modern Standard Traits: Using _t suffix aliases (e.g., std::add_pointer_t<T>).
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear time to process dynamic user elements into alias containers.
 * SPACE COMPLEXITY : Best Case: O(N) — Dynamic heap allocation allocated for vector and map elements.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <type_traits>

using namespace std;

// 1. BASIC TYPE ALIAS
// Equivalent to legacy C syntax: typedef vector<int> IntVector;
using IntVector = vector<int>;

// 2. ALIAS TEMPLATE (TEMPLATE TYPE ALIAS)
// Parametrized type alias (Impossible with legacy typedef without nested struct wrappers)
template <typename ValueType>
using StringMap = map<string, ValueType>;

// 3. FUNCTION POINTER TYPE ALIAS
// Clean syntax compared to legacy: typedef double (*MathFunc)(double, double);
using MathOpFunc = double(*)(double, double);

// Sample functions matching MathOpFunc signature
double addNumbers(double a, double b) noexcept { return a + b; }
double multiplyNumbers(double a, double b) noexcept { return a * b; }

int main() {
    size_t elementCount = 0;
    string dynamicKey;
    double dynamicVal = 0.0;
    double num1 = 0.0, num2 = 0.0;

    // 4. DYNAMIC INPUT COLLECTION WITH EXPLICIT STREAM FLUSHING
    cout << "Enter the number of elements for IntVector: " << flush;
    if (!(cin >> elementCount) || elementCount == 0) {
        cout << "Invalid element count provided. Program terminated." << endl;
        return 0;
    }

    // Utilizing the basic type alias 'IntVector'
    IntVector numbers(elementCount);
    cout << "Enter " << elementCount << " space-separated integers: " << flush;
    for (size_t i = 0; i < elementCount; ++i) {
        cin >> numbers[i];
    }

    cout << "Enter a key (string) for StringMap: " << flush;
    cin >> dynamicKey;

    cout << "Enter a value (double) for StringMap: " << flush;
    cin >> dynamicVal;

    cout << "Enter two floating-point numbers for MathOpFunc dynamic test: " << flush;
    cin >> num1 >> num2;

    // 5. BASIC ALIAS DEMONSTRATION
    cout << "\n================ 1. BASIC TYPE ALIAS DEMONSTRATION ================" << endl;
    cout << "IntVector Elements: ";
    for (const auto& val : numbers) {
        cout << val << " ";
    }
    cout << endl;

    // 6. ALIAS TEMPLATE DEMONSTRATION
    cout << "\n================ 2. ALIAS TEMPLATE DEMONSTRATION ================" << endl;
    // Instantiating the alias template with double as the ValueType parameter
    StringMap<double> dynamicMap;
    dynamicMap[dynamicKey] = dynamicVal;

    cout << "StringMap<double> Entry -> [" << dynamicKey << "] : " << dynamicMap[dynamicKey] << endl;

    // 7. FUNCTION POINTER ALIAS DEMONSTRATION
    cout << "\n================ 3. FUNCTION POINTER ALIAS DEMONSTRATION ================" << endl;
    MathOpFunc operation = addNumbers;
    cout << "MathOpFunc (addNumbers)      -> " << num1 << " + " << num2 << " = " << operation(num1, num2) << endl;

    operation = multiplyNumbers;
    cout << "MathOpFunc (multiplyNumbers) -> " << num1 << " * " << num2 << " = " << operation(num1, num2) << endl;

    // 8. MODERN TYPE TRAITS ALIASES (_t SUFFIX)
    cout << "\n================ 4. MODERN STANDARD LIBRARY ALIAS TRAITS ================" << endl;
    // C++14 _t suffix aliases (e.g., std::add_pointer_t<T> instead of typename std::add_pointer<T>::type)
    using IntPtrAlias = add_pointer_t<int>;
    int targetVal = 42;
    IntPtrAlias ptr = &targetVal;

    cout << "Demonstrating std::add_pointer_t<int> Alias:" << endl;
    cout << "  |- Target Int Value : " << targetVal << endl;
    cout << "  |- Dereferenced Ptr : " << *ptr << endl;

    return 0;
}