/*
 * =====================================================================================
 * CONCEPT        : C-Style Arrays in C++
 * DESCRIPTION    : Comprehensive implementation detailing C-style arrays:
 *                  1. Definition & Memory Layout : Contiguous block of homogeneous memory
 *                                                  allocated on stack/static storage.
 *                  2. Pointer Decay Mechanics    : Automatic conversion of array name to pointer 
 *                                                  to first element when evaluated or passed.
 *                  3. Pointer Arithmetic         : Equivalence of `arr[i]` and `*(arr + i)`.
 *                  4. Multi-dimensional Arrays   : Row-major contiguous layout for 2D/ND arrays.
 *                  5. Character Arrays (Strings) : Special C-style arrays terminated by '\0'.
 *                  6. Limitations & Drawbacks    : Lack of bounds checking, fixed compile-time size,
 *                                                  decay losing size information.
 *
 * TIME COMPLEXITY  : Index Access: O(1) | Search: O(N) | Traversal: O(N)
 * SPACE COMPLEXITY : Fixed stack allocation: O(N)
 * =====================================================================================
 */

#include <iostream>
#include <cstddef>
#include <iomanip>
#include <cstring>

using namespace std;

// =====================================================================================
// 1. FUNCTION DEMONSTRATING ARRAY DECAY
// When passed to functions, C-style arrays decay into raw pointers (`const int*`),
// losing all implicit size information.
// =====================================================================================
void processDecayedArray(const int* arr, size_t size) {
    cout << "  - [Decayed Array Pass] Base Address Received: " << static_cast<const void*>(arr) << "\n";
    cout << "    sizeof(arr) inside function = " << sizeof(arr) 
         << " bytes (Pointer size, lost original array dimensions!)\n";
    cout << "    Elements: ";
    for (size_t i = 0; i < size; ++i) {
        cout << arr[i] << (i + 1 < size ? ", " : "");
    }
    cout << "\n";
}

