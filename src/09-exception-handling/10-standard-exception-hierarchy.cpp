/*
 * =====================================================================================
 * CONCEPT        : Standard Exception Hierarchy in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the C++ Standard Library exception hierarchy tree, polymorphic catch
 *                  handling, standard branches, and extending standard exception types:
 *
 *                  C++ STANDARD EXCEPTION HIERARCHY TREE:
 *                  
 *                  std::exception (Root Base Class)
 *                   ├── std::logic_error (Precondition / Programming Logic Errors)
 *                   │    ├── std::invalid_argument  (Invalid parameter passed to function)
 *                   │    ├── std::domain_error     (Input outside valid domain, e.g. sqrt(-1))
 *                   │    ├── std::length_error     (Attempting to exceed maximum size/capacity)
 *                   │    ├── std::out_of_range     (Index or key outside valid container bounds)
 *                   │    └── std::future_error     (Asynchronous operation state error)
 *                   │
 *                   ├── std::runtime_error (Runtime / Environmental / System Failures)
 *                   │    ├── std::range_error      (Computation result outside valid range)
 *                   │    ├── std::overflow_error   (Arithmetic upper boundary overflow)
 *                   │    ├── std::underflow_error  (Floating-point precision loss to zero)
 *                   │    ├── std::system_error     (OS system API or I/O failure)
 *                   │    └── [Custom Exceptions]   (Domain exceptions extending runtime_error)
 *                   │
 *                   ├── std::bad_alloc             (Dynamic heap allocation `new` failure)
 *                   ├── std::bad_cast              (Failed polymorphic `dynamic_cast` on ref)
 *                   ├── std::bad_typeid            (Evaluating `typeid` on null polymorphic ptr)
 *                   ├── std::bad_weak_ptr          (Locking expired `std::weak_ptr`)
 *                   └── std::bad_optional_access   (Accessing empty `std::optional`)
 *
 * TIME COMPLEXITY  : Normal Control Flow (No Throw) : O(1) Zero-cost table lookup overhead.
 *                    Exception Matching & Unwind    : O(Depth of Call Stack).
 * SPACE COMPLEXITY : Exception Storage Overhead     : Small compiler/runtime exception object.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <limits>
#include <typeinfo>
#include <system_error>
#include <optional>

using namespace std;

// =====================================================================================
// 1. CUSTOM EXCEPTION EXTENDING THE STANDARD HIERARCHY
// Best Practice: Inherit from std::runtime_error to fit seamlessly into std::exception
// =====================================================================================
class CustomStorageException : public std::runtime_error {
private:
    int errorCode_;
    string storagePath_;

public:
    CustomStorageException(const string& message, int code, string path)
        : std::runtime_error(message), errorCode_(code), storagePath_(std::move(path)) {}

    [[nodiscard]] int getErrorCode() const noexcept { return errorCode_; }
    [[nodiscard]] const string& getStoragePath() const noexcept { return storagePath_; }

    // Extending what() response while remaining compatible with std::exception interface
    [[nodiscard]] string getDiagnosticReport() const {
        return "CustomStorageException [Code " + to_string(errorCode_) + 
               " | Path: '" + storagePath_ + "']: " + what();
    }
};

// =====================================================================================
// 2. LOGIC ERRORS BRANCH DEMONSTRATOR (`std::logic_error`)
// Faults theoretically preventable by proper precondition checking in code.
// =====================================================================================
class LogicErrorBranchDemo {
public:
    static void triggerLogicError(int scenarioCode, int baseValue) {
        switch (scenarioCode) {
            case 1:
                cout << "    [LOGIC BRANCH] Triggering `std::invalid_argument`...\n";
                if (baseValue > 0) {
                    throw std::invalid_argument("Invalid Argument: Value must be negative for this scenario!");
                }
                break;

            case 2:
                cout << "    [LOGIC BRANCH] Triggering `std::out_of_range`...\n";
                {
                    vector<int> sampleContainer = {10, 20, 30};
                    // Accessing index 99 on size 3 vector
                    [[maybe_unused]] int val = sampleContainer.at(99); 
                }
                break;

            case 3:
                cout << "    [LOGIC BRANCH] Triggering `std::length_error`...\n";
                {
                    vector<int> hugeVec;
                    // Attempting allocation exceeding max permitted size
                    hugeVec.resize(hugeVec.max_size() + 1); 
                }
                break;

            case 4:
                cout << "    [LOGIC BRANCH] Triggering `std::domain_error`...\n";
                throw std::domain_error("Domain Error: Input argument lies outside valid mathematical domain!");

            default:
                cout << "    [LOGIC BRANCH] No logic error triggered.\n";
                break;
        }
    }
};

// =====================================================================================
// 3. RUNTIME ERRORS BRANCH DEMONSTRATOR (`std::runtime_error`)
// Faults due to environmental, OS, or arithmetic boundary conditions at runtime.
// =====================================================================================
class RuntimeErrorBranchDemo {
public:
    static void triggerRuntimeError(int scenarioCode, const string& path) {
        switch (scenarioCode) {
            case 1:
                cout << "    [RUNTIME BRANCH] Triggering `std::overflow_error`...\n";
                throw std::overflow_error("Overflow Error: Arithmetic operation exceeded upper data type limit!");

            case 2:
                cout << "    [RUNTIME BRANCH] Triggering `std::underflow_error`...\n";
                throw std::underflow_error("Underflow Error: Floating-point precision lost to zero!");

            case 3:
                cout << "    [RUNTIME BRANCH] Triggering `std::range_error`...\n";
                throw std::range_error("Range Error: Computation result lies outside valid output range!");

            case 4:
                cout << "    [RUNTIME BRANCH] Triggering `std::system_error`...\n";
                throw std::system_error(make_error_code(errc::no_such_file_or_directory), 
                                         "System Error: POSIX file descriptor acquisition failed");

            case 5:
                cout << "    [RUNTIME BRANCH] Triggering `CustomStorageException`...\n";
                throw CustomStorageException("Storage write aborted due to corrupted sector", 507, path);

            default:
                cout << "    [RUNTIME BRANCH] No runtime error triggered.\n";
                break;
        }
    }
};

// =====================================================================================
// 4. LANGUAGE RUNTIME EXCEPTIONS (DIRECT DESCENDANTS OF `std::exception`)
// Exceptions generated by C++ language operators (`dynamic_cast`, `new`, `optional`, etc.)
// =====================================================================================
class PolymorphicBase { public: virtual ~PolymorphicBase() noexcept = default; };
class PolymorphicDerived : public PolymorphicBase { public: void derivedAction() {} };

class LanguageRuntimeExceptionsDemo {
public:
    static void triggerLanguageException(int scenarioCode) {
        switch (scenarioCode) {
            case 1:
                cout << "    [LANGUAGE EXCEPTION] Triggering `std::bad_cast`...\n";
                {
                    PolymorphicBase baseObj;
                    // Failing reference dynamic_cast throws std::bad_cast
                    [[maybe_unused]] auto& ref = dynamic_cast<PolymorphicDerived&>(baseObj); 
                }
                break;

            case 2:
                cout << "    [LANGUAGE EXCEPTION] Triggering `std::bad_optional_access`...\n";
                {
                    std::optional<int> emptyOpt = std::nullopt;
                    // Accessing value() on empty optional throws std::bad_optional_access
                    [[maybe_unused]] int val = emptyOpt.value(); 
                }
                break;

            case 3:
                cout << "    [LANGUAGE EXCEPTION] Triggering `std::bad_weak_ptr`...\n";
                {
                    std::weak_ptr<int> expiredWeak;
                    {
                        auto sharedVal = std::make_shared<int>(42);
                        expiredWeak = sharedVal;
                    } // sharedVal destroyed here -> expiredWeak is now expired
                    
                    // Constructing shared_ptr from expired weak_ptr throws std::bad_weak_ptr
                    std::shared_ptr<int> lockedShared(expiredWeak); 
                }
                break;

            default:
                cout << "    [LANGUAGE EXCEPTION] No language exception triggered.\n";
                break;
        }
    }
};

// =====================================================================================
// 5. UNIFIED POLYMORPHIC CATCH HANDLER
// Demonstrates how catching by `const std::exception&` safely catches any hierarchy node.
// =====================================================================================
void polymorphicDispatchCatchHandler(const std::exception& ex) {
    cout << "  - [UNIFIED CATCH HANDLER] Intercepted exception of RTTI type: " 
         << typeid(ex).name() << "\n";
    cout << "    * Virtual `what()` Message : \"" << ex.what() << "\"\n";

    // Downcasting safely to inspect custom attributes if applicable
    if (const auto* customEx = dynamic_cast<const CustomStorageException*>(&ex)) {
        cout << "    * Custom Diagnostic Report: " << customEx->getDiagnosticReport() << "\n";
    } else if (const auto* sysEx = dynamic_cast<const std::system_error*>(&ex)) {
        cout << "    * System Error Code Value : " << sysEx->code().value() 
             << " (" << sysEx->code().message() << ")\n";
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Standard Exception Hierarchy analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    const string samplePath = "/var/data/node_" + to_string(userInputValue) + ".dat";

    // =====================================================================================
    // 1. DEMONSTRATING `std::logic_error` BRANCH
    // =====================================================================================
    cout << "\n================ 1. `std::logic_error` BRANCH EXCEPTIONS ================\n";

    for (int scenario = 1; scenario <= 4; ++scenario) {
        try {
            LogicErrorBranchDemo::triggerLogicError(scenario, userInputValue);
        } catch (const std::logic_error& logicEx) {
            cout << "  - [CAUGHT LOGIC ERROR BRANCH]: " << logicEx.what() << "\n";
        } catch (const std::exception& baseEx) {
            cout << "  - [CAUGHT BASE FALLBACK]: " << baseEx.what() << "\n";
        }
    }

    // =====================================================================================
    // 2. DEMONSTRATING `std::runtime_error` BRANCH
    // =====================================================================================
    cout << "\n================ 2. `std::runtime_error` BRANCH EXCEPTIONS ================\n";

    for (int scenario = 1; scenario <= 5; ++scenario) {
        try {
            RuntimeErrorBranchDemo::triggerRuntimeError(scenario, samplePath);
        } catch (const CustomStorageException& customEx) {
            cout << "  - [CAUGHT SPECIFIC CUSTOM EXCEPTION]: " << customEx.getDiagnosticReport() << "\n";
        } catch (const std::runtime_error& runtimeEx) {
            cout << "  - [CAUGHT RUNTIME ERROR BRANCH]: " << runtimeEx.what() << "\n";
        }
    }

    // =====================================================================================
    // 3. DEMONSTRATING LANGUAGE RUNTIME EXCEPTIONS
    // =====================================================================================
    cout << "\n================ 3. DIRECT LANGUAGE `std::exception` SUBCLASSES ================\n";

    for (int scenario = 1; scenario <= 3; ++scenario) {
        try {
            LanguageRuntimeExceptionsDemo::triggerLanguageException(scenario);
        } catch (const std::bad_cast& castEx) {
            cout << "  - [CAUGHT bad_cast]: " << castEx.what() << "\n";
        } catch (const std::bad_optional_access& optEx) {
            cout << "  - [CAUGHT bad_optional_access]: " << optEx.what() << "\n";
        } catch (const std::bad_weak_ptr& weakEx) {
            cout << "  - [CAUGHT bad_weak_ptr]: " << weakEx.what() << "\n";
        } catch (const std::exception& baseEx) {
            cout << "  - [CAUGHT BASE std::exception]: " << baseEx.what() << "\n";
        }
    }

    // =====================================================================================
    // 4. UNIFIED POLYMORPHIC DISPATCH VIA ROOT `std::exception`
    // =====================================================================================
    cout << "\n================ 4. UNIFIED POLYMORPHIC CATCH DISPATCH ================\n";

    try {
        RuntimeErrorBranchDemo::triggerRuntimeError(5, samplePath); // CustomStorageException
    } catch (const std::exception& rootEx) {
        polymorphicDispatchCatchHandler(rootEx);
    }

    try {
        LanguageRuntimeExceptionsDemo::triggerLanguageException(1); // std::bad_cast
    } catch (const std::exception& rootEx) {
        polymorphicDispatchCatchHandler(rootEx);
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ STANDARD EXCEPTION HIERARCHY SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Exception Class       | Immediate Base Class              | Cause / Trigger Mechanism         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| `std::exception`      | [Root Base Class]                 | Base interface (`what()`)         |\n"
         << "| `std::logic_error`    | `std::exception`                  | Theoretical code logic error      |\n"
         << "| `std::invalid_arg`    | `std::logic_error`                | Invalid function parameter value  |\n"
         << "| `std::out_of_range`   | `std::logic_error`                | Index/key out of bounds (`at()`)  |\n"
         << "| `std::length_error`   | `std::logic_error`                | Size exceeds max capacity         |\n"
         << "| `std::domain_error`   | `std::logic_error`                | Argument outside math domain      |\n"
         << "| `std::runtime_error`  | `std::exception`                  | Environmental/system failure      |\n"
         << "| `std::overflow_error` | `std::runtime_error`              | Arithmetic upper boundary exceeded|\n"
         << "| `std::underflow_err`  | `std::runtime_error`              | Floating-point precision loss     |\n"
         << "| `std::system_error`   | `std::runtime_error`              | Operating system / I/O error      |\n"
         << "| `std::bad_alloc`      | `std::exception`                  | Dynamic heap memory depletion     |\n"
         << "| `std::bad_cast`       | `std::exception`                  | Failed polymorphic reference cast |\n"
         << "| `std::bad_optional`   | `std::exception`                  | Accessing empty `std::optional`   |\n"
         << "| `std::bad_weak_ptr`   | `std::exception`                  | Locking expired `std::weak_ptr`   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}