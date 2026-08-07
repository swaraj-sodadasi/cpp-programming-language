/*
 * =====================================================================================
 * CONCEPT        : RAII and Exception Safety in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the symbiotic relationship between Resource Acquisition Is Initialization 
 *                  (RAII) and C++ Exception Safety Guarantees:
 *
 *                  1. The Non-RAII Leak Hazard    : How throwing exceptions before explicit 
 *                                                   cleanup causes permanent resource leaks.
 *                  2. RAII & Stack Unwinding      : Unconditional cleanup of stack-allocated 
 *                                                   RAII wrappers during exception propagation.
 *                  3. Exception Safety Guarantees :
 *                     - Basic Guarantee           : Invariants maintained, no resource leaks.
 *                     - Strong Guarantee          : Commit-or-rollback semantics (Copy-and-Swap).
 *                     - Nothrow Guarantee (`noexcept`): Operation guaranteed never to throw.
 *                  4. The `noexcept` Destructor   : Why destructors MUST NOT throw exceptions 
 *                                                   (preventing `std::terminate`).
 *                  5. Exception-Safe Smart Pointers: `std::unique_ptr` and `std::shared_ptr` 
 *                                                   in exception-prone pipelines.
 *
 * TIME COMPLEXITY  : Acquisition / Release / Swap: O(1) constant time.
 * SPACE COMPLEXITY : RAII Wrapper Overhead: Uniform 8-16 bytes (raw handle footprint).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <algorithm>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE WITH ACTIVE INSTANCE TRACKING
// Used to observe whether memory is leaked or freed during exception unwinding.
// =====================================================================================
class TrackerNode {
private:
    int id_;
    string label_;

public:
    static inline int activeInstances = 0; // C++17 inline static tracker

    TrackerNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeInstances;
        cout << "    [RESOURCE ALLOCATED]   TrackerNode ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    ~TrackerNode() noexcept {
        --activeInstances;
        cout << "    [RESOURCE FREED]       TrackerNode ID: " << setw(3) << id_ << " (" << setw(18) << left << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Instances: " << activeInstances << "\n";
    }

    void executePayload() const {
        cout << "    [PAYLOAD EXECUTED]     TrackerNode ID: " << id_ << " (" << label_ << ") performing task.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: STRONG EXCEPTION GUARANTEE DEMO (COPY-AND-SWAP IDIOM)
// Provides commit-or-rollback semantics: state is unmodified if an operation throws.
// =====================================================================================
class StrongBuffer {
private:
    string name_;
    std::vector<int> data_;

public:
    StrongBuffer(string name, std::initializer_list<int> initialData)
        : name_(std::move(name)), data_(initialData) {}

    // Method offering the STRONG EXCEPTION GUARANTEE via Copy-and-Swap
    void appendWithStrongGuarantee(const std::vector<int>& newElements, bool triggerFailure) {
        cout << "    [STRONG GUARANTEE] Starting transaction for buffer '" << name_ << "'...\n";

        // Step 1: Work on a local copy (Temporary state)
        std::vector<int> tempState = data_;
        tempState.reserve(tempState.size() + newElements.size());

        for (size_t i = 0; i < newElements.size(); ++i) {
            if (triggerFailure && i == newElements.size() / 2) {
                cout << "    [EXCEPTION THROWN] Failure triggered midway during temporary copy processing!\n";
                // Exception thrown HERE does NOT alter `this->data_`!
                throw std::runtime_error("Simulated transaction failure during vector update!");
            }
            tempState.push_back(newElements[i]);
        }

        // Step 2: Non-throwing commit phase (swap handles)
        // std::vector::swap is noexcept -> Commit succeeds atomically
        std::swap(this->data_, tempState);

        cout << "    [STRONG GUARANTEE] Transaction committed successfully!\n";
    }

    void printState() const {
        cout << "    Buffer '" << name_ << "' Current Contents: [ ";
        for (int val : data_) {
            cout << val << " ";
        }
        cout << "]\n";
    }
};

// =====================================================================================
// DEMONSTRATION FUNCTIONS
// =====================================================================================

// 1. Non-RAII Anti-Pattern: Leaks memory when an exception occurs
void demonstrateUnsafeRawLeak(int id) {
    cout << "  - Allocating raw heap pointer without RAII...\n";
    TrackerNode* rawPtr = new TrackerNode(id, "UnsafeRawLeakNode");
    rawPtr->executePayload();

    cout << "  - Throwing exception before manual `delete rawPtr`...\n";
    throw std::runtime_error("Simulated failure in raw pointer function!");

    // UNREACHABLE CODE: Memory is permanently leaked!
    delete rawPtr;
}

// 2. RAII Solution: Basic Exception Guarantee (Zero Resource Leaks via Stack Unwinding)
void demonstrateSafeRaiiUnwind(int id) {
    cout << "  - Binding allocation to `std::unique_ptr`...\n";
    auto smartPtr = std::make_unique<TrackerNode>(id, "SafeRaiiUnwindNode");
    smartPtr->executePayload();

    cout << "  - Throwing exception inside RAII scope...\n";
    throw std::runtime_error("Simulated failure inside RAII scope!");

    // C++ Stack Unwinding automatically invokes `smartPtr` destructor HERE!
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for exception safety analysis (e.g., 800): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 800." << endl;
        userInputValue = 800;
    }

    // =====================================================================================
    // 1. THE NON-RAII HAZARD: RAW ALLOCATIONS & RESOURCE LEAKS
    // =====================================================================================
    cout << "\n================ 1. THE NON-RAII HAZARD (RAW POINTER LEAKS) ================\n";

    cout << "  - Initial Active Instances: " << TrackerNode::activeInstances << "\n";

    try {
        demonstrateUnsafeRawLeak(userInputValue);
    } catch (const std::exception& e) {
        cout << "  - Caught Expected Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - [HAZARD CONFIRMED] Active Instances Post-Raw Exception = " 
         << TrackerNode::activeInstances << " (PERMANENT LEAK: `delete` was bypassed!)\n";

    // =====================================================================================
    // 2. THE RAII SOLUTION: STACK UNWINDING & BASIC EXCEPTION GUARANTEE
    // =====================================================================================
    cout << "\n================ 2. RAII & BASIC EXCEPTION GUARANTEE (NO LEAKS) ================\n";

    try {
        demonstrateSafeRaiiUnwind(userInputValue + 10);
    } catch (const std::exception& e) {
        cout << "  - Caught Expected Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - [RAII GUARANTEE] Active Instances Post-RAII Exception = " 
         << TrackerNode::activeInstances << " (ZERO LEAKS: Destructor ran during stack unwind!)\n";

    // =====================================================================================
    // 3. STRONG EXCEPTION GUARANTEE (COMMIT-OR-ROLLBACK VIA COPY-AND-SWAP)
    // =====================================================================================
    cout << "\n================ 3. STRONG EXCEPTION GUARANTEE (COMMIT-OR-ROLLBACK) ================\n";

    StrongBuffer accountBuffer("PrimaryAccount", {10, 20, 30});
    accountBuffer.printState();

    // Successful Transaction
    cout << "\n  - Executing successful update transaction...\n";
    try {
        accountBuffer.appendWithStrongGuarantee({40, 50}, false);
    } catch (const std::exception& e) {
        cout << "  - Caught Exception: " << e.what() << "\n";
    }
    accountBuffer.printState();

    // Failing Transaction (Demonstrating Rollback)
    cout << "\n  - Executing failing update transaction (triggering failure midway)...\n";
    try {
        accountBuffer.appendWithStrongGuarantee({60, 70, 80}, true);
    } catch (const std::exception& e) {
        cout << "  - Caught Expected Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - [ROLLBACK VERIFICATION] Buffer state post-exception (Notice original state preserved!):\n";
    accountBuffer.printState();

    // =====================================================================================
    // 4. THE `noexcept` DESTRUCTOR CONTRACT
    // =====================================================================================
    cout << "\n================ 4. THE `noexcept` DESTRUCTOR CONTRACT ================\n";
    cout << "  - Rule: Destructors in C++11 and later are implicitly `noexcept`.\n";
    cout << "  - Why? If a destructor throws an exception during Stack Unwinding (when another\n";
    cout << "    exception is already active), C++ calls `std::terminate()` immediately!\n";
    cout << "  - Always catch and handle internal cleanup errors inside destructors locally.\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RAII AND EXCEPTION SAFETY SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Safety Level         | Guarantee Definition              | RAII / Modern C++ Implementation  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| No Safety (Non-RAII)  | Leaks resources, corrupts state   | Raw pointers without try/delete   |\n"
         << "| Basic Guarantee       | No leaks; object invariants valid | Standard RAII (`std::unique_ptr`) |\n"
         << "| Strong Guarantee      | Commit-or-rollback; state unchanged| Copy-and-Swap idiom (`std::swap`) |\n"
         << "| Nothrow (`noexcept`)  | Operation guaranteed never to fail| Destructors, move ctors, `swap()` |\n"
         << "| Stack Unwinding       | Auto-destructs stack objects      | Core C++ runtime mechanism        |\n"
         << "| Double Exception Risk | Triggers `std::terminate()`       | Throwing inside a destructor      |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}