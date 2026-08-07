/*
 * =====================================================================================
 * CONCEPT        : Memory Addresses in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade implementation inspecting
 *                  raw memory addresses, pointer representation, and address manipulation:
 *
 *                  1. Address Representation     : Printing memory addresses safely using 
 *                                                 `static_cast<const void*>`.
 *                  2. Address Conversion          : Converting pointers to `uintptr_t` for 
 *                                                 integer arithmetic and bitwise checks.
 *                  3. Cache Line & Boundary Alignment: Checking memory alignment against 
 *                                                 16-byte, 32-byte, and 64-byte cache lines.
 *                  4. Stack vs Heap Address Spaces: Observing address delta ranges across 
 *                                                 stack frames and dynamic heap buffers.
 *                  5. Address Deltas in Arrays   : Calculating exact byte offsets across 
 *                                                 contiguous array allocations.
 *                  6. Dangling Address Inspection : Observing pointer state post-`delete` 
 *                                                 before `nullptr` assignment.
 *
 * TIME COMPLEXITY  : Address inspection and alignment checks: O(1) constant time.
 * SPACE COMPLEXITY : O(1) auxiliary space footprint.
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS FOR ADDRESS MANIPULATION
// =====================================================================================

// Check if a pointer is aligned to a given byte boundary (e.g., 16, 32, 64 bytes)
bool isAddressAligned(const void* ptr, size_t alignmentBytes) {
    if (alignmentBytes == 0) return true;
    auto addrInt = reinterpret_cast<uintptr_t>(ptr);
    return (addrInt % alignmentBytes) == 0;
}

// Calculate signed byte offset between two memory addresses
ptrdiff_t calculateAddressDelta(const void* first, const void* second) {
    const auto* p1 = reinterpret_cast<const char*>(first);
    const auto* p2 = reinterpret_cast<const char*>(second);
    return p2 - p1;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userElementCount = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter dynamic allocation element count for address layout analysis (e.g., 4): " << flush;
    if (!(cin >> userElementCount) || userElementCount <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting element count to 4." << endl;
        userElementCount = 4;
    }

    // =====================================================================================
    // 1. RAW MEMORY ADDRESS PRINTING & POINTER SIZES
    // =====================================================================================
    cout << "\n================ 1. MEMORY ADDRESS PRINTING & SIZES ================\n";

    int stackVar = 42;
    int* stackPtr = &stackVar;

    cout << "  - Stack Variable Value               : " << stackVar << "\n";
    cout << "  - Raw Address (&stackVar)            : " << static_cast<const void*>(&stackVar) << "\n";
    cout << "  - Pointer Variable Address (&stackPtr): " << static_cast<const void*>(&stackPtr) << "\n";
    cout << "  - Pointer Stored Address (stackPtr)  : " << static_cast<const void*>(stackPtr) << "\n";
    cout << "  - Pointer Architecture Size          : " << sizeof(stackPtr) << " bytes (" 
         << (sizeof(stackPtr) == 8 ? "64-bit Virtual Addressing" : "32-bit Virtual Addressing") << ")\n";

    // =====================================================================================
    // 2. CONVERTING ADDRESSES TO `uintptr_t` FOR ARITHMETIC & ALIGNMENT
    // =====================================================================================
    cout << "\n================ 2. ADDRESS CASTING & CACHE LINE ALIGNMENT ================\n";

    auto rawNumericAddress = reinterpret_cast<uintptr_t>(stackPtr);

    cout << "  - Address in Hex Representation      : 0x" << hex << uppercase << rawNumericAddress << dec << "\n";
    cout << "  - Address as Unsigned Decimal Int    : " << rawNumericAddress << "\n";

    // Cache line alignment inspection (64-byte alignment critical for high performance / CPU cache)
    constexpr size_t CACHE_LINE_SIZE = 64;
    bool aligned64 = isAddressAligned(stackPtr, CACHE_LINE_SIZE);
    
    cout << "  - 16-Byte Boundary Alignment Check   : " << (isAddressAligned(stackPtr, 16) ? "ALIGNED" : "NOT ALIGNED") << "\n";
    cout << "  - 64-Byte Cache Line Alignment Check : " << (aligned64 ? "ALIGNED (Cache Line Boundary)" : "NOT ALIGNED") << "\n";
    cout << "  - Byte Offset to Next 64-B Boundary  : " 
         << (CACHE_LINE_SIZE - (rawNumericAddress % CACHE_LINE_SIZE)) % CACHE_LINE_SIZE << " bytes\n";

    // =====================================================================================
    // 3. STACK VS HEAP MEMORY ADDRESS REGIONS
    // =====================================================================================
    cout << "\n================ 3. STACK VS HEAP ADDRESS REGION DELTAS ================\n";

    int localStackArray[2] = {100, 200};
    int* heapArray = new int[static_cast<size_t>(userElementCount)]{1, 2, 3, 4};

    cout << "  - Local Stack Array Base Address     : " << static_cast<const void*>(localStackArray) << "\n";
    cout << "  - Heap Allocated Buffer Address      : " << static_cast<const void*>(heapArray) << "\n";

    ptrdiff_t stackHeapDelta = calculateAddressDelta(localStackArray, heapArray);
    cout << "  - Memory Region Delta (Heap - Stack) : " << stackHeapDelta << " bytes\n";
    cout << "  - Region Analysis                    : Stack and Heap reside in distinct virtual memory segments.\n";

    // =====================================================================================
    // 4. CONTIGUOUS ARRAY ADDRESS DELTAS & POINTER OFFSETS
    // =====================================================================================
    cout << "\n================ 4. CONTIGUOUS ARRAY ADDRESS DELTAS ================\n";

    cout << "  - Inspecting Address Step per Element (sizeof(int) = " << sizeof(int) << " bytes):\n";
    for (size_t i = 0; i < static_cast<size_t>(userElementCount); ++i) {
        const int* elementPtr = &heapArray[i];
        ptrdiff_t byteOffset = calculateAddressDelta(heapArray, elementPtr);
        
        cout << "    heapArray[" << i << "] | Addr: " << static_cast<const void*>(elementPtr)
             << " | Offset from Base: +" << setw(2) << byteOffset << " bytes\n";
    }

    // =====================================================================================
    // 5. DANGLING ADDRESS INSPECTION (POST-DELETE BEHAVIOR)
    // =====================================================================================
    cout << "\n================ 5. DANGLING ADDRESS STATE INSPECTION ================\n";

    int* dynamicVal = new int(999);
    cout << "  - Allocated Heap Address             : " << static_cast<const void*>(dynamicVal) << "\n";
    cout << "  - Dereferenced Value Before delete   : " << *dynamicVal << "\n";

    delete dynamicVal; // Memory returned to OS/allocator, BUT dynamicVal variable STILL holds raw address!

    cout << "  - Address Stored After delete (RAW)  : " << static_cast<const void*>(dynamicVal) 
         << " (Dangling Address! Memory is freed)\n";

    dynamicVal = nullptr; // RESOLVED: Disarm dangling pointer

    cout << "  - Address Stored After nullptr Reset : " << static_cast<const void*>(dynamicVal) << " (Safe Disarmed State)\n";

    delete[] heapArray;
    heapArray = nullptr;

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ MEMORY ADDRESS CONCEPTS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Operation   | Syntax / Technique                | Primary Purpose & Characteristics |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Address Extraction    | `&variable`                       | Yields pointer to object memory   |\n"
         << "| Safe Output Format    | `static_cast<const void*>(ptr)`   | Prevents character array printing |\n"
         << "| Integer Cast          | `reinterpret_cast<uintptr_t>(ptr)`| Enables bitwise & alignment math  |\n"
         << "| Address Offset        | `reinterpret_cast<const char*>`  | Calculates exact byte delta       |\n"
         << "| Cache Alignment       | `(addr % 64) == 0`                | Optimizes CPU cache line hits     |\n"
         << "| Dangling Address      | Raw address remaining after `delete`| Requires setting to `nullptr`     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}