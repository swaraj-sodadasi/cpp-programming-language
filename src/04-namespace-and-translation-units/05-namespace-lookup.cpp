/*
 * =====================================================================================
 * CONCEPT        : Namespace Lookup Mechanisms in C++
 * DESCRIPTION    : Comprehensive implementation detailing C++ symbol resolution rules:
 *                  1. Unqualified Name Lookup : Searches nested scopes inside-out (local scope ->
 *                                               enclosing namespace -> outer namespace -> global scope).
 *                  2. Qualified Name Lookup   : Explicit scope targeting via `Scope::symbol` or global `::symbol`.
 *                  3. Argument-Dependent Lookup: Automatically searches the namespaces of a function's argument 
 *                     (ADL / Koenig Lookup)     types during unqualified function calls.
 *                  4. Name Hiding / Shadowing : How inner scope declarations hide outer scope symbols.
 *                  5. `using` Interaction     : How `using` declarations and directives alter symbol visibility 
 *                                               and lookup paths.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Name lookup is strictly performed at compile time.
 * SPACE COMPLEXITY : Best Case: O(1) — Zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>

using namespace std;

// Global variable in root scope
int g_dataValue = 999;

// =====================================================================================
// 1. QUALIFIED AND UNQUALIFIED LOOKUP & NAME HIDING
// =====================================================================================

namespace OuterScope {
    int g_dataValue = 100; // Outer namespace variable

    namespace InnerScope {
        int g_dataValue = 200; // Inner namespace variable

        void demonstrateUnqualifiedAndQualifiedLookup() {
            int g_dataValue = 300; // Local stack variable shadowing outer declarations

            cout << "\n================ 1. UNQUALIFIED & QUALIFIED LOOKUP ================\n";
            
            // Unqualified lookup stops at the innermost matching scope (Local frame)
            cout << "  - Unqualified 'g_dataValue' (Local scope)       : " << g_dataValue << "\n";

            // Qualified lookup explicitly directs compiler lookup path:
            cout << "  - Qualified 'InnerScope::g_dataValue'           : " << InnerScope::g_dataValue << "\n";
            cout << "  - Qualified 'OuterScope::g_dataValue'           : " << OuterScope::g_dataValue << "\n";
            cout << "  - Qualified 'OuterScope::InnerScope::g_dataValue': " << OuterScope::InnerScope::g_dataValue << "\n";
            cout << "  - Qualified '::g_dataValue' (Global root scope) : " << ::g_dataValue << "\n";
        }
    }
}

// =====================================================================================
// 2. ARGUMENT-DEPENDENT LOOKUP (ADL / KOENIG LOOKUP)
// When calling an unqualified function, C++ searches namespaces of argument types!
// =====================================================================================

namespace PhysicsEngine {
    struct Vector2D {
        double x;
        double y;
    };

    // Function inside PhysicsEngine namespace
    void printVector(const Vector2D& vec) {
        cout << "  - [ADL Found] PhysicsEngine::printVector -> Vector2D(" 
             << vec.x << ", " << vec.y << ")\n";
    }

    // Overloaded stream insertion operator found via ADL
    ostream& operator<<(ostream& os, const Vector2D& vec) {
        os << "PhysicsEngine::Vector2D[" << vec.x << ", " << vec.y << "]";
        return os;
    }
}

void demonstrateADL(double userX, double userY) {
    cout << "\n================ 2. ARGUMENT-DEPENDENT LOOKUP (ADL) ================\n";

    // Create object belonging to PhysicsEngine namespace
    PhysicsEngine::Vector2D position{userX, userY};

    // NOTICE: We call 'printVector' WITHOUT 'PhysicsEngine::' qualification!
    // Unqualified lookup in global scope would normally FAIL, but ADL automatically 
    // inspects the namespace of parameter 'position' (PhysicsEngine) and finds the function.
    printVector(position);

    // ADL is also how standard stream operators work for custom namespace types:
    cout << "  - [ADL Operator<<] Output via std::cout : " << position << "\n";
}

// =====================================================================================
// 3. INTERACTION OF `using` STATEMENTS WITH LOOKUP
// =====================================================================================

namespace SystemA {
    void executeTask() {
        cout << "  - Executed SystemA::executeTask()\n";
    }
}

namespace SystemB {
    void executeTask() {
        cout << "  - Executed SystemB::executeTask()\n";
    }
}

void demonstrateUsingLookup() {
    cout << "\n================ 3. LOOKUP WITH USING DECLARATIONS & DIRECTIVES ================\n";

    // A. Using Declaration: Introduces symbol directly into current declarative scope.
    {
        using SystemA::executeTask;
        cout << "  - Calling 'executeTask()' after 'using SystemA::executeTask':\n  ";
        executeTask(); // Resolves directly to SystemA::executeTask
    }

    // B. Using Directive: Makes symbols visible as if declared in common parent scope.
    {
        using namespace SystemB;
        cout << "  - Calling 'executeTask()' after 'using namespace SystemB':\n  ";
        executeTask(); // Resolves to SystemB::executeTask
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double posX = 0.0;
    double posY = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter X coordinate for Vector2D (e.g., 12.5): " << flush;
    if (!(cin >> posX)) {
        cout << "Invalid input. Defaulting X to 12.5." << endl;
        posX = 12.5;
    }

    cout << "Enter Y coordinate for Vector2D (e.g., 8.0): " << flush;
    if (!(cin >> posY)) {
        cout << "Invalid input. Defaulting Y to 8.0." << endl;
        posY = 8.0;
    }

    // 1. UNQUALIFIED & QUALIFIED LOOKUP
    OuterScope::InnerScope::demonstrateUnqualifiedAndQualifiedLookup();

    // 2. ARGUMENT-DEPENDENT LOOKUP (ADL)
    demonstrateADL(posX, posY);

    // 3. LOOKUP WITH USING STATEMENTS
    demonstrateUsingLookup();

    cout << "\n================ NAMESPACE LOOKUP SUMMARY ================\n";
    cout << "1. Unqualified Lookup : Searches innermost block -> outer blocks -> enclosing namespace -> global scope.\n";
    cout << "2. Qualified Lookup   : Explicitly bypasses default scope searching via `Namespace::` or `::`.\n";
    cout << "3. ADL (Koenig)       : Unqualified function calls search namespaces associated with argument types.\n";
    cout << "4. Name Shadowing     : Inner scope declarations obscure identical names in outer scopes.\n";
    cout << "5. Compile-Time Cost  : All lookup algorithms operate entirely during compilation with 0 runtime cost.\n";

    return 0;
}