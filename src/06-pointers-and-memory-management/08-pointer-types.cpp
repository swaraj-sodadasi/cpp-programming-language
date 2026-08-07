/*
 * =====================================================================================
 * CONCEPT        : Pointer Types in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade implementation of pointer categories:
 *                  1. Typed Data Pointers        : Primitive (`int*`, `double*`) and object pointers (`CustomStruct*`).
 *                  2. Generic / Void Pointers    : Type-agnostic pointers (`void*`) & explicit static casting.
 *                  3. Function Pointers          : Raw C-style function pointers & modern `std::function`.
 *                  4. Class Member Pointers      : Pointers to class data members & member functions.
 *                  5. Null Pointer Type          : `std::nullptr_t` & type-safe `nullptr`.
 *                  6. Modern Smart Pointer Types : `std::unique_ptr`, `std::shared_ptr`, and `std::weak_ptr`.
 *
 * TIME COMPLEXITY  : Direct access / Dereference: O(1) constant time.
 * SPACE COMPLEXITY : Raw Pointer Size: Uniform 8 bytes (64-bit) / 4 bytes (32-bit).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <functional>
#include <limits>
#include <cstddef>

using namespace std;

// Helper functions for Function Pointer demonstration
int addNumbers(int a, int b) { return a + b; }
int multiplyNumbers(int a, int b) { return a * b; }

// Helper Class for Member Pointer demonstration
class Calculator {
public:
    int factor = 2;

    int compute(int value) const {
        return value * factor;
    }
};

// Helper functions for std::nullptr_t overload resolution
void inspectPointerType(int* p) {
    cout << "  - Called overload: `inspectPointerType(int*)` -> Address: " 
         << static_cast<const void*>(p) << "\n";
}

void inspectPointerType(std::nullptr_t) {
    cout << "  - Called overload: `inspectPointerType(std::nullptr_t)` -> Expressed explicitly as nullptr!\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputFactor = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for pointer type operations (e.g., 10): " << flush;
    if (!(cin >> userInputFactor) || userInputFactor == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid or zero input. Defaulting value to 10." << endl;
        userInputFactor = 10;
    }

    // =====================================================================================
    // 1. TYPED DATA POINTERS (PRIMITIVES & CUSTOM OBJECTS)
    // =====================================================================================
    cout << "\n================ 1. TYPED DATA POINTERS ================\n";

    int intVal = userInputFactor;
    double doubleVal = 99.99;
    Calculator calcObj;

    int* pInt = &intVal;
    double* pDouble = &doubleVal;
    Calculator* pCalc = &calcObj;

    cout << "  - `int*` Pointer Address       : " << static_cast<const void*>(pInt) << " | Value: " << *pInt << "\n";
    cout << "  - `double*` Pointer Address    : " << static_cast<const void*>(pDouble) << " | Value: " << *pDouble << "\n";
    cout << "  - Object Pointer (`Calculator*`): " << static_cast<const void*>(pCalc) << " | Member factor: " << pCalc->factor << "\n";

    // =====================================================================================
    // 2. GENERIC / VOID POINTERS (`void*`)
    // Type-agnostic pointer capable of storing any memory address (requires explicit cast)
    // =====================================================================================
    cout << "\n================ 2. GENERIC / VOID POINTERS (`void*`) ================\n";

    void* genericPtr = &intVal; // Binds to int address
    cout << "  - `void*` holding `intVal` address : " << genericPtr << "\n";

    // Must cast back to target type before dereferencing
    int* restoredIntPtr = static_cast<int*>(genericPtr);
    cout << "  - Dereferenced post-`static_cast<int*>`: " << *restoredIntPtr << "\n";

    genericPtr = &doubleVal; // Re-bound to double address
    cout << "  - `void*` re-bound to `doubleVal`  : " << genericPtr << "\n";
    double* restoredDoublePtr = static_cast<double*>(genericPtr);
    cout << "  - Dereferenced post-`static_cast<double*>`: " << *restoredDoublePtr << "\n";

    // =====================================================================================
    // 3. FUNCTION POINTERS & `std::function`
    // =====================================================================================
    cout << "\n================ 3. FUNCTION POINTERS & `std::function` ================\n";

    // Raw C-Style Function Pointer
    int (*rawFuncPtr)(int, int) = &addNumbers;
    cout << "  - Raw Function Pointer (`addNumbers`)      : " << rawFuncPtr(userInputFactor, 5) << "\n";

    rawFuncPtr = &multiplyNumbers;
    cout << "  - Re-bound Function Pointer (`multiply`)    : " << rawFuncPtr(userInputFactor, 5) << "\n";

    // Modern C++ `std::function` wrapper
    std::function<int(int, int)> modernFunc = addNumbers;
    cout << "  - Modern `std::function` wrapper (`add`)   : " << modernFunc(userInputFactor, 10) << "\n";

    // =====================================================================================
    // 4. CLASS MEMBER POINTERS (DATA & MEMBER FUNCTIONS)
    // =====================================================================================
    cout << "\n================ 4. CLASS MEMBER POINTERS ================\n";

    // Pointer to Data Member
    int Calculator::* dataMemberPtr = &Calculator::factor;
    calcObj.*dataMemberPtr = 5; // Accessing member via object and member pointer

    cout << "  - Mutated `Calculator::factor` via member ptr: " << calcObj.factor << "\n";

    // Pointer to Member Function
    int (Calculator::* memberFuncPtr)(int) const = &Calculator::compute;
    int result = (calcObj.*memberFuncPtr)(userInputFactor);

    cout << "  - Executed Member Function via member ptr   : " << result << "\n";

    // =====================================================================================
    // 5. NULL POINTER TYPE (`std::nullptr_t`)
    // =====================================================================================
    cout << "\n================ 5. NULL POINTER TYPE (`std::nullptr_t`) ================\n";

    std::nullptr_t explicitNullType = nullptr;

    cout << "  - Testing Overload Resolution with `nullptr` vs `nullptr_t`:\n";
    inspectPointerType(pInt);
    inspectPointerType(explicitNullType);

    // =====================================================================================
    // 6. MODERN SMART POINTER TYPES (`std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr`)
    // =====================================================================================
    cout << "\n================ 6. MODERN SMART POINTER TYPES ================\n";

    // A. std::unique_ptr (Exclusive Ownership)
    std::unique_ptr<int> uPtr = std::make_unique<int>(userInputFactor * 10);
    cout << "  - `std::unique_ptr` (Exclusive) Value : " << *uPtr 
         << " | Address: " << static_cast<const void*>(uPtr.get()) << "\n";

    // B. std::shared_ptr (Shared Reference-Counted Ownership)
    std::shared_ptr<int> sPtr1 = std::make_shared<int>(500);
    std::shared_ptr<int> sPtr2 = sPtr1; // Reference count increases to 2
    cout << "  - `std::shared_ptr` Use Count         : " << sPtr1.use_count() 
         << " | Value: " << *sPtr1 << "\n";

    // C. std::weak_ptr (Non-Owning Reference)
    std::weak_ptr<int> wPtr = sPtr1;
    cout << "  - `std::weak_ptr` Expired Check        : " << (wPtr.expired() ? "YES" : "NO") << "\n";
    if (auto lockedPtr = wPtr.lock()) { // Lock converts weak_ptr to shared_ptr safely
        cout << "  - Locked `std::weak_ptr` Value        : " << *lockedPtr << "\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ POINTER TYPES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Pointer Type Category | Syntax Example                    | Key Characteristics & Use Case    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Typed Data Pointer    | `int* ptr` / `MyClass* ptr`       | Direct typed memory dereferencing |\n"
         << "| Generic Pointer       | `void* ptr`                       | Type-agnostic storage, needs cast |\n"
         << "| Function Pointer      | `int (*ptr)(int, int)`            | Dynamic callback invocation       |\n"
         << "| Class Member Pointer  | `int MyClass::* ptr`              | Offset-based member variable/func |\n"
         << "| Null Pointer Type     | `std::nullptr_t`                  | Type-safe null pointer literal    |\n"
         << "| Unique Smart Pointer  | `std::unique_ptr<T>`              | Exclusive RAII single ownership   |\n"
         << "| Shared Smart Pointer  | `std::shared_ptr<T>`              | Shared reference-counted ownership|\n"
         << "| Weak Smart Pointer    | `std::weak_ptr<T>`                | Non-owning reference (breaks cycle|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}