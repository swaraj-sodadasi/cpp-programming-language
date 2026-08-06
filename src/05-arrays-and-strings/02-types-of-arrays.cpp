/*
 * =====================================================================================
 * CONCEPT        : Types of Arrays in C++
 * DESCRIPTION    : Comprehensive guide and implementation demonstrating the 6 main
 *                  types of arrays in C++:
 *                  1. Fixed C-Style Static Arrays : Stack-allocated, compile-time fixed size (`T arr[N]`).
 *                  2. STL Container Static Arrays : Modern, type-safe stack wrapper (`std::array<T, N>`).
 *                  3. Dynamic Heap Arrays         : Allocated on heap at runtime (`std::unique_ptr<T[]>` / `new[]`).
 *                  4. Dynamic Resizable Arrays    : Automatically resizable contiguous memory (`std::vector<T>`).
 *                  5. Multidimensional Arrays     : Rectangular 2D/3D grids (`T matrix[R][C]`).
 *                  6. Jagged / Variable-Row Arrays: Array of pointers storing rows of varying lengths (`T* jagged[R]`).
 *
 * TIME COMPLEXITY  : Index Access: O(1) | Search: O(N) | Resizing (vector): Amortized O(1)
 * SPACE COMPLEXITY : O(N) total allocated memory across stack and heap segments.
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <vector>
#include <memory>

using namespace std;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t dynamicSizeInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a size for the dynamic array demo (e.g., 5): " << flush;
    if (!(cin >> dynamicSizeInput) || dynamicSizeInput == 0) {
        cout << "Invalid input. Defaulting dynamic array size to 5." << endl;
        dynamicSizeInput = 5;
    }

    // =====================================================================================
    // 1. C-STYLE STATIC ARRAY (STACK ALLOCATED, FIXED SIZE)
    // Memory is allocated on the stack. Size must be known at compile time.
    // =====================================================================================
    cout << "\n================ 1. C-STYLE STATIC ARRAY (`int arr[N]`) ================\n";
    int cStyleArr[4] = {10, 20, 30, 40};

    cout << "  - Memory Allocation : Stack Segment\n";
    cout << "  - Array Elements    : ";
    for (size_t i = 0; i < 4; ++i) {
        cout << cStyleArr[i] << " ";
    }
    cout << "\n  - Total Size Bytes  : " << sizeof(cStyleArr) << " bytes\n";

    // =====================================================================================
    // 2. MODERN STL STATIC ARRAY (`std::array<T, N>`)
    // Fixed size on stack, but provides STL iterators, `.at()` bounds checking, and copy semantics.
    // =====================================================================================
    cout << "\n================ 2. STL STATIC ARRAY (`std::array<T, N>`) ================\n";
    std::array<int, 4> stlStaticArr = {100, 200, 300, 400};

    cout << "  - Memory Allocation : Stack Segment (Zero overhead over C-style)\n";
    cout << "  - Element Access    : stlStaticArr.at(2) = " << stlStaticArr.at(2) << "\n";
    cout << "  - Front & Back      : Front = " << stlStaticArr.front() << " | Back = " << stlStaticArr.back() << "\n";
    cout << "  - Iteration (Range) : ";
    for (int val : stlStaticArr) {
        cout << val << " ";
    }
    cout << "\n";

    // =====================================================================================
    // 3. DYNAMIC HEAP ARRAY (`std::unique_ptr<T[]>` / `new[]`)
    // Allocated on the heap at runtime when array size is not known until execution.
    // Managed via smart pointer (RAII) to guarantee clean deallocation without memory leaks.
    // =====================================================================================
    cout << "\n================ 3. DYNAMIC HEAP ARRAY (`std::unique_ptr<T[]>`) ================\n";
    
    // Allocate raw array on heap wrapped in RAII unique_ptr
    auto heapArr = std::make_unique<int[]>(dynamicSizeInput);
    
    for (size_t i = 0; i < dynamicSizeInput; ++i) {
        heapArr[i] = static_cast<int>((i + 1) * 11);
    }

    cout << "  - Memory Allocation : Heap Segment (Size determined at runtime)\n";
    cout << "  - Allocated Count   : " << dynamicSizeInput << " elements\n";
    cout << "  - Heap Elements     : ";
    for (size_t i = 0; i < dynamicSizeInput; ++i) {
        cout << heapArr[i] << " ";
    }
    cout << "\n  - Deallocation      : Automatic RAII destructor cleans up heap memory\n";

    // =====================================================================================
    // 4. DYNAMIC RESIZABLE ARRAY (`std::vector<T>`)
    // Heap-allocated contiguous memory that automatically grows or shrinks as elements are added/removed.
    // =====================================================================================
    cout << "\n================ 4. DYNAMIC RESIZABLE ARRAY (`std::vector<T>`) ================\n";
    std::vector<int> dynamicVec = {5, 10, 15};

    cout << "  - Initial State     : Size = " << dynamicVec.size() << " | Capacity = " << dynamicVec.capacity() << "\n";
    dynamicVec.push_back(20);
    dynamicVec.push_back(25);
    cout << "  - After `push_back` : Size = " << dynamicVec.size() << " | Capacity = " << dynamicVec.capacity() << "\n";
    cout << "  - Vector Elements   : ";
    for (int v : dynamicVec) {
        cout << v << " ";
    }
    cout << "\n";

    // =====================================================================================
    // 5. MULTIDIMENSIONAL ARRAY (RECTANGULAR GRID)
    // Stores data in row-major layout in contiguous memory.
    // =====================================================================================
    cout << "\n================ 5. MULTIDIMENSIONAL ARRAY (2D RECTANGULAR GRID) ================\n";
    int grid[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };

    cout << "  - Grid Structure (2 Rows x 3 Columns):\n";
    for (size_t r = 0; r < 2; ++r) {
        cout << "    Row " << r << ": ";
        for (size_t c = 0; c < 3; ++c) {
            cout << grid[r][c] << " ";
        }
        cout << "\n";
    }

    // =====================================================================================
    // 6. JAGGED ARRAY (ARRAY OF VARYING-LENGTH ROWS)
    // An array of pointers where each row can have a completely different size.
    // =====================================================================================
    cout << "\n================ 6. JAGGED ARRAY (VARIABLE ROW LENGTHS) ================\n";
    
    // Rows with lengths 2, 4, and 3 respectively
    int row1[] = {11, 12};
    int row2[] = {21, 22, 23, 24};
    int row3[] = {31, 32, 33};

    int* jaggedArr[3] = {row1, row2, row3};
    size_t rowSizes[3] = {2, 4, 3};

    cout << "  - Jagged Structure (Varying column sizes per row):\n";
    for (size_t r = 0; r < 3; ++r) {
        cout << "    Row " << r << " (" << rowSizes[r] << " cols): ";
        for (size_t c = 0; c < rowSizes[r]; ++c) {
            cout << jaggedArr[r][c] << " ";
        }
        cout << "\n";
    }

    // =====================================================================================
    // COMPARISON & SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ TYPES OF ARRAYS COMPARISON SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Array Type            | Memory Location   | Size Determinance | Resizable? / Key Characteristics  |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| C-Style Static        | Stack / Static    | Compile Time      | No  | Raw pointer decay, zero opt |\n"
         << "| `std::array<T, N>`    | Stack / Static    | Compile Time      | No  | Type-safe, STL container methods  |\n"
         << "| Dynamic Heap Array    | Heap              | Runtime           | No  | Fixed heap size, manual/RAII mgmt |\n"
         << "| `std::vector<T>`      | Heap              | Runtime           | YES | Dynamic growth, automatic memory  |\n"
         << "| Multidimensional      | Stack / Heap      | Compile/Runtime   | No  | Contiguous row-major grid         |\n"
         << "| Jagged Array          | Stack / Heap      | Runtime           | No  | Array of pointers, non-uniform    |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}