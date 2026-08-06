/*
 * =====================================================================================
 * CONCEPT        : Performance Considerations in Arrays and Strings (Modern C++)
 * DESCRIPTION    : Production-grade implementation benchmarking key performance 
 *                  optimizations for arrays and strings:
 *
 *                  1. Vector `.reserve()` vs Unreserved Reallocations:
 *                     - Eliminates exponential heap reallocations and O(N) element copies.
 *                  2. `std::string_view` / `const std::string&` vs Value Copying:
 *                     - Prevents temporary heap allocations during function calls.
 *                  3. Small String Optimization (SSO):
 *                     - Inspecting stack buffer thresholds (typically <= 15-23 chars) 
 *                       that bypass heap allocation entirely.
 *                  4. String Concatenation (`+=` / `.reserve()`) vs Repeated `+`:
 *                     - Prevents O(N^2) temporary string construction during string building.
 *                  5. Move Semantics (`std::move`):
 *                     - O(1) pointer swap vs O(N) deep memory copy for heap containers.
 *                  6. Cache Locality & Memory Layout:
 *                     - Sequential contiguous memory traversal vs non-contiguous access.
 *
 * TIME COMPLEXITY  : Access: O(1) | Reserved Push: Amortized O(1) | Move: O(1)
 * SPACE COMPLEXITY : Modern containers minimize allocation overhead and preserve cache line density.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <chrono>
#include <iomanip>
#include <utility>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// BENCHMARKING TIMER UTILITY
// Simple RAII-style timer to measure execution time in microseconds.
// =====================================================================================
class PrecisionTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime_;

public:
    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }

    [[nodiscard]] double stopMicroseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::micro>(endTime - startTime_).count();
    }
};

// Prevent compiler from completely optimizing away benchmarks
volatile size_t g_optimizationBarrier = 0;

// =====================================================================================
// 1. PARAMETER PASSING: PASS-BY-VALUE VS PASS-BY-VIEW / CONST REF
// =====================================================================================

void processByValue(std::string str) {
    g_optimizationBarrier += str.size();
}

void processByConstRef(const std::string& str) {
    g_optimizationBarrier += str.size();
}

void processByView(std::string_view sv) {
    g_optimizationBarrier += sv.size();
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t iterationsInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter operation scale for performance benchmarking (e.g., 50000): " << flush;
    if (!(cin >> iterationsInput) || iterationsInput == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting operation scale to 50,000 iterations." << endl;
        iterationsInput = 50000;
    }

    PrecisionTimer timer;

    // =====================================================================================
    // DEMO 1: VECTOR ALLOCATION OVERHEAD — UNRESERVED VS `.reserve()`
    // =====================================================================================
    cout << "\n================ 1. VECTOR ALLOCATION: UNRESERVED VS RESERVE ================\n";

    // Scenario A: Without reserve() -> Repeated heap reallocations
    timer.start();
    std::vector<int> unreservedVec;
    size_t reallocationCount = 0;
    size_t lastCapacity = 0;

    for (size_t i = 0; i < iterationsInput; ++i) {
        if (unreservedVec.capacity() != lastCapacity) {
            lastCapacity = unreservedVec.capacity();
            ++reallocationCount;
        }
        unreservedVec.push_back(static_cast<int>(i));
    }
    double timeUnreserved = timer.stopMicroseconds();

    // Scenario B: With reserve() -> Zero reallocations
    timer.start();
    std::vector<int> reservedVec;
    reservedVec.reserve(iterationsInput); // Pre-allocates target capacity
    size_t reservedReallocations = 0;
    size_t lastReservedCap = 0;

    for (size_t i = 0; i < iterationsInput; ++i) {
        if (reservedVec.capacity() != lastReservedCap) {
            lastReservedCap = reservedVec.capacity();
            ++reservedReallocations;
        }
        reservedVec.push_back(static_cast<int>(i));
    }
    double timeReserved = timer.stopMicroseconds();

    cout << "  - Unreserved push_back (" << iterationsInput << " items): " 
         << fixed << setprecision(2) << timeUnreserved << " us | Reallocations: " << reallocationCount << "\n";
    cout << "  - Reserved push_back   (" << iterationsInput << " items): " 
         << fixed << setprecision(2) << timeReserved << " us | Reallocations: " << reservedReallocations << "\n";
    cout << "  - Speedup Factor       : " << (timeUnreserved / timeReserved) << "x faster with .reserve()\n";

    // =====================================================================================
    // DEMO 2: PARAMETER PASSING OVERHEAD — VALUE COPY VS STRING_VIEW
    // =====================================================================================
    cout << "\n================ 2. PARAMETER PASSING: COPY VS STRING_VIEW ================\n";

    std::string largeText(256, 'X'); // Large string exceeding SSO threshold

    // Pass-by-Value (Forces deep heap copying)
    timer.start();
    for (size_t i = 0; i < iterationsInput; ++i) {
        processByValue(largeText);
    }
    double timeByValue = timer.stopMicroseconds();

    // Pass-by-Const-Reference (No copying)
    timer.start();
    for (size_t i = 0; i < iterationsInput; ++i) {
        processByConstRef(largeText);
    }
    double timeByRef = timer.stopMicroseconds();

    // Pass-by-string_view (No copying, zero-overhead pointer view)
    timer.start();
    for (size_t i = 0; i < iterationsInput; ++i) {
        processByView(largeText);
    }
    double timeByView = timer.stopMicroseconds();

    cout << "  - Pass by Value (O(N) heap copy)   : " << timeByValue << " us\n";
    cout << "  - Pass by const std::string&       : " << timeByRef << " us\n";
    cout << "  - Pass by std::string_view         : " << timeByView << " us\n";
    cout << "  - Speedup Factor (View vs Value)  : " << (timeByValue / timeByView) << "x faster\n";

    // =====================================================================================
    // DEMO 3: SMALL STRING OPTIMIZATION (SSO) THRESHOLD INSPECTION
    // =====================================================================================
    cout << "\n================ 3. SMALL STRING OPTIMIZATION (SSO) THRESHOLD ================\n";

    std::string ssoStringShort = "123456789012345"; // 15 characters
    std::string ssoStringLong  = "1234567890123456789012345678901234567890"; // 40 characters

    cout << "  - Short String (15 chars) capacity : " << ssoStringShort.capacity() 
         << " | SSO Stack Buffer Active\n";
    cout << "  - Long String  (40 chars) capacity : " << ssoStringLong.capacity() 
         << " | Dynamic Heap Buffer Allocated\n";
    cout << "  - [PERFORMANCE LESSON]: Small strings (<= 15-23 chars) bypass heap allocation completely!\n";

    // =====================================================================================
    // DEMO 4: STRING CONCATENATION EFFICIENCY (`+=` VS REPEATED `+`)
    // =====================================================================================
    cout << "\n================ 4. STRING CONCATENATION PERFORMANCE ================\n";

    constexpr size_t CONCAT_LOOPS = 5000;

    // BAD: Repeated operator + creates temporary string copies at every step -> O(N^2) time
    timer.start();
    std::string badConcatString = "";
    for (size_t i = 0; i < CONCAT_LOOPS; ++i) {
        badConcatString = badConcatString + "A";
    }
    double timeBadConcat = timer.stopMicroseconds();

    // GOOD: In-place append via += with .reserve() -> Amortized O(N) time
    timer.start();
    std::string goodConcatString = "";
    goodConcatString.reserve(CONCAT_LOOPS);
    for (size_t i = 0; i < CONCAT_LOOPS; ++i) {
        goodConcatString += "A";
    }
    double timeGoodConcat = timer.stopMicroseconds();

    cout << "  - Bad Concat (`s = s + \"A\"`)       : " << timeBadConcat << " us (O(N^2) temporary copies)\n";
    cout << "  - Good Concat (`s += \"A\"` + reserve): " << timeGoodConcat << " us (Amortized O(N) in-place)\n";
    cout << "  - Speedup Factor                   : " << (timeBadConcat / timeGoodConcat) << "x faster\n";

    // =====================================================================================
    // DEMO 5: MOVE SEMANTICS VS DEEP COPYING
    // =====================================================================================
    cout << "\n================ 5. MOVE SEMANTICS (`std::move`) ================\n";

    std::vector<int> sourceVector(1000000, 42); // 1 Million elements (~4MB)

    // Deep Copying
    timer.start();
    std::vector<int> copiedVector = sourceVector;
    double timeCopy = timer.stopMicroseconds();

    // Move Semantics (O(1) pointer swap)
    timer.start();
    std::vector<int> movedVector = std::move(sourceVector);
    double timeMove = timer.stopMicroseconds();

    cout << "  - Deep Copy Vector (1M elements)  : " << timeCopy << " us\n";
    cout << "  - Move Vector (`std::move`)       : " << timeMove << " us (O(1) pointer transfer)\n";
    cout << "  - Source Size post-move           : " << sourceVector.size() << " elements (Moved-from state)\n";
    cout << "  - Speedup Factor                  : " << (timeCopy / timeMove) << "x faster\n";

    // Prevent compiler optimizations
    g_optimizationBarrier += unreservedVec.size() + reservedVec.size() + 
                             copiedVector.size() + movedVector.size() + 
                             badConcatString.size() + goodConcatString.size();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ PERFORMANCE CONSIDERATIONS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Optimization Strategy | Naive Approach (AVOID)            | Performant Pattern (PREFER)       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Dynamic Allocations   | Unbounded `push_back()` / `+=`    | Pre-allocate via `.reserve(N)`    |\n"
         << "| Read-Only String Param| `void f(std::string s)` (Copy)    | `void f(std::string_view sv)`     |\n"
         << "| Container Transfers   | Pass/Return by value deep copy    | Use Move Semantics (`std::move`)  |\n"
         << "| String Building       | Repeated `s = s + chunk`          | `s += chunk` or `std::ostringstream`|\n"
         << "| Cache Locality        | Linked structures / Pointer arrays| Contiguous `std::vector`/`array`  |\n"
         << "| Fixed-Size Stack Buf  | Dynamic heap allocations          | `std::array<T, N>` or SSO strings |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}