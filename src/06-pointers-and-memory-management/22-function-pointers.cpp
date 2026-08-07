/*
 * =====================================================================================
 * CONCEPT        : Function Pointers in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the integration of function pointers with memory management mechanics:
 *
 *                  1. Raw Function Pointer Syntax  : `ReturnType (*ptrName)(ParamTypes...)`
 *                                                    storing execution code addresses.
 *                  2. Custom Factory & Allocators   : Passing function pointers as allocation 
 *                                                    strategies to dynamically construct memory.
 *                  3. Custom Smart Pointer Deleters: Configuring `std::unique_ptr` and 
 *                                                    `std::shared_ptr` with function pointer deleters.
 *                  4. Jump / Dispatch Tables        : Array of function pointers for O(1) dynamic 
 *                                                    routine selection.
 *                  5. Member Function Pointers      : `ReturnType (Class::*ptr)(Args...)` operating 
 *                                                    on specific object memory instances.
 *                  6. Modern `std::function` vs Raw : Footprint, closure capture, and heap allocation 
 *                                                    trade-offs.
 *
 * TIME COMPLEXITY  : Function Pointer Invocation / Callback: O(1) constant time.
 * SPACE COMPLEXITY : Raw Function Pointer size: Uniform 8 bytes (64-bit) / 4 bytes (32-bit).
 *                    Member Pointer size      : 16 bytes (64-bit platform offset layout).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <functional>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <string>

using namespace std;

// =====================================================================================
// HELPER FUNCTIONS & CLASSES FOR FUNCTION POINTER MEMORY HOOKS
// =====================================================================================

// Custom Deleter Functions for Smart Pointers
void customScalarDeleter(int* ptr) {
    if (ptr != nullptr) {
        cout << "    [CUSTOM SCALAR DELETER HOOK] Freeing heap scalar memory at address: " 
             << static_cast<const void*>(ptr) << "\n";
        delete ptr;
    }
}

void customArrayDeleter(int* ptr) {
    if (ptr != nullptr) {
        cout << "    [CUSTOM ARRAY DELETER HOOK] Freeing dynamic array buffer at address: " 
             << static_cast<const void*>(ptr) << "\n";
        delete[] ptr;
    }
}

// Custom Allocation Strategy Callback Signature
using AllocatorStrategy = int* (*)(size_t size, int initialFill);

// Concrete Allocation Strategies
int* zeroInitializedAllocator(size_t size, int fill) {
    (void)fill; // Unused for zero initialization
    cout << "    [ALLOCATOR STRATEGY] Allocating zero-initialized heap buffer...\n";
    return new int[size]{}; 
}

int* valueFilledAllocator(size_t size, int fill) {
    cout << "    [ALLOCATOR STRATEGY] Allocating filled heap buffer with initial value (" << fill << ")...\n";
    int* buffer = new int[size];
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = fill;
    }
    return buffer;
}

// Arithmetic Dispatch Operations
int opAdd(int a, int b) { return a + b; }
int opSub(int a, int b) { return a - b; }
int opMul(int a, int b) { return a * b; }

// Helper Class for Member Function Pointer Operations
class MemoryPoolEngine {
private:
    size_t allocatedBytes_;

public:
    explicit MemoryPoolEngine(size_t initialBytes) : allocatedBytes_(initialBytes) {}

    void printPoolInfo(const string& poolName) const {
        cout << "    [MEMORY POOL - " << poolName << "] Managing active allocation: " 
             << allocatedBytes_ << " bytes at instance address: " 
             << static_cast<const void*>(this) << "\n";
    }

    size_t scaleAllocation(size_t factor) {
        allocatedBytes_ *= factor;
        return allocatedBytes_;
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for function pointer and memory operations (e.g., 20): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting value to 20." << endl;
        userInputValue = 20;
    }

    // =====================================================================================
    // 1. RAW FUNCTION POINTER SYNTAX & DIRECT INVOCATION
    // =====================================================================================
    cout << "\n================ 1. RAW FUNCTION POINTER MECHANICS ================\n";

    // Syntax: ReturnType (*pointerName)(ParameterTypes...)
    int (*arithmeticPtr)(int, int) = nullptr;

    arithmeticPtr = &opAdd; // Storing code segment address of opAdd
    cout << "  - Function Pointer Address (`opAdd`) : " 
         << reinterpret_cast<const void*>(arithmeticPtr) << "\n";
    cout << "  - Executing via `arithmeticPtr(val, 10)` : " << arithmeticPtr(userInputValue, 10) << "\n";

    arithmeticPtr = &opMul; // Rebinding pointer to opMul
    cout << "  - Rebound Pointer Address (`opMul`)  : " 
         << reinterpret_cast<const void*>(arithmeticPtr) << "\n";
    cout << "  - Executing via `arithmeticPtr(val, 10)` : " << arithmeticPtr(userInputValue, 10) << "\n";

    // =====================================================================================
    // 2. FUNCTION POINTERS AS DYNAMIC ALLOCATION FACTORY HOOKS
    // =====================================================================================
    cout << "\n================ 2. ALLOCATION STRATEGY CALLBACKS ================\n";

    constexpr size_t BUFFER_SIZE = 4;

    // Factory Function accepting function pointer strategy
    auto allocateBuffer = [](size_t count, int fillVal, AllocatorStrategy strategy) -> int* {
        if (strategy == nullptr) return nullptr;
        return strategy(count, fillVal);
    };

    // Allocate using Zero Strategy
    int* zeroBuffer = allocateBuffer(BUFFER_SIZE, userInputValue, &zeroInitializedAllocator);
    cout << "  - Zero Buffer Base Address           : " << static_cast<const void*>(zeroBuffer) 
         << " | First Item: " << zeroBuffer[0] << "\n";

    // Allocate using Filled Strategy
    int* filledBuffer = allocateBuffer(BUFFER_SIZE, userInputValue, &valueFilledAllocator);
    cout << "  - Filled Buffer Base Address         : " << static_cast<const void*>(filledBuffer) 
         << " | First Item: " << filledBuffer[0] << "\n";

    delete[] zeroBuffer;
    delete[] filledBuffer;

    // =====================================================================================
    // 3. FUNCTION POINTERS AS CUSTOM DELETERS IN SMART POINTERS
    // =====================================================================================
    cout << "\n================ 3. CUSTOM SMART POINTER DELETERS ================\n";

    // A. std::unique_ptr with Function Pointer Deleter
    // Note: Signature type requires the function pointer type as the second template parameter
    cout << "  - Creating `std::unique_ptr` with `customScalarDeleter` function pointer...\n";
    {
        std::unique_ptr<int, void(*)(int*)> smartScalar(new int(userInputValue * 5), customScalarDeleter);
        cout << "    Inside scope: Managed value = " << *smartScalar 
             << " | Heap Addr = " << static_cast<const void*>(smartScalar.get()) << "\n";
        cout << "  - Exiting scope (custom scalar deleter will trigger automatically):\n";
    } // Custom deleter hook invoked HERE automatically!

    // B. Dynamic Array Smart Pointer with Custom Function Pointer Deleter
    cout << "\n  - Creating `std::unique_ptr` for array with `customArrayDeleter`...\n";
    {
        std::unique_ptr<int, void(*)(int*)> smartArray(new int[BUFFER_SIZE]{10, 20, 30, 40}, customArrayDeleter);
        cout << "    Inside scope: Managed Array Addr = " 
             << static_cast<const void*>(smartArray.get()) << "\n";
        cout << "  - Exiting scope (custom array deleter will trigger automatically):\n";
    } // Custom array deleter hook invoked HERE automatically!

    // =====================================================================================
    // 4. DISPATCH TABLES / ARRAY OF FUNCTION POINTERS
    // =====================================================================================
    cout << "\n================ 4. FUNCTION POINTER DISPATCH TABLES ================\n";

    // Array of function pointers serving as a jump table for O(1) routine execution
    int (*dispatchTable[3])(int, int) = { &opAdd, &opSub, &opMul };
    const char* opNames[3] = { "Addition (+)", "Subtraction (-)", "Multiplication (*)" };

    for (size_t i = 0; i < 3; ++i) {
        int res = dispatchTable[i](userInputValue, 5);
        cout << "  - Dispatch Index [" << i << "] (" << setw(16) << left << opNames[i] 
             << ") Result with 5: " << res << "\n";
    }

    // =====================================================================================
    // 5. CLASS MEMBER FUNCTION POINTERS (`Class::*`)
    // =====================================================================================
    cout << "\n================ 5. CLASS MEMBER FUNCTION POINTERS ================\n";

    MemoryPoolEngine poolObj(1024);

    // Declaration Syntax: ReturnType (ClassName::*memberPtrName)(ParamTypes...)
    void (MemoryPoolEngine::*infoFuncPtr)(const string&) const = &MemoryPoolEngine::printPoolInfo;
    size_t (MemoryPoolEngine::*scaleFuncPtr)(size_t) = &MemoryPoolEngine::scaleAllocation;

    cout << "  - Size of Member Function Pointer    : " << sizeof(infoFuncPtr) 
         << " bytes (Includes offset metadata)\n";

    // Invocation requires an object instance using `.*` or `->*` operators
    cout << "  - Invoking `infoFuncPtr` via object instance:\n";
    (poolObj.*infoFuncPtr)("PrimaryPool");

    cout << "  - Invoking `scaleFuncPtr` via object instance (Scaling x2):\n";
    size_t newSize = (poolObj.*scaleFuncPtr)(2);
    cout << "    Updated Pool Size returned: " << newSize << " bytes\n";

    // =====================================================================================
    // 6. RAW FUNCTION POINTERS VS MODERN `std::function`
    // =====================================================================================
    cout << "\n================ 6. RAW FUNCTION POINTERS VS `std::function` ================\n";

    // Raw function pointer footprint vs std::function
    int (*rawPtr)(int, int) = &opAdd;
    std::function<int(int, int)> modernWrapper = opAdd;

    cout << "  - `sizeof(Raw Function Pointer)`     : " << sizeof(rawPtr) << " bytes\n";
    cout << "  - `sizeof(std::function wrapper)`    : " << sizeof(modernWrapper) 
         << " bytes (Includes type-erasure buffer)\n";

    // Stateful Lambda capture demo (Raw function pointers CANNOT bind stateful lambdas)
    int captureState = 50;
    auto statefulLambda = [captureState](int x, int y) { return x + y + captureState; };

    // rawPtr = statefulLambda; // COMPILE ERROR: Stateful lambda cannot convert to raw function pointer!
    modernWrapper = statefulLambda; // ALLOWED: std::function handles stateful closures seamlessly

    cout << "  - Executed `std::function` stateful closure result: " << modernWrapper(userInputValue, 10) << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ FUNCTION POINTERS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Category    | Syntax / Declaration Example      | Memory & Operational Role         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Raw Function Pointer  | `int (*ptr)(int, int)`            | Stores code segment function addr |\n"
         << "| Strategy Callback     | `T* (*allocator)(size_t)`         | Configures dynamic allocation     |\n"
         << "| Smart Pointer Deleter | `unique_ptr<T, void(*)(T*)>`      | Custom heap deallocation hook     |\n"
         << "| Dispatch Jump Table   | `int (*table[N])(int)`            | O(1) dynamic routine dispatch     |\n"
         << "| Member Function Ptr   | `void (Class::*ptr)(args)`        | Offset-based method pointer (16B) |\n"
         << "| Modern std::function  | `std::function<int(int, int)>`    | Type-erased wrapper for any call  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}