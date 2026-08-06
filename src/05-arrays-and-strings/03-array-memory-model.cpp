/*
 * =====================================================================================
 * CONCEPT        : Array Memory Model in C++
 * DESCRIPTION    : Comprehensive guide and implementation illustrating the low-level 
 *                  memory model of arrays in C++:
 *                  1. Contiguous Memory Allocation: Array elements occupy sequential, 
 *                     unbroken memory addresses without padding between adjacent elements.
 *                  2. Address Calculation Formula  : Address(arr[i]) = BaseAddress + (i * sizeof(T)).
 *                  3. Alignment and Element Stride : Memory alignment rules for types and 
 *                     how pointer arithmetic accounts for element stride (`sizeof(T)`).
 *                  4. Stack vs Heap Memory Layout  : Memory address differences between stack-allocated 
 *                     arrays and heap-allocated arrays (`new[]` / `std::vector`).
 *                  5. Row-Major Order (2D Arrays) : Multi-dimensional arrays flattened into 1D memory 
 *                     using row-major ordering: Address(grid[r][c]) = Base + (r * COLS + c) * sizeof(T).
 *                  6. Cache Line Locality         : Spatial locality benefits of contiguous memory 
 *                     traversal on L1/L2 hardware CPU caches.
 *
 * TIME COMPLEXITY  : Element Offset Address Calculation: O(1)
 * SPACE COMPLEXITY : Stack/Heap Memory footprint: O(N * sizeof(T))
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <memory>
#include <cstdint>
#include <iomanip>
#include <cstddef>

using namespace std;

// Custom structure to demonstrate memory stride and alignment rules
struct AlignTestStruct {
    uint32_t id;    // 4 bytes
    double rate;    // 8 bytes (Forces 8-byte alignment)
    char flag;      // 1 byte (+ 7 bytes padding to align array stride to 24 bytes)
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t elementCountInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter array element count for memory layout inspection (e.g., 4): " << flush;
    if (!(cin >> elementCountInput) || elementCountInput == 0) {
        cout << "Invalid input. Defaulting element count to 4." << endl;
        elementCountInput = 4;
    }

    // =====================================================================================
    // 1. STACK ARRAY MEMORY MODEL & ADDRESS CALCULATION FORMULA
    // =====================================================================================
    cout << "\n================ 1. CONTIGUOUS STACK MEMORY & ADDRESS FORMULA ================\n";
    
    // Allocate stack array
    int stackArr[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    uintptr_t baseAddr = reinterpret_cast<uintptr_t>(&stackArr[0]);

    cout << "  - Base Address (&stackArr[0]) : 0x" << hex << baseAddr << dec << "\n";
    cout << "  - Element Stride (sizeof(int)) : " << sizeof(int) << " bytes\n\n";

    cout << "+-------+-------+--------------------+--------------------+--------------------+\n"
         << "| Index | Value | Actual Address     | Calculated Address | Address Offset     |\n"
         << "+-------+-------+--------------------+--------------------+--------------------+\n";

    for (size_t i = 0; i < elementCountInput && i < 8; ++i) {
        uintptr_t actualAddr = reinterpret_cast<uintptr_t>(&stackArr[i]);
        uintptr_t calculatedAddr = baseAddr + (i * sizeof(int));
        ptrdiff_t byteOffset = actualAddr - baseAddr;

        cout << "| " << setw(5) << i 
             << " | " << setw(5) << stackArr[i]
             << " | 0x" << hex << setw(16) << actualAddr << dec
             << " | 0x" << hex << setw(16) << calculatedAddr << dec
             << " | +" << setw(2) << byteOffset << " bytes          |\n";
    }
    cout << "+-------+-------+--------------------+--------------------+--------------------+\n";
    cout << "  - Formula Proof: Address(arr[i]) == BaseAddress + (i * sizeof(T)) -> PERFECT MATCH!\n";

    // =====================================================================================
    // 2. MEMORY ALIGNMENT AND ELEMENT STRIDE
    // Compiler pads struct members to align to boundary requirements.
    // =====================================================================================
    cout << "\n================ 2. STRUCT ALIGNMENT & ARRAY STRIDE ================\n";
    
    AlignTestStruct structArr[3] = {
        {101, 99.5, 'A'},
        {102, 88.4, 'B'},
        {103, 77.3, 'C'}
    };

    cout << "  - sizeof(AlignTestStruct) = " << sizeof(AlignTestStruct) << " bytes (Includes padding for 8-byte alignment)\n";
    cout << "  - Array element memory addresses:\n";

    uintptr_t structBase = reinterpret_cast<uintptr_t>(&structArr[0]);
    for (size_t i = 0; i < 3; ++i) {
        uintptr_t currentAddr = reinterpret_cast<uintptr_t>(&structArr[i]);
        cout << "    structArr[" << i << "] -> Address: 0x" << hex << currentAddr 
             << dec << " | Offset from Base: +" << (currentAddr - structBase) << " bytes\n";
    }

    // =====================================================================================
    // 3. STACK VS HEAP ARRAY MEMORY MODEL
    // Stack addresses grow downwards/upwards in stack frame; heap addresses live in OS heap.
    // =====================================================================================
    cout << "\n================ 3. STACK VS HEAP MEMORY LOCATIONS ================\n";
    
    int stackSample = 42;
    auto heapArr = make_unique<int[]>(4);
    vector<int> vectorArr = {100, 200, 300, 400};

    uintptr_t stackAddr = reinterpret_cast<uintptr_t>(&stackSample);
    uintptr_t heapPtrAddr = reinterpret_cast<uintptr_t>(heapArr.get());
    uintptr_t vecDataAddr = reinterpret_cast<uintptr_t>(vectorArr.data());
    uintptr_t vecObjAddr  = reinterpret_cast<uintptr_t>(&vectorArr);

    cout << "  - Stack Variable Address   : 0x" << hex << stackAddr << dec << " (Stack Frame)\n";
    cout << "  - Heap Array Data Address  : 0x" << hex << heapPtrAddr << dec << " (Heap Memory)\n";
    cout << "  - Vector Data Buffer Address: 0x" << hex << vecDataAddr << dec << " (Heap Memory)\n";
    cout << "  - Vector Control Object Addr: 0x" << hex << vecObjAddr  << dec << " (Stack Memory)\n";

    // =====================================================================================
    // 4. ROW-MAJOR ORDER (2D ARRAY FLATTENED MEMORY MODEL)
    // 2D arrays are stored in a 1D contiguous block of physical RAM using Row-Major Layout.
    // =====================================================================================
    cout << "\n================ 4. ROW-MAJOR ORDER (2D MATRIX FLATTENING) ================\n";
    
    constexpr size_t ROWS = 2;
    constexpr size_t COLS = 3;
    int matrix[ROWS][COLS] = {
        {11, 12, 13},
        {21, 22, 23}
    };

    uintptr_t matrixBase = reinterpret_cast<uintptr_t>(&matrix[0][0]);
    cout << "  - Matrix 2x3 Physical Memory Addresses:\n";

    for (size_t r = 0; r < ROWS; ++r) {
        for (size_t c = 0; c < COLS; ++c) {
            uintptr_t actual = reinterpret_cast<uintptr_t>(&matrix[r][c]);
            size_t flatIndex = (r * COLS) + c; // Row-major index formula
            uintptr_t formulaAddr = matrixBase + (flatIndex * sizeof(int));

            cout << "    matrix[" << r << "][" << c << "] | Value: " << matrix[r][c]
                 << " | Flat Index: " << flatIndex
                 << " | Address: 0x" << hex << actual << dec
                 << " | Match Formula: " << (actual == formulaAddr ? "YES" : "NO") << "\n";
        }
    }

    // Direct pointer traversal proving 1D physical contiguity
    cout << "  - Traversing 2D Matrix as 1D contiguous array via raw pointer: ";
    const int* flatPtr = &matrix[0][0];
    for (size_t i = 0; i < (ROWS * COLS); ++i) {
        cout << flatPtr[i] << " ";
    }
    cout << "\n";

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ ARRAY MEMORY MODEL SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Principle             | Definition                        | Architectural Consequence         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Contiguity            | Zero gap between array elements   | O(1) random element indexing      |\n"
         << "| Element Stride        | `sizeof(T)` bytes per slot        | Pointer arithmetic moves by size  |\n"
         << "| Alignment & Padding   | Structs padded to alignment boundary| Element stride = sizeof(Struct)   |\n"
         << "| Row-Major Order       | Rows stored sequentially in 2D arr| Flattened formula: `r * COLS + c` |\n"
         << "| Cache Locality        | Sequential memory prefetched by CPU| Fast iteration via L1/L2 cache    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}