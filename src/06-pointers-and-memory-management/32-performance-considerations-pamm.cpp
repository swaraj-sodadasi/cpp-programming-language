/*
 * =====================================================================================
 * CONCEPT        : Performance Considerations in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the primary hardware and software performance factors in memory management:
 *
 *                  1. Heap Allocation Overhead  : Measuring `new`/`malloc` OS overhead vs 
 *                                                 Pre-allocated Custom Arena (Bump) Allocators.
 *                  2. Cache Locality & Invalidation: Contiguous memory layouts vs pointer-chasing 
 *                                                 indirection (e.g., Arrays vs Linked Lists/Pointer Arrays).
 *                  3. Smart Pointer Cost Analysis: Zero-overhead `std::unique_ptr` vs `std::shared_ptr` 
 *                                                 atomic reference counting overhead (`std::atomic`).
 *                  4. Virtual Dispatch Indirection: Direct function calls vs VTable dynamic dispatch 
 *                                                 pointer lookups.
 *                  5. Smart Pointer Passing    : Pass-by-value `shared_ptr` (atomic ref increments) 
 *                                                 vs Pass-by-const-reference / raw observer pointers.
 *
 * TIME COMPLEXITY  : Arena Allocation: O(1) constant time | Heap Allocation: O(N) kernel overhead.
 *                    Contiguous Access: O(1) per cache line | Pointer Chasing: O(N) cache misses.
 * SPACE COMPLEXITY : Memory Footprint: Directly governed by alignment and pointer header metadata.
 * =====================================================================================
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <iomanip>
#include <limits>
#include <cstddef>

using namespace std;
using namespace std::chrono;

// =====================================================================================
// HELPER 1: CUSTOM ARENA / BUMP ALLOCATOR (ZERO HEAP OVERHEAD)
// Fast pre-allocated contiguous memory pool eliminating OS malloc/free locks.
// =====================================================================================
class MemoryArena {
private:
    char* buffer_;
    size_t capacity_;
    size_t offset_;

public:
    explicit MemoryArena(size_t bytes)
        : buffer_(new char[bytes]), capacity_(bytes), offset_(0) {}

    ~MemoryArena() {
        delete[] buffer_;
    }

    // Disable copies for strict memory pool ownership
    MemoryArena(const MemoryArena&) = delete;
    MemoryArena& operator=(const MemoryArena&) = delete;

    void* allocate(size_t bytes, size_t alignment = alignof(max_align_t)) {
        size_t currentAddr = reinterpret_cast<size_t>(buffer_ + offset_);
        size_t padding = (alignment - (currentAddr % alignment)) % alignment;

        if (offset_ + padding + bytes > capacity_) {
            throw std::bad_alloc();
        }

        offset_ += padding;
        void* ptr = buffer_ + offset_;
        offset_ += bytes;
        return ptr;
    }

    void reset() noexcept {
        offset_ = 0;
    }

    [[nodiscard]] size_t getUsedBytes() const { return offset_; }
};

// Simple object payload for benchmark tests
struct DataNode {
    uint64_t id;
    double value;
    uint64_t payload[2];
};

// Linked List Node for cache miss / pointer chasing demonstration
struct LinkedNode {
    uint64_t value;
    LinkedNode* next;
};

// =====================================================================================
// HELPER 2: VIRTUAL DISPATCH VS DIRECT CALL CLASSES
// =====================================================================================
class AbstractWorker {
public:
    virtual ~AbstractWorker() = default;
    virtual uint64_t compute(uint64_t input) const = 0;
};

class ConcreteWorker : public AbstractWorker {
public:
    uint64_t compute(uint64_t input) const override {
        return input * 3 + 7;
    }
};

struct DirectWorker {
    [[nodiscard]] uint64_t compute(uint64_t input) const {
        return input * 3 + 7;
    }
};

// =====================================================================================
// HELPER 3: SMART POINTER PASSING BENCHMARK FUNCTIONS
// =====================================================================================

// Slow: Pass-by-value forces atomic reference count increment and decrement
void passSharedByValue(std::shared_ptr<DataNode> ptr, uint64_t& accumulator) {
    if (ptr) {
        accumulator += ptr->id;
    }
}

// Fast: Pass-by-const-reference borrows view without atomic reference mutations
void passSharedByConstRef(const std::shared_ptr<DataNode>& ptr, uint64_t& accumulator) {
    if (ptr) {
        accumulator += ptr->id;
    }
}

// Fastest: Pass raw observer pointer (no smart pointer overhead)
void passRawObserver(const DataNode* ptr, uint64_t& accumulator) {
    if (ptr != nullptr) {
        accumulator += ptr->id;
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter element count for performance benchmarking (e.g., 50000): " << flush;
    if (!(cin >> userInputValue) || userInputValue == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting element count to 50000." << endl;
        userInputValue = 50000;
    }

    const size_t COUNT = userInputValue;

    // =====================================================================================
    // 1. ALLOCATION OVERHEAD: HEAP (`new`) VS ARENA ALLOCATOR
    // =====================================================================================
    cout << "\n================ 1. HEAP ALLOCATION VS ARENA BUMP ALLOCATOR ================\n";

    // A. Standard Dynamic Heap Allocation (`new`)
    vector<DataNode*> heapPointers;
    heapPointers.reserve(COUNT);

    auto startHeap = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        heapPointers.push_back(new DataNode{i, static_cast<double>(i), {0, 0}});
    }
    auto endHeap = high_resolution_clock::now();
    auto heapAllocTime = duration_cast<microseconds>(endHeap - startHeap).count();

    cout << "  - Individual `new` Heap Allocations (" << COUNT << " items) : " 
         << setw(8) << heapAllocTime << " us\n";

    // B. Custom Arena Bump Allocation
    MemoryArena arena(COUNT * sizeof(DataNode) + 1024);
    vector<DataNode*> arenaPointers;
    arenaPointers.reserve(COUNT);

    auto startArena = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        void* mem = arena.allocate(sizeof(DataNode));
        arenaPointers.push_back(new (mem) DataNode{i, static_cast<double>(i), {0, 0}});
    }
    auto endArena = high_resolution_clock::now();
    auto arenaAllocTime = duration_cast<microseconds>(endArena - startArena).count();

    cout << "  - Custom Arena Pool Allocations (" << COUNT << " items)     : " 
         << setw(8) << arenaAllocTime << " us\n";
    cout << "  - [SPEEDUP]: Arena Allocation is ~" << fixed << setprecision(1) 
         << (static_cast<double>(heapAllocTime) / (arenaAllocTime > 0 ? arenaAllocTime : 1)) 
         << "x faster than individual dynamic heap calls!\n";

    // Heap Cleanup
    for (auto* ptr : heapPointers) {
        delete ptr;
    }

    // =====================================================================================
    // 2. CACHE LOCALITY: CONTIGUOUS VECTOR VS POINTER-CHASING INDIRECTION
    // =====================================================================================
    cout << "\n================ 2. CACHE LOCALITY VS POINTER-CHASING ================\n";

    // A. Contiguous Memory Layout (`std::vector<DataNode>`)
    vector<DataNode> contiguousBuffer;
    contiguousBuffer.reserve(COUNT);
    for (size_t i = 0; i < COUNT; ++i) {
        contiguousBuffer.push_back({i, static_cast<double>(i), {0, 0}});
    }

    uint64_t contiguousSum = 0;
    auto startContiguous = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        contiguousSum += contiguousBuffer[i].id; // Hardware prefetcher friendly!
    }
    auto endContiguous = high_resolution_clock::now();
    auto contiguousTime = duration_cast<microseconds>(endContiguous - startContiguous).count();

    cout << "  - Contiguous Array Traversal (`vector<T>`) Sum: " << contiguousSum 
         << " | Time: " << setw(8) << contiguousTime << " us\n";

    // B. Dispersed Pointer-Chased Nodes (Linked List simulation)
    vector<LinkedNode*> scatteredNodes;
    scatteredNodes.reserve(COUNT);
    for (size_t i = 0; i < COUNT; ++i) {
        scatteredNodes.push_back(new LinkedNode{i, nullptr});
    }
    // Link nodes sequentially
    for (size_t i = 0; i < COUNT - 1; ++i) {
        scatteredNodes[i]->next = scatteredNodes[i + 1];
    }

    uint64_t chasedSum = 0;
    LinkedNode* current = scatteredNodes[0];
    auto startChased = high_resolution_clock::now();
    while (current != nullptr) {
        chasedSum += current->value; // Pointer chasing induces potential CPU cache misses
        current = current->next;
    }
    auto endChased = high_resolution_clock::now();
    auto chasedTime = duration_cast<microseconds>(endChased - startChased).count();

    cout << "  - Pointer-Chased Traversal (`LinkedNode*`)   Sum: " << chasedSum 
         << " | Time: " << setw(8) << chasedTime << " us\n";
    cout << "  - [CACHE PERFORMANCE]: Contiguous memory layout traversed ~" 
         << (static_cast<double>(chasedTime) / (contiguousTime > 0 ? contiguousTime : 1)) 
         << "x faster due to CPU cache-line prefetching!\n";

    // Cleanup Linked Nodes
    for (auto* node : scatteredNodes) {
        delete node;
    }

    // =====================================================================================
    // 3. SMART POINTER PASSING: PASS-BY-VALUE VS PASS-BY-CONST-REF VS RAW OBSERVER
    // =====================================================================================
    cout << "\n================ 3. SMART POINTER PASSING OVERHEAD ================\n";

    auto sharedObj = std::make_shared<DataNode>(DataNode{999, 1.23, {0, 0}});
    uint64_t benchmarkAccumulator = 0;

    // A. Pass Shared Pointer By Value (Forces atomic ref count increment/decrement)
    auto startPassValue = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        passSharedByValue(sharedObj, benchmarkAccumulator);
    }
    auto endPassValue = high_resolution_clock::now();
    auto passValueTime = duration_cast<microseconds>(endPassValue - startPassValue).count();

    cout << "  - Pass `shared_ptr` By Value (Atomic Ref Count)   : " 
         << setw(8) << passValueTime << " us\n";

    // B. Pass Shared Pointer By Const Reference (Zero atomic operations)
    benchmarkAccumulator = 0;
    auto startPassRef = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        passSharedByConstRef(sharedObj, benchmarkAccumulator);
    }
    auto endPassRef = high_resolution_clock::now();
    auto passRefTime = duration_cast<microseconds>(endPassRef - startPassRef).count();

    cout << "  - Pass `shared_ptr` By Const Reference (`const T&`): " 
         << setw(8) << passRefTime << " us\n";

    // C. Pass Raw Observer Pointer
    benchmarkAccumulator = 0;
    const DataNode* rawObs = sharedObj.get();
    auto startPassRaw = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        passRawObserver(rawObs, benchmarkAccumulator);
    }
    auto endPassRaw = high_resolution_clock::now();
    auto passRawTime = duration_cast<microseconds>(endPassRaw - startPassRaw).count();

    cout << "  - Pass Raw Observer Pointer (`const T*`)          : " 
         << setw(8) << passRawTime << " us\n";

    cout << "  - [ATOMIC OVERHEAD]: Passing by `const shared_ptr&` or raw observer is ~" 
         << (static_cast<double>(passValueTime) / (passRefTime > 0 ? passRefTime : 1)) 
         << "x faster than pass-by-value!\n";

    // =====================================================================================
    // 4. FUNCTION DISPATCH: VIRTUAL DYNAMIC DISPATCH VS DIRECT CALLS
    // =====================================================================================
    cout << "\n================ 4. VIRTUAL DISPATCH VS DIRECT CALL PERFORMANCE ================\n";

    ConcreteWorker dynamicWorker;
    const AbstractWorker* basePtr = &dynamicWorker;
    DirectWorker staticWorker;

    uint64_t vtableResult = 0;
    auto startVirtual = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        vtableResult += basePtr->compute(i); // Vtable pointer dereference lookup
    }
    auto endVirtual = high_resolution_clock::now();
    auto virtualTime = duration_cast<microseconds>(endVirtual - startVirtual).count();

    uint64_t directResult = 0;
    auto startDirect = high_resolution_clock::now();
    for (size_t i = 0; i < COUNT; ++i) {
        directResult += staticWorker.compute(i); // Directly inlined call
    }
    auto endDirect = high_resolution_clock::now();
    auto directTime = duration_cast<microseconds>(endDirect - startDirect).count();

    cout << "  - Virtual Function Call (VTable Indirection)      : " 
         << setw(8) << virtualTime << " us | Result: " << vtableResult << "\n";
    cout << "  - Direct Function Call (Inlined Static Call)      : " 
         << setw(8) << directTime << " us | Result: " << directResult << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ POINTER & MEMORY PERFORMANCE SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Metric / Paradigm     | Fast / Recommended Approach       | Slow / Overhead Bottleneck        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Dynamic Memory Alloc  | Pre-allocated Memory Arena / Pool | Frequent individual `new`/`malloc`|\n"
         << "| Cache Efficiency      | `std::vector<T>` contiguous array | Linked lists / `vector<T*>` pointers|\n"
         << "| Smart Pointer Copying | Pass `const std::shared_ptr<T>&`  | Pass `std::shared_ptr<T>` by value|\n"
         << "| Function Dispatch     | Static direct calls / templates   | Deep Virtual Function VTable calls|\n"
         << "| Memory Layout         | Struct-of-Arrays (SoA) layout     | Array-of-Structures with padding  |\n"
         << "| Smart Pointer Choice  | `std::unique_ptr<T>` (0 Cost)     | Unnecessary `std::shared_ptr<T>`  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}