/*
 * =====================================================================================
 * CONCEPT        : Pointers to Members in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the declaration, binding, memory footprint, and dereferencing mechanics
 *                  of pointers to class data members and member functions:
 *
 *                  1. Pointer to Data Member    : Declaring and accessing offset-based member 
 *                                                 variables (`T Class::*`).
 *                  2. Pointer to Member Function: Binding and invoking class methods 
 *                                                 (`R (Class::*)(Args...)`).
 *                  3. Direct (`.*`) vs Indirect (`->*`): Dereferencing operators on stack objects vs 
 *                                                 heap object pointers.
 *                  4. Const Member Pointers     : Distinguishing const vs non-const member 
 *                                                 function pointer signatures.
 *                  5. Size & Memory Footprint   : Inspecting why member pointers are larger than 
 *                                                 raw pointers (16 bytes vs 8 bytes).
 *                  6. Member Pointer Arrays     : Creating dispatch tables of class member functions.
 *                  7. Modern RAII Integration   : Invoking pointers to members on heap objects 
 *                                                 managed by `std::unique_ptr`.
 *
 * TIME COMPLEXITY  : Member Pointer Dereference / Invocation: O(1) constant time.
 * SPACE COMPLEXITY : Raw Pointer size: 8 bytes | Member Function Pointer size: 16 bytes (64-bit).
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER CLASS FOR POINTER TO MEMBER DEMONSTRATIONS
// =====================================================================================
class ProcessingUnit {
public:
    int unitId;
    double loadFactor;

    ProcessingUnit(int id, double load) : unitId(id), loadFactor(load) {
        cout << "    [ProcessingUnit Ctor] Unit ID: " << unitId 
             << " created at heap/stack address: " << static_cast<const void*>(this) << "\n";
    }

    ~ProcessingUnit() {
        cout << "    [ProcessingUnit Dtor] Unit ID: " << unitId 
             << " destructed at address: " << static_cast<const void*>(this) << "\n";
    }

    // Const Member Function
    void displayStatus(const string& header) const {
        cout << "    [" << header << "] ProcessingUnit ID: " << unitId 
             << " | Load Factor: " << fixed << setprecision(2) << loadFactor << "\n";
    }

    // Non-Const Member Functions
    void scaleLoad(double factor) {
        loadFactor *= factor;
        cout << "    [Action: scaleLoad] Scaled Load Factor by " << factor 
             << " -> New Load: " << loadFactor << "\n";
    }

    void resetLoad(double defaultLoad) {
        loadFactor = defaultLoad;
        cout << "    [Action: resetLoad] Reset Load Factor to default: " << loadFactor << "\n";
    }
};

// Helper function taking a pointer to data member
void inspectMemberValue(const ProcessingUnit& unit, int ProcessingUnit::* memberPtr) {
    cout << "    [INSPECTOR FUNCTION] Member value via `unit.*memberPtr`: " << unit.*memberPtr << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base ID for processing unit objects (e.g., 101): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting unit ID to 101." << endl;
        userInputValue = 101;
    }

    // =====================================================================================
    // 1. POINTER TO DATA MEMBERS (`T Class::*`) & ACCESS OPERATORS (`.*` vs `->*`)
    // =====================================================================================
    cout << "\n================ 1. POINTER TO DATA MEMBERS ================\n";

    ProcessingUnit stackUnit(userInputValue, 1.5);

    // Declaration of Pointer to Data Member
    int ProcessingUnit::* idDataPtr = &ProcessingUnit::unitId;
    double ProcessingUnit::* loadDataPtr = &ProcessingUnit::loadFactor;

    // Accessing stack object data members via `.*` operator
    cout << "  - Accessing `unitId` via `stackUnit.*idDataPtr`       : " << stackUnit.*idDataPtr << "\n";
    cout << "  - Accessing `loadFactor` via `stackUnit.*loadDataPtr` : " << stackUnit.*loadDataPtr << "\n";

    // Mutating data members through pointer-to-member
    stackUnit.*loadDataPtr = 2.75;
    cout << "  - Value after mutation `stackUnit.*loadDataPtr = 2.75`: " << stackUnit.loadFactor << "\n";

    // Pointer-to-member usage inside helper function
    inspectMemberValue(stackUnit, idDataPtr);

    // =====================================================================================
    // 2. POINTER TO MEMBER FUNCTIONS (`R (Class::*)(Args...)`)
    // =====================================================================================
    cout << "\n================ 2. POINTER TO MEMBER FUNCTIONS ================\n";

    // Declaration of Pointer to Const Member Function
    void (ProcessingUnit::* displayFuncPtr)(const string&) const = &ProcessingUnit::displayStatus;

    // Declaration of Pointer to Non-Const Member Function
    void (ProcessingUnit::* scaleFuncPtr)(double) = &ProcessingUnit::scaleLoad;

    cout << "  - Invoking `displayStatus()` via `(stackUnit.*displayFuncPtr)(\"StackUnit\")`:\n";
    (stackUnit.*displayFuncPtr)("StackUnit");

    cout << "  - Invoking `scaleLoad()` via `(stackUnit.*scaleFuncPtr)(2.0)`:\n";
    (stackUnit.*scaleFuncPtr)(2.0);

    // =====================================================================================
    // 3. DYNAMIC HEAP OBJECTS & THE `->*` DEREFERENCE OPERATOR
    // =====================================================================================
    cout << "\n================ 3. HEAP OBJECTS & THE `->*` OPERATOR ================\n";

    ProcessingUnit* heapUnit = new ProcessingUnit(userInputValue + 10, 3.20);

    // Accessing heap object data member via `->*` operator
    cout << "  - Accessing `heapUnit->*idDataPtr`                     : " << heapUnit->*idDataPtr << "\n";
    cout << "  - Accessing `heapUnit->*loadDataPtr`                   : " << heapUnit->*loadDataPtr << "\n";

    // Invoking member function on heap object via `->*` operator
    cout << "  - Invoking `displayStatus()` on heap object via `(heapUnit->*displayFuncPtr)`:\n";
    (heapUnit->*displayFuncPtr)("HeapUnit");

    cout << "  - Invoking `scaleLoad()` on heap object via `(heapUnit->*scaleFuncPtr)`:\n";
    (heapUnit->*scaleFuncPtr)(1.5);

    delete heapUnit;
    heapUnit = nullptr; // Safe disarm

    // =====================================================================================
    // 4. MEMORY FOOTPRINT & SIZE ANALYSIS
    // =====================================================================================
    cout << "\n================ 4. MEMORY FOOTPRINT & SIZE ANALYSIS ================\n";

    int* rawDataPtr = nullptr;
    void (*rawFuncPtr)() = nullptr;

    cout << "  - Size of Raw Data Pointer (`int*`)                       : " << sizeof(rawDataPtr) << " bytes\n";
    cout << "  - Size of Raw Function Pointer (`void(*)()`)               : " << sizeof(rawFuncPtr) << " bytes\n";
    cout << "  - Size of Pointer to Data Member (`int ProcessingUnit::*`)  : " << sizeof(idDataPtr) 
         << " bytes (Offset from class base)\n";
    cout << "  - Size of Pointer to Member Function (`void(Class::*)()`) : " << sizeof(scaleFuncPtr) 
         << " bytes (Includes offset + virtual table metadata)\n";

    // =====================================================================================
    // 5. MEMBER FUNCTION POINTER DISPATCH TABLES / ARRAYS
    // =====================================================================================
    cout << "\n================ 5. MEMBER FUNCTION POINTER DISPATCH TABLES ================\n";

    // Array of member function pointers with matching signature: void (ProcessingUnit::*)(double)
    void (ProcessingUnit::* actionTable[2])(double) = {
        &ProcessingUnit::scaleLoad,
        &ProcessingUnit::resetLoad
    };

    const char* actionNames[2] = { "Scale Load (x3.0)", "Reset Load (0.5)" };
    double actionParams[2] = { 3.0, 0.5 };

    cout << "  - Executing dispatch table on `stackUnit`:\n";
    for (size_t i = 0; i < 2; ++i) {
        cout << "    Executing Step [" << i << "] - " << actionNames[i] << ":\n";
        (stackUnit.*actionTable[i])(actionParams[i]);
    }

    // =====================================================================================
    // 6. MODERN RAII INTEGRATION (`std::unique_ptr` WITH `->*`)
    // =====================================================================================
    cout << "\n================ 6. MODERN RAII SMART POINTER INTEGRATION ================\n";

    {
        auto smartUnit = std::make_unique<ProcessingUnit>(userInputValue + 50, 4.0);

        cout << "  - Accessing smart pointer member via `(smartUnit.get()->*idDataPtr)`:\n";
        cout << "    Smart Unit ID = " << smartUnit.get()->*idDataPtr << "\n";

        cout << "  - Invoking member function via smart pointer `(smartUnit.get()->*displayFuncPtr)`:\n";
        (smartUnit.get()->*displayFuncPtr)("SmartUnit");

        cout << "  - Leaving block scope (Smart pointer auto-destructs heap object):\n";
    } // Automatic destruction HERE!

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ POINTERS TO MEMBERS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Member Pointer Type   | Syntax Example                    | Dereference Operator & Role       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Data Member Pointer   | `int Class::* p = &Class::var;`   | `obj.*p` / `pObj->*p` (Member Offset)|\n"
         << "| Member Function Ptr   | `void (Class::*p)(int);`          | `(obj.*p)(args)` / `(pObj->*p)()` |\n"
         << "| Const Member Func Ptr | `void (Class::*p)() const;`       | Binds strictly to `const` methods |\n"
         << "| Size (Data Member Ptr)| `sizeof(T Class::*)` = 8 bytes    | Stores byte offset inside struct  |\n"
         << "| Size (Member Func Ptr)| `sizeof(R(Class::*)(A))` = 16 B  | Stores code address + `this` adjust|\n"
         << "| Member Dispatch Table | `void (Class::*table[N])(arg);`   | O(1) dynamic method dispatch table|\n"
         << "| Smart Pointer Invocation| `(smartPtr.get()->*pMember)()`  | Safe RAII member pointer call     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}