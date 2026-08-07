/*
 * =====================================================================================
 * CONCEPT        : Memory Management in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how Modern C++ handles dynamic heap memory management using the 
 *                  Resource Acquisition Is Initialization (RAII) idiom:
 *
 *                  1. Heap Memory Encapsulation  : Binding dynamic heap allocations (`new` / `new[]`) 
 *                                                   directly to stack-bound object lifetimes.
 *                  2. Automatic Heap Cleanup      : Deterministic deallocation (`delete` / `delete[]`) 
 *                                                   upon scope exit without manual intervention.
 *                  3. Custom Memory RAII Wrapper  : Designing a production-grade `ScopedHeapArray<T>` 
 *                                                   implementing the Rule of 5 and move-only ownership.
 *                  4. Standard Memory Wrappers    : Utilizing `std::unique_ptr<T[]>`, `std::unique_ptr<T>`, 
 *                                                   and `std::shared_ptr<T>` for zero-leak heap management.
 *                  5. Custom Memory Deleters      : Extending RAII memory management to legacy C-style 
 *                                                   allocations (`malloc` / `free`, aligned memory).
 *                  6. Exception Safety & Unwinding: Unconditional heap cleanup when exceptions occur 
 *                                                   during allocation or processing pipelines.
 *
 * TIME COMPLEXITY  : Allocation / Deallocation / Access / Move: O(1) constant time.
 * SPACE COMPLEXITY : Memory Footprint: Directly equal to the dynamically allocated heap buffer payload.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <cstdlib>
#include <string>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: HEAP ALLOCATION TRACKER
// Tracks active heap memory nodes to visually verify zero-leak RAII execution.
// =====================================================================================
class HeapNode {
private:
    int id_;
    string label_;

public:
    static inline int activeHeapObjects = 0; // C++17 inline static tracker

    HeapNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeHeapObjects;
        cout << "    [HEAP ALLOCATED]   Node ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Objects: " << activeHeapObjects << "\n";
    }

    ~HeapNode() noexcept {
        --activeHeapObjects;
        cout << "    [HEAP DEALLOCATED] Node ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at address " << static_cast<const void*>(this) 
             << " | Active Objects: " << activeHeapObjects << "\n";
    }

    void execute() const {
        cout << "    [HEAP BUSY]        Node ID: " << id_ << " (" << label_ << ") executing payload.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: CUSTOM RAII HEAP ARRAY WRAPPER (`ScopedHeapArray<T>`)
// Production-grade implementation of dynamic array memory management using Rule of 5.
// =====================================================================================
template <typename T>
class ScopedHeapArray {
private:
    T* buffer_;
    size_t capacity_;

public:
    // 1. Constructor: Acquires dynamic heap array memory directly on initialization
    explicit ScopedHeapArray(size_t capacity) 
        : buffer_(capacity > 0 ? new T[capacity] : nullptr), capacity_(capacity) {
        cout << "    [ScopedHeapArray Ctor] Allocated dynamic heap array of size " << capacity_ 
             << " at address " << static_cast<const void*>(buffer_) << "\n";
    }

    // 2. Destructor: Deterministic cleanup using dynamic array release operator (`delete[]`)
    ~ScopedHeapArray() noexcept {
        cout << "    [ScopedHeapArray Dtor] Deallocating dynamic heap array at address " 
             << static_cast<const void*>(buffer_) << "\n";
        delete[] buffer_; // Correct pairing for new[]
    }

    // 3. Rule of 5: Suppress Copy Operations (Enforce single ownership, prevent double-free)
    ScopedHeapArray(const ScopedHeapArray&) = delete;
    ScopedHeapArray& operator=(const ScopedHeapArray&) = delete;

    // 4. Rule of 5: Move Constructor (Safe ownership transfer)
    ScopedHeapArray(ScopedHeapArray&& other) noexcept 
        : buffer_(other.buffer_), capacity_(other.capacity_) {
        other.buffer_ = nullptr;
        other.capacity_ = 0;
        cout << "    [ScopedHeapArray Move Ctor] Dynamic array heap ownership transferred.\n";
    }

    // 5. Rule of 5: Move Assignment Operator
    ScopedHeapArray& operator=(ScopedHeapArray&& other) noexcept {
        if (this != &other) {
            delete[] buffer_;          // Release existing heap memory buffer
            buffer_ = other.buffer_;     // Transfer ownership
            capacity_ = other.capacity_;
            other.buffer_ = nullptr;
            other.capacity_ = 0;
            cout << "    [ScopedHeapArray Move Assign] Existing freed, new ownership transferred.\n";
        }
        return *this;
    }

    // Subscript Operators
    T& operator[](size_t index) { return buffer_[index]; }
    const T& operator[](size_t index) const { return buffer_[index]; }

    // Utility Accessors
    [[nodiscard]] size_t capacity() const { return capacity_; }
    [[nodiscard]] T* get() const { return buffer_; }
    [[nodiscard]] explicit operator bool() const { return buffer_ != nullptr; }
};

// =====================================================================================
// HELPER FUNCTOR: CUSTOM DELETER FOR C-STYLE DYNAMIC MEMORY ALLOCATIONS
// Demonstrates extending RAII memory management to `malloc` / `free` legacy buffers.
// =====================================================================================
struct CFreeDeleter {
    void operator()(void* ptr) const noexcept {
        if (ptr != nullptr) {
            cout << "    [CFreeDeleter CALL] Releasing C-style dynamic memory at address " 
                 << static_cast<const void*>(ptr) << " via `std::free()`\n";
            std::free(ptr);
        }
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t userElementCount = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter dynamic array element capacity for memory management testing (e.g., 4): " << flush;
    if (!(cin >> userElementCount) || userElementCount == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting element count to 4." << endl;
        userElementCount = 4;
    }

    // =====================================================================================
    // 1. CUSTOM RAII HEAP MEMORY WRAPPER (`ScopedHeapArray`)
    // =====================================================================================
    cout << "\n================ 1. CUSTOM RAII HEAP ARRAY MEMORY MANAGEMENT ================\n";

    {
        cout << "  - Allocating custom scoped heap array for primitive integers...\n";
        ScopedHeapArray<int> intArray(userElementCount);

        // Populate heap array
        for (size_t i = 0; i < intArray.capacity(); ++i) {
            intArray[i] = static_cast<int>((i + 1) * 100);
        }

        cout << "  - Array Elements on Heap: [ ";
        for (size_t i = 0; i < intArray.capacity(); ++i) {
            cout << intArray[i] << " ";
        }
        cout << "]\n";

        cout << "  - Leaving scope (ScopedHeapArray destructor will call `delete[]` automatically):\n";
    } // Memory released HERE automatically!

    cout << "  - Active Heap Objects Post-Custom Scope: " << HeapNode::activeHeapObjects << "\n";

    // =====================================================================================
    // 2. STANDARD LIBRARY RAII MEMORY MANAGEMENT (`std::unique_ptr` & Arrays)
    // =====================================================================================
    cout << "\n================ 2. STANDARD SMART POINTER HEAP MANAGEMENT ================\n";

    // A. Single Heap Object Allocation
    {
        cout << "  - Case A: Single Object Heap Allocation (`std::make_unique<HeapNode>`):\n";
        auto singleNode = std::make_unique<HeapNode>(101, "SingleSmartNode");
        singleNode->execute();

        cout << "  - Exiting single node scope...\n";
    } // Memory released automatically

    cout << "  - Active Heap Objects Post-Single Node: " << HeapNode::activeHeapObjects << "\n";

    // B. Array Heap Allocation (`std::unique_ptr<T[]>`)
    {
        cout << "\n  - Case B: Dynamic Array Heap Allocation (`std::make_unique<int[]>`):\n";
        auto smartArray = std::make_unique<int[]>(userElementCount);

        for (size_t i = 0; i < userElementCount; ++i) {
            smartArray[i] = static_cast<int>((i + 1) * 55);
        }

        cout << "  - Smart Array Elements: [ ";
        for (size_t i = 0; i < userElementCount; ++i) {
            cout << smartArray[i] << " ";
        }
        cout << "]\n";

        cout << "  - Exiting smart array scope...\n";
    } // Array memory freed via `delete[]` automatically

    // =====================================================================================
    // 3. SHARED HEAP MEMORY MANAGEMENT (`std::shared_ptr`)
    // =====================================================================================
    cout << "\n================ 3. SHARED HEAP MEMORY MANAGEMENT (`std::shared_ptr`) ================\n";

    {
        cout << "  - Allocating shared heap object via `std::make_shared`...\n";
        std::shared_ptr<HeapNode> shared1 = std::make_shared<HeapNode>(202, "SharedHeapNode");
        cout << "    * `shared1` Reference Count = " << shared1.use_count() << "\n";

        {
            cout << "  - Creating secondary co-owner `shared2 = shared1`...\n";
            std::shared_ptr<HeapNode> shared2 = shared1;
            cout << "    * Reference Count with 2 co-owners = " << shared1.use_count() << "\n";

            shared2->execute();
            cout << "  - Leaving inner scope...\n";
        } // shared2 destroyed, ref count decrements to 1

        cout << "  - Post inner scope Reference Count = " << shared1.use_count() << "\n";
        cout << "  - Leaving outer shared scope...\n";
    } // Ref count drops to 0 -> Heap object destroyed HERE automatically!

    cout << "  - Active Heap Objects Post-Shared Scope: " << HeapNode::activeHeapObjects << "\n";

    // =====================================================================================
    // 4. CUSTOM DELETERS FOR NON-STANDARD HEAP ALLOCATIONS
    // =====================================================================================
    cout << "\n================ 4. CUSTOM DELETERS FOR C-STYLE HEAP MEMORY ================\n";

    {
        cout << "  - Allocating legacy byte buffer via `std::malloc`...\n";
        size_t byteSize = userElementCount * sizeof(int);
        
        // Wrapping malloc allocation inside std::unique_ptr with CFreeDeleter
        std::unique_ptr<int, CFreeDeleter> rawCBuffer(
            static_cast<int*>(std::malloc(byteSize)), 
            CFreeDeleter{}
        );

        if (rawCBuffer) {
            cout << "    Successfully allocated " << byteSize << " bytes at " 
                 << static_cast<const void*>(rawCBuffer.get()) << "\n";
            rawCBuffer.get()[0] = 999;
            cout << "    Written test value to C-buffer: " << rawCBuffer.get()[0] << "\n";
        }

        cout << "  - Exiting scope (Smart pointer will invoke `CFreeDeleter` automatically)...\n";
    } // std::free called automatically HERE!

    // =====================================================================================
    // 5. EXCEPTION SAFETY & HEAP MEMORY UNWINDING
    // =====================================================================================
    cout << "\n================ 5. EXCEPTION SAFETY & HEAP MEMORY UNWINDING ================\n";

    try {
        cout << "  - Entering try block allocating heap memory via RAII...\n";
        auto exceptionSafeNode = std::make_unique<HeapNode>(303, "ExceptionSafeHeapNode");
        exceptionSafeNode->execute();

        cout << "  - Throwing runtime exception before explicit deletion...\n";
        throw runtime_error("Simulated execution failure inside heap processing pipeline!");

        // Unreachable code
    } catch (const exception& e) {
        cout << "  - [CATCH BLOCK EXECUTED] Caught Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Heap Objects Post-Exception: " << HeapNode::activeHeapObjects 
         << " (RAII GUARANTEE: Zero memory leaks during stack unwinding!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MEMORY MANAGEMENT IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Memory Allocation Type| Modern C++ RAII Management Type   | Deallocation & Release Mechanics  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Single Heap Object    | `std::unique_ptr<T>`              | Automatically calls `delete ptr`  |\n"
         << "| Dynamic Heap Array    | `std::unique_ptr<T[]>` / Scoped   | Automatically calls `delete[] ptr`|\n"
         << "| Shared Heap Resource  | `std::shared_ptr<T>`              | Ref-counted; freed when count == 0|\n"
         << "| Custom C Allocation   | `unique_ptr<T, CustomDeleter>`    | Custom callable (`free`, custom)  |\n"
         << "| Standard Container    | `std::vector<T>` / `std::string`  | Handles heap buffer resizing/free |\n"
         << "| Exception Unwinding   | All RAII Memory Wrappers          | Guarantees leak-free stack unwind |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}