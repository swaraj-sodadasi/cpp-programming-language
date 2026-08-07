/*
 * =====================================================================================
 * CONCEPT        : Memory Manipulation in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  low-level byte manipulation, memory copying, zeroing, alignment, and 
 *                  manual raw buffer lifecycle management:
 *
 *                  1. Raw Memory Filling & Zeroing  : `std::memset` for fast byte-level initialization.
 *                  2. Memory Copying Mechanics      : Non-overlapping `std::memcpy` vs overlapping 
 *                                                     safe `std::memmove`.
 *                  3. Raw Byte Comparison           : `std::memcmp` for binary buffer comparison.
 *                  4. Byte-Level Inspection         : Accessing object memory layout via `const unsigned char*`.
 *                  5. Manual Raw Memory Lifecycle   : Placement `new` and explicit destructor invocation 
 *                                                     for manual object lifetime inside raw byte buffers.
 *                  6. Memory Alignment Mechanics    : `alignof`, `alignas`, and pointer alignment via `std::align`.
 *
 * TIME COMPLEXITY  : Memory Copy / Fill / Compare: O(N) over byte count N.
 *                    Alignment & Inspection     : O(1) constant time.
 * SPACE COMPLEXITY : Buffer memory footprint    : O(N) bytes depending on buffer sizes.
 * =====================================================================================
 */

#include <iostream>
#include <cstring>
#include <memory>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <cstdint>
#include <new>

using namespace std;

// =====================================================================================
// HELPER STRUCT FOR MANUAL LIFECYCLE MANAGEMENT DEMONSTRATION
// =====================================================================================
struct MemoryPayload {
    int id;
    double value;

    MemoryPayload(int i, double v) : id(i), value(v) {
        cout << "    [MemoryPayload Ctor] Constructed ID: " << id << " | Value: " << value 
             << " at address: " << static_cast<const void*>(this) << "\n";
    }

    ~MemoryPayload() {
        cout << "    [MemoryPayload Dtor] Destructed ID: " << id 
             << " at address: " << static_cast<const void*>(this) << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for memory manipulation analysis (e.g., 65): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting value to 65." << endl;
        userInputValue = 65;
    }

    // =====================================================================================
    // 1. RAW MEMORY FILLING (`std::memset`) & COPYING (`std::memcpy` VS `std::memmove`)
    // =====================================================================================
    cout << "\n================ 1. MEMORY FILLING (`memset`) & COPYING (`memcpy` / `memmove`) ================\n";

    constexpr size_t BUFFER_SIZE = 8;
    char bufferA[BUFFER_SIZE];

    // A. Fill memory buffer with a specific byte pattern using std::memset
    std::memset(bufferA, userInputValue, BUFFER_SIZE - 1);
    bufferA[BUFFER_SIZE - 1] = '\0'; // Null-terminate string representation

    cout << "  - [std::memset] Buffer filled with byte value (" << userInputValue << "):\n";
    cout << "    String representation : \"" << bufferA << "\"\n";
    cout << "    Hex representation    : [ ";
    for (size_t i = 0; i < BUFFER_SIZE - 1; ++i) {
        cout << "0x" << hex << setfill('0') << setw(2) << static_cast<int>(static_cast<unsigned char>(bufferA[i])) << " " << dec;
    }
    cout << "]\n";

    // B. Fast Non-Overlapping Copy using std::memcpy
    char bufferB[BUFFER_SIZE];
    std::memcpy(bufferB, bufferA, BUFFER_SIZE);

    cout << "\n  - [std::memcpy] Copied BufferA -> BufferB:\n";
    cout << "    BufferB String        : \"" << bufferB << "\"\n";

    // C. Safe Overlapping Memory Shift using std::memmove
    char overlapBuffer[12] = "ABCDEFGHIJ";
    cout << "\n  - [std::memmove] Original Overlap Buffer : \"" << overlapBuffer << "\"\n";

    // Shifting 5 characters ("ABCDE") forward by 2 positions (Destination overlaps Source)
    std::memmove(overlapBuffer + 2, overlapBuffer, 5);
    cout << "  - Post `memmove(buf + 2, buf, 5)` Shift  : \"" << overlapBuffer << "\"\n";
    cout << "    (Safe overlapping movement prevents byte corruption!)\n";

    // =====================================================================================
    // 2. RAW BYTE COMPARISON (`std::memcmp`)
    // =====================================================================================
    cout << "\n================ 2. RAW BYTE COMPARISON (`std::memcmp`) ================\n";

    int array1[3] = {10, 20, 30};
    int array2[3] = {10, 20, 30};
    int array3[3] = {10, 20, 99};

    int cmpResult1 = std::memcmp(array1, array2, sizeof(array1));
    int cmpResult2 = std::memcmp(array1, array3, sizeof(array1));

    cout << "  - `memcmp(array1, array2)` (Identical buffers) : " << cmpResult1 
         << " (0 indicates exact byte match)\n";
    cout << "  - `memcmp(array1, array3)` (Differing buffers) : " << cmpResult2 
         << " (Non-zero indicates byte difference)\n";

