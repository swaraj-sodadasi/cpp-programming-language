/*
 * =====================================================================================
 * CONCEPT        : The `throw` Statement in C++ Exception Handling
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the syntax, semantics, rules, and best practices for the `throw` statement:
 *
 *                  1. Throwing Primitives vs. Exception Objects :
 *                     - Throwing primitive values (`throw 404;`, `throw "string";`) [Anti-pattern].
 *                     - Throwing Standard Library exceptions (`std::runtime_error`, `std::invalid_argument`).
 *                     - Throwing Custom Exception objects containing domain-specific state.
 *
 *                  2. Rethrowing Exceptions (`throw;` vs. `throw ex;`) :
 *                     - Bare `throw;` rethrows the CURRENT active exception intact (preserves dynamic type).
 *                     - `throw ex;` rethrows a COPY of the local catch parameter (causes OBJECT SLICING).
 *
 *                  3. Throw Expression Syntax (Ternary Operator) :
 *                     - `throw` is a value-returning expression of type `void`, enabling usage in 
 *                       conditional ternary expressions: `cond ? value : throw std::runtime_error(...)`.
 *
 *                  4. Throwing from Constructors & RAII :
 *                     - Signalling initialization failure by throwing from a constructor.
 *                     - Stack unwinding automatically destructs fully-constructed member sub-objects.
 *
 *                  5. `noexcept` Violation Semantics :
 *                     - Executing a `throw` statement inside a function marked `noexcept` bypasses
 *                       catch handlers and immediately triggers `std::terminate()`.
 *
 * TIME COMPLEXITY  : Throw Dispatch & Catch Matching : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Storage Footprint    : ABI-managed runtime memory pool.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <limits>

using namespace std;

// =====================================================================================
// 1. POLYMORPHIC EXCEPTION HIERARCHY (TO DEMONSTRATE RETHROW SLICING)
// =====================================================================================
class CustomBaseException : public std::runtime_error {
public:
    explicit CustomBaseException(const string& msg)
        : std::runtime_error(msg) {}

    [[nodiscard]] virtual string getDiagnosticCategory() const {
        return "CustomBaseException [Category: BASE_GENERIC]";
    }
};

class CustomDerivedException : public CustomBaseException {
private:
    int errorCode_;

public:
    CustomDerivedException(const string& msg, int code)
        : CustomBaseException(msg), errorCode_(code) {}

    [[nodiscard]] string getDiagnosticCategory() const override {
        return "CustomDerivedException [Category: DERIVED_SPECIFIC, Code: " + to_string(errorCode_) + "]";
    }

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }
};

// =====================================================================================
// 2. HELPER CLASSES FOR CONSTRUCTOR THROW & RETHROW DEMONSTRATIONS
// =====================================================================================
class ResourceTracker {
private:
    string resourceName_;

public:
    explicit ResourceTracker(string name) : resourceName_(std::move(name)) {
        cout << "      [Resource Ctor] Acquired: '" << resourceName_ << "'\n";
    }

    ~ResourceTracker() noexcept {
        cout << "      [Resource Dtor] Released: '" << resourceName_ << "' (Stack Unwound)\n";
    }
};

class ConstructorThrowDemo {
private:
    ResourceTracker member1_;
    ResourceTracker member2_;

public:
    ConstructorThrowDemo(int value)
        : member1_("MemberSubObject_1"),
          member2_("MemberSubObject_2") {
        
        cout << "    [Ctor Body] Inspecting initialization parameter value = " << value << "...\n";
        if (value < 0) {
            cout << "    [Ctor Body] Triggering `throw std::invalid_argument()`...\n";
            throw std::invalid_argument("Constructor failed: Negative configuration value!");
        }
        cout << "    [Ctor Body] Object successfully initialized.\n";
    }

    ~ConstructorThrowDemo() noexcept {
        cout << "    [Dtor] ConstructorThrowDemo object destroyed.\n";
    }
};

// =====================================================================================
// 3. FUNCTIONS DEMONSTRATING VARIOUS THROW MECHANISMS
// =====================================================================================

// Demonstrating Expression Throw in Ternary Operator
int calculatePercentage(int score, int maxScore) {
    // `throw` used as an expression inside the ternary operator
    return (maxScore > 0) 
        ? static_cast<int>((static_cast<double>(score) / maxScore) * 100.0)
        : throw std::invalid_argument("Division by zero: maxScore must be greater than zero!");
}

// Demonstrating Bare `throw;` vs Slicing `throw ex;`
void intermediateRethrowManager(bool correctRethrow) {
    try {
        cout << "    [Internal Worker] Throwing dynamic type `CustomDerivedException`...\n";
        throw CustomDerivedException("Network Timeout on Socket 8080", 504);
    } catch (const CustomBaseException& caughtEx) {
        cout << "    [Internal Catch] Intercepted as `const CustomBaseException&`.\n";
        cout << "    [Internal Catch] Polymorphic Diagnostic: " << caughtEx.getDiagnosticCategory() << "\n";

        if (correctRethrow) {
            cout << "    [Internal Catch] Executing bare `throw;` (Preserves dynamic Derived type)...\n";
            throw; // RETHROWS EXACT ORIGINAL DERIVED EXCEPTION INTACT
        } else {
            cout << "    [Internal Catch] Executing sliced `throw caughtEx;` (Slices to Base type)...\n";
            throw caughtEx; // SLICES DERIVED OBJECT TO BASE COPY! (ANTI-PATTERN)
        }
    }
}

// Function demonstrating throwing primitive types vs exception objects
void validateUserAccess(int accessLevel) {
    if (accessLevel == 0) {
        cout << "    [Validate] Throwing primitive integer `throw 403;`...\n";
        throw 403; // Primitive throw (Anti-pattern in production code)
    } else if (accessLevel < 0) {
        cout << "    [Validate] Throwing string literal `throw \"Denied\";`...\n";
        throw "Access Level Cannot Be Negative!"; // Primitive C-string throw (Anti-pattern)
    } else if (accessLevel > 100) {
        cout << "    [Validate] Throwing standard `std::out_of_range`...\n";
        throw std::out_of_range("Access level exceeds maximum permitted boundary (100)!");
    } else {
        cout << "    [Validate] Access level " << accessLevel << " verified.\n";
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for `throw` statement analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. THROWING PRIMITIVES VS STANDARD EXCEPTION OBJECTS
    // =====================================================================================
    cout << "\n================ 1. THROWING PRIMITIVES VS STANDARD OBJECTS ================\n";

    // Scenario A: Primitive Integer Throw
    try {
        validateUserAccess(0);
    } catch (int errorCode) {
        cout << "  - [CAUGHT PRIMITIVE INT]: Error Code = " << errorCode << "\n";
    }

    // Scenario B: Primitive C-String Throw
    try {
        validateUserAccess(-5);
    } catch (const char* msg) {
        cout << "  - [CAUGHT PRIMITIVE C-STRING]: Message = \"" << msg << "\"\n";
    }

    // Scenario C: Standard Exception Object Throw
    try {
        validateUserAccess(userInputValue + 500);
    } catch (const std::out_of_range& ex) {
        cout << "  - [CAUGHT `std::out_of_range`]: \"" << ex.what() << "\"\n";
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT BASE `std::exception`]: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // 2. RETHROWING: BARE `throw;` VS SLICED `throw ex;`
    // =====================================================================================
    cout << "\n================ 2. RETHROWING (`throw;` VS SLICED `throw ex;`) ================\n";

    // Correct Bare Rethrow (`throw;`)
    cout << "  - Case A: Bare `throw;` (Preserves Dynamic Derived Type):\n";
    try {
        intermediateRethrowManager(true); // Uses bare throw;
    } catch (const CustomBaseException& ex) {
        cout << "  - [MAIN CAUGHT] Polymorphic Category: " << ex.getDiagnosticCategory() << "\n";
        cout << "    Notice above: Dynamic Derived type was preserved correctly!\n";
    }

    // Sliced Rethrow (`throw ex;`)
    cout << "\n  - Case B: Sliced `throw caughtEx;` (Anti-Pattern - Slices to Base):\n";
    try {
        intermediateRethrowManager(false); // Uses throw caughtEx;
    } catch (const CustomBaseException& ex) {
        cout << "  - [MAIN CAUGHT] Polymorphic Category: " << ex.getDiagnosticCategory() << "\n";
        cout << "    Notice above: Dynamic Derived type was SLICED down to CustomBaseException!\n";
    }

    // =====================================================================================
    // 3. THROW EXPRESSIONS IN TERNARY OPERATORS
    // =====================================================================================
    cout << "\n================ 3. THROW EXPRESSIONS IN TERNARY OPERATORS ================\n";

    try {
        cout << "  - Calculating percentage with valid score (85/100)...\n";
        int validPct = calculatePercentage(85, 100);
        cout << "    Result: " << validPct << "%\n";

        cout << "  - Calculating percentage with invalid max score (85/0)...\n";
        int invalidPct = calculatePercentage(85, 0); // Triggers ternary throw
        cout << "    Result: " << invalidPct << "%\n"; // Unreachable
    } catch (const std::invalid_argument& ex) {
        cout << "  - [CAUGHT TERNARY THROW]: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // 4. THROWING FROM CONSTRUCTORS & RAII CLEANUP
    // =====================================================================================
    cout << "\n================ 4. THROWING FROM CONSTRUCTORS & RAII ================\n";

    try {
        cout << "  - Instantiating `ConstructorThrowDemo` with parameter = -50...\n";
        ConstructorThrowDemo failingObj(-50);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT CONSTRUCTOR EXCEPTION]: \"" << ex.what() << "\"\n";
        cout << "    RAII Guarantee: Fully-constructed member sub-objects were cleaned up!\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ THROW STATEMENT SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Throw Variant         | C++ Syntax / Form                 | Behavior / Architectural Rule     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Primitive Throw       | `throw 404;` or `throw \"msg\";`    | Anti-pattern; lacks standard API |\n"
         << "| Standard Object Throw | `throw std::runtime_error(...);`  | Best practice; inherits exception |\n"
         << "| Custom Object Throw   | `throw CustomException(...);`     | Carries rich domain error state   |\n"
         << "| Bare Rethrow          | `throw;`                          | Preserves original dynamic type   |\n"
         << "| Variable Rethrow      | `throw caughtEx;`                 | Slices derived types to base copy |\n"
         << "| Ternary Throw Expr    | `cond ? val : throw std::err();`  | Value-returning throw expression  |\n"
         << "| Constructor Throw     | `throw` inside Ctor body          | Triggers RAII member unwinding    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}