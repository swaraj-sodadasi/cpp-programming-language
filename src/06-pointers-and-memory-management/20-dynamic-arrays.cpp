/*
 * =====================================================================================
 * CONCEPT        : Dynamic Arrays in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  dynamic array allocation, resizing strategies, memory layout paradigms, 
 *                  pointer offset arithmetic, and RAII dynamic buffer management:
 *
 *                  1. Basic Dynamic Arrays       : Allocation via `new[]` and mandatory `delete[]` cleanup.
 *                  2. Dynamic Buffer Resizing    : Simulating capacity growth (doubling) with buffer re-allocation.
 *                  3. 2D Dynamic Array Layouts   : Contiguous 1D flat layout vs Pointer-of-Pointers (`T**`).
 *                  4. Pointer Offset Traversal   : Accessing dynamic elements via `*(ptr + i)` vs `ptr[i]`.
 *                  5. Modern RAII Smart Arrays   : Exception-safe dynamic arrays using `std::unique_ptr<T[]>`.
 *
 * TIME COMPLEXITY  : Element Access / Offset: O(1) | Allocation / Traversal / Resizing: O(N)
 * SPACE COMPLEXITY : Heap storage footprint: O(N) bytes depending on capacity.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <utility>

using namespace std;

// =====================================================================================
// HELPER CLASS: CUSTOM RESIZABLE DYNAMIC ARRAY (SIMULATING STD::VECTOR MECHANICS)
// Demonstrates capacity expansion, memory reallocation, and proper RAII deallocation.
// =====================================================================================
template <typename T>
class CustomDynamicArray {
private:
    T* buffer_;
    size_t size_;
    size_t capacity_;

    void reallocate(size_t newCapacity) {
        cout << "    [BUFFER REALLOCATION] Growing capacity from " << capacity_ 
             << " to " << newCapacity << " elements...\n";

        T* newBuffer = new T[newCapacity];

        // Copy elements from old buffer to new buffer
        for (size_t i = 0; i < size_; ++i) {
            newBuffer[i] = std::move(buffer_[i]);
        }

        // Deallocate old heap buffer
        delete[] buffer_;

        buffer_ = newBuffer;
        capacity_ = newCapacity;
    }

public:
    CustomDynamicArray() : buffer_(nullptr), size_(0), capacity_(0) {}

    explicit CustomDynamicArray(size_t initialCapacity) 
        : buffer_(new T[initialCapacity]{}), size_(0), capacity_(initialCapacity) {}

    ~CustomDynamicArray() {
        delete[] buffer_; // RAII Cleanup
    }

    // Disable copy semantics for simple ownership demonstration
    CustomDynamicArray(const CustomDynamicArray&) = delete;
    CustomDynamicArray& operator=(const CustomDynamicArray&) = delete;

    void push_back(const T& value) {
        if (size_ == capacity_) {
            size_t newCapacity = (capacity_ == 0) ? 2 : capacity_ * 2; // Doubling strategy
            reallocate(newCapacity);
        }
        buffer_[size_++] = value;
    }

    [[nodiscard]] size_t size() const { return size_; }
    [[nodiscard]] size_t capacity() const { return capacity_; }
    [[nodiscard]] const T* data() const { return buffer_; }

    T& operator[](size_t index) { return buffer_[index]; }
    const T& operator[](size_t index) const { return buffer_[index]; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t userInitialSize = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter initial dynamic array element count (e.g., 4): " << flush;
    if (!(cin >> userInitialSize) || userInitialSize == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting element count to 4." << endl;
        userInitialSize = 4;
    }

    // =====================================================================================
    // 1. BASIC DYNAMIC ARRAY ALLOCATION & DEALLOCATION (`new[]` / `delete[]`)
    // =====================================================================================
    cout << "\n================ 1. BASIC DYNAMIC ARRAY ALLOCATION ================\n";

    // Allocate dynamic array initialized with values
    int* rawDynamicArray = new int[userInitialSize];

    for (size_t i = 0; i < userInitialSize; ++i) {
        rawDynamicArray[i] = static_cast<int>((i + 1) * 10);
    }

    cout << "  - Heap Base Buffer Address           : " << static_cast<const void*>(rawDynamicArray) << "\n";
    cout << "  - Array Elements                     : [ ";
    for (size_t i = 0; i < userInitialSize; ++i) {
        cout << rawDynamicArray[i] << " ";
    }
    cout << "]\n";

    // Deallocate memory using array delete
    delete[] rawDynamicArray;
    rawDynamicArray = nullptr; // DISARM: Prevent dangling pointer

    cout << "  - Memory freed safely via `delete[]` and pointer set to `nullptr`.\n";

    // =====================================================================================
    // 2. DYNAMIC BUFFER RESIZING MECHANICS (CAPACITY GROWTH)
    // =====================================================================================
    cout << "\n================ 2. DYNAMIC BUFFER RESIZING MECHANICS ================\n";

    CustomDynamicArray<int> resizableArray(2);
    cout << "  - Initial Array Size: " << resizableArray.size() 
         << " | Capacity: " << resizableArray.capacity() << "\n";

    cout << "  - Pushing 5 elements to demonstrate dynamic capacity growth:\n";
    for (int i = 1; i <= 5; ++i) {
        resizableArray.push_back(i * 100);
        cout << "    Appended " << setw(3) << i * 100 
             << " | Current Size: " << resizableArray.size() 
             << " | Current Capacity: " << resizableArray.capacity() << "\n";
    }

    cout << "  - Final Elements in Resizable Array : [ ";
    for (size_t i = 0; i < resizableArray.size(); ++i) {
        cout << resizableArray[i] << " ";
    }
    cout << "]\n";

    // =====================================================================================
    // 3. 2D DYNAMIC ARRAYS: FLAT 1D vs POINTER-OF-POINTERS (`T**`)
    // =====================================================================================
    cout << "\n================ 3. 2D DYNAMIC ARRAY LAYOUT PARADIGMS ================\n";

    constexpr size_t ROWS = 2;
    constexpr size_t COLS = 3;

    // Paradigm A: Pointer of Pointers (Non-contiguous row buffers)
    cout << "  - [Paradigm A] Allocation via Pointer-of-Pointers (`int**`):\n";
    int** pointerArray2D = new int*[ROWS];
    for (size_t r = 0; r < ROWS; ++r) {
        pointerArray2D[r] = new int[COLS];
        for (size_t c = 0; c < COLS; ++c) {
            pointerArray2D[r][c] = static_cast<int>((r + 1) * 10 + c);
        }
    }

    for (size_t r = 0; r < ROWS; ++r) {
        cout << "    * Row [" << r << "] Address: " << static_cast<const void*>(pointerArray2D[r]) 
             << " | Items: [ ";
        for (size_t c = 0; c < COLS; ++c) {
            cout << pointerArray2D[r][c] << " ";
        }
        cout << "]\n";
    }

    // Cleanup Paradigm A
    for (size_t r = 0; r < ROWS; ++r) {
        delete[] pointerArray2D[r];
    }
    delete[] pointerArray2D;
    pointerArray2D = nullptr;

    // Paradigm B: Flat 1D Array (Contiguous cache-friendly 2D layout)
    cout << "\n  - [Paradigm B] Cache-Friendly Flat 1D Array (`index = r * COLS + c`):\n";
    int* flatArray2D = new int[ROWS * COLS];

    for (size_t r = 0; r < ROWS; ++r) {
        for (size_t c = 0; c < COLS; ++c) {
            flatArray2D[r * COLS + c] = static_cast<int>((r + 1) * 100 + c);
        }
    }

    cout << "    * Flat Buffer Base Address: " << static_cast<const void*>(flatArray2D) << "\n";
    cout << "    * Grid Contents:\n";
    for (size_t r = 0; r < ROWS; ++r) {
        cout << "      [ ";
        for (size_t c = 0; c < COLS; ++c) {
            cout << flatArray2D[r * COLS + c] << " ";
        }
        cout << "]\n";
    }

    delete[] flatArray2D;
    flatArray2D = nullptr;

    // =====================================================================================
    // 4. POINTER OFFSET TRAVERSAL ON DYNAMIC BUFFERS
    // =====================================================================================
    cout << "\n================ 4. POINTER OFFSET TRAVERSAL MECHANICS ================\n";

    int* offsetArray = new int[userInitialSize];
    for (size_t i = 0; i < userInitialSize; ++i) {
        offsetArray[i] = static_cast<int>((i + 1) * 5);
    }

    cout << "  - Demonstrating Subscript `ptr[i]` vs Pointer Offset `*(ptr + i)`:\n";
    for (size_t i = 0; i < userInitialSize; ++i) {
        cout << "    Element [" << i << "] Addr: " << static_cast<const void*>(offsetArray + i)
             << " | `offsetArray[" << i << "]`: " << setw(2) << offsetArray[i]
             << " | `*(offsetArray + " << i << ")`: " << setw(2) << *(offsetArray + i) << "\n";
    }

    delete[] offsetArray;
    offsetArray = nullptr;

    // =====================================================================================
    // 5. MODERN C++ SOLUTION: RAII SMART POINTER ARRAYS (`std::unique_ptr<int[]>`)
    // =====================================================================================
    cout << "\n================ 5. MODERN RAII SMART DYNAMIC ARRAYS ================\n";

    // Exception-safe dynamic array with zero manual delete overhead
    std::unique_ptr<int[]> smartDynamicArray = std::make_unique<int[]>(userInitialSize);

    for (size_t i = 0; i < userInitialSize; ++i) {
        smartDynamicArray[i] = static_cast<int>((i + 1) * 111);
    }

    cout << "  - `std::unique_ptr<int[]>` Heap Address : " << static_cast<const void*>(smartDynamicArray.get()) << "\n";
    cout << "  - Smart Array Elements                  : [ ";
    for (size_t i = 0; i < userInitialSize; ++i) {
        cout << smartDynamicArray[i] << " ";
    }
    cout << "]\n";
    cout << "  - [RAII ADVANTAGE]: Buffer freed automatically when `smartDynamicArray` leaves scope.\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ DYNAMIC ARRAYS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Dynamic Array Paradigm| Allocation / Declaration Syntax    | Key Characteristics & Use Case    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Basic Dynamic Array   | `int* arr = new int[N];`          | Heap buffer, requires `delete[]`  |\n"
         << "| Deallocation Rule     | `delete[] arr; arr = nullptr;`    | Calls destructors for all items   |\n"
         << "| Resizing Strategy     | Allocate new -> Copy -> Free old  | Amortized O(1) growth strategy    |\n"
         << "| 2D Pointer-of-Pointers| `int** arr = new int*[ROWS];`     | Allows non-uniform row lengths    |\n"
         << "| 2D Flat 1D Layout     | `int* arr = new int[R * C];`      | Contiguous, highly cache-efficient|\n"
         << "| Subscript Equivalence | `arr[i]` <==> `*(arr + i)`        | Pointer arithmetic offset access  |\n"
         << "| RAII Smart Array      | `auto p = std::make_unique<T[]>(N)`| Exception-safe automatic cleanup |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}