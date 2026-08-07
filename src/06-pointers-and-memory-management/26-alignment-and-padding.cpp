/*
 * =====================================================================================
 * CONCEPT        : Alignment and Padding in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  hardware alignment rules, compiler struct padding, memory layout 
 *                  optimization, custom alignment specifiers, and dynamic alignment:
 *
 *                  1. Alignment & Padding Fundamentals : Why CPUs enforce alignment (bus performance, 
 *                                                       atomic operations, hardware fault prevention).
 *                  2. Struct Reordering & Layout      : Inspecting padding bytes via `sizeof`, `alignof`, 
 *                                                       and `offsetof` macros.
 *                  3. Custom Alignment (`alignas`)     : Enforcing cache-line alignment (e.g., 64-byte) 
 *                                                       to prevent false sharing in concurrent systems.
 *                  4. Pointer Misalignment Detection   : Verifying whether raw addresses satisfy `alignof(T)`.
 *                  5. Dynamic Pointer Alignment       : Using `std::align` to adjust unaligned pointers 
 *                                                       inside generic byte buffers.
 *                  6. C++17 Aligned Allocation        : Overloaded `operator new` with `std::align_val_t`.
 *
 * TIME COMPLEXITY  : Alignment calculations and pointer adjustments: O(1) constant time.
 * SPACE COMPLEXITY : Structural overhead: Overhead dictated by compiler padding bytes.
 * =====================================================================================
 */

#include <iostream>
#include <cstddef>  // offsetof
#include <cstdint>  // uintptr_t
#include <memory>   // std::align
#include <new>      // std::align_val_t
#include <limits>

using namespace std;

// =====================================================================================
// STRUCTS DEMONSTRATING STRUCT LAYOUT, PADDING, AND REORDERING
// =====================================================================================

// Unoptimized Struct Layout: Sub-optimal variable order causes excessive internal padding
struct UnoptimizedStruct {
    char flag1;       // 1 byte
                      // 7 bytes internal padding (to align next double to 8-byte boundary)
    double value;     // 8 bytes
    int id;           // 4 bytes
                      // 4 bytes tail padding (to make total struct size multiple of alignof(double) = 8)
}; // Total Size: 24 bytes | Waste: 11 bytes padding

// Optimized Struct Layout: Ordered from largest alignment requirement to smallest
struct OptimizedStruct {
    double value;     // 8 bytes (offset 0)
    int id;           // 4 bytes (offset 8)
    char flag1;       // 1 byte  (offset 12)
                      // 3 bytes tail padding (to make total struct size multiple of alignof(double) = 8)
}; // Total Size: 16 bytes | Waste: 3 bytes padding (Saved 8 bytes!)

// Custom Cache-Line Aligned Struct (64-byte alignment)
struct alignas(64) CacheLineAlignedStruct {
    uint64_t counter;
    uint64_t dataPayload[7]; // 8 * 8 = 64 bytes total struct payload
};

