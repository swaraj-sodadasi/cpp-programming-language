/*
 * =====================================================================================
 * CONCEPT        : Constructors & Destructors under Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the strict rules, memory guarantees, and lifecycle mechanics of 
 *                  Constructors and Destructors when exceptions occur:
 *
 *                  1. Constructors & Exceptions :
 *                     - Object Lifetime Rule: An object's destructor executes ONLY if its
 *                       constructor completes successfully without throwing an exception.
 *                     - Sub-object Cleanup: If a constructor throws mid-way (body or initializer
 *                       list), all fully-constructed base class and member sub-objects are 
 *                       destructed in exact REVERSE order of construction.
 *                     - Raw Pointer Danger: Raw pointers allocated before a constructor throw 
 *                       WILL LEAK because the class destructor never runs. Smart pointers
 *                       (`std::unique_ptr`) must be used for member resource management.
 *                     - Function-Try Blocks in Constructors: Used to intercept initializer list 
 *                       exceptions. Catch blocks in constructor function-try blocks MUST 
 *                       implicitly rethrow to prevent partially-constructed objects.
 *
 *                  2. Destructors & Exceptions :
 *                     - Implicit `noexcept` Rule: Destructors in C++11+ are implicitly `noexcept`.
 *                     - The Two-Exception Rule (`std::terminate`): If a destructor throws an 
 *                       exception while stack unwinding is ALREADY in progress due to another 
 *                       active exception, `std::terminate()` is immediately invoked by the runtime.
 *                     - Exception Swallowing: Destructors must swallow internal exceptions 
 *                       using `try/catch(...)` or check `std::uncaught_exceptions()` to guarantee
 *                       they never let exceptions escape their scope.
 *
 * TIME COMPLEXITY  : Stack Unwinding & Sub-object Cleanup : O(Number of Constructed Members).
 * SPACE COMPLEXITY : Exception Storage Footprint          : ABI-managed runtime memory.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <memory>
#include <utility>
#include <string>
#include <limits>

using namespace std;

// =====================================================================================
// 1. MEMBER SUB-OBJECT CLASS TO TRACE LIFECYCLE DURING CONSTRUCTOR THROWS
// =====================================================================================
class MemberSubObject {
private:
    string memberName_;

public:
    explicit MemberSubObject(string name) : memberName_(std::move(name)) {
        cout << "      [Member Ctor] Initialized sub-object: '" << memberName_ << "'\n";
    }

    ~MemberSubObject() noexcept {
        cout << "      [Member Dtor] Destructed sub-object:  '" << memberName_ << "'\n";
    }
};

// =====================================================================================
// 2. CONSTRUCTOR EXCEPTION DEMONSTRATIONS (PARTIAL CONSTRUCTION & RAII)
// =====================================================================================

// Class demonstrating Partial Construction:
// If Constructor throws, the class's OWN destructor WILL NOT RUN, but ready member sub-objects DO.
class PartialConstructionDemo {
private:
    MemberSubObject subMember1_;
    MemberSubObject subMember2_;

public:
    PartialConstructionDemo(bool triggerFailure)
        : subMember1_("SubMember_1"),
          subMember2_("SubMember_2") {
        
        cout << "    [Ctor Body] Members subMember1_ and subMember2_ are fully constructed.\n";
        
        if (triggerFailure) {
            cout << "    [Ctor Body THROW] Triggering exception inside constructor body...\n";
            throw std::runtime_error("Constructor body initialization failed!");
        }

        cout << "    [Ctor Body] Construction completed successfully.\n";
    }

    ~PartialConstructionDemo() noexcept {
        // CRITICAL C++ RULE: THIS DESTRUCTOR WILL NEVER BE CALLED IF CONSTRUCTOR THROWS!
        cout << "    [Class Dtor] PartialConstructionDemo object destroyed.\n";
    }
};

// Demonstration of Raw Pointer Leak vs. Smart Pointer Safety in Constructor
class PointerSafetyConstructorDemo {
private:
    int* unsafeRawPtr_{nullptr};
    std::unique_ptr<int[]> safeSmartPtr_;

public:
    PointerSafetyConstructorDemo(bool useUnsafeLeak, bool triggerFailure) {
        if (useUnsafeLeak) {
            cout << "    [Ctor] Allocating raw heap array via raw pointer...\n";
            unsafeRawPtr_ = new int[100](); // WILL LEAK IF CTOR THROWS BELOW!
        } else {
            cout << "    [Ctor] Allocating heap memory via RAII `std::unique_ptr`...\n";
            safeSmartPtr_ = std::make_unique<int[]>(100); // SAFE ON UNWIND!
        }

        if (triggerFailure) {
            cout << "    [Ctor THROW] Exception thrown inside constructor body!\n";
            throw std::runtime_error("Resource acquisition failed during constructor execution!");
        }

        // Cleanup raw pointer if constructor succeeded
        delete[] unsafeRawPtr_;
        unsafeRawPtr_ = nullptr;
    }

    ~PointerSafetyConstructorDemo() noexcept {
        delete[] unsafeRawPtr_;
        cout << "    [Class Dtor] Cleaned up resources.\n";
    }
};

// Demonstration of Function-Try Block in Constructor Initializer List
class FunctionTryBlockCtorDemo {
private:
    MemberSubObject subMember_;

public:
    // Function-Try block enclosing initializer list and body
    explicit FunctionTryBlockCtorDemo(bool triggerFailure) try
        : subMember_("InitializerSubMember") {
        
        if (triggerFailure) {
            cout << "    [Ctor Body] Throwing exception...\n";
            throw std::invalid_argument("Initializer list / body failure!");
        }
    } catch (const std::exception& ex) {
        cout << "    [FUNCTION-TRY BLOCK CATCH] Caught in constructor initializer list scope: \"" 
             << ex.what() << "\"\n";
        cout << "    [ABI RULE] Constructor function-try catch blocks implicitly RETHROW automatically!\n";
        // Note: The compiler implicitly inserts 'throw;' here if no exception is explicitly thrown!
    }
};

// =====================================================================================
// 3. DESTRUCTOR EXCEPTION DEMONSTRATIONS (`noexcept` & `std::uncaught_exceptions`)
// =====================================================================================

// Best Practice Safe Destructor: Swallowing internal errors
class SafeDestructorDemo {
private:
    string resourceName_;

public:
    explicit SafeDestructorDemo(string name) : resourceName_(std::move(name)) {
        cout << "    [SafeDtor Ctor] Acquired resource: '" << resourceName_ << "'\n";
    }

    ~SafeDestructorDemo() noexcept { // Destructors are implicitly noexcept!
        cout << "    [SafeDtor Dtor] Cleaning up resource: '" << resourceName_ << "'...\n";
        
        try {
            // Simulating internal cleanup operation that might throw
            bool cleanupErrorOccurred = true;
            if (cleanupErrorOccurred) {
                throw std::runtime_error("Internal I/O flush error during destructor teardown");
            }
        } catch (const std::exception& ex) {
            // SWALLOW EXCEPTION INSIDE DESTRUCTOR TO PREVENT std::terminate()
            cout << "    [SafeDtor SWALLOWED EXCEPTION] Caught internal error: \"" << ex.what() 
                 << "\" | Prevented exception escape!\n";
        }
    }
};

// Advanced Destructor inspecting `std::uncaught_exceptions()` (C++17)
class UncaughtExceptionAwareDestructor {
private:
    string objectName_;

public:
    explicit UncaughtExceptionAwareDestructor(string name) : objectName_(std::move(name)) {}

    ~UncaughtExceptionAwareDestructor() noexcept {
        // Inspect if destructor is running during active stack unwinding
        int uncaughtCount = std::uncaught_exceptions();
        if (uncaughtCount > 0) {
            cout << "    [UncaughtAware Dtor] Object '" << objectName_ 
                 << "' destructing due to STACK UNWINDING (Active exceptions: " << uncaughtCount << ")\n";
            cout << "    [UncaughtAware Dtor] Skipping throwing operations to ensure safety!\n";
        } else {
            cout << "    [UncaughtAware Dtor] Object '" << objectName_ 
                 << "' destructing NORMALLY (No active exceptions).\n";
        }
    }
};

// Demonstration of why destructors MUST NOT throw during unwinding
void simulateDoubleExceptionRisk() {
    try {
        cout << "  - Creating stack-allocated `UncaughtExceptionAwareDestructor` object...\n";
        UncaughtExceptionAwareDestructor guard("StackGuard");

        cout << "  - Throwing primary operational exception...\n";
        throw std::runtime_error("Primary Operational Failure");

    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT PRIMARY EXCEPTION]: \"" << ex.what() << "\"\n";
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Ctor/Dtor Exception analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CONSTRUCTORS & PARTIAL CONSTRUCTION
    // =====================================================================================
    cout << "\n================ 1. CONSTRUCTORS & PARTIAL CONSTRUCTION ================\n";

    try {
        cout << "  - Attempting instantiation of `PartialConstructionDemo` with failure flag = TRUE...\n";
        PartialConstructionDemo partialObj(true);
    } catch (const std::exception& ex) {
        cout << "  - [MAIN CAUGHT]: \"" << ex.what() << "\"\n";
        cout << "    [LIFECYCLE RULE VERIFIED]: Member sub-objects (subMember2_ then subMember1_) were\n"
             << "    destructed in reverse order, but the main class destructor WAS NEVER CALLED!\n";
    }

    // =====================================================================================
    // 2. CONSTRUCTOR RESOURCE LEAKS (RAW POINTER VS SMART POINTER)
    // =====================================================================================
    cout << "\n================ 2. CONSTRUCTOR RESOURCE LEAKS (RAW VS SMART POINTERS) ================\n";

    // Safe Smart Pointer Scenario
    try {
        cout << "  - Testing Constructor Failure with `std::unique_ptr` RAII...\n";
        PointerSafetyConstructorDemo safeObj(false, true); // Safe smart pointer, triggers fail
    } catch (const std::exception& ex) {
        cout << "  - [MAIN CAUGHT]: \"" << ex.what() << "\"\n";
        cout << "    [RAII GUARANTEE]: Smart pointer automatically freed heap memory when Ctor failed!\n";
    }

    // =====================================================================================
    // 3. FUNCTION-TRY BLOCKS IN CONSTRUCTORS
    // =====================================================================================
    cout << "\n================ 3. FUNCTION-TRY BLOCKS IN CONSTRUCTORS ================\n";

    try {
        cout << "  - Testing Constructor Function-Try block...\n";
        FunctionTryBlockCtorDemo tryBlockObj(true);
    } catch (const std::exception& ex) {
        cout << "  - [MAIN CATCH]: Caught rethrown exception in main: \"" << ex.what() << "\"\n";
    }

    // =====================================================================================
    // 4. DESTRUCTORS & EXCEPTION SAFETY (`noexcept` & SWALLOWING)
    // =====================================================================================
    cout << "\n================ 4. DESTRUCTORS & EXCEPTION SAFETY ================\n";

    cout << "  - Scenario A: Executing Safe Destructor with internal error swallowing...\n";
    {
        SafeDestructorDemo safeDtorObj("Resource_" + to_string(userInputValue));
    } // SafeDestructorDemo destructor executes HERE at scope exit

    cout << "\n  - Scenario B: Inspecting `std::uncaught_exceptions()` during Stack Unwinding...\n";
    simulateDoubleExceptionRisk();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CONSTRUCTORS & DESTRUCTORS EXCEPTION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Component / Context   | C++ Language Handling Rule        | Architectural Best Practice       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Class Dtor Execution  | Runs ONLY if Ctor finishes 100%   | Never assume Dtor runs on throw   |\n"
         << "| Sub-object Teardown   | Destructs ready members in reverse| Ensures ready sub-members clean up|\n"
         << "| Ctor Raw Pointer      | LEAKS if constructor throws       | Always wrap members in smart ptrs |\n"
         << "| Function-Try Block    | Auto-rethrows in Ctor catch       | Use to translate/log Ctor errors  |\n"
         << "| Dtor `noexcept` Rule  | Implicitly `noexcept` in Ctor/Dtor| Destructors MUST NOT let throws escape|\n"
         << "| Two-Exception Rule    | Exception in unwind = terminate() | Always swallow errors inside Dtor |\n"
         << "| Uncaught Inspector    | `std::uncaught_exceptions()` > 0  | Detects active unwinding state    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}