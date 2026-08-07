/*
 * =====================================================================================
 * CONCEPT        : Try and Catch Blocks under Exception Handling in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the syntax, mechanics, rules, and execution flow of `try` and `catch`
 *                  blocks in C++:
 *
 *                  1. Try Block Mechanics (`try { ... }`) :
 *                     - Defines a monitored region of code where exceptions might be thrown.
 *                     - If an exception occurs, execution immediately exits the try block
 *                       and initiates stack unwinding to locate a matching catch handler.
 *
 *                  2. Multiple Catch Blocks (`catch (...)`) :
 *                     - A single try block can be followed by multiple catch blocks to handle
 *                       different exception types.
 *                     - Handlers are evaluated sequentially top-to-bottom. Most specific
 *                       derived exception types MUST be placed BEFORE base exception types.
 *
 *                  3. Catch-By-Const-Reference :
 *                     - ALWAYS catch exception objects by `const ExceptionType&` to prevent
 *                       expensive copy construction, avoid object slicing, and preserve 
 *                       dynamic polymorphic `what()` virtual function dispatch.
 *
 *                  4. Catch-All Handler (`catch (...)`) :
 *                     - Intercepts any unhandled or non-standard/primitive exceptions.
 *                     - Must be placed as the final catch block in a sequence.
 *
 *                  5. Nested Try-Catch Blocks :
 *                     - Inner try blocks handle local errors or allow partial recovery before
 *                       escalating unhandled exceptions to outer try-catch scopes.
 *
 *                  6. Function-Level Try Blocks (`void func() try { ... } catch(...) { ... }`) :
 *                     - Encloses an entire function or constructor body, allowing exception
 *                       handling for initializer lists and function execution blocks.
 *
 * TIME COMPLEXITY  : Normal Path (No Exception) : O(1) Zero-cost table lookup overhead.
 *                    Exception Dispatch / Matching : O(Number of Catch Blocks * Stack Depth).
 * SPACE COMPLEXITY : Exception Handling Metadata : ABI-managed unwind tables & RTTI structures.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <limits>

using namespace std;

// =====================================================================================
// 1. CUSTOM EXCEPTION HIERARCHY FOR CATCH MATCHING DEMONSTRATION
// =====================================================================================
class NetworkException : public std::runtime_error {
public:
    explicit NetworkException(const string& msg)
        : std::runtime_error(msg) {}

    [[nodiscard]] virtual string getDiagnosticCategory() const {
        return "NetworkException [Base Network Error]";
    }
};

class TimeoutException : public NetworkException {
private:
    int timeoutMs_;

public:
    TimeoutException(const string& msg, int timeoutMs)
        : NetworkException(msg), timeoutMs_(timeoutMs) {}

    [[nodiscard]] int getTimeoutMs() const noexcept { return timeoutMs_; }

    [[nodiscard]] string getDiagnosticCategory() const override {
        return "TimeoutException [Derived Timeout Error, Limit: " + to_string(timeoutMs_) + "ms]";
    }
};

class ConnectionRefusedException : public NetworkException {
private:
    int port_;

public:
    ConnectionRefusedException(const string& msg, int port)
        : NetworkException(msg), port_(port) {}

    [[nodiscard]] int getPort() const noexcept { return port_; }

    [[nodiscard]] string getDiagnosticCategory() const override {
        return "ConnectionRefusedException [Derived Refusal Error on Port " + to_string(port_) + "]";
    }
};

// =====================================================================================
// 2. DEMONSTRATION FUNCTIONS FOR TRY-CATCH SCENARIOS
// =====================================================================================

// A. Demonstrating Order of Multiple Catch Blocks
void demonstrateMultipleCatchOrder(int scenarioCode) {
    try {
        cout << "    [Try Block] Executing scenario " << scenarioCode << "...\n";

        if (scenarioCode == 1) {
            throw TimeoutException("HTTP GET Request timed out after 5000ms", 5000);
        } else if (scenarioCode == 2) {
            throw ConnectionRefusedException("TCP Handshake rejected by host", 8080);
        } else if (scenarioCode == 3) {
            throw NetworkException("General socket write failure");
        } else if (scenarioCode == 4) {
            throw std::out_of_range("Buffer index out of range during packet parse");
        } else if (scenarioCode == 5) {
            throw 404; // Primitive integer throw
        }

        cout << "    [Try Block] Operation completed without throwing.\n";

    // CATCH 1: Most specific derived type MUST come first
    } catch (const TimeoutException& ex) {
        cout << "    [Catch Handler 1 - Specific] Caught `TimeoutException`:\n"
             << "      * Message   : " << ex.what() << "\n"
             << "      * Category  : " << ex.getDiagnosticCategory() << "\n"
             << "      * Timeout   : " << ex.getTimeoutMs() << " ms\n";

    // CATCH 2: Another specific derived type
    } catch (const ConnectionRefusedException& ex) {
        cout << "    [Catch Handler 2 - Specific] Caught `ConnectionRefusedException`:\n"
             << "      * Message   : " << ex.what() << "\n"
             << "      * Category  : " << ex.getDiagnosticCategory() << "\n"
             << "      * Port      : " << ex.getPort() << "\n";

    // CATCH 3: Base class of TimeoutException & ConnectionRefusedException
    } catch (const NetworkException& ex) {
        cout << "    [Catch Handler 3 - Base Class] Caught `NetworkException`:\n"
             << "      * Message   : " << ex.what() << "\n"
             << "      * Category  : " << ex.getDiagnosticCategory() << "\n";

    // CATCH 4: General Standard Library Exception Root
    } catch (const std::exception& ex) {
        cout << "    [Catch Handler 4 - Root std::exception] Caught `std::exception`:\n"
             << "      * Message   : " << ex.what() << "\n";

    // CATCH 5: Catch-All Handler (intercepts primitives and non-std exceptions)
    } catch (...) {
        cout << "    [Catch Handler 5 - Catch-All (...)] Intercepted primitive or unknown exception!\n";
    }
}

// B. Demonstrating Nested Try-Catch Blocks
void demonstrateNestedTryCatch(int inputVal) {
    try {
        cout << "  - [Outer Try Block] Entering outer monitored region...\n";

        try {
            cout << "    - [Inner Try Block] Entering inner monitored region...\n";

            if (inputVal > 100) {
                cout << "    - [Inner Try Block] Throwing `TimeoutException`...\n";
                throw TimeoutException("Inner task operation timed out", 2000);
            } else if (inputVal < 0) {
                cout << "    - [Inner Try Block] Throwing `std::invalid_argument`...\n";
                throw std::invalid_argument("Negative parameter not allowed in inner task!");
            }

            cout << "    - [Inner Try Block] Inner operation executed successfully.\n";

        } catch (const TimeoutException& innerEx) {
            // Inner catch handles TimeoutException locally and recovers
            cout << "    - [Inner Catch] Locally handled `TimeoutException`: " << innerEx.what() << "\n";
            cout << "    - [Inner Catch] Local recovery completed. Execution continues normally.\n";
        }

        // If inputVal < 0, std::invalid_argument bypasses inner catch and escalates to outer catch!
        cout << "  - [Outer Try Block] Outer block execution continuing after inner try-catch.\n";

    } catch (const std::invalid_argument& outerEx) {
        cout << "  - [Outer Catch] Intercepted escalated `std::invalid_argument`: " << outerEx.what() << "\n";
    }
}

// C. Demonstrating Function-Level Try Block
void demonstrateFunctionTryBlock(int val) try {
    cout << "  - [Function-Try Body] Executing function try block with value = " << val << "...\n";
    if (val == 0) {
        throw std::domain_error("Zero value error triggered inside function-try body!");
    }
    cout << "  - [Function-Try Body] Function-try execution finished successfully.\n";
} catch (const std::exception& ex) {
    cout << "  - [FUNCTION-TRY CATCH] Handled exception at function level: " << ex.what() << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Try-Catch analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MULTIPLE CATCH BLOCKS & TYPE MATCHING ORDER
    // =====================================================================================
    cout << "\n================ 1. MULTIPLE CATCH BLOCKS & TYPE MATCHING ================\n";

    cout << "  - Case 1: Triggering `TimeoutException` (Caught by specific Catch 1):\n";
    demonstrateMultipleCatchOrder(1);

    cout << "\n  - Case 2: Triggering `ConnectionRefusedException` (Caught by specific Catch 2):\n";
    demonstrateMultipleCatchOrder(2);

    cout << "\n  - Case 3: Triggering `NetworkException` (Caught by Base Class Catch 3):\n";
    demonstrateMultipleCatchOrder(3);

    cout << "\n  - Case 4: Triggering `std::out_of_range` (Caught by Root std::exception Catch 4):\n";
    demonstrateMultipleCatchOrder(4);

    cout << "\n  - Case 5: Triggering Primitive Int 404 (Caught by Catch-All Catch 5):\n";
    demonstrateMultipleCatchOrder(5);

    // =====================================================================================
    // 2. NESTED TRY-CATCH BLOCKS & ESCALATION
    // =====================================================================================
    cout << "\n================ 2. NESTED TRY-CATCH BLOCKS & ESCALATION ================\n";

    cout << "  - Scenario A: Inner catch handles exception locally (Full recovery):\n";
    demonstrateNestedTryCatch(userInputValue + 50); // inputVal > 100 -> TimeoutException handled internally

    cout << "\n  - Scenario B: Inner catch bypasses; exception escalates to outer catch:\n";
    demonstrateNestedTryCatch(-10); // inputVal < 0 -> std::invalid_argument caught by outer

    // =====================================================================================
    // 3. FUNCTION-LEVEL TRY BLOCK
    // =====================================================================================
    cout << "\n================ 3. FUNCTION-LEVEL TRY BLOCK ================\n";

    cout << "  - Scenario A: Successful execution of function-try block:\n";
    demonstrateFunctionTryBlock(userInputValue);

    cout << "\n  - Scenario B: Exception handled directly by function-try catch:\n";
    demonstrateFunctionTryBlock(0);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TRY-CATCH BLOCKS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Try-Catch Mechanism   | Syntax / Structure                | Architectural Rule & Guarantee    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Basic Try Block       | `try { /* code */ }`              | Monitors execution region         |\n"
         << "| Catch By Const Ref    | `catch (const ExType& ex)`        | Prevents slicing; preserves vtable|\n"
         << "| Multiple Catches      | `catch (Derived) catch (Base)`    | Order specific derived BEFORE base|\n"
         << "| Catch-All Handler     | `catch (...)`                     | Intercepts primitive/unknown types|\n"
         << "| Nested Try Blocks     | `try { try {} catch() {} }`       | Allows local handling & escalation|\n"
         << "| Function-Try Block    | `void f() try {} catch() {}`      | Encloses entire function/ctor body|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}