/*
 * =====================================================================================
 * CONCEPT        : Pointer Initialization in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade implementation covering:
 *                  1. Null Pointer Initialization  : `nullptr` (C++11) vs legacy `NULL`/`0`.
 *                  2. Lvalue Address Initialization: Direct binding to variable addresses (`&var`).
 *                  3. Dynamic Memory Initialization: Heap allocation (`new T(val)`, `new T{val}`).
 *                  4. Const Pointer Initializations: `const T*`, `T* const`, `const T* const`.
 *                  5. Modern Smart Pointer Init    : `std::make_unique` & `std::make_shared`.
 *
 * TIME COMPLEXITY  : Initialization & Access: O(1) constant time.
 * SPACE COMPLEXITY : Raw Pointer Size: Uniform 8 bytes (64-bit) / 4 bytes (32-bit).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <limits>

using namespace std;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputVal = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value for pointer binding (e.g., 88): " << flush;
    if (!(cin >> userInputVal)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 88." << endl;
        userInputVal = 88;
    }

    // =====================================================================================
    // 1. NULL POINTER INITIALIZATION (`nullptr`)
    // =====================================================================================
    cout << "\n================ 1. NULL POINTER INITIALIZATION ================\n";

    // Modern C++ standard: ALWAYS initialize pointers to nullptr if not immediately bound
    int* nullPtr = nullptr;

    cout << "  - `nullptr` Initialized Pointer Address : " << static_cast<const void*>(nullPtr) << "\n";
    cout << "  - Safety Status                          : Safe disarmed state (Defensive check ready)\n";

    if (nullPtr == nullptr) {
        cout << "  - Defensive Check                        : Pointer is null, safe from dereference crash.\n";
    }

    // =====================================================================================
    // 2. LVALUE ADDRESS INITIALIZATION (`&var`)
    // =====================================================================================
    cout << "\n================ 2. LVALUE ADDRESS INITIALIZATION ================\n";

    int stackVar = userInputVal;
    int* boundPtr = &stackVar; // Initialized directly with stack variable address

    cout << "  - Stack Variable Value                   : " << stackVar << "\n";
    cout << "  - Stack Variable Address                 : " << static_cast<const void*>(&stackVar) << "\n";
    cout << "  - Bound Pointer Stored Address           : " << static_cast<const void*>(boundPtr) << "\n";
    cout << "  - Dereferenced Pointer Value             : " << *boundPtr << "\n";

    // =====================================================================================
    // 3. DYNAMIC HEAP MEMORY INITIALIZATION (`new`)
    // =====================================================================================
    cout << "\n================ 3. DYNAMIC HEAP MEMORY INITIALIZATION ================\n";

    // Direct initialization on heap
    int* heapDirectPtr = new int(100);

    // Uniform/List initialization on heap (C++11)
    int* heapListPtr = new int{200};

    // Zero/Value-initialized dynamic array
    int* heapArrayPtr = new int[3]{10, 20, 30};

    cout << "  - Direct Init Heap Pointer (`new int(100)`) : Address: " 
         << static_cast<const void*>(heapDirectPtr) << " | Value: " << *heapDirectPtr << "\n";
    cout << "  - Uniform Init Heap Pointer (`new int{200}`): Address: " 
         << static_cast<const void*>(heapListPtr) << " | Value: " << *heapListPtr << "\n";
    cout << "  - Dynamic Array Base Pointer               : Address: " 
         << static_cast<const void*>(heapArrayPtr) << " | First Elem: " << heapArrayPtr[0] << "\n";

    delete heapDirectPtr;
    heapDirectPtr = nullptr;

    delete heapListPtr;
    heapListPtr = nullptr;

    delete[] heapArrayPtr;
    heapArrayPtr = nullptr;

    // =====================================================================================
    // 4. CONST-QUALIFIED POINTER INITIALIZATIONS
    // =====================================================================================
    cout << "\n================ 4. CONST-QUALIFIED POINTER INITIALIZATIONS ================\n";

    int mutableVar = 50;
    const int constantVar = 100;

    // A. Pointer to Constant Data (Data read-only, Pointer rebindable)
    const int* ptrToConst = &constantVar;

    // B. Constant Pointer to Mutable Data (Data mutable, Pointer frozen)
    int* const constPtr = &mutableVar;

    // C. Constant Pointer to Constant Data (Data & Pointer frozen)
    const int* const constPtrToConst = &constantVar;

    cout << "  - `const int*` (Pointer to Const Data)    : Address: " 
         << static_cast<const void*>(ptrToConst) << " | Value: " << *ptrToConst << "\n";
    cout << "  - `int* const` (Const Pointer to Data)   : Address: " 
         << static_cast<const void*>(constPtr) << " | Value: " << *constPtr << "\n";
    cout << "  - `const int* const` (Fully Const)       : Address: " 
         << static_cast<const void*>(constPtrToConst) << " | Value: " << *constPtrToConst << "\n";

    // =====================================================================================
    // 5. MODERN SMART POINTER INITIALIZATION
    // =====================================================================================
    cout << "\n================ 5. MODERN SMART POINTER INITIALIZATION ================\n";

    // A. std::unique_ptr via std::make_unique (Preferred RAII pattern)
    std::unique_ptr<int> uniqueSmartPtr = std::make_unique<int>(userInputVal * 2);

    // B. std::shared_ptr via std::make_shared
    std::shared_ptr<int> sharedSmartPtr = std::make_shared<int>(777);

    cout << "  - `std::unique_ptr` Initialized Value      : " << *uniqueSmartPtr 
         << " | Heap Addr: " << static_cast<const void*>(uniqueSmartPtr.get()) << "\n";
    cout << "  - `std::shared_ptr` Initialized Value      : " << *sharedSmartPtr 
         << " | Use Count: " << sharedSmartPtr.use_count() << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ POINTER INITIALIZATION SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Initialization Type   | Syntax Example                    | Memory Location & Characteristics |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Null Initialization   | `int* ptr = nullptr;`             | Points to address 0x0 (Disarmed)  |\n"
         << "| Lvalue Address Binding| `int* ptr = &variable;`           | Points to stack/data variable     |\n"
         << "| Dynamic Heap Direct   | `int* ptr = new int(42);`         | Heap allocated with direct value  |\n"
         << "| Dynamic Heap Uniform  | `int* ptr = new int{42};`         | Heap allocated with list init     |\n"
         << "| Dynamic Array Init    | `int* ptr = new int[N]{};`        | Heap allocated contiguous array   |\n"
         << "| Pointer to Const Data | `const int* ptr = &var;`          | Read-only data access             |\n"
         << "| Const Pointer Address | `int* const ptr = &var;`          | Immutable target address          |\n"
         << "| Unique Smart Pointer  | `auto p = std::make_unique<T>(v);`| RAII Heap allocation (Exclusive)  |\n"
         << "| Shared Smart Pointer  | `auto p = std::make_shared<T>(v);`| RAII Heap allocation (Ref Count)  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}