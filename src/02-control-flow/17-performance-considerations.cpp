/*
 * =====================================================================================
 * CONCEPT        : Performance Considerations in C++ Control Flow
 * DESCRIPTION    : Comprehensive implementation demonstrating CPU pipeline efficiency, 
 *                  branch prediction optimizations, and branchless programming patterns:
 *                  1. Branch Prediction Impact : Predictable vs unpredictable branch behavior 
 *                                                on CPU hardware instruction pipelines.
 *                  2. Branchless Programming   : Replacing conditional branches with bitwise/
 *                                                arithmetic expressions (conditional moves / CMOV).
 *                  3. Direct Lookup Tables     : O(1) array indexing vs sequential if-else / switch
 *                                                branching chains.
 *                  4. Zero-Cost Compile-Time   : Eliminating runtime branch checks using 'if constexpr'.
 *                  5. Loop Unrolling           : Reducing loop jump instruction overhead and 
 *                                                branching frequency.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear traversal over dynamic benchmark datasets.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal cache-friendly stack frame and array allocations.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <array>
#include <iomanip>
#include <cstdint>

using namespace std;

/**
 * @brief Section 1: Demonstrates Branch Prediction Impact (Sorted vs Unsorted Data).
 * CPUs attempt to predict branch directions (`if` outcomes) using History Tables.
 * Unpredictable branches trigger pipeline flushes (10–20 cycle misprediction penalties).
 */
void demonstrateBranchPrediction(size_t dataSize, int threshold) {
    cout << "\n================ 1. BRANCH PREDICTION IMPACT ================" << endl;

    vector<int> data(dataSize);
    mt19937 rng(42); // Fixed seed for reproducible benchmarks
    uniform_int_distribution<int> dist(0, 100);

    for (size_t i = 0; i < dataSize; ++i) {
        data[i] = dist(rng);
    }

    // Benchmark A: Unsorted Data (Unpredictable Branching)
    auto startUnsorted = chrono::high_resolution_clock::now();
    uint64_t sumUnsorted = 0;
    for (size_t i = 0; i < dataSize; ++i) {
        if (data[i] >= threshold) { // Unpredictable 50% branch outcome
            sumUnsorted += static_cast<uint64_t>(data[i]);
        }
    }
    auto endUnsorted = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsedUnsorted = endUnsorted - startUnsorted;

    // Benchmark B: Sorted Data (Highly Predictable Branching)
    sort(data.begin(), data.end());

    auto startSorted = chrono::high_resolution_clock::now();
    uint64_t sumSorted = 0;
    for (size_t i = 0; i < dataSize; ++i) {
        if (data[i] >= threshold) { // Predictable: false, false... then true, true...
            sumSorted += static_cast<uint64_t>(data[i]);
        }
    }
    auto endSorted = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsedSorted = endSorted - startSorted;

    cout << "Dataset Size                   : " << dataSize << " elements" << endl;
    cout << "Unsorted Data (Branch Mispredicts) : " << fixed << setprecision(3) 
         << elapsedUnsorted.count() << " ms (Sum: " << sumUnsorted << ")" << endl;
    cout << "Sorted Data   (Branch Predictable)  : " << fixed << setprecision(3) 
         << elapsedSorted.count() << " ms (Sum: " << sumSorted << ")" << endl;
    cout << "  |- Insight: Predictable branches allow CPU pipelines to execute without stalls." << endl;
}

/**
 * @brief Section 2: Demonstrates Branchless Execution.
 * Eliminates `if` statements using arithmetic mask expressions, enabling compiler
 * generation of `cmov` (conditional move) instructions without pipeline branches.
 */
void demonstrateBranchlessExecution(size_t dataSize, int threshold) {
    cout << "\n================ 2. BRANCHLESS PROGRAMMING PATTERNS ================" << endl;

    vector<int> data(dataSize);
    mt19937 rng(123);
    uniform_int_distribution<int> dist(0, 100);

    for (size_t i = 0; i < dataSize; ++i) {
        data[i] = dist(rng);
    }

    // Standard Branched Approach
    auto startBranched = chrono::high_resolution_clock::now();
    uint64_t sumBranched = 0;
    for (size_t i = 0; i < dataSize; ++i) {
        if (data[i] >= threshold) {
            sumBranched += static_cast<uint64_t>(data[i]);
        }
    }
    auto endBranched = chrono::high_resolution_clock::now();

    // Branchless Arithmetic Approach
    auto startBranchless = chrono::high_resolution_clock::now();
    uint64_t sumBranchless = 0;
    for (size_t i = 0; i < dataSize; ++i) {
        // (data[i] >= threshold) evaluates to 1 (true) or 0 (false)
        // Multiplies element by 1 or 0, eliminating conditional branching completely
        sumBranchless += static_cast<uint64_t>(data[i]) * static_cast<uint64_t>(data[i] >= threshold);
    }
    auto endBranchless = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> elapsedBranched = endBranched - startBranched;
    chrono::duration<double, milli> elapsedBranchless = endBranchless - startBranchless;

    cout << "Branched Execution   : " << elapsedBranched.count() << " ms (Sum: " << sumBranched << ")" << endl;
    cout << "Branchless Execution : " << elapsedBranchless.count() << " ms (Sum: " << sumBranchless << ")" << endl;
    cout << "  |- Insight: Branchless logic converts jumps into deterministic arithmetic/cmov operations." << endl;
}

/**
 * @brief Section 3: Demonstrates Direct Lookup Table vs Conditional Switch.
 * Direct array lookup guarantees O(1) time without branch instruction evaluation.
 */
