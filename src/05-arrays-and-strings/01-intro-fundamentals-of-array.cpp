/*
 * =====================================================================================
 * CONCEPT        : Introduction and Fundamentals of Arrays in C++
 * DESCRIPTION    : Comprehensive guide and implementation of Array fundamentals:
 *                  1. Array Definition & Layout   : Contiguous memory allocation storing fixed-size 
 *                                                   homogeneous elements.
 *                  2. Initialization Styles       : Aggregate, zero-initialization, partial fill, 
 *                                                   and automatic size deduction.
 *                  3. Element Access & Indexing   : 0-based indexing, pointer arithmetic equivalents, 
 *                                                   and memory address inspection.
 *                  4. Size Calculation            : Legacy `sizeof(arr)/sizeof(arr[0])` vs 
 *                                                   Modern C++ `std::size(arr)`.
 *                  5. Array Decay & Pass-By-Ref   : Passing arrays to functions (decaying to pointers 
 *                                                   vs passing by reference/template).
 *                  6. Multidimensional Arrays     : 2D arrays and row-major contiguous layout.
 *                  7. Modern C++ `std::array`     : Modern wrapper providing array performance 
 *                                                   with STL container safety.
 *
 * TIME COMPLEXITY  : Access by index: O(1) | Linear Search: O(N) | Traversal: O(N)
 * SPACE COMPLEXITY : Static allocation: O(N) on stack.
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <cstddef>
#include <iomanip>
#include <string>

using namespace std;

// =====================================================================================
// SECTION 1: ARRAY DECAY DEMONSTRATION & PASSING TO FUNCTIONS
// =====================================================================================

// Function 1: Array decays to pointer (Size information is LOST)
void processDecayedArray(const int* arr, size_t size) {
    cout << "  - [Decayed Array] sizeof(arr) inside function = " << sizeof(arr) 
         << " bytes (Pointer size, NOT array size!)\n";
    cout << "    Elements: ";
    for (size_t i = 0; i < size; ++i) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

// Function 2: Pass by Reference via Template (Preserves size & bounds at compile time)
template <size_t N>
void processArrayByReference(const int (&arr)[N]) {
    cout << "  - [Pass-By-Reference] Compile-time size detected via template N = " << N << "\n";
    cout << "    sizeof(arr) inside function = " << sizeof(arr) << " bytes (Full array size preserved!)\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int searchTarget = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an integer search target value for array inspection (e.g., 30): " << flush;
    if (!(cin >> searchTarget)) {
        cout << "Invalid input. Defaulting search target to 30." << endl;
        searchTarget = 30;
    }

    // =====================================================================================
    // 1. ARRAY INITIALIZATION & CONTIGUOUS MEMORY
    // =====================================================================================
    cout << "\n================ 1. DECLARATION & INITIALIZATION ================\n";
    
    int uninitArr[5];                   // Uninitialized array demo (Contains indeterminate values)
    (void)uninitArr;                    // RESOLVED: Explicitly void-cast to suppress -Wunused-variable warning

    int zeroArr[5] = {};                // Zero-initialized: {0, 0, 0, 0, 0}
    int aggregateArr[5] = {10, 20, 30}; // Partial init: {10, 20, 30, 0, 0}
    int deducedArr[] = {5, 15, 25, 35};  // Deduced size: 4 elements

    cout << "  - Zero-initialized array [0]: " << zeroArr[0] << ", [4]: " << zeroArr[4] << "\n";
    cout << "  - Aggregate initialized array elements: ";
    for (int val : aggregateArr) {
        cout << val << " ";
    }
    cout << "\n";

    // Inspect contiguous memory addresses
    cout << "\n  - Contiguous Memory Inspection (4-byte spacing for int):\n";
    for (size_t i = 0; i < 4; ++i) {
        cout << "    deducedArr[" << i << "] | Value: " << setw(2) << deducedArr[i] 
             << " | Address: " << static_cast<const void*>(&deducedArr[i]) << "\n";
    }

    // =====================================================================================
    // 2. INDEXING & POINTER ARITHMETIC EQUIVALENCE
    // =====================================================================================
    cout << "\n================ 2. INDEXING & POINTER ARITHMETIC ================\n";
    cout << "  - arr[i] is syntactically equivalent to *(arr + i):\n";
    cout << "    aggregateArr[2]   = " << aggregateArr[2] << "\n";
    cout << "    *(aggregateArr+2) = " << *(aggregateArr + 2) << "\n";
    cout << "    2[aggregateArr]   = " << 2[aggregateArr] << " (Valid C++ syntax due to commutativity of addition!)\n";

    // Linear search demo
    bool found = false;
    for (size_t i = 0; i < std::size(aggregateArr); ++i) {
        if (aggregateArr[i] == searchTarget) {
            cout << "  - Target " << searchTarget << " FOUND at index [" << i << "].\n";
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "  - Target " << searchTarget << " NOT found in aggregateArr.\n";
    }

    // =====================================================================================
    // 3. SIZE CALCULATION (sizeof vs std::size)
    // =====================================================================================
    cout << "\n================ 3. COMPUTING ARRAY SIZE ================\n";
    size_t legacySize = sizeof(aggregateArr) / sizeof(aggregateArr[0]);
    size_t modernSize = std::size(aggregateArr); // C++17 std::size

    cout << "  - Total byte size of aggregateArr : " << sizeof(aggregateArr) << " bytes\n";
    cout << "  - Element size                    : " << sizeof(aggregateArr[0]) << " bytes\n";
    cout << "  - Calculated Count (Legacy sizeof): " << legacySize << " elements\n";
    cout << "  - Calculated Count (C++17 std::size): " << modernSize << " elements\n";

    // =====================================================================================
    // 4. ARRAY DECAY & PASSING TO FUNCTIONS
    // =====================================================================================
    cout << "\n================ 4. ARRAY DECAY vs PASS-BY-REFERENCE ================\n";
    int sampleScores[] = {95, 88, 72, 100, 64};
    
    // Demonstrating decay
    processDecayedArray(sampleScores, std::size(sampleScores));
    
    // Demonstrating pass by reference preserving type
    processArrayByReference(sampleScores);

    // =====================================================================================
    // 5. MULTIDIMENSIONAL ARRAYS (2D MATRIX)
    // =====================================================================================
    cout << "\n================ 5. MULTIDIMENSIONAL ARRAYS (ROW-MAJOR LAYOUT) ================\n";
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };

    cout << "  - 2x3 Matrix Traversal:\n";
    for (size_t r = 0; r < 2; ++r) {
        cout << "    Row " << r << ": ";
        for (size_t c = 0; c < 3; ++c) {
            cout << matrix[r][c] << " ";
        }
        cout << "\n";
    }

    // Prove contiguous row-major memory mapping
    const int* flatPtr = &matrix[0][0];
    cout << "  - Flattened memory inspection of 2D matrix: ";
    for (size_t i = 0; i < 6; ++i) {
        cout << flatPtr[i] << " ";
    }
    cout << "\n";

    // =====================================================================================
    // 6. MODERN C++ ALTERNATIVE: std::array
    // =====================================================================================
    cout << "\n================ 6. MODERN C++ ALTERNATIVE (std::array) ================\n";
    std::array<int, 4> modernArr = {100, 200, 300, 400};

    cout << "  - std::array size()     : " << modernArr.size() << "\n";
    cout << "  - Bounds-checked at()   : modernArr.at(2) = " << modernArr.at(2) << "\n";
    cout << "  - Front & Back access   : Front = " << modernArr.front() << " | Back = " << modernArr.back() << "\n";

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ ARRAYS FUNDAMENTALS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature               | Raw C-Style Array (`int arr[N]`)  | Modern Container (`std::array`)   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Size Flexibility      | Fixed at compile time             | Fixed at compile time             |\n"
         << "| Memory Location       | Stack / Static Segment            | Stack / Static Segment            |\n"
         << "| Function Passing      | Decays to raw pointer `T*`        | Passed as object (Value/Ref)      |\n"
         << "| Bounds Safety         | None (Undefined Behavior)         | `.at()` throws `std::out_of_range`|\n"
         << "| STL Algorithm Support | Partial (via `std::begin/end`)    | Full STL Iterator Interface       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}