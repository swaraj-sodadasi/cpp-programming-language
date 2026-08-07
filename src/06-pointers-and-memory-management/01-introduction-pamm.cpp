/*
 * =====================================================================================
 * CONCEPT        : Introduction to Pointers and Memory Management in Modern C++
 * DESCRIPTION    : Comprehensive guide and implementation of pointer mechanics and dynamic memory:
 *                  1. Pointer Fundamentals     : Memory addresses (`&`), dereferencing (`*`), 
 *                                                 and `nullptr` safety.
 *                  2. Dynamic Memory (RAII)    : Heap allocation (`new`, `delete`, `new[]`, `delete[]`).
 *                  3. Pointer Arithmetic       : Moving pointers across contiguous memory offsets.
 *                  4. Memory Pitfalls          : Dangling pointers, memory leaks, double free, 
 *                                                 and pointer resetting.
 *                  5. Pointers vs References   : Nullability, re-assignment, and indirection syntax.
 *                  6. Modern Smart Pointers    : `std::unique_ptr` as standard RAII wrapper over raw pointers.
 *
 * TIME COMPLEXITY  : Direct access / Dereference: O(1) | Dynamic Allocation: O(1) system overhead
 * SPACE COMPLEXITY : Raw pointer size: 8 bytes (64-bit systems) / 4 bytes (32-bit systems)
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER FUNCTION: POINTER VS REFERENCE DEMONSTRATION
// =====================================================================================
void modifyByPointer(int* ptr) {
    if (ptr != nullptr) {
        *ptr += 10;
    }
}

void modifyByReference(int& ref) {
    ref += 10;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int initialValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value for memory address inspection (e.g., 42): " << flush;
    if (!(cin >> initialValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting value to 42." << endl;
        initialValue = 42;
    }

    // =====================================================================================
    // 1. POINTER FUNDAMENTALS (ADDRESS-OF & DEREFERENCING)
    // =====================================================================================
    cout << "\n================ 1. POINTER DECLARATION & DEREFERENCING ================\n";

    int val = initialValue;
    int* ptr = &val; // ptr stores the memory address of val

    cout << "  - Value of `val`                       : " << val << "\n";
    cout << "  - Memory Address of `val` (`&val`)     : " << static_cast<const void*>(&val) << "\n";
    cout << "  - Value of `ptr` (Address stored)      : " << static_cast<const void*>(ptr) << "\n";
    cout << "  - Dereferenced `*ptr` (Value pointed)  : " << *ptr << "\n";
    cout << "  - Size of Raw Pointer (`sizeof(ptr)`)  : " << sizeof(ptr) << " bytes\n";

    // Modifying value via pointer
    *ptr = 100;
    cout << "  - After `*ptr = 100`, `val` is now     : " << val << "\n";

    // Nullptr initialization safety
    int* nullPtr = nullptr; // Modern C++ nullptr (Type-safe vs legacy 0/NULL)
    cout << "  - `nullptr` inspection                 : " << static_cast<const void*>(nullPtr) << "\n";

    // =====================================================================================
    // 2. DYNAMIC MEMORY ALLOCATION (`new` AND `delete`)
    // =====================================================================================
    cout << "\n================ 2. DYNAMIC HEAP MEMORY ALLOCATION ================\n";

    // Allocating single object on heap
    int* heapValue = new int(250); // Allocated on heap
    cout << "  - Single Heap Allocation Value         : " << *heapValue 
         << " | Address: " << static_cast<const void*>(heapValue) << "\n";

    delete heapValue;      // Deallocate heap memory
    heapValue = nullptr;   // BEST PRACTICE: Prevent dangling pointer by setting to nullptr

    // Allocating dynamic array on heap
    constexpr size_t ARRAY_SIZE = 4;
    int* heapArray = new int[ARRAY_SIZE]{10, 20, 30, 40};

    cout << "  - Dynamic Heap Array Elements          : [ ";
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        cout << heapArray[i] << " ";
    }
    cout << "]\n";

    delete[] heapArray;    // CRITICAL: Use delete[] for array allocations
    heapArray = nullptr;   // Reset pointer

    // =====================================================================================
    // 3. POINTER ARITHMETIC & CONTIGUOUS MEMORY
    // =====================================================================================
    cout << "\n================ 3. POINTER ARITHMETIC ================\n";

    int stackArray[3] = {1000, 2000, 3000};
    int* pWalk = stackArray;

    cout << "  - Base Address (`pWalk`)               : " << static_cast<const void*>(pWalk) 
         << " | Value: " << *pWalk << "\n";

    ++pWalk; // Advances pointer by sizeof(int) = 4 bytes
    cout << "  - After `++pWalk` (+4 bytes offset)    : " << static_cast<const void*>(pWalk) 
         << " | Value: " << *pWalk << "\n";

    pWalk += 1; // Advances to element index 2
    cout << "  - After `pWalk += 1` (+4 bytes offset) : " << static_cast<const void*>(pWalk) 
         << " | Value: " << *pWalk << "\n";

    ptrdiff_t distance = pWalk - stackArray; // Calculating offset distance
    cout << "  - Element Distance (`pWalk - base`)    : " << distance << " elements\n";

    // =====================================================================================
    // 4. MEMORY PITFALLS & PREVENTATIVE SAFETY
    // =====================================================================================
    cout << "\n================ 4. MEMORY SAFETY & DANGLING POINTERS ================\n";

    int* danglingPtr = nullptr;
    {
        int tempVal = 999;
        danglingPtr = &tempVal;
        cout << "  - Inside Scope: `*danglingPtr`         : " << *danglingPtr << "\n";
    } // tempVal goes out of scope and is destroyed HERE!

    cout << "  - [PITFALL WARNING]: `danglingPtr` now references destroyed stack memory!\n";
    danglingPtr = nullptr; // RESOLVED: Disarm dangling pointer by setting to nullptr

    // Safe dereference check pattern
    if (danglingPtr != nullptr) {
        cout << "  - Value: " << *danglingPtr << "\n";
    } else {
        cout << "  - [SAFE]: Pointer check prevented null/dangling dereference.\n";
    }

    // =====================================================================================
    // 5. POINTERS VS REFERENCES COMPARISON
    // =====================================================================================
    cout << "\n================ 5. POINTERS VS REFERENCES ================\n";

    int target = 50;
    cout << "  - Initial Target Value                 : " << target << "\n";

    modifyByPointer(&target);
    cout << "  - After `modifyByPointer(&target)`     : " << target << "\n";

    modifyByReference(target);
    cout << "  - After `modifyByReference(target)`   : " << target << "\n";

    // =====================================================================================
    // 6. MODERN C++ RAII: SMART POINTERS (`std::unique_ptr`)
    // =====================================================================================
    cout << "\n================ 6. MODERN SMART POINTERS (`std::unique_ptr`) ================\n";

    // Modern C++ alternative to manual new/delete
    std::unique_ptr<int> smartVal = std::make_unique<int>(777);

    cout << "  - Smart Pointer Dereferenced (`*smartVal`): " << *smartVal << "\n";
    cout << "  - Smart Pointer Address (`smartVal.get()`): " << static_cast<const void*>(smartVal.get()) << "\n";
    cout << "  - [RAII ADVANTAGE]: Automatic deallocation when `smartVal` goes out of scope! Zero manual `delete` required.\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ POINTERS VS REFERENCES VS SMART POINTERS SUMMARY ================\n";
    cout << "+-----------------------+---------------------------+---------------------------+---------------------------+\n"
         << "| Trait / Feature       | Raw Pointer (`T*`)        | Reference (`T&`)          | Smart Pointer (`unique_ptr`)|\n"
         << "+-----------------------+---------------------------+---------------------------+---------------------------+\n"
         << "| Nullability           | YES (`nullptr` allowed)   | NO (Must bind to object)  | YES (`nullptr` allowed)   |\n"
         << "| Re-assignable?        | YES (Can point elsewhere) | NO (Bound once at init)   | YES (via move semantics)  |\n"
         << "| Memory Ownership      | Manual (`new`/`delete`)   | Non-owning alias          | Automatic (RAII Scope)    |\n"
         << "| Arithmetic (`++`, `--`)| Supported                 | Not Supported             | Not Supported             |\n"
         << "| Indirection Syntax    | Explicit `*ptr` / `ptr->` | Implicit direct usage     | Explicit `*sptr` / `sptr->`|\n"
         << "| Memory Leak Risk      | HIGH                      | NONE                      | NONE                      |\n"
         << "+-----------------------+---------------------------+---------------------------+---------------------------+\n";

    return 0;
}