    // =====================================================================================
    // 3. BYTE-LEVEL OBJECT INSPECTION (`unsigned char*`)
    // =====================================================================================
    cout << "\n================ 3. BYTE-LEVEL OBJECT INSPECTION ================\n";

    uint32_t targetValue = static_cast<uint32_t>(userInputValue * 0x01010101); // Uniform byte pattern
    const auto* bytePtr = reinterpret_cast<const unsigned char*>(&targetValue);

    cout << "  - Inspecting 32-bit Integer Value         : " << targetValue << " (0x" 
         << hex << targetValue << dec << ")\n";
    cout << "  - Raw Byte Storage Layout in Memory       : [ ";
    for (size_t i = 0; i < sizeof(targetValue); ++i) {
        cout << "Byte[" << i << "]: 0x" << hex << setfill('0') << setw(2) 
             << static_cast<int>(bytePtr[i]) << " " << dec;
    }
    cout << "]\n";

    // =====================================================================================
    // 4. MANUAL RAW MEMORY LIFECYCLE (PLACEMENT NEW & EXPLICIT DESTRUCTOR)
    // =====================================================================================
    cout << "\n================ 4. MANUAL RAW MEMORY LIFECYCLE ================\n";

    // Step A: Allocate raw uninitialized byte buffer with correct alignment
    alignas(alignof(MemoryPayload)) char rawStorage[sizeof(MemoryPayload)];

    cout << "  - Pre-allocated Stack Raw Byte Buffer Address : " << static_cast<const void*>(rawStorage) << "\n";

    // Step B: Manual construction inside uninitialized storage using placement new
    auto* payloadPtr = reinterpret_cast<MemoryPayload*>(rawStorage);
    new (static_cast<void*>(payloadPtr)) MemoryPayload(501, static_cast<double>(userInputValue) * 1.5);

    cout << "  - Constructed Object Member `id`              : " << payloadPtr->id << "\n";
    cout << "  - Constructed Object Member `value`           : " << payloadPtr->value << "\n";

    // Step C: Explicit manual destructor invocation (DO NOT call delete on raw placement memory!)
    cout << "  - Explicitly invoking destructor `payloadPtr->~MemoryPayload()`...\n";
    payloadPtr->~MemoryPayload();
    payloadPtr = nullptr;

    // =====================================================================================
    // 5. MEMORY ALIGNMENT MECHANICS (`alignof` & `std::align`)
    // =====================================================================================
    cout << "\n================ 5. MEMORY ALIGNMENT MECHANICS ================\n";

    cout << "  - Alignment Requirement `alignof(char)`          : " << alignof(char) << " byte(s)\n";
    cout << "  - Alignment Requirement `alignof(int)`           : " << alignof(int) << " byte(s)\n";
    cout << "  - Alignment Requirement `alignof(double)`        : " << alignof(double) << " byte(s)\n";
    cout << "  - Alignment Requirement `alignof(MemoryPayload)` : " << alignof(MemoryPayload) << " byte(s)\n";

    // Demonstrating pointer alignment via std::align
    char unalignedBuffer[256];
    void* ptrToAlign = unalignedBuffer + 1; // Intentionally offset/unaligned address
    size_t spaceLeft = sizeof(unalignedBuffer) - 1;

    cout << "\n  - Original Off-By-One Buffer Address             : " << static_cast<const void*>(ptrToAlign) << "\n";

    // Align pointer to an 8-byte boundary
    void* alignedPtr = std::align(8, sizeof(double), ptrToAlign, spaceLeft);

    if (alignedPtr != nullptr) {
        cout << "  - [std::align] Successfully Aligned Address      : " << static_cast<const void*>(alignedPtr) 
             << " (Aligned to 8-byte boundary)\n";
        cout << "  - Remaining Buffer Space After Alignment          : " << spaceLeft << " bytes\n";
    }

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ MEMORY MANIPULATION SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Function / Concept    | Syntax Example                    | Primary Operational Feature       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Memory Fill           | `std::memset(buf, val, size);`    | Sets every byte in buffer to `val`|\n"
         << "| Non-Overlapping Copy  | `std::memcpy(dest, src, size);`   | High-speed fast byte block copy   |\n"
         << "| Overlapping Safe Move | `std::memmove(dest, src, size);`  | Safe byte copy for overlapping memory|\n"
         << "| Memory Compare        | `std::memcmp(b1, b2, size);`      | Lexicographical byte-by-byte compare|\n"
         << "| Byte Inspection       | `reinterpret_cast<const char*>`   | Direct access to binary memory layout|\n"
         << "| Placement New         | `new (buf) T(args...);`           | Constructs object in raw buffer   |\n"
         << "| Explicit Destructor   | `ptr->~T();`                      | Invokes destructor on raw object  |\n"
         << "| Memory Alignment      | `std::align(alignment, sz, p, s);`| Adjusts pointer to aligned boundary|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}