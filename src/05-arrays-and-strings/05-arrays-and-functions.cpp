/*
 * =====================================================================================
 * CONCEPT        : Arrays and Functions in C++
 * DESCRIPTION    : Comprehensive guide demonstrating how arrays interact with functions:
 *                  1. Array Decay Mechanism        : Passing a raw C-style array passes a pointer 
 *                                                    to its first element (`T*`), losing size info.
 *                  2. Passing Size Explicitly      : Conventional C-style passing requiring `size_t`.
 *                  3. Passing Pass-by-Reference    : Using template references `const T (&arr)[N]` to preserve 
 *                                                    array dimensions and avoid decay.
 *                  4. Multidimensional Parameters  : Passing 2D/3D matrices (requires fixed column bounds).
 *                  5. Returning Arrays via Struct  : Returning arrays wrapped in a aggregate struct or `std::array`.
 *                  6. Modern Containers            : Passing `std::array` and `std::vector` by value / reference.
 *
 * TIME COMPLEXITY  : Function argument passing: O(1) for pointers/references, O(N) for pass-by-value copies.
 * SPACE COMPLEXITY : O(1) auxiliary stack frame overhead for reference/pointer passes.
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <vector>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. DECAYED PASSING (POINTER TO FIRST ELEMENT)
// Size information is completely lost inside the function.
// =====================================================================================

void printDecayedArray(const int* arr, size_t size) {
    cout << "  - [Decayed Pointer Pass] sizeof(arr) = " << sizeof(arr) 
         << " bytes (Pointer size!)\n    Elements: ";
    for (size_t i = 0; i < size; ++i) {
        cout << arr[i] << (i + 1 < size ? ", " : "");
    }
    cout << "\n";
}

// =====================================================================================
// 2. PASS-BY-REFERENCE VIA TEMPLATES (PRESERVES ARRAY TYPE & SIZE)
// Prevents decay to pointer; compiler enforces exact array bounds at compile time.
// =====================================================================================

template <typename T, size_t N>
void printArrayByReference(const T (&arr)[N]) {
    cout << "  - [Template Pass-By-Ref] Inferred Array Size N = " << N 
         << " | sizeof(arr) = " << sizeof(arr) << " bytes (Full array preserved!)\n    Elements: ";
    for (size_t i = 0; i < N; ++i) {
        cout << arr[i] << (i + 1 < N ? ", " : "");
    }
    cout << "\n";
}

// Function modifying array contents in-place via reference/pointer
template <typename T, size_t N>
void scaleArrayInPlace(T (&arr)[N], T factor) {
    for (size_t i = 0; i < N; ++i) {
        arr[i] *= factor;
    }
}

// =====================================================================================
// 3. PASSING MULTIDIMENSIONAL ARRAYS (2D MATRICES)
// C++ requires all dimensions except the first to be specified as constants.
// =====================================================================================

constexpr size_t MATRIX_COLS = 3;

void print2DMatrix(const int matrix[][MATRIX_COLS], size_t rows) {
    cout << "  - [2D Matrix Decay] Printing " << rows << "x" << MATRIX_COLS << " Matrix:\n";
    for (size_t r = 0; r < rows; ++r) {
        cout << "    Row " << r << ": ";
        for (size_t c = 0; c < MATRIX_COLS; ++c) {
            cout << matrix[r][c] << " ";
        }
        cout << "\n";
    }
}

// Template approach for fully generic dimensions
template <typename T, size_t R, size_t C>
void print2DMatrixGeneric(const T (&matrix)[R][C]) {
    cout << "  - [Generic Template 2D Matrix] " << R << "x" << C << " Matrix:\n";
    for (size_t r = 0; r < R; ++r) {
        cout << "    Row " << r << ": ";
        for (size_t c = 0; c < C; ++c) {
            cout << matrix[r][c] << " ";
        }
        cout << "\n";
    }
}

// =====================================================================================
// 4. RETURNING ARRAYS FROM FUNCTIONS
// Raw C-style local arrays CANNOT be returned from functions! (Dangling stack pointer).
// Solution: Wrap in std::array, struct, or std::vector.
// =====================================================================================

struct IntArrayWrapper {
    int values[4];
};

IntArrayWrapper createWrappedArray(int baseValue) {
    IntArrayWrapper wrapper;
    for (int i = 0; i < 4; ++i) {
        wrapper.values[i] = baseValue + i * 10;
    }
    return wrapper; // Safe return by value
}

std::array<int, 4> createStdArray(int baseValue) {
    std::array<int, 4> arr;
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = baseValue * static_cast<int>(i + 1);
    }
    return arr; // Modern, safe return by value (NRVO optimized)
}

// =====================================================================================
// 5. MODERN STL CONTAINERS WITH FUNCTIONS (`std::array` and `std::vector`)
// =====================================================================================

void processStdArrayRef(const std::array<int, 4>& arr) {
    cout << "  - [std::array Pass-By-Const-Ref] Size: " << arr.size() << " | Elements: ";
    for (int val : arr) {
        cout << val << " ";
    }
    cout << "\n";
}

void processVectorRef(const std::vector<int>& vec) {
    cout << "  - [std::vector Pass-By-Const-Ref] Size: " << vec.size() << " | Elements: ";
    for (int val : vec) {
        cout << val << " ";
    }
    cout << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int scaleFactorInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a scale factor integer for array transformation (e.g., 3): " << flush;
    if (!(cin >> scaleFactorInput) || scaleFactorInput == 0) {
        cout << "Invalid input. Defaulting scale factor to 3." << endl;
        scaleFactorInput = 3;
    }

    // 1. RAW C-STYLE ARRAY PASSING (DECAY VS TEMPLATE REFERENCE)
    cout << "\n================ 1. C-STYLE ARRAY PASSING & DECAY ================\n";
    int rawScores[5] = {10, 20, 30, 40, 50};

    printDecayedArray(rawScores, 5);
    printArrayByReference(rawScores);

    cout << "\n  - Scaling raw array elements in-place by factor " << scaleFactorInput << "...\n";
    scaleArrayInPlace(rawScores, scaleFactorInput);
    printArrayByReference(rawScores);

    // 2. MULTIDIMENSIONAL ARRAY PASSING
    cout << "\n================ 2. MULTIDIMENSIONAL ARRAYS & FUNCTIONS ================\n";
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };

    print2DMatrix(matrix, 2);
    print2DMatrixGeneric(matrix);

    // 3. RETURNING ARRAYS FROM FUNCTIONS
    cout << "\n================ 3. RETURNING ARRAYS FROM FUNCTIONS ================\n";
    IntArrayWrapper wrapped = createWrappedArray(100);
    cout << "  - Array returned via Struct Wrapper : [ ";
    for (int v : wrapped.values) {
        cout << v << " ";
    }
    cout << "]\n";

    std::array<int, 4> returnedStdArr = createStdArray(5);
    cout << "  - Array returned via std::array     : [ ";
    for (int v : returnedStdArr) {
        cout << v << " ";
    }
    cout << "]\n";

    // 4. MODERN STL CONTAINERS
    cout << "\n================ 4. MODERN STL CONTAINERS WITH FUNCTIONS ================\n";
    std::array<int, 4> modernArray = {100, 200, 300, 400};
    std::vector<int> dynamicVector = {5, 15, 25, 35, 45};

    processStdArrayRef(modernArray);
    processVectorRef(dynamicVector);

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ ARRAYS AND FUNCTIONS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Passing Mechanism     | Decay to Pointer? | Size Information  | Safety & Performance Notes        |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| `T arr[]` / `T* arr`  | YES (Decays)      | Lost (Needs `N`)  | O(1) pass; no bounds checking     |\n"
         << "| `const T (&arr)[N]`   | NO (Type intact)  | Preserved in `N`  | O(1) pass; compile-time bounds    |\n"
         << "| `const std::array&`   | NO (STL Container)| `.size()` method  | O(1) pass; full type safety       |\n"
         << "| `const std::vector&`  | NO (Dynamic Heap) | `.size()` method  | O(1) pass; dynamic runtime resize |\n"
         << "| Return Raw Array `T[]`| IMPOSSIBLE        | N/A               | Returns dangling stack pointer!   |\n"
         << "| Return `std::array`   | Safe Return       | Preserved         | Safe value return (NRVO optimized)|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}