// =====================================================================================
// 2. FUNCTION DEMONSTRATING PASS-BY-REFERENCE VIA TEMPLATES
// Passing array by reference (`const int (&arr)[N]`) prevents decay and preserves size.
// =====================================================================================
template <size_t N>
void processArrayByRef(const int (&arr)[N]) {
    cout << "  - [Pass-by-Reference] Preserved compile-time size N = " << N << "\n";
    cout << "    sizeof(arr) inside function = " << sizeof(arr) << " bytes (Full array size preserved!)\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int scaleMultiplier = 1;

    // Dynamic input collection with stream flushing
    cout << "Enter a multiplier integer for array scaling (e.g., 2): " << flush;
    if (!(cin >> scaleMultiplier) || scaleMultiplier <= 0) {
        cout << "Invalid input. Defaulting multiplier to 2." << endl;
        scaleMultiplier = 2;
    }

    // =====================================================================================
    // 1. DECLARATION AND INITIALIZATION SYNTAX
    // =====================================================================================
    cout << "\n================ 1. DECLARATION & INITIALIZATION ================\n";
    
    int arrZero[5] = {};              // Zero-initialized: {0, 0, 0, 0, 0}
    int arrInit[5] = {10, 20, 30};    // Partial initialization: {10, 20, 30, 0, 0}
    int arrDeduced[] = {1, 2, 3, 4};  // Size deduced automatically (4 elements)

    cout << "  - Zero-initialized array element [0]: " << arrZero[0] << ", [4]: " << arrZero[4] << "\n";
    cout << "  - Partially initialized array elements: ";
    for (size_t i = 0; i < 5; ++i) {
        cout << arrInit[i] << " ";
    }
    cout << "\n  - Deduced size array length: " << (sizeof(arrDeduced) / sizeof(arrDeduced[0])) << " elements\n";

    // =====================================================================================
    // 2. POINTER DECAY & POINTER ARITHMETIC EQUIVALENCE
    // =====================================================================================
    cout << "\n================ 2. POINTER DECAY & POINTER ARITHMETIC ================\n";
    int numbers[] = {100, 200, 300, 400, 500};
    
    cout << "  - Array name 'numbers' decays to pointer to first element:\n";
    cout << "    Address of numbers    : " << static_cast<void*>(numbers) << "\n";
    cout << "    Address of numbers[0] : " << static_cast<void*>(&numbers[0]) << "\n";

    cout << "  - Array subscripting equivalence `arr[i] == *(arr + i)`:\n";
    for (size_t i = 0; i < 5; ++i) {
        cout << "    numbers[" << i << "] = " << setw(3) << numbers[i] 
             << " | *(numbers + " << i << ") = " << setw(3) << *(numbers + i) 
             << " | Addr: " << static_cast<void*>(numbers + i) << "\n";
    }

    // =====================================================================================
    // 3. ARRAY DECAY WHEN PASSED TO FUNCTIONS
    // =====================================================================================
    cout << "\n================ 3. PASSING ARRAYS TO FUNCTIONS ================\n";
    size_t numElements = sizeof(numbers) / sizeof(numbers[0]);
    processDecayedArray(numbers, numElements);
    processArrayByRef(numbers);

    // =====================================================================================
    // 4. MULTI-DIMENSIONAL C-STYLE ARRAYS
    // Stored sequentially in memory using row-major ordering.
    // =====================================================================================
    cout << "\n================ 4. MULTI-DIMENSIONAL C-STYLE ARRAYS ================\n";
    int grid[2][3] = {
        {10, 20, 30},
        {40, 50, 60}
    };

    cout << "  - 2D Grid (2 rows x 3 cols) row-major memory mapping:\n";
    for (size_t r = 0; r < 2; ++r) {
        for (size_t c = 0; c < 3; ++c) {
            cout << "    grid[" << r << "][" << c << "] = " << setw(2) << grid[r][c]
                 << " @ Address: " << static_cast<void*>(&grid[r][c]) << "\n";
        }
    }

    // =====================================================================================
    // 5. C-STYLE CHARACTER ARRAYS (STRINGS)
    // Character arrays terminated by a null character ('\0').
    // =====================================================================================
    cout << "\n================ 5. C-STYLE CHARACTER ARRAYS (STRINGS) ================\n";
    char cstr[] = "C-Style String"; // Contains hidden '\0' null terminator at end
    cout << "  - C-String content : \"" << cstr << "\"\n";
    cout << "  - strlen(cstr)     : " << strlen(cstr) << " characters (Excludes null-terminator)\n";
    cout << "  - sizeof(cstr)     : " << sizeof(cstr) << " bytes (Includes '\\0')\n";

    // =====================================================================================
    // 6. SCALING OPERATION WITH DYNAMIC MULTIPLIER
    // =====================================================================================
    cout << "\n================ 6. ARRAY MODIFICATION (SCALING) ================\n";
    cout << "  - Scaling elements by user multiplier (" << scaleMultiplier << "): ";
    for (size_t i = 0; i < numElements; ++i) {
        numbers[i] *= scaleMultiplier;
        cout << numbers[i] << " ";
    }
    cout << "\n";

    // =====================================================================================
    // 7. SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ C-STYLE ARRAYS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Trait       | C-Style Array (`T arr[N]`)        | Modern Container (`std::array`)   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Size Storage          | Implicit (Not stored in object)   | Preserved via `.size()`           |\n"
         << "| Function Decay        | Decays to raw pointer (`T*`)      | Retains container object type     |\n"
         << "| Bounds Checking       | None (Undefined behavior)         | `.at()` throws `std::out_of_range`|\n"
         << "| Memory Overhead       | Zero (Pure raw memory block)      | Zero (Zero-abstraction overhead)  |\n"
         << "| Assignment / Copy     | Cannot assign `arr1 = arr2`       | Native copy assignment supported  |\n"
         << "| STL Interoperability  | Requires manual pointer bounds    | Native STL iterator interface     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}