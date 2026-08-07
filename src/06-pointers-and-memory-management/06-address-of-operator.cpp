/*
 * =====================================================================================
 * CONCEPT        : Address-of Operator (`&`) in Modern C++
 * DESCRIPTION    : Comprehensive guide and implementation of the unary address-of operator (`&`):
 *                  1. Unary `&` Operator Mechanics : Extracts the memory address of lvalue objects.
 *                  2. Character Pointer Printing    : Safe address printing for `char` types using 
 *                                                    `static_cast<const void*>`.
 *                  3. Compound Types & Arrays       : Addresses of structs, functions, and array decaying.
 *                  4. Lvalues vs Rvalues Bounds      : Explaining why lvalues have memory addresses 
 *                                                    while rvalue temporaries do not.
 *                  5. Overloaded `operator&` Hazard : Handling custom `operator&` overloads using 
 *                                                    `std::addressof` from `<memory>`.
 *                  6. Address of Pointer Variables  : Extracting addresses of pointer variables 
 *                                                    themselves (`&ptr` -> double pointer).
 *
 * TIME COMPLEXITY  : Address extraction (`&` / `std::addressof`): O(1) constant time.
 * SPACE COMPLEXITY : Pointer variable footprint: 8 bytes (64-bit) / 4 bytes (32-bit).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER STRUCTS & CLASSES FOR ADDRESS DEMONSTRATION
// =====================================================================================

struct Point2D {
    int x;
    int y;
};

// Dummy function to demonstrate function addresses
void targetFunction() {}

// Class with an overloaded operator& to demonstrate std::addressof necessity
class OverloadedAddressClass {
public:
    int data = 100;

    // Overloaded address-of operator returning nullptr (simulates custom smart pointer/proxy wrapper)
    OverloadedAddressClass* operator&() {
        return nullptr;
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial integer value to inspect memory address (e.g., 77): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting initial value to 77." << endl;
        userInputValue = 77;
    }

    // =====================================================================================
    // 1. BASIC ADDRESS-OF OPERATOR USAGE & CHAR PRINTING TRAP
    // =====================================================================================
    cout << "\n================ 1. BASIC ADDRESS-OF OPERATOR USAGE ================\n";

    int intVal = userInputValue;
    double doubleVal = 3.14159;
    char charVal = 'Z';

    cout << "  - `intVal` Value               : " << intVal << "\n";
    cout << "  - `&intVal` Address            : " << static_cast<const void*>(&intVal) << "\n";
    
    cout << "  - `doubleVal` Value            : " << doubleVal << "\n";
    cout << "  - `&doubleVal` Address         : " << static_cast<const void*>(&doubleVal) << "\n";

    // TRAP DEMO: 'cout << &charVal' attempts to print as C-style string, causing buffer read overflow!
    // FIX: Cast to const void* for address printing
    cout << "  - `charVal` Value              : " << charVal << "\n";
    cout << "  - `&charVal` Safe Address      : " << static_cast<const void*>(&charVal) 
         << " (Casted to `void*` to avoid string output trap!)\n";

    // =====================================================================================
    // 2. ADDRESSES OF COMPOUND TYPES, FUNCTIONS & ARRAYS
    // =====================================================================================
    cout << "\n================ 2. ADDRESSES OF STRUCTS, ARRAYS & FUNCTIONS ================\n";

    Point2D point{10, 20};
    cout << "  - Struct `Point2D` Base Addr   : " << static_cast<const void*>(&point) << "\n";
    cout << "  - First Member `&point.x` Addr : " << static_cast<const void*>(&point.x) 
         << " (Matches Struct Base Address!)\n";
    cout << "  - Second Member `&point.y` Addr: " << static_cast<const void*>(&point.y) 
         << " (Offset by sizeof(int) = " << sizeof(int) << " bytes)\n";

    // Function Address
    cout << "  - Function `&targetFunction`   : " 
         << reinterpret_cast<const void*>(&targetFunction) << " (Text/Code Segment)\n";

    // Array vs Array Member Address
    int arr[3] = {100, 200, 300};
    cout << "  - Array Base (`arr`)           : " << static_cast<const void*>(arr) << "\n";
    cout << "  - First Element (`&arr[0]`)    : " << static_cast<const void*>(&arr[0]) << "\n";
    cout << "  - Full Array Addr (`&arr`)     : " << static_cast<const void*>(&arr) 
         << " (Type is `int(*)[3]` pointer-to-array)\n";

    // =====================================================================================
    // 3. LVALUES VS RVALUES (WHAT CAN TAKE `&`)
    // =====================================================================================
    cout << "\n================ 3. LVALUES VS RVALUES & ADDRESS-OF ================\n";

    int lvalueVar = 50;
    cout << "  - Address of Lvalue (`&lvalueVar`): " << static_cast<const void*>(&lvalueVar) << "\n";

    // Rvalues (Literals, temporary expressions) do NOT have persistent memory addresses!
    // Un-commenting any of the following lines will trigger a compile-time error:
    // int* p1 = &100;       // ERROR: Cannot take the address of an rvalue literal!
    // int* p2 = &(x + 5);   // ERROR: Cannot take the address of a temporary expression!
    
    cout << "  - [RVALUE RULE]: Literals (`&100`) and temporary expressions `&(a + b)`\n"
         << "    do NOT have persistent storage and CANNOT take the `&` operator.\n";

    // =====================================================================================
    // 4. OVERLOADED `operator&` VS `std::addressof`
    // =====================================================================================
    cout << "\n================ 4. OVERLOADED `operator&` VS `std::addressof` ================\n";

    OverloadedAddressClass customObj;

    // Direct `&customObj` invokes the class's overloaded operator& which returns nullptr!
    OverloadedAddressClass* hijackedPtr = &customObj;
    
    // std::addressof bypasses custom operator& overloads to yield the true physical address!
    OverloadedAddressClass* truePtr = std::addressof(customObj);

    cout << "  - Result using `&customObj`       : " << static_cast<const void*>(hijackedPtr) 
         << " (Hijacked by custom operator&!)\n";
    cout << "  - Result using `std::addressof`   : " << static_cast<const void*>(truePtr) 
         << " (True physical memory address!)\n";

    // =====================================================================================
    // 5. ADDRESS-OF POINTER VARIABLES (DOUBLE POINTERS)
    // =====================================================================================
    cout << "\n================ 5. ADDRESS-OF POINTER VARIABLES ================\n";

    int primaryVar = 999;
    int* ptrVar = &primaryVar;   // Pointer storing address of primaryVar
    int** ptrPtrVar = &ptrVar;   // Address-of pointer variable itself

    cout << "  - Primary Var Address (`&primaryVar`) : " << static_cast<const void*>(&primaryVar) << "\n";
    cout << "  - Value inside Pointer (`ptrVar`)     : " << static_cast<const void*>(ptrVar) << "\n";
    cout << "  - Address of Pointer Var (`&ptrVar`)   : " << static_cast<const void*>(&ptrVar) << "\n";
    cout << "  - Double Pointer Value (`ptrPtrVar`)  : " << static_cast<const void*>(ptrPtrVar) << "\n";
    cout << "  - Double Dereference (`**ptrPtrVar`)  : " << **ptrPtrVar << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ ADDRESS-OF OPERATOR SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Target Category       | Syntax Example                    | Behavioral Trait & Result         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Primitive Lvalue      | `&intVar`                         | Returns pointer `T*` to memory    |\n"
         << "| Character Variable    | `static_cast<void*>(&charVar)`    | Prevents string buffer printing   |\n"
         << "| Struct / Class        | `&structVar` / `&structVar.member`| Base address matches 1st member   |\n"
         << "| Function              | `&funcName`                       | Returns function code segment ptr |\n"
         << "| Array Variable        | `&arr` vs `arr`                   | `arr` = `T*`, `&arr` = `T(*)[N]`  |\n"
         << "| Rvalue Temporary      | `&42` / `&(a + b)`                | COMPILE ERROR (No storage identity)|\n"
         << "| Overloaded class      | `std::addressof(obj)`             | Safely gets real address vs `&`   |\n"
         << "| Pointer Variable      | `&ptrVar`                         | Returns double pointer `T**`      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}