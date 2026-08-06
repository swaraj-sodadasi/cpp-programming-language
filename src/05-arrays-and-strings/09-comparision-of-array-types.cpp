/*
 * =====================================================================================
 * CONCEPT        : Comprehensive Comparison of Array Types in C++
 * DESCRIPTION    : Side-by-side comparative analysis and practical implementation of the 
 *                  four major array representations in C++:
 *                  1. C-Style Static Array      : Raw memory block on stack/static storage (`T arr[N]`).
 *                  2. Modern STL Static Array   : Zero-overhead container wrapper (`std::array<T, N>`).
 *                  3. Dynamic Heap Array        : Runtime fixed allocation on heap (`std::unique_ptr<T[]>`).
 *                  4. Dynamic Resizable Array   : Dynamically resizing heap buffer (`std::vector<T>`).
 *
 * KEY COMPARISON METRICS:
 * -------------------------------------------------------------------------------------
 * Feature / Trait       | C-Style Array      | std::array        | Dynamic Heap Array| std::vector
 * ----------------------+--------------------+-------------------+-------------------+-----------------
 * Memory Location       | Stack / Static     | Stack / Static    | Heap              | Heap
 * Size Determinance     | Compile-Time       | Compile-Time      | Runtime           | Runtime
 * Dynamic Resizing?     | NO                 | NO                | NO                | YES
 * Pointer Decay         | Automatic (`T*`)   | NO                | Decays (`T*`)     | NO
 * Native `==` Comparison| Address (Unary `+`)| Value Comparison  | Pointer Compare   | Value Comparison
 * Pass to Function      | Decays to `T*`     | Object Value/Ref  | Decays to `T*`    | Object Value/Ref
 * Bounds Checking       | None               | `.at()` throws    | None              | `.at()` throws
 * Modern C++ Best Choice| Legacy C Interop   | Fixed Stack Sizes | Low-Level Buffers | General Purpose
 *
 * TIME COMPLEXITY  : Random Access: O(1) for all types | Re-allocation (vector): Amortized O(1)
 * SPACE COMPLEXITY : Stack/Heap Memory footprint: O(N * sizeof(T))
 * =====================================================================================
 */

#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <iomanip>
#include <string>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS FOR DISPLAYING AND COMPARING ARRAYS
// =====================================================================================

// 1. C-Style Array Processing (Decays to raw pointer)
void printCStyleArray(const int* arr, size_t size, const string& label) {
    cout << "  - [" << left << setw(24) << label << "] Addr: " 
         << static_cast<const void*>(arr) << " | Elements: [ ";
    for (size_t i = 0; i < size; ++i) {
        cout << arr[i] << (i + 1 < size ? ", " : " ");
    }
    cout << "]\n";
}

// 2. std::array Processing (Passed by Const Reference - No Decay!)
template <size_t N>
void printStdArray(const std::array<int, N>& arr, const string& label) {
    cout << "  - [" << left << setw(24) << label << "] Addr: " 
         << static_cast<const void*>(arr.data()) << " | Elements: [ ";
    for (size_t i = 0; i < N; ++i) {
        cout << arr[i] << (i + 1 < N ? ", " : " ");
    }
    cout << "]\n";
}

