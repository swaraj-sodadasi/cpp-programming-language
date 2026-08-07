/*
 * =====================================================================================
 * CONCEPT        : Memory Organisation in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive breakdown and runtime inspection of C++ memory organization:
 *                  1. C++ Memory Segments      : Text/Code Segment, Data Segment (Initialized),
 *                                                 BSS Segment (Uninitialized), Heap, and Stack.
 *                  2. Stack vs Heap Layout     : Stack growth (typically downward) vs Heap growth 
 *                                                 (typically upward) & memory addresses.
 *                  3. Memory Alignment & Padding: `alignof`, `sizeof`, and structure member 
 *                                                 byte alignment in memory.
 *                  4. Byte-Level Memory Visualiser: Raw byte inspection & endianness analysis 
 *                                                 using `reinterpret_cast<const uint8_t*>`.
 *                  5. Pointer Indirection Layout: Pointers pointing across Stack, Heap, Data, and Text.
 *
 * TIME COMPLEXITY  : Memory Address Inspection & Byte Iteration: O(N) over memory byte width.
 * SPACE COMPLEXITY : O(1) auxiliary space footprint.
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdint>
#include <limits>

using namespace std;

// =====================================================================================
// GLOBAL / STATIC VARIABLES (DATA SEGMENT & BSS SEGMENT)
// =====================================================================================
int g_initializedGlobal = 42;    // Resides in Initialized Data Segment (.data)
int g_uninitializedGlobal;       // Resides in Uninitialized Data Segment (.bss)

// Const literal stored in Text/Code Segment (.rodata or text)
const char* const g_codeSegmentLiteral = "Read-Only Text Segment String";

// Dummy function to obtain a Text/Code Segment memory address
void sampleFunction() {}

// =====================================================================================
// STRUCT LAYOUT DEMONSTRATION (MEMORY ALIGNMENT & PADDING)
// =====================================================================================
struct UnpaddedStruct {
    uint8_t  a;  // 1 byte
    uint64_t b;  // 8 bytes
    uint32_t c;  // 4 bytes
};

struct OptimizedStruct {
    uint64_t b;  // 8 bytes
    uint32_t c;  // 4 bytes
    uint8_t  a;  // 1 byte + 3 bytes padding
};

// =====================================================================================
// HELPER FUNCTION: BYTE-LEVEL MEMORY INSPECTOR
// Prints memory representation byte-by-byte in Hexadecimal format
// =====================================================================================
void inspectRawBytes(const void* ptr, size_t size, const string& label) {
    const auto* bytePtr = reinterpret_cast<const uint8_t*>(ptr);
    cout << "  - [" << left << setw(28) << label << "] Address: " 
         << static_cast<const void*>(ptr) << " | Hex Bytes: ";
    
    for (size_t i = 0; i < size; ++i) {
        cout << hex << uppercase << setw(2) << setfill('0') 
             << static_cast<unsigned int>(bytePtr[i]) << " ";
    }
    cout << dec << setfill(' ') << "\n";
}

// =====================================================================================
// HELPER FUNCTION: RECURSIVE STACK GROWTH INSPECTOR
// =====================================================================================
void demonstrateStackGrowth(int depth, const int* parentAddr) {
    int localFrameVar = depth;
    const void* currentAddr = static_cast<const void*>(&localFrameVar);

    cout << "    Frame Depth " << depth << " | Local Var Address: " << currentAddr;
    if (parentAddr != nullptr) {
        ptrdiff_t byteOffset = reinterpret_cast<const char*>(currentAddr) - reinterpret_cast<const char*>(parentAddr);
        cout << " | Offset from prev frame: " << byteOffset << " bytes " 
             << (byteOffset < 0 ? "(Grows Downward)" : "(Grows Upward)");
    }
    cout << "\n";

    if (depth < 3) {
        demonstrateStackGrowth(depth + 1, &localFrameVar);
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a sample integer for memory layout analysis (e.g., 305419896 / 0x12345678): " << flush;
    if (!(cin >> userInputValue) || userInputValue == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid or zero input. Defaulting value to 305419896 (0x12345678)." << endl;
        userInputValue = 0x12345678; // 305419896 in decimal
    }

    // Explicit usage of BSS segment variable
    g_uninitializedGlobal = 100;

    // =====================================================================================
    // 1. INSPECTING C++ MEMORY SEGMENTS
    // =====================================================================================
    cout << "\n================ 1. C++ MEMORY SEGMENTS LAYOUT ================\n";

    int stackVar = 10;
    int* heapVar = new int(20);

    // Printing addresses of items located in various segments
    cout << "  - [Text / Code Segment] Function Address   : " 
         << reinterpret_cast<const void*>(&sampleFunction) << "\n";
    cout << "  - [Text / Code Segment] String Literal Addr: " 
         << static_cast<const void*>(g_codeSegmentLiteral) << "\n";
    cout << "  - [Data Segment (Init)] Global Init Addr   : " 
         << static_cast<const void*>(&g_initializedGlobal) << "\n";
    cout << "  - [BSS Segment (Uninit)] Global Uninit Addr: " 
         << static_cast<const void*>(&g_uninitializedGlobal) << "\n";
    cout << "  - [Heap Segment] Dynamic Alloc Address     : " 
         << static_cast<const void*>(heapVar) << "\n";
    cout << "  - [Stack Segment] Local Stack Var Address  : " 
         << static_cast<const void*>(&stackVar) << "\n";

    // =====================================================================================
    // 2. STACK VS HEAP GROWTH DIRECTION
    // =====================================================================================
    cout << "\n================ 2. STACK VS HEAP GROWTH DEMONSTRATION ================\n";

    cout << "  - Inspecting Stack Frame Memory Allocation (Recursive Call Stack):\n";
    demonstrateStackGrowth(1, nullptr);

    cout << "\n  - Inspecting Heap Memory Allocation (Sequential `new` Allocs):\n";
    int* heapAlloc1 = new int(100);
    int* heapAlloc2 = new int(200);
    int* heapAlloc3 = new int(300);

    ptrdiff_t heapDiff = reinterpret_cast<const char*>(heapAlloc2) - reinterpret_cast<const char*>(heapAlloc1);
    cout << "    Heap Alloc 1 Address: " << static_cast<const void*>(heapAlloc1) << "\n";
    cout << "    Heap Alloc 2 Address: " << static_cast<const void*>(heapAlloc2) 
         << " | Offset: +" << heapDiff << " bytes (Grows Upward)\n";
    cout << "    Heap Alloc 3 Address: " << static_cast<const void*>(heapAlloc3) << "\n";

    delete heapAlloc1;
    delete heapAlloc2;
    delete heapAlloc3;
    delete heapVar;

    // =====================================================================================
    // 3. MEMORY ALIGNMENT AND STRUCT PADDING
    // =====================================================================================
    cout << "\n================ 3. MEMORY ALIGNMENT & STRUCT PADDING ================\n";

    cout << "  - Raw Data Sizes: uint8_t (1B) + uint64_t (8B) + uint32_t (4B) = 13 Bytes Total\n";
    cout << "  - UnpaddedStruct  `sizeof`: " << sizeof(UnpaddedStruct) 
         << " bytes | `alignof`: " << alignof(UnpaddedStruct) << " bytes (Padding Added!)\n";
    cout << "  - OptimizedStruct `sizeof`: " << sizeof(OptimizedStruct) 
         << " bytes | `alignof`: " << alignof(OptimizedStruct) << " bytes (Reordered Alignment!)\n";

    UnpaddedStruct uStruct{0xAA, 0x1122334455667788ULL, 0xBBCCDDEE};
    inspectRawBytes(&uStruct, sizeof(UnpaddedStruct), "UnpaddedStruct Layout");

    // =====================================================================================
    // 4. BYTE-LEVEL INSPECTION & ENDIANNESS ANALYSIS
    // =====================================================================================
    cout << "\n================ 4. BYTE-LEVEL INSPECTION & ENDIANNESS ================\n";

    int32_t sampleInt = userInputValue;
    cout << "  - Inspecting 32-bit Integer Value: " << sampleInt 
         << " (Hex: 0x" << hex << uppercase << sampleInt << dec << ")\n";

    inspectRawBytes(&sampleInt, sizeof(sampleInt), "32-bit Integer Bytes");

    const auto* firstByte = reinterpret_cast<const uint8_t*>(&sampleInt);
    if (*firstByte == (sampleInt & 0xFF)) {
        cout << "  - System Architecture Detector   : Little-Endian (Least Significant Byte stored first)\n";
    } else {
        cout << "  - System Architecture Detector   : Big-Endian (Most Significant Byte stored first)\n";
    }

    // =====================================================================================
    // 5. POINTER INDIRECTION & ADDRESS MAPPING
    // =====================================================================================
    cout << "\n================ 5. POINTER INDIRECTION & ADDRESS MAPPING ================\n";

    int targetVal = 999;
    int* ptrToTarget = &targetVal;
    int** doublePtr = &ptrToTarget;

    cout << "  - Target Value (`targetVal`)              : " << targetVal << "\n";
    cout << "  - Target Address (`&targetVal`)           : " << static_cast<const void*>(&targetVal) << "\n";
    cout << "  - Single Pointer (`ptrToTarget` Stores)   : " << static_cast<const void*>(ptrToTarget) << "\n";
    cout << "  - Single Pointer Address (`&ptrToTarget`)  : " << static_cast<const void*>(&ptrToTarget) << "\n";
    cout << "  - Double Pointer (`doublePtr` Stores)     : " << static_cast<const void*>(doublePtr) << "\n";
    cout << "  - Dereferenced Double Pointer (`**double`): " << **doublePtr << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ C++ MEMORY SEGMENTS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Memory Segment        | Lifetime Scope    | Growth Direction  | Typical Contents Stored           |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Text / Code Segment   | Program Execution | Static Fixed Size | Compiled Machine Code, Literals   |\n"
         << "| Data Segment (.data)  | Program Execution | Static Fixed Size | Initialized Globals & Statics     |\n"
         << "| BSS Segment (.bss)    | Program Execution | Static Fixed Size | Uninitialized Globals & Statics   |\n"
         << "| Heap Segment          | Manual / RAII     | Upward (Higher)   | Dynamic `new` / `malloc` Buffers  |\n"
         << "| Stack Segment         | Function Scope    | Downward (Lower)  | Local Variables, Frame Return Addr|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}