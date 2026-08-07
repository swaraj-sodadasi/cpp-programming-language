/*
 * =====================================================================================
 * CONCEPT        : Arrays and Pointers in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the relationship, interplay, and key differences between arrays and pointers:
 *
 *                  1. Array Decay               : Automatic conversion of an array name to a 
 *                                                 pointer to its first element (`&arr[0]`).
 *                  2. Array Name vs Pointer     : `sizeof(arr)` vs `sizeof(ptr)`, mutability 
 *                                                 (`ptr++` allowed, `arr++` illegal).
 *                  3. Array Access Syntax       : `arr[i]`, `*(arr + i)`, `ptr[i]`, and `*(ptr + i)`.
 *                  4. Pointer to Array vs Array : `int (*pArr)[N]` (pointer to entire array) vs 
 *                     of Pointers                 `int* arrOfPtrs[N]` (array storing pointers).
 *                  5. Dynamic Arrays            : Allocating dynamic arrays via pointers (`new[]` / `delete[]`).
 *                  6. Array Function Parameters : Pass-by-pointer (decayed) vs pass-by-reference.
 *
 * TIME COMPLEXITY  : Access / Pointer Arithmetic: O(1) | Traversal / Allocation: O(N)
 * SPACE COMPLEXITY : Stack: O(N) for array storage | Pointer size: 8 bytes (64-bit) / 4 bytes (32-bit).
 * =====================================================================================
 */

#include <iostream>
#include <cstddef>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS FOR FUNCTION PARAMETER DEMONSTRATIONS
// =====================================================================================

// Function 1: Array parameters decay into raw pointers (Size is lost)
void processArrayAsPointer(const int* ptr, size_t size) {
    cout << "  - [Decayed Pointer Param] Address inside function: " 
         << static_cast<const void*>(ptr) << "\n";
    cout << "  - [Decayed Pointer Param] sizeof(ptr)            : " 
         << sizeof(ptr) << " bytes (Pointer size!)\n";
    cout << "  - Elements accessed via pointer offset           : [ ";
    for (size_t i = 0; i < size; ++i) {
        cout << *(ptr + i) << " ";
    }
    cout << "]\n";
}