int lookupTaxRateBranchless(int categoryCode) {
    // Direct array access indexing (O(1) memory lookup with zero conditional branches)
    static constexpr std::array<int, 4> taxRates = {0, 10, 20, 30};
    if (categoryCode >= 0 && categoryCode < 4) {
        return taxRates[static_cast<size_t>(categoryCode)];
    }
    return 0; // Default fallback
}

int lookupTaxRateSwitch(int categoryCode) {
    switch (categoryCode) {
        case 0: return 0;
        case 1: return 10;
        case 2: return 20;
        case 3: return 30;
        default: return 0;
    }
}

void demonstrateLookupTableVsSwitch() {
    cout << "\n================ 3. DIRECT LOOKUP TABLE vs SWITCH ==================" << endl;
    int testCategory = 2;

    cout << "Lookup Table Result (Category " << testCategory << ") : " 
         << lookupTaxRateBranchless(testCategory) << "% tax rate" << endl;
    cout << "Switch-Case Result  (Category " << testCategory << ") : " 
         << lookupTaxRateSwitch(testCategory) << "% tax rate" << endl;
    cout << "  |- Insight: Lookup tables convert dynamic conditional logic into direct memory loads." << endl;
}

/**
 * @brief Section 4: Demonstrates Zero-Cost Compile-Time Branching ('if constexpr').
 * 'if constexpr' evaluates conditions at compile-time and discards non-matching branches,
 * yielding 0 runtime instruction overhead.
 */
template <bool EnableLogging>
void processTransaction(int transactionId) {
    if constexpr (EnableLogging) {
        // This code is compiled ONLY when EnableLogging == true
        cout << "  - [Logging Enabled] Processed Transaction ID: " << transactionId << endl;
    } else {
        // Zero runtime branch cost when EnableLogging == false
        // The logging code block is completely pruned at compile-time!
    }
}

void demonstrateCompileTimeBranching() {
    cout << "\n================ 4. COMPILE-TIME BRANCHING ('if constexpr') ================" << endl;
    cout << "Executing with EnableLogging = true  :" << endl;
    processTransaction<true>(1001);

    cout << "Executing with EnableLogging = false (Zero-cost dead code elimination) :" << endl;
    processTransaction<false>(1002);
    cout << "  - [Done] Executed quietly without runtime 'if' evaluation." << endl;
}

/**
 * @brief Section 5: Demonstrates Loop Unrolling (Branch Frequency Reduction).
 * Unrolling reduces loop comparison and counter increment operations by a factor of N.
 */
void demonstrateLoopUnrolling(size_t dataSize) {
    cout << "\n================ 5. LOOP UNROLLING & BRANCH REDUCTION ================" << endl;

    vector<int> data(dataSize, 1);

    // Standard Loop (Evaluates loop conditional branch N times)
    auto startStandard = chrono::high_resolution_clock::now();
    uint64_t sumStandard = 0;
    for (size_t i = 0; i < dataSize; ++i) {
        sumStandard += static_cast<uint64_t>(data[i]);
    }
    auto endStandard = chrono::high_resolution_clock::now();

    // Unrolled Loop (Evaluates loop conditional branch N / 4 times)
    auto startUnrolled = chrono::high_resolution_clock::now();
    uint64_t sumUnrolled = 0;
    size_t i = 0;
    size_t unrolledLimit = dataSize - (dataSize % 4);

    for (; i < unrolledLimit; i += 4) {
        sumUnrolled += static_cast<uint64_t>(data[i]);
        sumUnrolled += static_cast<uint64_t>(data[i + 1]);
        sumUnrolled += static_cast<uint64_t>(data[i + 2]);
        sumUnrolled += static_cast<uint64_t>(data[i + 3]);
    }
    // Cleanup remainder loop for unaligned elements
    for (; i < dataSize; ++i) {
        sumUnrolled += static_cast<uint64_t>(data[i]);
    }
    auto endUnrolled = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> elapsedStandard = endStandard - startStandard;
    chrono::duration<double, milli> elapsedUnrolled = endUnrolled - startUnrolled;

    cout << "Standard Loop Time (N branches)    : " << elapsedStandard.count() << " ms" << endl;
    cout << "Unrolled Loop Time (N/4 branches)  : " << elapsedUnrolled.count() << " ms" << endl;
    cout << "  |- Insight: Unrolling reduces loop overhead and helps instruction-level parallelism (ILP)." << endl;
}

int main() {
    size_t benchmarkSize = 0;
    int filterThreshold = 50;

    // Dynamic input collection with stream flushing
    cout << "Enter dataset element count for control-flow benchmark (e.g., 1000000): " << flush;
    if (!(cin >> benchmarkSize) || benchmarkSize <= 0) {
        cout << "Invalid element count provided. Defaulting to 1,000,000 elements." << endl;
        benchmarkSize = 1000000;
    }

    // Execute Performance Demonstrations
    demonstrateBranchPrediction(benchmarkSize, filterThreshold);
    demonstrateBranchlessExecution(benchmarkSize, filterThreshold);
    demonstrateLookupTableVsSwitch();
    demonstrateCompileTimeBranching();
    demonstrateLoopUnrolling(benchmarkSize);

    cout << "\n================ CONTROL FLOW PERFORMANCE BEST PRACTICES ================" << endl;
    cout << "1. Sort / Group Data : Keep conditional branch conditions predictable for the CPU." << endl;
    cout << "2. Go Branchless      : Use arithmetic masks `(x * (cond))` or `cmov` in tight inner loops." << endl;
    cout << "3. Direct Lookups     : Prefer O(1) lookup tables over long if-else / switch chains." << endl;
    cout << "4. Compile-Time Inits : Use `if constexpr` to eliminate unwanted runtime checks." << endl;
    cout << "5. Loop Optimization  : Leverage loop unrolling to reduce jump instruction overhead." << endl;

    return 0;
}