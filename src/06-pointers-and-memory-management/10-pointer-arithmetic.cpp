/*
 * =====================================================================================
 * CONCEPT        : Pointer Arithmetic in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade implementation covering:
 *                  1. Byte Scaling Mechanics   : Operations (`+`, `-`, `++`, `--`) scale by 
 *                                                 `sizeof(T)` bytes automatically.
 *                  2. Pointer Increment/Decrement: Advancing and rewinding across contiguous memory.
 *                  3. Pointer Difference       : Subtracting two pointers (`p2 - p1`) yielding 
 *                                                 a signed distance of type `std::ptrdiff_t`.
 *                  4. Subscript Equivalence    : `arr[i]` is identical to `*(arr + i)` and `i[arr]`.
 *                  5. Pointer Relational Ops   : Comparing pointers (`<`, `>`, `==`) within 
 *                                                 the same contiguous array boundary.
 *
 * TIME COMPLEXITY  : Arithmetic operations & comparisons: O(1) constant time.
 * SPACE COMPLEXITY : Pointer size: 8 bytes (64-bit systems) / 4 bytes (32-bit systems).
 * =====================================================================================
 */

#include <iostream>
#include <cstddef>
#include <limits>

using namespace std;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userOffset = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an element offset step for pointer arithmetic demonstration (e.g., 2): " << flush;
    if (!(cin >> userOffset) || userOffset < 0 || userOffset > 4) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid or out-of-range offset. Defaulting offset step to 2." << endl;
        userOffset = 2;
    }

    // =====================================================================================
    // 1. SCALING MECHANICS & INCREMENT / DECREMENT (`++`, `--`)
    // =====================================================================================
    cout << "\n================ 1. POINTER SCALING & INCREMENT / DECREMENT ================\n";

    int intArray[5] = {10, 20, 30, 40, 50};
    double doubleArray[5] = {1.1, 2.2, 3.3, 4.4, 5.5};

    int* pInt = intArray;          // Points to intArray[0]
    double* pDouble = doubleArray; // Points to doubleArray[0]

    cout << "  - Base `pInt` Address    : " << static_cast<const void*>(pInt) 
         << " | Value: " << *pInt << "\n";
    cout << "  - Base `pDouble` Address : " << static_cast<const void*>(pDouble) 
         << " | Value: " << *pDouble << "\n";

    // Incrementing pointers
    ++pInt;    // Advances address by sizeof(int) = 4 bytes
    ++pDouble; // Advances address by sizeof(double) = 8 bytes

    cout << "\n  - [POST `++pInt`] Address    : " << static_cast<const void*>(pInt) 
         << " | Value: " << *pInt << " (+ " << sizeof(int) << " bytes offset)\n";
    cout << "  - [POST `++pDouble`] Address : " << static_cast<const void*>(pDouble) 
         << " | Value: " << *pDouble << " (+ " << sizeof(double) << " bytes offset)\n";

    // Decrementing back to start
    --pInt;
    --pDouble;

    // =====================================================================================
    // 2. POINTER ADDITION & SUBTRACTION (`ptr + n`, `ptr - n`)
    // =====================================================================================
    cout << "\n================ 2. POINTER ADDITION & SUBTRACTION ================\n";

    int* pOffset = intArray + userOffset; // Moves forward by userOffset * sizeof(int)

    cout << "  - Base Address (`intArray`)          : " << static_cast<const void*>(intArray) 
         << " | Value: " << intArray[0] << "\n";
    cout << "  - Offset Address (`intArray + " << userOffset << "`) : " << static_cast<const void*>(pOffset) 
         << " | Value: " << *pOffset << "\n";

    // Subtracting offset back
    int* pBack = pOffset - userOffset;
    cout << "  - Rewound Address (`pOffset - " << userOffset << "`) : " << static_cast<const void*>(pBack) 
         << " | Value: " << *pBack << "\n";

    // =====================================================================================
    // 3. POINTER DIFFERENCE (`ptr2 - ptr1` -> `std::ptrdiff_t`)
    // =====================================================================================
    cout << "\n================ 3. POINTER DIFFERENCE (`ptrdiff_t`) ================\n";

    int* pStart = &intArray[0];
    int* pEnd = &intArray[4];

    std::ptrdiff_t elementDistance = pEnd - pStart;
    std::ptrdiff_t byteDistance = reinterpret_cast<const char*>(pEnd) - reinterpret_cast<const char*>(pStart);

    cout << "  - Start Address (`&intArray[0]`) : " << static_cast<const void*>(pStart) << "\n";
    cout << "  - End Address   (`&intArray[4]`) : " << static_cast<const void*>(pEnd) << "\n";
    cout << "  - Distance in Elements (`pEnd - pStart`) : " << elementDistance << " elements\n";
    cout << "  - Distance in Raw Bytes                 : " << byteDistance << " bytes (" 
         << elementDistance << " * " << sizeof(int) << " bytes)\n";

    // =====================================================================================
    // 4. ARRAY SUBSCRIPT EQUIVALENCE (`arr[i]` == `*(arr + i)`)
    // =====================================================================================
    cout << "\n================ 4. SUBSCRIPT EQUIVALENCE ================\n";

    size_t testIdx = static_cast<size_t>(userOffset);

    cout << "  - Standard Subscript Notation `intArray[" << testIdx << "]` : " << intArray[testIdx] << "\n";
    cout << "  - Pointer Offset Dereference `*(intArray + " << testIdx << ")`: " << *(intArray + testIdx) << "\n";
    cout << "  - Commutative Subscript Notation `" << testIdx << "[intArray]` : " << testIdx[intArray] 
         << " (Valid C++ syntax since a[b] == *(a + b)!)\n";

    // =====================================================================================
    // 5. POINTER RELATIONAL COMPARISONS (`<`, `>`, `==`, `!=`)
    // =====================================================================================
    cout << "\n================ 5. POINTER RELATIONAL COMPARISONS ================\n";

    int* pCurrent = intArray;
    int* pLimit = intArray + 5; // One past the end pointer

    cout << "  - Iterating array using pointer comparison (`pCurrent < pLimit`):\n    [ ";
    while (pCurrent < pLimit) {
        cout << *pCurrent << " ";
        ++pCurrent;
    }
    cout << "]\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ POINTER ARITHMETIC SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Operation             | Expression / Syntax               | Scaled Byte Offset Result         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Increment             | `++ptr` / `ptr++`                 | Advances address by `1 * sizeof(T)`|\n"
         << "| Decrement             | `--ptr` / `ptr--`                 | Rewinds address by `1 * sizeof(T)` |\n"
         << "| Pointer Addition      | `ptr + n`                         | Advances address by `n * sizeof(T)`|\n"
         << "| Pointer Subtraction   | `ptr - n`                         | Rewinds address by `n * sizeof(T)` |\n"
         << "| Pointer Difference    | `ptr2 - ptr1`                     | Yields distance of `ptrdiff_t`    |\n"
         << "| Subscript Equivalence | `arr[i]` == `*(arr + i)`          | Array index is syntactic pointer +|\n"
         << "| Relational Compare    | `ptr1 < ptr2`, `ptr1 == ptr2`     | Compares underlying memory addrs  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}