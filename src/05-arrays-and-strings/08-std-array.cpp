/*
 * =====================================================================================
 * CONCEPT        : `std::array` in C++ (Fixed-Size Sequential Container)
 * DESCRIPTION    : Comprehensive guide and implementation of Modern C++ `std::array`:
 *                  1. What is `std::array`?: A thin, zero-overhead wrapper around a C-style 
 *                     fixed-size array providing STL container semantics.
 *                  2. Memory Layout       : Allocated directly on the stack (or static storage); 
 *                     zero dynamic heap overhead.
 *                  3. Bounds Checking     : Access via `.at(i)` throws `std::out_of_range` exception, 
 *                     whereas `[]` provides zero-cost unchecked access.
 *                  4. Iterators & STL     : Full support for iterators (`begin()`, `end()`, `rbegin()`, etc.) 
 *                     and standard algorithms (`std::sort`, `std::accumulate`).
 *                  5. Structural Utilities: Structured binding (C++17), tuple-like interface (`std::get`), 
 *                     and `std::to_array` (C++20).
 *
 * TIME COMPLEXITY  : Random Access: O(1) | Size Lookup: O(1) | Traversal: O(N) | Sorting: O(N log N)
 * SPACE COMPLEXITY : Fixed stack allocation: O(N * sizeof(T))
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <string>
#include <stdexcept>

using namespace std;

// =====================================================================================
// 1. PASSING `std::array` TO FUNCTIONS
// `std::array` is a true C++ object, so passing by value creates a full copy.
// Pass by const reference to avoid copying while maintaining safety.
// =====================================================================================

template <typename T, size_t N>
void printArrayInfo(const std::array<T, N>& arr, const string& label) {
    cout << "  - [" << left << setw(22) << label << "] Size: " << arr.size() 
         << " | Elements: [ ";
    for (size_t i = 0; i < arr.size(); ++i) {
        cout << arr[i] << (i + 1 < arr.size() ? ", " : " ");
    }
    cout << "]\n";
}

template <typename T, size_t N>
void scaleArray(std::array<T, N>& arr, T factor) {
    for (auto& elem : arr) {
        elem *= factor;
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int scaleFactorInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a scale factor integer for std::array manipulation (e.g., 2): " << flush;
    if (!(cin >> scaleFactorInput) || scaleFactorInput == 0) {
        cout << "Invalid input. Defaulting scale factor to 2." << endl;
        scaleFactorInput = 2;
    }

    // =====================================================================================
    // 1. DECLARATION AND INITIALIZATION
    // =====================================================================================
    cout << "\n================ 1. DECLARATION & INITIALIZATION ================\n";

    std::array<int, 5> arrZero{};                 // Zero-initialized: {0, 0, 0, 0, 0}
    std::array<int, 5> arrPartial{10, 20, 30};    // Partial init: {10, 20, 30, 0, 0}
    std::array<int, 5> arrFilled;
    arrFilled.fill(42);                            // Fill method: {42, 42, 42, 42, 42}

    printArrayInfo(arrZero, "Zero-Initialized");
    printArrayInfo(arrPartial, "Partially Initialized");
    printArrayInfo(arrFilled, "Filled with 42");

    // Modern C++20 std::to_array deduction
#if __cplusplus >= 202002L
    auto arrDeduced = std::to_array({1, 3, 5, 7, 9});
    printArrayInfo(arrDeduced, "std::to_array (C++20)");
#endif

    // =====================================================================================
    // 2. ELEMENT ACCESS AND BOUNDS CHECKING
    // =====================================================================================
    cout << "\n================ 2. ELEMENT ACCESS & BOUNDS CHECKING ================\n";
    std::array<int, 5> sampleData = {15, 45, 25, 85, 65};

    cout << "  - Fast Unchecked Access `[]`     : sampleData[1] = " << sampleData[1] << "\n";
    cout << "  - Bounds-Checked Access `.at()`  : sampleData.at(3) = " << sampleData.at(3) << "\n";
    cout << "  - First Element `.front()`       : " << sampleData.front() << "\n";
    cout << "  - Last Element `.back()`         : " << sampleData.back() << "\n";
    cout << "  - Compile-Time Access `std::get` : std::get<0>(sampleData) = " << std::get<0>(sampleData) << "\n";

    // Exception handling with .at()
    try {
        cout << "  - Attempting out-of-bounds access `.at(10)`...\n";
        int value = sampleData.at(10);
        (void)value;
    } catch (const std::out_of_range& e) {
        cout << "  - [EXCEPTIONAL SAFETY] Caught std::out_of_range exception: " << e.what() << "\n";
    }

    // =====================================================================================
    // 3. CAPACITY & MEMORY PROPERTIES
    // =====================================================================================
    cout << "\n================ 3. CAPACITY & MEMORY PROPERTIES ================\n";
    cout << "  - Container Size `.size()`       : " << sampleData.size() << " elements\n";
    cout << "  - Max Size `.max_size()`         : " << sampleData.max_size() << " elements\n";
    cout << "  - Is Empty? `.empty()`           : " << (sampleData.empty() ? "Yes" : "No") << "\n";
    cout << "  - Raw Buffer Size `sizeof()`     : " << sizeof(sampleData) << " bytes\n";
    cout << "  - Direct Data Pointer `.data()`  : " << static_cast<const void*>(sampleData.data()) << "\n";

    // =====================================================================================
    // 4. ITERATORS AND STL ALGORITHMS
    // =====================================================================================
    cout << "\n================ 4. ITERATORS & STL ALGORITHMS ================\n";
    printArrayInfo(sampleData, "Before Sorting");

    // Sorting via std::sort and standard iterators
    std::sort(sampleData.begin(), sampleData.end());
    printArrayInfo(sampleData, "After std::sort");

    // Accumulating sum via std::accumulate
    int totalSum = std::accumulate(sampleData.begin(), sampleData.end(), 0);
    cout << "  - Total Sum via std::accumulate : " << totalSum << "\n";

    // Scaling array elements in-place via non-const reference parameter
    scaleArray(sampleData, scaleFactorInput);
    printArrayInfo(sampleData, "After Scaling");

    // =====================================================================================
    // 5. STRUCTURED BINDING (C++17) & MULTI-DIMENSIONAL std::array
    // =====================================================================================
    cout << "\n================ 5. STRUCTURED BINDING & 2D std::array ================\n";

    // C++17 Structured Binding
    std::array<int, 3> point = {100, 200, 300};
    auto [x, y, z] = point;
    cout << "  - Structured Binding Unpacking: x = " << x << ", y = " << y << ", z = " << z << "\n";

    // 2D std::array matrix allocation
    std::array<std::array<int, 3>, 2> matrix2D = {{{1, 2, 3}, {4, 5, 6}}};
    cout << "  - 2D std::array (2x3 Matrix):\n";
    for (size_t r = 0; r < matrix2D.size(); ++r) {
        cout << "    Row " << r << ": ";
        for (size_t c = 0; c < matrix2D[r].size(); ++c) {
            cout << matrix2D[r][c] << " ";
        }
        cout << "\n";
    }

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ std::array VS C-STYLE ARRAY SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Metric / Trait        | C-Style Array (`T arr[N]`)        | Modern Container (`std::array`)   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Memory Location       | Stack / Static Segment            | Stack / Static Segment            |\n"
         << "| Performance Overhead  | Zero Abstraction Overhead         | Zero Abstraction Overhead         |\n"
         << "| Function Passing      | Decays to raw pointer `T*`        | Passed as object (Value/Ref)      |\n"
         << "| Bounds Safety         | None (Undefined Behavior)         | Safe via `.at()` exception        |\n"
         << "| Copy & Assignment     | Unsupported (`arr1 = arr2` invalid)| Supported (`arr1 = arr2` valid)   |\n"
         << "| STL Iterators         | Standard Pointers (`arr`, `arr+N`)| Full STL Iterators (`begin/end`)  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}