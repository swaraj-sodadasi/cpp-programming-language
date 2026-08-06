/*
 * =====================================================================================
 * CONCEPT        : Performance Considerations in C++ Functions
 * DESCRIPTION    : Comprehensive implementation showcasing core C++ function performance drivers:
 *                  1. Parameter Passing Semantics : Cost of copy (`T`) vs zero-copy (`const T&`) 
 *                                                   vs move semantics (`T&&`).
 *                  2. Function Inlining Overhead  : Eliminating stack frame allocation, register 
 *                                                   saving, and call branching overhead.
 *                  3. Return Value Optimization   : RVO / NRVO eliminating temporaries and copy/move 
 *                                                   construction during return.
 *                  4. Virtual Dispatch vs Static  : Dynamic vtable pointer chasing overhead vs 
 *                                                   direct compile-time static dispatch.
 *                  5. Branch Prediction Layout    : Utilizing `[[likely]]` and `[[unlikely]]` to 
 *                                                   optimize CPU instruction prefetching.
 *                  6. Benchmarking Call Costs     : Real-time benchmarking using `std::chrono`.
 *
 * TIME COMPLEXITY  : Best Case: O(1) for inlined, zero-copy, and RVO functions.
 * SPACE COMPLEXITY : Best Case: O(1) stack frame footprint with minimal allocation overhead.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <utility>
#include <iomanip>
#include <cstdint>

// Feature detection macro for C++20 branch likelihood attributes
#if defined(__has_cpp_attribute) && __has_cpp_attribute(likely)
    #define LIKELY [[likely]]
    #define UNLIKELY [[unlikely]]
#else
    #define LIKELY
    #define UNLIKELY
#endif

using namespace std;

// =====================================================================================
// RESOURCE CLASS FOR TRACKING COPY AND MOVE ALLOCATION COSTS
// =====================================================================================
class MemoryBuffer {
private:
    vector<int> data_;

public:
    explicit MemoryBuffer(size_t size) : data_(size, 42) {}

    // Copy Constructor (Expensive Deep Copy)
    MemoryBuffer(const MemoryBuffer& other) : data_(other.data_) {}

    // Move Constructor (Cheap Pointer Transfer - O(1))
    MemoryBuffer(MemoryBuffer&& other) noexcept : data_(std::move(other.data_)) {}

    [[nodiscard]] size_t size() const { return data_.size(); }
    [[nodiscard]] const int* rawData() const { return data_.data(); }
};

// =====================================================================================
// 1. PARAMETER PASSING COST COMPARISON
// =====================================================================================

// Slow: Triggers full vector deep copy on every invocation
void passByValue(MemoryBuffer buffer) {
    volatile size_t sz = buffer.size();
    (void)sz;
}

// Fast: Zero-copy read-only access via reference alias
void passByConstRef(const MemoryBuffer& buffer) {
    volatile size_t sz = buffer.size();
    (void)sz;
}

// Fast: Zero-copy ownership transfer via move semantics
void passByMove(MemoryBuffer&& buffer) {
    MemoryBuffer internalStorage = std::move(buffer);
    volatile size_t sz = internalStorage.size();
    (void)sz;
}

// =====================================================================================
// 2. INLINING vs STANDARD FUNCTION CALL OVERHEAD
// =====================================================================================

// Forced non-inlined function (Simulates stack frame setup overhead)
#if defined(_MSC_VER)
    __declspec(noinline) uint64_t computeNonInlined(uint64_t a, uint64_t b) { return a + b; }
#elif defined(__GNUC__) || defined(__clang__)
    __attribute__((noinline)) uint64_t computeNonInlined(uint64_t a, uint64_t b) { return a + b; }
#else
    uint64_t computeNonInlined(uint64_t a, uint64_t b) { return a + b; }
#endif

// Inlined function (Direct instruction substitution at call site)
inline uint64_t computeInlined(uint64_t a, uint64_t b) {
    return a + b;
}

// =====================================================================================
// 3. VIRTUAL DISPATCH COST vs STATIC DISPATCH
// =====================================================================================

class BaseProcessor {
public:
    virtual ~BaseProcessor() = default;
    virtual uint64_t process(uint64_t input) const = 0; // Virtual lookup via vtable
};

class DerivedProcessor final : public BaseProcessor {
public:
    uint64_t process(uint64_t input) const override {
        return input * 2 + 1;
    }
};

// Direct non-virtual static function call
struct DirectProcessor {
    uint64_t process(uint64_t input) const {
        return input * 2 + 1;
    }
};

// =====================================================================================
// 4. BRANCH PREDICTION LAYOUT OPTIMIZATION
// =====================================================================================

uint64_t processWithBranchHints(uint64_t input, bool rareCondition) {
    if (rareCondition) UNLIKELY {
        // Cold path: Placed far away in instruction cache to optimize hot path prefetching
        return input * 999;
    } else LIKELY {
        // Hot path: Kept in linear CPU instruction stream
        return input + 1;
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int iterationsInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter benchmark iteration count in thousands (e.g., 1000 for 1M iterations): " << flush;
    if (!(cin >> iterationsInput) || iterationsInput <= 0) {
        cout << "Invalid input. Defaulting to 1000 (1,000,000 iterations)." << endl;
        iterationsInput = 1000;
    }

    const size_t totalIterations = static_cast<size_t>(iterationsInput) * 1000;
    cout << "Running performance benchmarks over " << totalIterations << " iterations...\n";

    // ---------------------------------------------------------------------------------
    // BENCHMARK 1: PASS-BY-VALUE VS PASS-BY-CONST-REF
    // ---------------------------------------------------------------------------------
    cout << "\n================ 1. PARAMETER PASSING OVERHEAD ================" << endl;
    MemoryBuffer largeBuffer(5000); // 5000 ints vector buffer

    // Pass-by-Const-Ref Timing
    auto start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < totalIterations; ++i) {
        passByConstRef(largeBuffer);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> constRefDuration = end - start;
    cout << "  - Pass-by-Const-Ref (" << totalIterations << " calls) : " 
         << fixed << setprecision(3) << constRefDuration.count() << " ms (Zero-Copy)" << endl;

    // Small iterations for pass-by-value to prevent prolonged lockups
    const size_t smallValueIterations = min(totalIterations / 100, static_cast<size_t>(5000));
    start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < smallValueIterations; ++i) {
        passByValue(largeBuffer);
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> valueDuration = end - start;
    cout << "  - Pass-by-Value     (" << smallValueIterations << " calls) : " 
         << fixed << setprecision(3) << valueDuration.count() << " ms (Expensive Deep Copies!)" << endl;

    // ---------------------------------------------------------------------------------
    // BENCHMARK 2: INLINED VS NON-INLINED FUNCTION CALL OVERHEAD
    // ---------------------------------------------------------------------------------
    cout << "\n================ 2. INLINING & CALL STACK OVERHEAD ================" << endl;
    
    uint64_t accumInlined = 0;
    start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < totalIterations; ++i) {
        accumInlined += computeInlined(i, 5);
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> inlinedDuration = end - start;
    cout << "  - Inlined Function Call     : " << fixed << setprecision(3) 
         << inlinedDuration.count() << " ms (Sum: " << accumInlined << ")" << endl;

    uint64_t accumNonInlined = 0;
    start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < totalIterations; ++i) {
        accumNonInlined += computeNonInlined(i, 5);
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> nonInlinedDuration = end - start;
    cout << "  - Non-Inlined Function Call : " << fixed << setprecision(3) 
         << nonInlinedDuration.count() << " ms (Sum: " << accumNonInlined << ")" << endl;

    // ---------------------------------------------------------------------------------
    // BENCHMARK 3: VIRTUAL FUNCTION DISPATCH VS DIRECT STATIC DISPATCH
    // ---------------------------------------------------------------------------------
    cout << "\n================ 3. VIRTUAL DISPATCH VS STATIC DISPATCH ================" << endl;
    
    DerivedProcessor derivedObj;
    BaseProcessor* virtualPtr = &derivedObj;
    
    uint64_t accumVirtual = 0;
    start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < totalIterations; ++i) {
        accumVirtual += virtualPtr->process(i);
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> virtualDuration = end - start;
    cout << "  - Virtual Function Call (vtable indirection) : " 
         << fixed << setprecision(3) << virtualDuration.count() << " ms" << endl;

    DirectProcessor directObj;
    uint64_t accumDirect = 0;
    start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < totalIterations; ++i) {
        accumDirect += directObj.process(i);
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> directDuration = end - start;
    cout << "  - Direct Static Dispatch (Inlinable call)     : " 
         << fixed << setprecision(3) << directDuration.count() << " ms" << endl;

    // ---------------------------------------------------------------------------------
    // SUMMARY OF BEST PRACTICES
    // ---------------------------------------------------------------------------------
    cout << "\n================ FUNCTION PERFORMANCE BEST PRACTICES ================" << endl;
    cout << "1. Parameter Choice : Pass primitives by value, non-scalar objects by `const T&`, and temporaries by `T&&`." << endl;
    cout << "2. Inlining Impact  : Mark small, frequently called helper functions `inline` to remove call frame setup." << endl;
    cout << "3. Virtual Overhead : Avoid virtual function dispatch in tight inner loops; consider CRTP or Templates." << endl;
    cout << "4. Return Efficiency: Trust Return Value Optimization (RVO / NRVO) and return objects by value." << endl;
    cout << "5. Branch Hints     : Use C++20 `[[likely]]` / `[[unlikely]]` to guide instruction prefetching for critical paths." << endl;

    return 0;
}