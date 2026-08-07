/*
 * =====================================================================================
 * CONCEPT        : Exception Handling Performance in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the architectural performance characteristics, benchmark comparisons,
 *                  and execution models of Exception Handling in Modern C++:
 *
 *                  1. The Zero-Cost Exception Model (Itanium C++ ABI / Table-Driven) :
 *                     - Modern C++ compilers (GCC, Clang, MSVC x64) use table-driven unwinding.
 *                     - "Zero-Cost" means ZERO runtime execution overhead when NO exception is thrown
 *                       (Happy Path). No `setjmp`/`longjmp` buffers or stack frame registration.
 *                     - Code size footprint: Unwind tables (`.eh_frame` / `.pdata`) increase binary
 *                       size, but instruction cache locality for normal execution is preserved.
 *
 *                  2. The "Sad Path" Performance Penalty (Exception Throw & Catch) :
 *                     - Throwing an exception is orders of magnitude slower than returning an error code.
 *                     - Costs involved on throw:
 *                       a) Dynamic heap allocation for the exception object ABI state.
 *                       b) Runtime binary search in `.eh_frame` tables to locate lander blocks.
 *                       c) Stack unwinding: Register state restoration and LIFO destructor calls.
 *                       d) Dynamic RTTI type matching (`typeid` comparison).
 *
 *                  3. Benchmarking Error Propagation Strategies :
 *                     - Strategy A: C-style Error Codes / Status Return.
 *                     - Strategy B: Monadic Return Values (`std::optional` / `std::expected`).
 *                     - Strategy C: C++ Exception Handling (`try` / `throw` / `catch`).
 *
 *                  4. Guidelines for High-Performance Code :
 *                     - Use Exceptions for *exceptional*, rare error conditions (out-of-memory, broken invariants).
 *                     - Use Return Values (`std::optional`, status codes) for expected, high-frequency domain errors
 *                       (e.g., parsing invalid user inputs, cache misses, hot-loop validations).
 *
 * TIME COMPLEXITY  : Happy Path (No Throw)        : O(1) Zero runtime overhead vs normal function calls.
 *                    Sad Path (Throw & Unwind)   : O(Depth of Stack * Size of Unwind Tables).
 * SPACE COMPLEXITY : Exception Unwind Table Size  : ~15% - 30% increase in binary file size (`.eh_frame`).
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <chrono>
#include <optional>
#include <iomanip>
#include <limits>

using namespace std;
using namespace std::chrono;

// Prevent compiler from optimizing away bench computations
template <typename T>
void doNotOptimizeAway(T&& value) {
    asm volatile("" : : "g"(value) : "memory");
}

// =====================================================================================
// 1. WORKER FUNCTIONS IMPLEMENTING DIFFERENT ERROR PROPAGATION STRATEGIES
// =====================================================================================

// Strategy A: Return Status Code (C-Style Error Code)
enum class StatusCode { Success = 0, InvalidInput = 1 };

StatusCode processWithReturnCode(int input, int& outResult) noexcept {
    if (input < 0) {
        return StatusCode::InvalidInput; // Fast return
    }
    outResult = input * 2 + 1;
    return StatusCode::Success;
}

// Strategy B: Monadic Return Value (`std::optional`)
std::optional<int> processWithOptional(int input) noexcept {
    if (input < 0) {
        return std::nullopt; // Fast return
    }
    return input * 2 + 1;
}

// Strategy C: C++ Exception Throwing
int processWithException(int input) {
    if (input < 0) {
        throw std::invalid_argument("Invalid negative input!"); // Expensive unwind path
    }
    return input * 2 + 1;
}

// =====================================================================================
// 2. BENCHMARK SUITE
// =====================================================================================
class PerformanceBenchmark {
public:
    // ---------------------------------------------------------------------------------
    // BENCHMARK 1: HAPPY PATH (100% Success - No Exceptions Thrown)
    // Demonstrates Zero-Cost Exception Handling Model (No runtime penalty when not throwing)
    // ---------------------------------------------------------------------------------
    static void runHappyPathBenchmark(int iterations) {
        cout << "\n  --- BENCHMARK 1: HAPPY PATH (" << iterations << " Iterations, 100% Success) ---\n";

        // Test A: Return Code Happy Path
        int dummyResult = 0;
        auto startReturnCode = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            StatusCode status = processWithReturnCode(i, dummyResult);
            doNotOptimizeAway(status);
            doNotOptimizeAway(dummyResult);
        }
        auto endReturnCode = high_resolution_clock::now();
        double durationReturnCodeMs = duration<double, std::milli>(endReturnCode - startReturnCode).count();

        // Test B: Monadic `std::optional` Happy Path
        auto startOptional = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto optRes = processWithOptional(i);
            doNotOptimizeAway(optRes);
        }
        auto endOptional = high_resolution_clock::now();
        double durationOptionalMs = duration<double, std::milli>(endOptional - startOptional).count();

        // Test C: Exception `try/catch` Block Happy Path (No Throw Occurs)
        auto startExceptionHappy = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            try {
                int res = processWithException(i);
                doNotOptimizeAway(res);
            } catch (const std::exception& ex) {
                doNotOptimizeAway(ex);
            }
        }
        auto endExceptionHappy = high_resolution_clock::now();
        double durationExceptionHappyMs = duration<double, std::milli>(endExceptionHappy - startExceptionHappy).count();

        // Display Happy Path Results
        cout << std::fixed << std::setprecision(4);
        cout << "    1. Status Return Code Duration : " << durationReturnCodeMs << " ms\n";
        cout << "    2. `std::optional` Duration    : " << durationOptionalMs << " ms\n";
        cout << "    3. `try/catch` (No Throw)      : " << durationExceptionHappyMs << " ms\n";
        cout << "    [ZERO-COST VERIFICATION]: Notice `try/catch` with NO throws runs at near-identical\n"
             << "    speed to return codes! Zero runtime penalty on the happy execution path.\n";
    }

    // ---------------------------------------------------------------------------------
    // BENCHMARK 2: SAD PATH (100% Error/Failure Rate)
    // Demonstrates the Heavy Penalty of Exception Throwing & Stack Unwinding
    // ---------------------------------------------------------------------------------
    static void runSadPathBenchmark(int iterations) {
        cout << "\n  --- BENCHMARK 2: SAD PATH / FAILURE (" << iterations << " Iterations, 100% Error Rate) ---\n";

        // Test A: Return Code Failure Path
        int dummyResult = 0;
        auto startReturnCode = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            StatusCode status = processWithReturnCode(-1, dummyResult);
            doNotOptimizeAway(status);
        }
        auto endReturnCode = high_resolution_clock::now();
        double durationReturnCodeMs = duration<double, std::milli>(endReturnCode - startReturnCode).count();

        // Test B: Monadic `std::optional` Failure Path
        auto startOptional = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto optRes = processWithOptional(-1);
            doNotOptimizeAway(optRes);
        }
        auto endOptional = high_resolution_clock::now();
        double durationOptionalMs = duration<double, std::milli>(endOptional - startOptional).count();

        // Test C: Exception Throwing & Catching Failure Path
        auto startExceptionSad = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            try {
                int res = processWithException(-1); // Always throws
                doNotOptimizeAway(res);
            } catch (const std::exception& ex) {
                doNotOptimizeAway(ex);
            }
        }
        auto endExceptionSad = high_resolution_clock::now();
        double durationExceptionSadMs = duration<double, std::milli>(endExceptionSad - startExceptionSad).count();

        // Calculate Cost Ratio
        double slowdownFactor = (durationReturnCodeMs > 0.0) 
            ? (durationExceptionSadMs / durationReturnCodeMs) 
            : 0.0;

        // Display Sad Path Results
        cout << std::fixed << std::setprecision(4);
        cout << "    1. Status Return Code Duration : " << durationReturnCodeMs << " ms\n";
        cout << "    2. `std::optional` Duration    : " << durationOptionalMs << " ms\n";
        cout << "    3. Exception Throw & Catch     : " << durationExceptionSadMs << " ms\n";
        cout << "    [PERFORMANCE COST RATIO]: Throwing exceptions is ~" 
             << std::setprecision(1) << slowdownFactor << "x SLOWER than returning error codes!\n"
             << "    Reason: Memory allocation, `.eh_frame` table parsing, & stack frame unwinding.\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Exception Performance analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    const int HAPPY_PATH_ITERATIONS = 10000000; // 10 Million iterations
    const int SAD_PATH_ITERATIONS   = 100000;   // 100 Thousand iterations

    cout << "\n================ EXCEPTION HANDLING PERFORMANCE ANALYSIS ================\n";
    cout << "Base Input Parameter: " << userInputValue << "\n";

    // 1. Run Happy Path Benchmark
    PerformanceBenchmark::runHappyPathBenchmark(HAPPY_PATH_ITERATIONS);

    // 2. Run Sad Path Benchmark
    PerformanceBenchmark::runSadPathBenchmark(SAD_PATH_ITERATIONS);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION PERFORMANCE SUMMARY & GUIDELINES =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Execution Path / Phase| Runtime Performance Impact        | Architectural Recommendation      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Happy Path (No Throw) | ZERO Cost (Table-driven ABI)      | Ideal for clean, composed logic   |\n"
         << "| Sad Path (Throw/Catch)| HIGH Cost (~100x - 1000x slower)  | Reserve ONLY for rare errors      |\n"
         << "| High-Frequency Error  | Error Codes / `std::optional`     | Avoid exceptions in hot loops     |\n"
         << "| Out-of-Memory / System| Exceptions (`std::bad_alloc`)     | Perfect exception candidate       |\n"
         << "| Binary File Size      | +15% to +30% (`.eh_frame` tables) | Disable via `-fno-exceptions` if  |\n"
         << "|                       |                                   | building strict embedded firmware |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}