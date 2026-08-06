/*
 * =====================================================================================
 * CONCEPT        : Performance Considerations in Namespaces and Translation Units (TUs)
 * DESCRIPTION    : Comprehensive program demonstrating the performance impacts of TUs,
 *                  linkage, and namespaces across two primary dimensions:
 *                  
 *                  1. RUNTIME PERFORMANCE:
 *                     - Inlining & Intra-TU Optimizations: Anonymous namespaces and `static`
 *                       grant internal linkage, enabling the compiler to perform aggressive
 *                       inlining, dead code elimination, and custom calling conventions
 *                       without requiring Link-Time Optimization (LTO).
 *                     - Symbol Visibility & Dynamic Linking: Hidden symbols (`namespace {}`)
 *                       reduce the dynamic symbol table size (.dynsym), accelerating library
 *                       load times and indirect function dispatch.
 *                     - Instruction Cache (I-Cache) & Code Bloat: Excessive inlining across
 *                       TUs increases binary footprint, impacting I-cache locality.
 *
 *                  2. COMPILE-TIME & LINK-TIME PERFORMANCE:
 *                     - `extern template` (C++11): Prevents redundant template instantiations
 *                       in multiple TUs, drastically cutting compile time, link time, and object size.
 *                     - ADL (Argument-Dependent Lookup) Cost: Deep namespace hierarchies with
 *                       broad function overloads force the compiler to perform costly symbol lookups.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime after inlining & intra-TU optimization.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. RUNTIME OPTIMIZATION: INTERNAL LINKAGE VIA ANONYMOUS NAMESPACES
// Functions in anonymous namespaces have Internal Linkage. Because the compiler knows 
// no other TU can call these functions, it can:
// - Inline aggressively without emitting a public symbol.
// - Eliminate dead/unused functions entirely.
// - Pass arguments in registers rather than standard calling conventions.
// =====================================================================================

namespace {
    // Highly optimizable intra-TU function
    [[nodiscard]] inline double optimizedIntraTuKernel(double x, double y) noexcept {
        return (x * y) + (x / (y + 1.0));
    }

    void executeInternalTuWorkload(vector<double>& data, double factor) {
        for (double& val : data) {
            val = optimizedIntraTuKernel(val, factor);
        }
    }
} // Unnamed / Anonymous Namespace

// Simulated Non-Inline External Linkage Function (Simulating cross-TU function call overhead)
// Compiler cannot easily inline this across TUs without Link-Time Optimization (LTO)
[[gnu::noinline]] double externalLinkageKernel(double x, double y) {
    return (x * y) + (x / (y + 1.0));
}

void executeExternalTuWorkload(vector<double>& data, double factor) {
    for (double& val : data) {
        val = externalLinkageKernel(val, factor);
    }
}

// =====================================================================================
// 2. COMPILE-TIME & LINK-TIME OPTIMIZATION: EXTERN TEMPLATES (C++11)
// Standard templates are instantiated in EVERY TU that uses them, requiring the linker 
// to deduplicate identical machine code. `extern template` skips redundant compilation.
// =====================================================================================

template <typename T>
class HeavyMatrixProcessor {
public:
    static T computeSum(const vector<T>& data) {
        return accumulate(data.begin(), data.end(), static_cast<T>(0));
    }
};

// EXTERN TEMPLATE DECLARATION (In headers):
// Tells caller TUs: "Do NOT instantiate HeavyMatrixProcessor<double> here; it is explicitly
// instantiated in another source file." Saves CPU compile cycles and link-time merging.
extern template class HeavyMatrixProcessor<double>;

// EXPLICIT TEMPLATE INSTANTIATION (In exactly ONE .cpp file):
template class HeavyMatrixProcessor<double>;


// =====================================================================================
// 3. COMPILE-TIME PERFORMANCE: ADL & OVERLOAD SET COST DEMONSTRATION
// Deep namespaces with excessive overloads slow down compilation due to complex ADL searches.
// =====================================================================================

namespace Subsystem::Audio::Processing {
    struct AudioSignal {
        vector<double> samples;
    };

    // Kept inside specific namespace so ADL stays fast and confined
    void processSignal(const AudioSignal& sig) {
        cout << "  - [ADL Confined] Processed " << sig.samples.size() << " audio samples.\n";
    }
}


// =====================================================================================
// MAIN ENTRY POINT & PERFORMANCE BENCHMARKING
// =====================================================================================
int main() {
    size_t elementCount = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter dataset processing size (e.g., 5000000): " << flush;
    if (!(cin >> elementCount) || elementCount <= 0) {
        cout << "Invalid input. Defaulting dataset size to 5,000,000 elements." << endl;
        elementCount = 5000000;
    }

    // Initialize benchmark vector
    vector<double> datasetA(elementCount, 1.5);
    vector<double> datasetB(elementCount, 1.5);

    cout << "\n================ 1. RUNTIME PERFORMANCE BENCHMARK ================\n";
    cout << "Comparing Intra-TU (Anonymous Namespace) vs Cross-TU (External Linkage) execution...\n\n";

    // Benchmark 1: Intra-TU / Internal Linkage (Inlined & Register Optimized)
    auto startA = chrono::high_resolution_clock::now();
    executeInternalTuWorkload(datasetA, 2.5);
    auto endA = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> durationA = endA - startA;

    cout << "  - Intra-TU (Anonymous Namespace / Inlined) Execution Time : " 
         << fixed << setprecision(3) << durationA.count() << " ms\n";

    // Benchmark 2: External Linkage (Simulated Cross-TU Function Call)
    auto startB = chrono::high_resolution_clock::now();
    executeExternalTuWorkload(datasetB, 2.5);
    auto endB = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> durationB = endB - startB;

    cout << "  - External Linkage (Cross-TU Call Overhead) Execution Time: " 
         << fixed << setprecision(3) << durationB.count() << " ms\n";

    if (durationA.count() > 0.0) {
        double speedup = (durationB.count() - durationA.count()) / durationA.count() * 100.0;
        cout << "  - Speedup / Efficiency Gain via Intra-TU Optimization    : " 
             << fixed << setprecision(1) << speedup << "%\n";
    }

    // 2. EXTERN TEMPLATE DEMO
    cout << "\n================ 2. TEMPLATE INSTANTIATION & LINK-TIME COST ================\n";
    double totalSum = HeavyMatrixProcessor<double>::computeSum(datasetA);
    cout << "  - Computed dataset sum using 'extern template' processor: " 
         << setprecision(2) << totalSum << "\n";
    cout << "  - Benefit: `extern template` prevents duplicate template instantiations across TUs,\n"
         << "             reducing compilation time, linker deduplication work, and binary size bloat.\n";

    // 3. ADL DEMO
    cout << "\n================ 3. ADL & COMPILE-TIME OVERHEAD ================\n";
    Subsystem::Audio::Processing::AudioSignal signal{{0.1, 0.4, -0.2, 0.8}};
    processSignal(signal); // Solved via ADL without global namespace pollution

    // 4. PERFORMANCE CONSIDERATIONS SUMMARY
    cout << "\n================ PERFORMANCE CONSIDERATIONS SUMMARY ================\n";
    cout << "+---------------------------------+-----------------------------------+-----------------------------------+\n"
         << "| Metric Category                 | Technique / Strategy              | Impact on Performance             |\n"
         << "+---------------------------------+-----------------------------------+-----------------------------------+\n"
         << "| Runtime Speed                   | Anonymous Namespaces (`static`)   | Enables aggressive intra-TU       |\n"
         << "|                                 |                                   | inlining & register passing       |\n"
         << "| Dynamic Linking Speed           | Hidden Symbol Visibility          | Reduces dynamic symbol table size |\n"
         << "|                                 | (`-fvisibility=hidden`)           | and accelerates `.so`/`.dll` loads|\n"
         << "| Compile & Link Time             | `extern template` (C++11)         | Eliminates redundant template     |\n"
         << "|                                 |                                   | compilations across multiple TUs  |\n"
         << "| Instruction Cache (I-Cache)     | Strategic Inlining                | Prevents binary code bloat and    |\n"
         << "|                                 |                                   | instruction cache thrashing      |\n"
         << "| Build-Time Symbol Lookup        | Confined Namespace ADL Overloads  | Prevents massive compiler overload|\n"
         << "|                                 |                                   | lookup trees during parsing       |\n"
         << "+---------------------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}