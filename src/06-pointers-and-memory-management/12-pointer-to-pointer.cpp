/*
 * =====================================================================================
 * CONCEPT        : Pointer to Pointer (Double Pointers `T**`) in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  multi-level pointer indirection (`T**`, `T***`):
 *
 *                  1. Indirection Mechanics     : Storing memory addresses of pointer variables.
 *                  2. Address Mapping           : Tracing memory layout: Value -> Single Pointer -> Double Pointer.
 *                  3. Multi-Level Dereference   : `*p2` (yields single pointer address) vs 
 *                                                 `**p2` (yields target value).
 *                  4. Rebinding vs Value Mutation: Modifying pointed-to addresses (`*p2 = &y`) vs 
 *                                                 modifying target values (`**p2 = val`).
 *                  5. Pointer-to-Pointer Params : Modifying caller pointers inside functions 
 *                                                 (`T**` vs modern C++ reference-to-pointer `T*&`).
 *                  6. Dynamic 2D Arrays         : Heap allocation (`new int*[rows]`) & proper RAII cleanup.
 *
 * TIME COMPLEXITY  : Direct access / Dereferencing: O(1) constant time.
 * SPACE COMPLEXITY : Pointer variable size: 8 bytes (64-bit systems) / 4 bytes (32-bit systems).
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS FOR FUNCTION PARAMETER DEMONSTRATIONS
// =====================================================================================

// C-Style: Modifies caller's pointer using a raw double pointer
void rebindPointerRaw(int** ptrToPtr, int* newTarget) {
    if (ptrToPtr != nullptr) {
        *ptrToPtr = newTarget; // Changes the memory address stored inside caller's single pointer
    }
}

// Modern C++ Style: Modifies caller's pointer using a reference to pointer
void rebindPointerRef(int*& ptrRef, int* newTarget) {
    ptrRef = newTarget; // Cleaner syntax, eliminates explicit double pointer dereference
}

// Dynamic 2D Array Allocator via Double Pointer
int** create2DArray(size_t rows, size_t cols, int initialValue) {
    int** matrix = new int*[rows]; // Allocate array of row pointers
    for (size_t r = 0; r < rows; ++r) {
        matrix[r] = new int[cols]; // Allocate row buffers
        for (size_t c = 0; c < cols; ++c) {
            matrix[r][c] = initialValue + static_cast<int>(r * cols + c);
        }
    }
    return matrix;
}

// Dynamic 2D Array Deallocator
void free2DArray(int**& matrix, size_t rows) {
    if (matrix == nullptr) return;
    for (size_t r = 0; r < rows; ++r) {
        delete[] matrix[r]; // Deallocate individual row buffers
    }
    delete[] matrix; // Deallocate pointer array header
    matrix = nullptr;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value to trace through double pointers (e.g., 42): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 42." << endl;
        userInputValue = 42;
    }

    // =====================================================================================
    // 1. POINTER TO POINTER DECLARATION & ADDRESS MAPPING
    // =====================================================================================
    cout << "\n================ 1. DECLARATION & MEMORY ADDRESS MAPPING ================\n";

    int baseValue = userInputValue;
    int* singlePtr = &baseValue;    // Single Pointer (stores address of baseValue)
    int** doublePtr = &singlePtr;   // Double Pointer (stores address of singlePtr)

    cout << "  - Base Variable `baseValue`   | Value: " << setw(5) << baseValue 
         << " | Address (&baseValue): " << static_cast<const void*>(&baseValue) << "\n";
    cout << "  - Single Pointer `singlePtr`  | Stores: " << static_cast<const void*>(singlePtr) 
         << " | Address (&singlePtr): " << static_cast<const void*>(&singlePtr) << "\n";
    cout << "  - Double Pointer `doublePtr`  | Stores: " << static_cast<const void*>(doublePtr) 
         << " | Address (&doublePtr): " << static_cast<const void*>(&doublePtr) << "\n";

    // =====================================================================================
    // 2. MULTI-LEVEL DEREFERENCING (`*p2` vs `**p2`)
    // =====================================================================================
    cout << "\n================ 2. MULTI-LEVEL DEREFERENCING ================\n";

    cout << "  - Single Dereference `*doublePtr`  (Yields singlePtr value) : " 
         << static_cast<const void*>(*doublePtr) << " (Matches &baseValue)\n";
    cout << "  - Double Dereference `**doublePtr` (Yields target value)   : " 
         << **doublePtr << " (Matches baseValue)\n";

    // Modifying target value through double pointer
    **doublePtr = 100;
    cout << "  - Value of `baseValue` post `**doublePtr = 100`             : " << baseValue << "\n";

    // =====================================================================================
    // 3. REBINDING TARGET ADDRESS VIA DOUBLE POINTER
    // =====================================================================================
    cout << "\n================ 3. REBINDING POINTER ADDRESS VIA `T**` ================\n";

    int secondaryValue = 999;
    cout << "  - Secondary Variable `secondaryValue` Address               : " 
         << static_cast<const void*>(&secondaryValue) << "\n";

    // Rebinding singlePtr target through doublePtr
    *doublePtr = &secondaryValue;

    cout << "  - Single Pointer `singlePtr` now points to                   : " 
         << static_cast<const void*>(singlePtr) << "\n";
    cout << "  - Dereferenced `*singlePtr` Value                           : " << *singlePtr << "\n";
    cout << "  - Double Dereferenced `**doublePtr` Value                   : " << **doublePtr << "\n";

    // Reset back to baseValue for subsequent sections
    *doublePtr = &baseValue;

    // =====================================================================================
    // 4. FUNCTION PARAMETERS: MODIFYING POINTERS (`T**` vs `T*&`)
    // =====================================================================================
    cout << "\n================ 4. FUNCTION PARAMETERS: MODIFYING POINTERS ================\n";

    int targetA = 10;
    int targetB = 20;
    int* activePtr = &targetA;

    cout << "  - `activePtr` before function calls points to               : " 
         << static_cast<const void*>(activePtr) << " (Value: " << *activePtr << ")\n";

    // C-Style: Pass address of pointer (&activePtr)
    rebindPointerRaw(&activePtr, &targetB);
    cout << "  - `activePtr` after `rebindPointerRaw(&activePtr, &targetB)` : " 
         << static_cast<const void*>(activePtr) << " (Value: " << *activePtr << ")\n";

    // Modern C++ Style: Pass pointer by reference
    rebindPointerRef(activePtr, &targetA);
    cout << "  - `activePtr` after `rebindPointerRef(activePtr, &targetA)`  : " 
         << static_cast<const void*>(activePtr) << " (Value: " << *activePtr << ")\n";

    // =====================================================================================
    // 5. DYNAMIC 2D ARRAYS USING DOUBLE POINTERS
    // =====================================================================================
    cout << "\n================ 5. DYNAMIC 2D ARRAYS VIA DOUBLE POINTERS ================\n";

    constexpr size_t ROWS = 2;
    constexpr size_t COLS = 3;

    int** matrix = create2DArray(ROWS, COLS, 10);

    cout << "  - Heap 2D Matrix Base Pointer Address (`matrix`)            : " 
         << static_cast<const void*>(matrix) << "\n";

    for (size_t r = 0; r < ROWS; ++r) {
        cout << "    * Row [" << r << "] Header Address (`matrix[" << r << "]`): " 
             << static_cast<const void*>(matrix[r]) << " | Elements: [ ";
        for (size_t c = 0; c < COLS; ++c) {
            cout << matrix[r][c] << " ";
        }
        cout << "]\n";
    }

    // Cleanup dynamic matrix
    free2DArray(matrix, ROWS);
    cout << "  - Matrix deallocated safely via `free2DArray()`. Matrix set to `nullptr`.\n";

    // =====================================================================================
    // 6. TRIPLE POINTERS (`T***`) & ARBITRARY INDIRECTION DEPTH
    // =====================================================================================
    cout << "\n================ 6. HIGHER-LEVEL INDIRECTION (`T***`) ================\n";

    int tripleBase = 777;
    int* p1 = &tripleBase;
    int** p2 = &p1;
    int*** p3 = &p2; // Triple Pointer storing address of double pointer

    cout << "  - Triple Pointer Value (`***p3`)                             : " << ***p3 << "\n";
    cout << "  - Size of `T*`, `T**`, and `T***`                            : " 
         << sizeof(p1) << " B | " << sizeof(p2) << " B | " << sizeof(p3) << " B (Uniform architecture size)\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ POINTER TO POINTER SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Indirection Level     | Syntax Example                    | Key Characteristics & Use Case    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Single Pointer        | `int* p1 = &var;`                 | Point to data values directly     |\n"
         << "| Double Pointer        | `int** p2 = &p1;`                 | Point to pointer variable addresses|\n"
         << "| Triple Pointer        | `int*** p3 = &p2;`                | Arbitrary deep indirection chains |\n"
         << "| Dereference Level 1   | `*p2`                             | Yields single pointer (`int*`)    |\n"
         << "| Dereference Level 2   | `**p2`                            | Yields underlying target value    |\n"
         << "| C-Style Function Param| `void func(int** p)`              | Allows function to rebind caller's|\n"
         << "| C++ Reference Param   | `void func(int*& p)`              | Modern clean syntax to rebind ptr |\n"
         << "| Dynamic 2D Array      | `int** arr = new int*[N]`         | Row-header list allocation        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}