// 3. std::vector Processing (Passed by Const Reference)
void printVector(const std::vector<int>& vec, const string& label) {
    cout << "  - [" << left << setw(24) << label << "] Addr: " 
         << static_cast<const void*>(vec.data()) 
         << " | Size: " << vec.size() << "/" << vec.capacity() << " | Elements: [ ";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << (i + 1 < vec.size() ? ", " : " ");
    }
    cout << "]\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t runtimeSizeInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter dynamic array size for comparison (e.g., 5): " << flush;
    if (!(cin >> runtimeSizeInput) || runtimeSizeInput == 0) {
        cout << "Invalid input. Defaulting dynamic size to 5." << endl;
        runtimeSizeInput = 5;
    }

    // =====================================================================================
    // 1. C-STYLE STATIC ARRAY (`int arr[N]`)
    // =====================================================================================
    cout << "\n================ 1. C-STYLE STATIC ARRAY ================\n";
    int cArr1[5] = {10, 20, 30, 40, 50};
    int cArr2[5] = {10, 20, 30, 40, 50};

    printCStyleArray(cArr1, 5, "cArr1 (Stack)");
    printCStyleArray(cArr2, 5, "cArr2 (Stack)");

    // RESOLVED: Direct comparison `cArr1 == cArr2` is deprecated in C++20.
    // Unary '+' explicitly decays array operands to pointers to compare memory addresses cleanly.
    cout << "  - Behavior of `+cArr1 == +cArr2`: " 
         << (+cArr1 == +cArr2 ? "EQUAL (Same Memory Address)" : "NOT EQUAL (Different Memory Addresses)") 
         << " <-- Compares raw pointer addresses via explicit unary '+' decay!\n";

    // =====================================================================================
    // 2. MODERN STL STATIC ARRAY (`std::array<int, N>`)
    // =====================================================================================
    cout << "\n================ 2. STL STATIC ARRAY (`std::array`) ================\n";
    std::array<int, 5> stlArr1 = {10, 20, 30, 40, 50};
    std::array<int, 5> stlArr2 = {10, 20, 30, 40, 50};

    printStdArray(stlArr1, "stlArr1 (Stack)");
    printStdArray(stlArr2, "stlArr2 (Stack)");

    // SAFETY: `==` on std::array performs lexicographical VALUE COMPARISON!
    cout << "  - Behavior of `stlArr1 == stlArr2`: " 
         << (stlArr1 == stlArr2 ? "EQUAL (Identical Element Values)" : "NOT EQUAL") 
         << " <-- Performs value-based element comparison!\n";

    // =====================================================================================
    // 3. DYNAMIC HEAP ARRAY (`std::unique_ptr<int[]>`)
    // =====================================================================================
    cout << "\n================ 3. DYNAMIC HEAP ARRAY (`std::unique_ptr<int[]>`) ================\n";
    
    // Size determined at runtime, allocated on the heap
    auto heapArr1 = std::make_unique<int[]>(runtimeSizeInput);
    for (size_t i = 0; i < runtimeSizeInput; ++i) {
        heapArr1[i] = static_cast<int>((i + 1) * 10);
    }

    printCStyleArray(heapArr1.get(), runtimeSizeInput, "heapArr1 (Heap RAII)");
    cout << "  - Memory Segment   : Heap (Size = " << runtimeSizeInput << " elements determined at runtime)\n";
    cout << "  - Safety Guarantee : Cleaned up automatically via RAII unique_ptr destructor.\n";

    // =====================================================================================
    // 4. DYNAMIC RESIZABLE ARRAY (`std::vector<int>`)
    // =====================================================================================
    cout << "\n================ 4. DYNAMIC RESIZABLE ARRAY (`std::vector`) ================\n";
    
    std::vector<int> vec1 = {10, 20, 30, 40, 50};
    std::vector<int> vec2 = {10, 20, 30, 40, 50};

    printVector(vec1, "vec1 Initial State");
    
    // Dynamic Growth Demonstration
    vec1.push_back(60);
    vec1.push_back(70);
    printVector(vec1, "vec1 After push_back");

    // Value Comparison Demonstration
    cout << "  - Behavior of `vec1 == vec2`: " 
         << (vec1 == vec2 ? "EQUAL" : "NOT EQUAL (Different Sizes / Contents)") << "\n";

    // =====================================================================================
    // 5. DETAILED FEATURE MATRIX & TECHNICAL SUMMARY
    // =====================================================================================
    cout << "\n================ ARRAY TYPES COMPREHENSIVE FEATURE MATRIX ================\n";
    cout << "+-----------------------+-------------------+-------------------+-------------------+-------------------+\n"
         << "| Feature / Attribute   | C-Style Array     | std::array        | Dynamic Heap Array| std::vector       |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+-------------------+\n"
         << "| Memory Location       | Stack / Static    | Stack / Static    | Heap              | Heap              |\n"
         << "| Size Known At         | Compile-Time      | Compile-Time      | Runtime           | Runtime           |\n"
         << "| Resizable at Runtime  | NO                | NO                | NO                | YES               |\n"
         << "| Function Parameter    | Decays to `T*`    | Value / Ref Pass  | Decays to `T*`    | Value / Ref Pass  |\n"
         << "| Native `operator==`   | Use Unary `+`     | Value Compare     | Pointer Compare   | Value Compare     |\n"
         << "| Safe Bounds (`.at()`) | NO                | YES               | NO                | YES               |\n"
         << "| STL Iterators         | Raw Pointers      | Native Iterators  | Raw Pointers      | Native Iterators  |\n"
         << "| Value Copy / Assign   | Unsupported       | Supported         | Unsupported       | Supported         |\n"
         << "| Zero-Cost Abstraction | YES               | YES               | YES               | Minor Pointer O/H |\n"
         << "+-----------------------+-------------------+-------------------+-------------------+-------------------+\n";

    return 0;
}