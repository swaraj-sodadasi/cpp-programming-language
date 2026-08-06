/*
 * =====================================================================================
 * CONCEPT        : Scope in C++ (Global, Local/Block, Namespace, Class, Statement Scope)
 * DESCRIPTION    : Comprehensive implementation showcasing all C++ variable visibility scopes:
 *                  1. Global Scope     : Accessible throughout the translation unit (via :: operator).
 *                  2. Local/Block Scope: Restricted to enclosing curly braces {}.
 *                  3. Statement Scope  : Restricted to control flow statements (e.g., if/for init).
 *                  4. Namespace Scope  : Encapsulated within named logical namespaces.
 *                  5. Class Scope      : Bound to class instances or static class definitions.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct constant-time access to variables across all scopes.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack and data segment memory allocations.
 * =====================================================================================
 */

#include <iostream>

using namespace std;

// 1. GLOBAL SCOPE
// Variable accessible anywhere in the program unless shadowed
int globalVal = 100;

// 2. NAMESPACE SCOPE
// Variable encapsulated within a distinct named namespace to prevent collisions
namespace CustomScope {
    int namespaceVal = 500;

    void displayNamespaceVal() {
        cout << "  |- CustomScope::namespaceVal : " << namespaceVal << endl;
    }
}

// 3. CLASS SCOPE
// Identifiers bounded by class structure declaration
class ScopeDemo {
private:
    int classPrivateVal; // Accessible only within class member scope

public:
    explicit ScopeDemo(int val) : classPrivateVal(val) {}

    void displayClassVal() const {
        cout << "  |- ScopeDemo::classPrivateVal : " << classPrivateVal << endl;
    }
};

int main() {
    int userInput = 0;

    // Dynamic input collection with explicit stream flushing
    cout << "Enter a dynamic integer multiplier: " << flush;
    if (!(cin >> userInput)) {
        cout << "Invalid input provided. Program terminated." << endl;
        return 0;
    }

    cout << "\n================ 1. GLOBAL VS LOCAL SCOPE (SHADOWING) ================" << endl;
    // Local variable with SAME name as global variable (Shadowing)
    int globalVal = userInput * 10; // Local shadow variable

    cout << "Local shadowed globalVal      : " << globalVal << endl;
    cout << "Global ::globalVal (via ::)   : " << ::globalVal << endl;

    cout << "\n================ 2. BLOCK / LOCAL SCOPE ================" << endl;
    {
        // Inner Block Scope
        int blockScopedVal = userInput * 5;
        cout << "  |- Inside Inner Block Scope : blockScopedVal = " << blockScopedVal << endl;
        cout << "  |- Accessing Shadowed Local : globalVal = " << globalVal << endl;
    } 
    // blockScopedVal goes out of scope here and cannot be accessed outside

    cout << "\n================ 3. STATEMENT SCOPE ================" << endl;
    // Variable 'statementVar' is scoped exclusively to the if-else conditional block
    if (int statementVar = userInput + 25; statementVar > 30) {
        cout << "  |- Inside IF Statement Scope: statementVar = " << statementVar << " (> 30)" << endl;
    } else {
        cout << "  |- Inside ELSE Statement Scope: statementVar = " << statementVar << " (<= 30)" << endl;
    }
    // statementVar goes out of scope here

    cout << "\n================ 4. NAMESPACE SCOPE ================" << endl;
    cout << "Accessing Namespace Variable  : " << CustomScope::namespaceVal << endl;
    CustomScope::displayNamespaceVal();

    cout << "\n================ 5. CLASS SCOPE ================" << endl;
    ScopeDemo demoInstance(userInput * 2);
    demoInstance.displayClassVal();

    return 0;
}