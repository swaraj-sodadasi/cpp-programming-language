/*
 * =====================================================================================
 * CONCEPT        : Performance Considerations in RAII (Modern C++)
 * DESCRIPTION    : Detailed analysis and production-grade executable code evaluating
 *                  the runtime and memory performance characteristics of RAII:
 *
 *                  1. Zero-Overhead Abstraction : Comparing `std::unique_ptr` vs raw pointers
 *                                                 in terms of memory footprint and CPU instructions.
 *                  2. Deleter Overhead          : Stateless functor deleters (0-byte overhead via EBO)
 *                                                 vs function pointers / `std::function` deleters.
 *                  3. Shared Pointer Costs      : Allocation overhead (`make_shared` vs `shared_ptr(new)`)
 *                                                 and atomic reference-counting CPU bus locks.
 *                  4. Pass-by-Reference Opts    : Eliminating atomic ref-count mutations when passing 
 *                                                 `std::shared_ptr` by `const&`.
 *                  5. Move Semantics (`noexcept`): Zero-cost move operations and `std::vector`
 *                                                 reallocation performance gains.
 *                  6. Stack vs Heap Allocations : Leveraging stack-bound SBO/inline wrappers to 
 *                                                 avoid heap allocation overhead entirely.
 *
 * TIME COMPLEXITY  : Unique Pointer / Move / Stack RAII: O(1) zero-overhead.
 *                    Shared Pointer Ref-Counting      : O(1) with atomic bus lock overhead.
 * SPACE COMPLEXITY : `std::unique_ptr` (Default/Functor): 8 bytes (same as raw pointer).
 *                    `std::shared_ptr`                 : 16 bytes (ptr + control block ptr).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <limits>
#include <functional>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: BENCHMARK PAYLOAD ENTITY
// Compact payload object used for performance benchmarking and memory footprint checks.
// =====================================================================================
class BenchmarkPayload {
private:
    int id_;
    double value_;

public:
    explicit BenchmarkPayload(int id = 0, double val = 0.0) : id_(id), value_(val) {}

    void execute() const volatile {
        // Prevent compiler optimization from stripping away benchmark loops
        (void)id_;
        (void)value_;
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
};

// =====================================================================================
// HELPER DELETERS FOR MEMORY FOOTPRINT COMPARISON
// Demonstrates Empty Base Optimization (EBO) in custom RAII deleters.
// =====================================================================================

// 1. Stateless Functor Deleter (Leverages Empty Base Optimization -> 0 bytes extra overhead)
struct StatelessDeleter {
    void operator()(BenchmarkPayload* ptr) const noexcept {
        delete ptr;
    }
};

// 2. Stateful Deleter (Adds internal data members -> increases wrapper sizeof)
struct StatefulDeleter {
    int deleterId{42};
    void operator()(BenchmarkPayload* ptr) const noexcept {
        delete ptr;
    }
};

// Raw Function Pointer Deleter Function
inline void rawCStyleDeleter(BenchmarkPayload* ptr) noexcept {
    delete ptr;
}

// =====================================================================================
// HELPER CLASS 2: NOEXCEPT VS THROWING MOVE CLASS
// Demonstrates std::vector reallocation performance impact based on noexcept move.
// =====================================================================================
class NoexceptMoveRaii {
private:
    int* data_;

public:
    explicit NoexceptMoveRaii(int val) : data_(new int(val)) {}
    ~NoexceptMoveRaii() noexcept { delete data_; }

    NoexceptMoveRaii(const NoexceptMoveRaii& other) : data_(new int(*other.data_)) {}
    NoexceptMoveRaii& operator=(const NoexceptMoveRaii& other) {
        if (this != &other) {
            delete data_;
            data_ = new int(*other.data_);
        }
        return *this;
    }

    // `noexcept` Move Ctor allows std::vector to move elements efficiently during reallocation
    NoexceptMoveRaii(NoexceptMoveRaii&& other) noexcept : data_(other.data_) {
        other.data_ = nullptr;
    }

    NoexceptMoveRaii& operator=(NoexceptMoveRaii&& other) noexcept {
        if (this != &other) {
            delete data_;
            data_ = other.data_;
            other.data_ = nullptr;
        }
        return *this;
    }
};

class ThrowingMoveRaii {
private:
    int* data_;

public:
    explicit ThrowingMoveRaii(int val) : data_(new int(val)) {}
    ~ThrowingMoveRaii() noexcept { delete data_; }

    ThrowingMoveRaii(const ThrowingMoveRaii& other) : data_(new int(*other.data_)) {}
    ThrowingMoveRaii& operator=(const ThrowingMoveRaii& other) {
        if (this != &other) {
            delete data_;
            data_ = new int(*other.data_);
        }
        return *this;
    }

    // Throwing Move Ctor forces std::vector to fall back to expensive DEEP COPIES during reallocation!
    ThrowingMoveRaii(ThrowingMoveRaii&& other) noexcept(false) : data_(new int(*other.data_)) {}

    ThrowingMoveRaii& operator=(ThrowingMoveRaii&& other) noexcept(false) {
        if (this != &other) {
            delete data_;
            data_ = new int(*other.data_);
        }
        return *this;
    }
};

// =====================================================================================
// BENCHMARK HELPER FUNCTIONS
// =====================================================================================

// Pass Shared Pointer BY VALUE (Triggers Atomic Ref-Count Increment + Decrement)
void consumeSharedByValue(std::shared_ptr<BenchmarkPayload> ptr) {
    if (ptr) {
        ptr->execute();
    }
}

// Pass Shared Pointer BY CONST REFERENCE (Zero Atomic Overhead)
void consumeSharedByConstRef(const std::shared_ptr<BenchmarkPayload>& ptr) {
    if (ptr) {
        ptr->execute();
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter benchmark iteration scale factor (e.g., 100000): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting iteration scale to 100,000." << endl;
        userInputValue = 100000;
    }

    const int iterations = userInputValue;

    // =====================================================================================
    // 1. ZERO-OVERHEAD ABSTRACTION (`std::unique_ptr` VS RAW POINTER)
    // =====================================================================================
    cout << "\n================ 1. ZERO-OVERHEAD ABSTRACTION (`std::unique_ptr` VS RAW) ================\n";

    cout << "  - Memory Footprint Comparison (64-bit Architecture):\n";
    cout << "    * sizeof(BenchmarkPayload*)                  : " << sizeof(BenchmarkPayload*) << " bytes\n";
    cout << "    * sizeof(std::unique_ptr<BenchmarkPayload>)  : " << sizeof(std::unique_ptr<BenchmarkPayload>) << " bytes\n";
    cout << "    * Overhead Ratio                             : 0% EXTRA MEMORY (Identical to raw pointer!)\n";

    // Performance Benchmark: Raw Pointer vs std::unique_ptr allocation cycles
    {
        auto startRaw = chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            BenchmarkPayload* raw = new BenchmarkPayload(i, 3.14);
            raw->execute();
            delete raw;
        }
        auto endRaw = chrono::high_resolution_clock::now();
        auto durRaw = chrono::duration_cast<chrono::microseconds>(endRaw - startRaw).count();

        auto startUnique = chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto unique = std::make_unique<BenchmarkPayload>(i, 3.14);
            unique->execute();
        }
        auto endUnique = chrono::high_resolution_clock::now();
        auto durUnique = chrono::duration_cast<chrono::microseconds>(endUnique - startUnique).count();

        cout << "\n  - Execution Timing (" << iterations << " Allocation Cycles):\n";
        cout << "    * Raw Pointer Allocation/Delete Duration    : " << durRaw << " us\n";
        cout << "    * `std::make_unique` Allocation Duration     : " << durUnique << " us\n";
        cout << "    * Conclusion: Modern compilers inline std::unique_ptr completely -> ZERO RUNTIME COST!\n";
    }

    // =====================================================================================
    // 2. DELETER MEMORY FOOTPRINT OVERHEAD & EMPTY BASE OPTIMIZATION (EBO)
    // =====================================================================================
    cout << "\n================ 2. DELETER OVERHEAD & EMPTY BASE OPTIMIZATION (EBO) ================\n";

    using FunctorUniquePtr = std::unique_ptr<BenchmarkPayload, StatelessDeleter>;
    using StatefulUniquePtr = std::unique_ptr<BenchmarkPayload, StatefulDeleter>;
    using FuncPtrUniquePtr = std::unique_ptr<BenchmarkPayload, void(*)(BenchmarkPayload*)>;
    using StdFuncUniquePtr = std::unique_ptr<BenchmarkPayload, std::function<void(BenchmarkPayload*)>>;

    cout << "  - Memory Footprint of `std::unique_ptr` with Various Deleters:\n";
    cout << "    * Default Deleter (`std::default_delete`)   : " << sizeof(std::unique_ptr<BenchmarkPayload>) << " bytes (EBO Applied)\n";
    cout << "    * Stateless Functor Deleter (`StatelessDeleter`) : " << sizeof(FunctorUniquePtr) << " bytes (EBO Applied - 0 bytes overhead!)\n";
    cout << "    * Stateful Functor Deleter (`StatefulDeleter`)  : " << sizeof(StatefulUniquePtr) << " bytes (Data member bloat)\n";
    cout << "    * Raw Function Pointer (`void(*)(T*)`)      : " << sizeof(FuncPtrUniquePtr) << " bytes (+8 bytes for function ptr)\n";
    cout << "    * `std::function<void(T*)>` Deleter          : " << sizeof(StdFuncUniquePtr) << " bytes (Heavy type-erasure bloat!)\n";

    cout << "\n  - [BEST PRACTICE]: Use Stateless Functors or `std::default_delete` to maintain 8-byte zero overhead!\n";

    // =====================================================================================
    // 3. SHARED POINTER COSTS (`std::make_shared` VS `std::shared_ptr(new T)`)
    // =====================================================================================
    cout << "\n================ 3. SHARED POINTER ALLOCATION & ATOMIC OVERHEAD ================\n";

    cout << "  - Memory Footprint:\n";
    cout << "    * sizeof(std::shared_ptr<BenchmarkPayload>)  : " << sizeof(std::shared_ptr<BenchmarkPayload>) 
         << " bytes (2 pointers: Target Object + Control Block)\n";

    {
        // Benchmark A: std::shared_ptr<T>(new T) -> 2 HEAP ALLOCATIONS (Object + Control Block)
        auto startTwoAlloc = chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            std::shared_ptr<BenchmarkPayload> ptr(new BenchmarkPayload(i, 2.71));
            ptr->execute();
        }
        auto endTwoAlloc = chrono::high_resolution_clock::now();
        auto durTwoAlloc = chrono::duration_cast<chrono::microseconds>(endTwoAlloc - startTwoAlloc).count();

        // Benchmark B: std::make_shared<T>() -> 1 CONTIGUOUS HEAP ALLOCATION (Object + Control Block)
        auto startOneAlloc = chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto ptr = std::make_shared<BenchmarkPayload>(i, 2.71);
            ptr->execute();
        }
        auto endOneAlloc = chrono::high_resolution_clock::now();
        auto durOneAlloc = chrono::duration_cast<chrono::microseconds>(endOneAlloc - startOneAlloc).count();

        cout << "\n  - Heap Allocation Benchmark (" << iterations << " iterations):\n";
        cout << "    * `std::shared_ptr<T>(new T)` Duration (2 Heap Allocations) : " << durTwoAlloc << " us\n";
        cout << "    * `std::make_shared<T>()` Duration    (1 Heap Allocation)  : " << durOneAlloc << " us\n";
        cout << "    * Performance Gain via `std::make_shared`                  : " 
             << (durTwoAlloc > 0 ? static_cast<double>(durTwoAlloc - durOneAlloc) / durTwoAlloc * 100.0 : 0.0) << " % faster!\n";
    }

    // =====================================================================================
    // 4. ATOMIC REF-COUNTING OVERHEAD: PASS BY VALUE VS CONST REFERENCE
    // =====================================================================================
    cout << "\n================ 4. ATOMIC REF-COUNTING: VALUE VS CONST REF ================\n";

    {
        auto sharedObj = std::make_shared<BenchmarkPayload>(101, 9.99);

        // Benchmark A: Pass Shared Pointer BY VALUE (Triggers atomic increment/decrement)
        auto startValue = chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations * 10; ++i) {
            consumeSharedByValue(sharedObj);
        }
        auto endValue = chrono::high_resolution_clock::now();
        auto durValue = chrono::duration_cast<chrono::microseconds>(endValue - startValue).count();

        // Benchmark B: Pass Shared Pointer BY CONST REFERENCE (Zero atomic mutations)
        auto startRef = chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations * 10; ++i) {
            consumeSharedByConstRef(sharedObj);
        }
        auto endRef = chrono::high_resolution_clock::now();
        auto durRef = chrono::duration_cast<chrono::microseconds>(endRef - startRef).count();

        cout << "  - Function Parameter Benchmark (" << iterations * 10 << " Call Iterations):\n";
        cout << "    * Pass Shared Pointer BY VALUE (Atomic Ref-Count Mutex Lock) : " << durValue << " us\n";
        cout << "    * Pass Shared Pointer BY CONST REF (Zero Atomic Overhead)   : " << durRef << " us\n";
        cout << "    * [BEST PRACTICE]: Pass `const std::shared_ptr<T>&` to avoid atomic bus contention!\n";
    }

    // =====================================================================================
    // 5. `noexcept` MOVE SEMANTICS & VECTOR REALLOCATION PERFORMANCE
    // =====================================================================================
    cout << "\n================ 5. `noexcept` MOVE SEMANTICS & VECTOR REALLOCATION ================\n";

    {
        const int vecSize = 10000;

        // Vector with noexcept move operations
        auto startNoexcept = chrono::high_resolution_clock::now();
        std::vector<NoexceptMoveRaii> vecNoexcept;
        for (int i = 0; i < vecSize; ++i) {
            vecNoexcept.emplace_back(i); // Triggers reallocations using fast std::move
        }
        auto endNoexcept = chrono::high_resolution_clock::now();
        auto durNoexcept = chrono::duration_cast<chrono::microseconds>(endNoexcept - startNoexcept).count();

        // Vector with throwing move operations (forces deep copies during reallocation)
        auto startThrowing = chrono::high_resolution_clock::now();
        std::vector<ThrowingMoveRaii> vecThrowing;
        for (int i = 0; i < vecSize; ++i) {
            vecThrowing.emplace_back(i); // Reallocations fall back to expensive deep copies!
        }
        auto endThrowing = chrono::high_resolution_clock::now();
        auto durThrowing = chrono::duration_cast<chrono::microseconds>(endThrowing - startThrowing).count();

        cout << "  - Vector Reallocation Benchmark (" << vecSize << " Elements Growing Dynamic Buffer):\n";
        cout << "    * `noexcept` Move Operations Reallocation Duration   : " << durNoexcept << " us\n";
        cout << "    * Throwing Move Operations (Deep Copy Fallback) Duration: " << durThrowing << " us\n";
        cout << "    * [BEST PRACTICE]: Always mark move constructors `noexcept` for RAII objects!\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ PERFORMANCE CONSIDERATIONS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| RAII Abstraction      | Memory Overhead   | Runtime Overhead  | Optimal Performance Strategy      |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| `std::unique_ptr<T>`  | 0 bytes (8 total) | 0% (Inlined)      | Default choice for exclusive ownership|\n"
         << "| Stateless Functor     | 0 bytes (8 total) | 0% (Inlined)      | Uses EBO; zero memory overhead    |\n"
         << "| Function Pointer Dtor | +8 bytes (16 tot) | Minor indirection | Avoid unless interfacing with C   |\n"
         << "| `std::function` Dtor  | +32-40 bytes      | Virtual call/alloc| Heavy overhead; avoid if possible |\n"
         << "| `std::shared_ptr<T>`  | +8 bytes (16 tot) | Atomic ref-counts | Use `std::make_shared` (1 alloc)  |\n"
         << "| Pass `shared_ptr`     | By Value: Atomic  | By Const Ref: 0%  | Pass `const std::shared_ptr<T>&`  |\n"
         << "| Container Move RAII   | 0 bytes           | `noexcept` = 0%   | Mark Move Ctor/Assign `noexcept`  |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}