/*
 * =====================================================================================
 * CONCEPT        : Namespace Fundamentals and the Scope Resolution Operator [::]
 * DESCRIPTION    : Comprehensive implementation detailing namespace scope and `::`:
 *                  1. Scope Resolution `::` : Explicitly specifying symbol origins to avoid 
 *                                             ambiguity (Global `::var` vs Local `var`).
 *                  2. Named Namespaces      : Packaging functions, classes, and variables 
 *                                             into logical, collision-free scopes.
 *                  3. Disambiguation        : Resolving identically named identifiers across 
 *                                             different namespaces.
 *                  4. Nested & Aliases      : Accessing multi-level namespaces and defining 
 *                                             concise scope aliases (`namespace Alias = ...`).
 *                  5. Scope Importing       : Comparing `using` declarations (selective) 
 *                                             vs `using` directives (whole namespace).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Scope resolution is completely resolved at compile time.
 * SPACE COMPLEXITY : Best Case: O(1) — Namespaces introduce zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>

using namespace std;

// =====================================================================================
// GLOBAL SCOPE SYMBOLS
// =====================================================================================
int g_systemValue = 1000; // Global variable in the root/global scope

// =====================================================================================
// 1. NAMED NAMESPACES
// Groups related code under distinct identifiers to prevent global name collision.
// =====================================================================================
namespace MathOps {
    int g_systemValue = 50; // Identical name, isolated inside MathOps

    double calculateResult(double val) {
        return val * 2.0;
    }
}

namespace FinancialOps {
    int g_systemValue = 9999; // Identical name, isolated inside FinancialOps

    double calculateResult(double val) {
        return val * 1.05; // Applies 5% markup
    }
}

// =====================================================================================
// 2. NESTED NAMESPACES (C++17 Concise Syntax)
// Multi-level hierarchy for modular software architecture.
// =====================================================================================
namespace Enterprise::Billing::Taxation {
    double applyRegionalTax(double amount, double rate) {
        return amount + (amount * rate);
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double userAmount = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base numeric amount for calculation (e.g., 200.0): " << flush;
    if (!(cin >> userAmount) || userAmount <= 0.0) {
        cout << "Invalid input amount. Defaulting to 200.0." << endl;
        userAmount = 200.0;
    }

    // 1. GLOBAL SCOPE RESOLUTION OPERATOR (`::`)
    cout << "\n================ 1. GLOBAL SCOPE RESOLUTION (`::var`) ================" << endl;
    
    int g_systemValue = 10; // Local variable shadowing global g_systemValue

    cout << "  - Unqualified 'g_systemValue' (Local Frame) : " << g_systemValue << endl;
    cout << "  - Qualified '::g_systemValue' (Global Scope): " << ::g_systemValue << endl;
    cout << "  - Insight: Prefixing `::` bypasses local shadowing to reach global symbols." << endl;

    // 2. NAMED NAMESPACE DISAMBIGUATION
    cout << "\n================ 2. NAMED NAMESPACES & DISAMBIGUATION ================" << endl;
    cout << "Accessing identically named variables across distinct namespaces:" << endl;
    cout << "  - MathOps::g_systemValue      = " << MathOps::g_systemValue << endl;
    cout << "  - FinancialOps::g_systemValue = " << FinancialOps::g_systemValue << endl;

    cout << "\nExecuting identically named functions via scope qualification:" << endl;
    cout << "  - MathOps::calculateResult(" << userAmount << ")      = " 
         << fixed << setprecision(2) << MathOps::calculateResult(userAmount) << endl;
    cout << "  - FinancialOps::calculateResult(" << userAmount << ") = " 
         << fixed << setprecision(2) << FinancialOps::calculateResult(userAmount) << endl;

    // 3. NESTED NAMESPACES & NAMESPACE ALIASES
    cout << "\n================ 3. NESTED NAMESPACES & ALIASES ================" << endl;
    
    // Direct access via fully qualified path
    double taxedTotal1 = Enterprise::Billing::Taxation::applyRegionalTax(userAmount, 0.08);
    cout << "  - Fully Qualified Call: " << taxedTotal1 << endl;

    // Creating a Namespace Alias for deep hierarchies
    namespace Tax = Enterprise::Billing::Taxation;
    double taxedTotal2 = Tax::applyRegionalTax(userAmount, 0.08);
    cout << "  - Alias 'Tax::applyRegionalTax' Call: " << taxedTotal2 << endl;

    // 4. `using` DECLARATIONS vs `using` DIRECTIVES
    cout << "\n================ 4. `using` DECLARATION VS DIRECTIVE ================" << endl;
    
    {
        // Selective import: Imports ONLY specific symbol into this scope block
        using MathOps::calculateResult;
        cout << "  - Selective import 'using MathOps::calculateResult': " 
             << calculateResult(50.0) << endl;
    }

    {
        // Bulk import: Imports ALL symbols from FinancialOps into this scope block
        using namespace FinancialOps;
        cout << "  - Directive import 'using namespace FinancialOps': " 
             << calculateResult(50.0) << " (Uses FinancialOps::calculateResult)" << endl;
    }

    cout << "\n================ NAMESPACE & `::` FUNDAMENTALS SUMMARY ================" << endl;
    cout << "1. Global Scope (`::var`)    : Explicitly references symbols in the root global scope." << endl;
    cout << "2. Qualified (`Scope::var`)  : Precise targeting of symbols declared inside named namespaces." << endl;
    cout << "3. Namespace Aliases         : `namespace Short = Long::Path;` cleans up deep hierarchies." << endl;
    cout << "4. Using Declarations        : Prefer `using Name::Symbol;` over `using namespace Name;` to avoid collisions." << endl;

    return 0;
}