// Helper function to check if a pointer address satisfies alignment N
bool isPointerAligned(const void* ptr, size_t alignment) {
    auto address = reinterpret_cast<uintptr_t>(ptr);
    return (address % alignment) == 0;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for alignment calculations (e.g., 64): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 64." << endl;
        userInputValue = 64;
    }

    // =====================================================================================
    // 1. `alignof` OPERATOR & PRIMITIVE ALIGNMENT REQUIREMENTS
    // =====================================================================================
    cout << "\n================ 1. PRIMITIVE TYPE ALIGNMENT REQUIREMENTS ================\n";

    cout << "  - `alignof(char)`               : " << alignof(char) << " byte(s)\n";
    cout << "  - `alignof(short)`              : " << alignof(short) << " byte(s)\n";
    cout << "  - `alignof(int)`                : " << alignof(int) << " byte(s)\n";
    cout << "  - `alignof(double)`             : " << alignof(double) << " byte(s)\n";
    cout << "  - `alignof(void*)`              : " << alignof(void*) << " byte(s) (Pointer address size)\n";
    cout << "  - `alignof(UnoptimizedStruct)`  : " << alignof(UnoptimizedStruct) << " byte(s)\n";
    cout << "  - `alignof(OptimizedStruct)`    : " << alignof(OptimizedStruct) << " byte(s)\n";

    // =====================================================================================
    // 2. STRUCT PADDING & FIELD OFFSET ANALYSIS (`offsetof`)
    // =====================================================================================
    cout << "\n================ 2. STRUCT LAYOUT & PADDING ANALYSIS ================\n";

    cout << "  - [UnoptimizedStruct Layout]: Total Size = " << sizeof(UnoptimizedStruct) 
         << " bytes | Alignment = " << alignof(UnoptimizedStruct) << " bytes\n";
    cout << "    * Offset of `flag1` (char)   : " << offsetof(UnoptimizedStruct, flag1) << " bytes\n";
    cout << "    * Offset of `value` (double) : " << offsetof(UnoptimizedStruct, value) 
         << " bytes (Inserted 7 padding bytes before this!)\n";
    cout << "    * Offset of `id`    (int)    : " << offsetof(UnoptimizedStruct, id) << " bytes\n";

    cout << "\n  - [OptimizedStruct Layout]: Total Size = " << sizeof(OptimizedStruct) 
         << " bytes | Alignment = " << alignof(OptimizedStruct) << " bytes\n";
    cout << "    * Offset of `value` (double) : " << offsetof(OptimizedStruct, value) << " bytes\n";
    cout << "    * Offset of `id`    (int)    : " << offsetof(OptimizedStruct, id) << " bytes\n";
    cout << "    * Offset of `flag1` (char)   : " << offsetof(OptimizedStruct, flag1) << " bytes\n";
    cout << "  - [MEMORY SAVINGS]: Reordering members reduced size from 24 bytes to 16 bytes!\n";

    // =====================================================================================
    // 3. CUSTOM CACHE-LINE ALIGNMENT (`alignas`)
    // =====================================================================================
    cout << "\n================ 3. CUSTOM CACHE-LINE ALIGNMENT (`alignas`) ================\n";

    CacheLineAlignedStruct cacheObj;
    cacheObj.counter = userInputValue;

    cout << "  - Size of `CacheLineAlignedStruct`   : " << sizeof(CacheLineAlignedStruct) << " bytes\n";
    cout << "  - Alignment `alignof(CacheLine)`     : " << alignof(CacheLineAlignedStruct) << " bytes\n";
    cout << "  - Memory Address of `cacheObj`       : " << static_cast<const void*>(&cacheObj) << "\n";
    cout << "  - Is Address 64-Byte Aligned?        : " 
         << (isPointerAligned(&cacheObj, 64) ? "YES (Aligned)" : "NO") << "\n";

    // =====================================================================================
    // 4. POINTER MISALIGNMENT DETECTION & DYNAMIC ALIGNMENT (`std::align`)
    // =====================================================================================
    cout << "\n================ 4. POINTER ALIGNMENT & `std::align` ================\n";

    alignas(64) char rawBuffer[256];
    void* unalignedPtr = rawBuffer + 3; // Intentionally offset address by 3 bytes (Misaligned for double)
    size_t spaceLeft = sizeof(rawBuffer) - 3;

    cout << "  - Original Unaligned Pointer Address : " << static_cast<const void*>(unalignedPtr) << "\n";
    cout << "  - Is 8-Byte Aligned for `double`?    : " 
         << (isPointerAligned(unalignedPtr, alignof(double)) ? "YES" : "NO (Misaligned!)") << "\n";

    // Dynamically align pointer using std::align
    void* alignedPtr = unalignedPtr;
    void* result = std::align(alignof(double), sizeof(double), alignedPtr, spaceLeft);

    if (result != nullptr) {
        cout << "  - [std::align] Adjusted Aligned Addr : " << static_cast<const void*>(alignedPtr) << "\n";
        cout << "  - Is Adjusted Address 8-Byte Aligned?: " 
             << (isPointerAligned(alignedPtr, alignof(double)) ? "YES (Successfully Aligned)" : "NO") << "\n";
        cout << "  - Bytes Shifted for Alignment        : " 
             << (reinterpret_cast<uintptr_t>(alignedPtr) - reinterpret_cast<uintptr_t>(unalignedPtr)) << " bytes\n";
    }

    // =====================================================================================
    // 5. C++17 ALIGNED DYNAMIC MEMORY ALLOCATION (`std::align_val_t`)
    // =====================================================================================
    cout << "\n================ 5. C++17 ALIGNED DYNAMIC ALLOCATION ================\n";

    constexpr size_t CUSTOM_ALIGNMENT = 128; // Require 128-byte boundary

    // Allocate dynamic heap buffer with 128-byte alignment
    void* dynamicAlignedBuffer = ::operator new(1024, static_cast<std::align_val_t>(CUSTOM_ALIGNMENT));

    cout << "  - Dynamic 128-Byte Aligned Heap Addr : " << static_cast<const void*>(dynamicAlignedBuffer) << "\n";
    cout << "  - Is Address 128-Byte Aligned?       : " 
         << (isPointerAligned(dynamicAlignedBuffer, CUSTOM_ALIGNMENT) ? "YES (Aligned)" : "NO") << "\n";

    // Deallocate aligned memory using matching align_val_t operator delete
    ::operator delete(dynamicAlignedBuffer, static_cast<std::align_val_t>(CUSTOM_ALIGNMENT));
    dynamicAlignedBuffer = nullptr;

    cout << "  - Aligned heap memory freed via `::operator delete(ptr, align_val_t)`.\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ ALIGNMENT AND PADDING SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Operator    | Syntax Example                    | Primary Purpose & Operational Role|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Alignment Specifier   | `alignof(T)`                      | Queries min byte alignment of type|\n"
         << "| Custom Alignment      | `struct alignas(64) Cell {};`     | Enforces specific boundary (cache)|\n"
         << "| Field Offset Query    | `offsetof(StructType, member)`    | Returns byte offset inside struct |\n"
         << "| Compiler Padding      | Inserted between unequal fields   | Satisfies hardware alignment rules|\n"
         << "| Layout Optimization   | Sort members largest to smallest  | Minimizes structure padding waste |\n"
         << "| Dynamic Pointer Align | `std::align(align, sz, ptr, buf)` | Adjusts ptr to aligned boundary   |\n"
         << "| Aligned Dynamic Alloc | `operator new(sz, align_val_t(N))`| Allocates N-byte aligned heap memory|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}