// Function 2: Array pass-by-reference using templates (Preserves array size & type)
template <size_t N>
void processArrayByRef(const int (&arr)[N]) {
    cout << "  - [Array Reference Param] Preserved Compile-Time Size N = " << N << "\n";
    cout << "  - [Array Reference Param] sizeof(arr)                  = " 
         << sizeof(arr) << " bytes (Full array size preserved!)\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for array initialization (e.g., 10): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 10." << endl;
        userInputValue = 10;
    }

    // =====================================================================================
    // 1. ARRAY DECAY & ADDRESS EQUIVALENCE
    // =====================================================================================
    cout << "\n================ 1. ARRAY DECAY & ADDRESS EQUIVALENCE ================\n";

    int stackArray[4] = {userInputValue, userInputValue + 10, userInputValue + 20, userInputValue + 30};

    // An array name 'stackArray' implicitly decays to a pointer to its first element (&stackArray[0])
    int* ptrToFirst = stackArray;

    cout << "  - Array Base Name (`stackArray`)           : " << static_cast<const void*>(stackArray) << "\n";
    cout << "  - Address of 1st Element (`&stackArray[0]`) : " << static_cast<const void*>(&stackArray[0]) << "\n";
    cout << "  - Pointer Value (`ptrToFirst`)             : " << static_cast<const void*>(ptrToFirst) << "\n";
    cout << "  - [DECAY RULE]: `stackArray` and `&stackArray[0]` yield the exact same memory address.\n";

    // =====================================================================================
    // 2. KEY DIFFERENCES: ARRAY NAME VS POINTER
    // =====================================================================================
    cout << "\n================ 2. ARRAY NAME VS POINTER DIFFERENCES ================\n";

    cout << "  - Total byte size of Array (`sizeof(stackArray)`) : " << sizeof(stackArray) 
         << " bytes (4 ints * 4 bytes)\n";
    cout << "  - Byte size of Pointer     (`sizeof(ptrToFirst)`)  : " << sizeof(ptrToFirst) 
         << " bytes (Native address size)\n";

    // Pointer is a rebindable variable; Array name is an immutable lvalue
    ptrToFirst++; // Valid: Pointer now points to stackArray[1]
    cout << "  - After `ptrToFirst++`, points to element [1]     : " << *ptrToFirst << "\n";

    // stackArray++; // COMPILE ERROR: Cannot reassign or increment an array name!
    cout << "  - [MUTABILITY RULE]: Pointers can be incremented (`ptr++`), array names CANNOT (`arr++` is illegal).\n";

    // Rewind pointer back
    --ptrToFirst;

    // =====================================================================================
    // 3. POINTER TO ARRAY VS ARRAY OF POINTERS
    // =====================================================================================
    cout << "\n================ 3. POINTER TO ARRAY VS ARRAY OF POINTERS ================\n";

    // A. Pointer to an Array of 4 ints: int (*pArr)[4]
    int (*pArr)[4] = &stackArray;
    cout << "  - Pointer to Array (`pArr`) Base Address   : " << static_cast<const void*>(pArr) << "\n";
    cout << "  - Dereferenced `(*pArr)[0]` Value          : " << (*pArr)[0] << "\n";
    cout << "  - Incrementing `pArr + 1` jumps by full array size: +" << sizeof(*pArr) << " bytes!\n";

    // B. Array of Pointers: int* arrOfPtrs[3]
    int valA = 100, valB = 200, valC = 300;
    int* arrOfPtrs[3] = {&valA, &valB, &valC};

    cout << "  - Array of Pointers (`arrOfPtrs`):\n";
    for (size_t i = 0; i < 3; ++i) {
        cout << "    * Index [" << i << "] stores address: " << static_cast<const void*>(arrOfPtrs[i]) 
             << " | Dereferenced: " << *arrOfPtrs[i] << "\n";
    }

    // =====================================================================================
    // 4. ELEMENT ACCESS VIA POINTER ARITHMETIC
    // =====================================================================================
    cout << "\n================ 4. ACCESSING ELEMENTS VIA POINTER ARITHMETIC ================\n";

    cout << "  - Accessing elements of `stackArray`:\n";
    for (size_t i = 0; i < 4; ++i) {
        cout << "    Index [" << i << "] -> Subscript `stackArray[" << i << "]`: " << stackArray[i]
             << " | Pointer Arithmetic `*(ptrToFirst + " << i << ")`: " << *(ptrToFirst + i) << "\n";
    }

    // =====================================================================================
    // 5. DYNAMIC ARRAYS & POINTERS (`new[]` / `delete[]`)
    // =====================================================================================
    cout << "\n================ 5. DYNAMIC ARRAYS WITH POINTERS ================\n";

    constexpr size_t DYNAMIC_SIZE = 3;
    int* dynamicArray = new int[DYNAMIC_SIZE]{500, 600, 700};

    cout << "  - Dynamic Array allocated on heap at address: " << static_cast<const void*>(dynamicArray) << "\n";
    cout << "  - Dynamic Elements: [ ";
    for (size_t i = 0; i < DYNAMIC_SIZE; ++i) {
        cout << dynamicArray[i] << " ";
    }
    cout << "]\n";

    delete[] dynamicArray; // Deallocate dynamic array
    dynamicArray = nullptr;
    cout << "  - Heap memory deallocated via `delete[]` and pointer set to `nullptr`.\n";

    // =====================================================================================
    // 6. PASSING ARRAYS TO FUNCTIONS
    // =====================================================================================
    cout << "\n================ 6. PASSING ARRAYS TO FUNCTIONS ================\n";

    cout << "  - Calling `processArrayAsPointer` (Decays array to pointer):\n";
    processArrayAsPointer(stackArray, 4);

    cout << "\n  - Calling `processArrayByRef` (Preserves array reference & size):\n";
    processArrayByRef(stackArray);

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ ARRAYS VS POINTERS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Trait / Feature       | C-Style Array (`int arr[N]`)      | Pointer (`int* ptr`)              |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Definition            | Contiguous block of elements      | Variable storing a memory address |\n"
         << "| `sizeof` Operator     | Total array size (`N * sizeof(T)`)| Pointer variable size (4/8 bytes) |\n"
         << "| Re-assignability      | Non-modifiable lvalue (`arr++` Err)| Rebindable (`ptr++` / `ptr = &x`)  |\n"
         << "| Function Passing      | Decays automatically to `T*`      | Passed as pointer address value   |\n"
         << "| Element Subscripting  | `arr[i]` <==> `*(arr + i)`        | `ptr[i]` <==> `*(ptr + i)`        |\n"
         << "| Memory Location       | Stack / BSS / Data segment        | Stack (points to Stack or Heap)   |\n"
         << "| Pointer to Array      | `int (*p)[N] = &arr;`             | Pointer to single element `T*`    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}