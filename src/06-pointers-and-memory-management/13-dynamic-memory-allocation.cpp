/*
 * =====================================================================================
 * CONCEPT        : Dynamic Memory Allocation in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  heap memory management mechanics, allocation paradigms, and RAII:
 *
 *                  1. Single Object Allocation  : `new` and `delete` operators.
 *                  2. Array Allocation          : `new[]` and `delete[]` operators.
 *                  3. Exception Handling        : Catching `std::bad_alloc` on memory failure.
 *                  4. Non-Throwing Allocation   : `new (std::nothrow)` returning `nullptr`.
 *                  5. Placement New             : Constructing objects inside pre-allocated memory.
 *                  6. C vs C++ Allocation       : `malloc`/`free` vs `new`/`delete` key differences.
 *                  7. Modern RAII Alternatives  : Smart pointers (`std::unique_ptr` & `std::shared_ptr`).
 *
 * TIME COMPLEXITY  : Allocation / Deallocation: O(1) average OS kernel overhead.
 * SPACE COMPLEXITY : Heap storage footprint: O(N) bytes depending on allocation size.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <new>
#include <cstdlib>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER STRUCT FOR PLACEMENT NEW DEMONSTRATION
// =====================================================================================
struct DataNode {
    int id;
    double val;

    DataNode(int node_id, double node_val) : id(node_id), val(node_val) {
        cout << "    [Constructor] DataNode(" << id << ", " << val << ") initialized.\n";
    }

    ~DataNode() {
        cout << "    [Destructor] DataNode(" << id << ") destroyed.\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t userElementCount = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter dynamic array allocation size (e.g., 5): " << flush;
    if (!(cin >> userElementCount) || userElementCount == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting element count to 5." << endl;
        userElementCount = 5;
    }

    // =====================================================================================
    // 1. SINGLE OBJECT ALLOCATION (`new` AND `delete`)
    // =====================================================================================
    cout << "\n================ 1. SINGLE OBJECT HEAP ALLOCATION ================\n";

    // Allocating a single primitive integer on the heap
    int* heapScalar = new int(100); // Direct initialization

    cout << "  - Allocated Single Integer Address : " << static_cast<const void*>(heapScalar) << "\n";
    cout << "  - Allocated Value                  : " << *heapScalar << "\n";

    // Deallocate memory and disarm pointer
    delete heapScalar;
    heapScalar = nullptr; // BEST PRACTICE: Prevent dangling pointer

    cout << "  - Memory deallocated safely via `delete` and pointer reset to `nullptr`.\n";

    // =====================================================================================
    // 2. DYNAMIC ARRAY ALLOCATION (`new[]` AND `delete[]`)
    // =====================================================================================
    cout << "\n================ 2. DYNAMIC ARRAY HEAP ALLOCATION ================\n";

    // Allocating dynamic array initialized to zero
    int* heapArray = new int[userElementCount]{};

    // Populating dynamic array
    for (size_t i = 0; i < userElementCount; ++i) {
        heapArray[i] = static_cast<int>((i + 1) * 10);
    }

    cout << "  - Array Base Memory Address        : " << static_cast<const void*>(heapArray) << "\n";
    cout << "  - Array Elements                   : [ ";
    for (size_t i = 0; i < userElementCount; ++i) {
        cout << heapArray[i] << " ";
    }
    cout << "]\n";

    // CRITICAL: Use `delete[]` for array allocations to invoke proper array destructors
    delete[] heapArray;
    heapArray = nullptr;

    cout << "  - Array memory deallocated safely via `delete[]`.\n";

    // =====================================================================================
    // 3. EXCEPTION HANDLING (`std::bad_alloc`) & NON-THROWING ALLOCATION
    // =====================================================================================
    cout << "\n================ 3. EXCEPTION HANDLING & NOTHROW ALLOCATION ================\n";

    // A. Handling memory allocation failure via try-catch
    try {
        cout << "  - Attempting oversized allocation to trigger `std::bad_alloc`...\n";
        // Attempting enormous allocation (exceeding address space)
        size_t oversizedCount = static_cast<size_t>(-1) / 4;
        char* hugeBuffer = new char[oversizedCount];
        delete[] hugeBuffer;
    } catch (const std::bad_alloc& e) {
        cout << "    [EXCEPTIONAL SAFETY] Caught exception: " << e.what() << "\n";
    }

    // B. Non-throwing allocation using std::nothrow
    cout << "  - Attempting non-throwing allocation using `new (std::nothrow)`...\n";
    // Scale count by sizeof(int) to keep byte request below PTRDIFF_MAX while exceeding physical RAM
    size_t hugeCount = static_cast<size_t>(-1) / (sizeof(int) * 4);
    int* noThrowPtr = new (std::nothrow) int[hugeCount];

    if (noThrowPtr == nullptr) {
        cout << "    [NOTHROW SAFETY] Allocation failed gracefully! Returned `nullptr` without throwing.\n";
    } else {
        delete[] noThrowPtr;
    }

    // =====================================================================================
    // 4. PLACEMENT NEW (CONSTRUCTING IN PRE-ALLOCATED BUFFER)
    // =====================================================================================
    cout << "\n================ 4. PLACEMENT NEW ================\n";

    // Step A: Pre-allocate raw stack or heap byte buffer
    alignas(alignof(DataNode)) char rawBuffer[sizeof(DataNode)];
    cout << "  - Pre-allocated Buffer Address     : " << static_cast<const void*>(rawBuffer) << "\n";

    // Step B: Construct object inside the pre-allocated buffer
    DataNode* nodePtr = new (rawBuffer) DataNode(101, 98.6);
    cout << "  - Constructed Object Address       : " << static_cast<const void*>(nodePtr) << "\n";

    // Step C: Explicit destructor call required for placement new objects (DO NOT call delete!)
    nodePtr->~DataNode();

    // =====================================================================================
    // 5. C-STYLE DYNAMIC MEMORY (`malloc` / `free`) VS C++ OPERATORS
    // =====================================================================================
    cout << "\n================ 5. C-STYLE MALLOC/FREE VS C++ NEW/DELETE ================\n";

    // Allocating raw bytes via malloc
    int* cStyleArr = static_cast<int*>(std::malloc(userElementCount * sizeof(int)));
    if (cStyleArr != nullptr) {
        cStyleArr[0] = 55;
        cout << "  - C-Style `malloc` Address        : " << static_cast<const void*>(cStyleArr) 
             << " | Value: " << cStyleArr[0] << "\n";
        
        // Reallocating memory buffer size
        int* reallocArr = static_cast<int*>(std::realloc(cStyleArr, (userElementCount + 2) * sizeof(int)));
        if (reallocArr != nullptr) {
            cStyleArr = reallocArr;
            cout << "  - C-Style `realloc` Expanded Address: " << static_cast<const void*>(cStyleArr) << "\n";
        }

        std::free(cStyleArr);
        cStyleArr = nullptr;
    }

    // =====================================================================================
    // 6. MODERN C++ RAII SMART POINTERS (`std::unique_ptr` & `std::shared_ptr`)
    // =====================================================================================
    cout << "\n================ 6. MODERN RAII SMART POINTERS ================\n";

    // Safe auto-managed heap allocation (No manual delete needed!)
    {
        std::unique_ptr<int[]> smartArray = std::make_unique<int[]>(userElementCount);
        smartArray[0] = 777;

        cout << "  - `std::unique_ptr` Managed Address: " << static_cast<const void*>(smartArray.get()) 
             << " | Value: " << smartArray[0] << "\n";
        cout << "  - [RAII ADVANTAGE] Heap memory will be automatically freed at block end.\n";
    } // Memory released automatically HERE!

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DYNAMIC MEMORY ALLOCATION SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Allocation Syntax     | Memory Source     | Cleanup Mechanism | Primary Features & Use Case       |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| `new T`               | Heap Segment      | `delete ptr`      | Constructs scalar object + init   |\n"
         << "| `new T[N]`            | Heap Segment      | `delete[] ptr`    | Constructs dynamic array buffer   |\n"
         << "| `new (nothrow) T`     | Heap Segment      | `delete ptr`      | Returns `nullptr` on fail (No ex) |\n"
         << "| Placement `new (buf)` | Provided Buffer   | Manual `ptr->~T()`| Constructs in existing memory     |\n"
         << "| `malloc` / `free`     | Heap Segment      | `free(ptr)`       | C-legacy byte allocation (No ctor)|\n"
         << "| `std::make_unique<T>` | Heap Segment      | Automatic Scope   | Modern C++ RAII safe allocation   |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}