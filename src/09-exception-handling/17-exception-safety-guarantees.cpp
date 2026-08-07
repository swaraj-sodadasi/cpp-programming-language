/*
 * =====================================================================================
 * CONCEPT        : Exception Safety Guarantees in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the 4 standardized levels of Exception Safety Guarantees in Modern C++:
 *
 *                  1. Nothrow / No-fail Guarantee (`noexcept`) :
 *                     - Operations are guaranteed never to throw an exception or fail.
 *                     - Essential for destructors, move constructors, swap functions, and
 *                       deallocators. If a `noexcept` function throws, `std::terminate()` is called.
 *
 *                  2. Strong Exception Guarantee (Commit-or-Rollback) :
 *                     - Operations either succeed completely or fail with no side effects.
 *                     - If an exception occurs, object and program state remain EXACTLY as
 *                       they were prior to the function call (e.g., via Copy-and-Swap idiom).
 *
 *                  3. Basic Exception Guarantee (No Leaks & Valid Invariants) :
 *                     - If an exception is thrown, no resources (memory, handles, locks) are leaked,
 *                       and objects remain in a valid, usable state—though internal state may change.
 *
 *                  4. No Guarantee (Anti-Pattern / Fragile Code) :
 *                     - If an exception occurs, resources may leak, memory may be corrupted,
 *                       dangling pointers created, or object invariants destroyed.
 *
 * RESOLVED ISSUE : Reordered class member declarations in `UnsafeArray` (`size_` declared 
 *                  before `data_`) to match constructor initializer list order, eliminating 
 *                  GCC/Clang `-Wreorder` compiler warnings.
 *
 * TIME COMPLEXITY  : Copy-and-Swap (Strong Guarantee) : O(N) allocation + element copies.
 *                    Nothrow Swap / Move               : O(1) pointer swap time.
 * SPACE COMPLEXITY : Strong Guarantee Buffer Overhead  : O(N) temporary commit buffer.
 * =====================================================================================
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <memory>
#include <utility>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. NO EXCEPTION GUARANTEE (ANTI-PATTERN DEMONSTRATION)
// Violates invariants and leaks memory when an exception occurs midway.
// =====================================================================================
class UnsafeArray {
private:
    size_t size_{0};      // Declared before data_ to match initialization order
    int* data_{nullptr};

public:
    UnsafeArray(size_t sz, int initialVal) : size_(sz), data_(new int[sz]) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = initialVal;
        }
    }

    ~UnsafeArray() noexcept {
        delete[] data_;
    }

    // UNSAFE METHOD: Modifies half the array, allocates memory without guard, and throws.
    // Leaves object in a partially modified, corrupted state.
    void unsafeBatchUpdate(const vector<int>& newValues, bool triggerFailure) {
        cout << "    [UnsafeArray] Starting update on array of size " << size_ << "...\n";
        
        for (size_t i = 0; i < std::min(size_, newValues.size()); ++i) {
            data_[i] = newValues[i]; // Partial modification occurs here
            
            if (triggerFailure && i == 1) {
                cout << "    [UnsafeArray THROW] Exception encountered at element index " << i << "!\n";
                throw std::runtime_error("Unsafe update failure: Execution aborted mid-way!");
            }
        }
    }

    void printState() const {
        cout << "    [UnsafeArray Contents]: { ";
        for (size_t i = 0; i < size_; ++i) {
            cout << data_[i] << (i + 1 < size_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// =====================================================================================
// 2. BASIC EXCEPTION GUARANTEE DEMONSTRATION
// Guarantees zero resource leaks and preserved invariants, but state may be modified.
// =====================================================================================
class BasicSafeArray {
private:
    std::unique_ptr<int[]> data_;
    size_t size_;

public:
    BasicSafeArray(size_t sz, int initialVal)
        : data_(std::make_unique<int[]>(sz)), size_(sz) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = initialVal;
        }
    }

    // BASIC GUARANTEE METHOD: No leaks because std::unique_ptr handles memory.
    // However, array elements modified prior to throw remain modified!
    void basicUpdate(const vector<int>& newValues, bool triggerFailure) {
        cout << "    [BasicSafeArray] Applying in-place modifications...\n";

        for (size_t i = 0; i < std::min(size_, newValues.size()); ++i) {
            data_[i] = newValues[i]; // In-place change

            if (triggerFailure && i == 1) {
                cout << "    [BasicSafeArray THROW] Exception triggered at index " << i << "!\n";
                throw std::runtime_error("Basic guarantee update failed mid-way!");
            }
        }
    }

    void printState() const {
        cout << "    [BasicSafeArray Contents]: { ";
        for (size_t i = 0; i < size_; ++i) {
            cout << data_[i] << (i + 1 < size_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// =====================================================================================
// 3. STRONG EXCEPTION GUARANTEE DEMONSTRATION (COPY-AND-SWAP IDIOM)
// Commit-or-Rollback semantics: Operates on temporary buffer, swaps only on success.
// =====================================================================================
class StrongSafeArray {
private:
    std::unique_ptr<int[]> data_;
    size_t size_;

public:
    StrongSafeArray(size_t sz, int initialVal)
        : data_(std::make_unique<int[]>(sz)), size_(sz) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = initialVal;
        }
    }

    // Copy Constructor
    StrongSafeArray(const StrongSafeArray& other)
        : data_(std::make_unique<int[]>(other.size_)), size_(other.size_) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    // Move Constructor (Nothrow)
    StrongSafeArray(StrongSafeArray&& other) noexcept
        : data_(std::move(other.data_)), size_(other.size_) {
        other.size_ = 0;
    }

    // Nothrow Swap method
    void swap(StrongSafeArray& other) noexcept {
        using std::swap;
        swap(data_, other.data_);
        swap(size_, other.size_);
    }

    // Strong Exception Guarantee via Copy-and-Swap / Temporary Buffer
    void strongUpdate(const vector<int>& newValues, bool triggerFailure) {
        cout << "    [StrongSafeArray] Allocating temporary staging buffer (Copy phase)...\n";
        
        // 1. Create a temporary copy / buffer
        auto tempBuffer = std::make_unique<int[]>(size_);
        for (size_t i = 0; i < size_; ++i) {
            tempBuffer[i] = data_[i];
        }

        // 2. Modify temporary buffer
        for (size_t i = 0; i < std::min(size_, newValues.size()); ++i) {
            tempBuffer[i] = newValues[i];

            if (triggerFailure && i == 1) {
                cout << "    [StrongSafeArray THROW] Exception in temporary buffer at index " << i << "!\n";
                throw std::runtime_error("Strong guarantee operation failed! Rolling back completely.");
            }
        }

        // 3. Commit phase: Swap only if modifying temp buffer succeeded without throwing
        data_.swap(tempBuffer);
        cout << "    [StrongSafeArray COMMIT] Successfully swapped updated temporary buffer into main state!\n";
    }

    void printState() const {
        cout << "    [StrongSafeArray Contents]: { ";
        for (size_t i = 0; i < size_; ++i) {
            cout << data_[i] << (i + 1 < size_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// Nothrow swap overload for StrongSafeArray
void swap(StrongSafeArray& a, StrongSafeArray& b) noexcept {
    a.swap(b);
}

// =====================================================================================
// 4. NOTHROW GUARANTEE DEMONSTRATION (`noexcept`)
// Guarantees zero failures; required for move operations and destructors.
// =====================================================================================
class NothrowResourceAgent {
private:
    int agentId_;
    string agentName_;

public:
    NothrowResourceAgent(int id, string name)
        : agentId_(id), agentName_(std::move(name)) {}

    // Nothrow Guarantee function
    void safeReset(int newId) noexcept {
        agentId_ = newId;
        // Non-throwing primitive operations only
    }

    [[nodiscard]] int getId() const noexcept { return agentId_; }
    [[nodiscard]] const string& getName() const noexcept { return agentName_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Exception Safety analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    vector<int> updateData = {userInputValue + 1, userInputValue + 2, userInputValue + 3, userInputValue + 4};

    // =====================================================================================
    // 1. NO GUARANTEE (FRAGILE / CORRUPTED STATE)
    // =====================================================================================
    cout << "\n================ 1. NO GUARANTEE (FRAGILE / CORRUPTED STATE) ================\n";

    UnsafeArray unsafeObj(4, userInputValue);
    cout << "  - Initial State:\n";
    unsafeObj.printState();

    try {
        cout << "  - Executing `unsafeBatchUpdate()` with failure trigger = TRUE...\n";
        unsafeObj.unsafeBatchUpdate(updateData, true);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT EXCEPTION]: " << ex.what() << "\n";
        cout << "  - State After Failure:\n";
        unsafeObj.printState();
        cout << "    [ANALYSIS]: Notice element 0 was updated while rest were not! Invariants corrupted.\n";
    }

    // =====================================================================================
    // 2. BASIC EXCEPTION GUARANTEE (NO LEAKS, VALID INVARIANTS)
    // =====================================================================================
    cout << "\n================ 2. BASIC EXCEPTION GUARANTEE (NO LEAKS, VALID STATE) ================\n";

    BasicSafeArray basicObj(4, userInputValue);
    cout << "  - Initial State:\n";
    basicObj.printState();

    try {
        cout << "  - Executing `basicUpdate()` with failure trigger = TRUE...\n";
        basicObj.basicUpdate(updateData, true);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT EXCEPTION]: " << ex.what() << "\n";
        cout << "  - State After Failure:\n";
        basicObj.printState();
        cout << "    [ANALYSIS]: Zero memory leaks occurred (RAII managed), object remains valid,\n"
             << "                but element 0 retains the partial update.\n";
    }

    // =====================================================================================
    // 3. STRONG EXCEPTION GUARANTEE (COMMIT-OR-ROLLBACK)
    // =====================================================================================
    cout << "\n================ 3. STRONG EXCEPTION GUARANTEE (COMMIT OR ROLLBACK) ================\n";

    StrongSafeArray strongObj(4, userInputValue);
    cout << "  - Initial State:\n";
    strongObj.printState();

    // Scenario A: Failure causing complete rollback
    try {
        cout << "\n  - Scenario A: Executing `strongUpdate()` with failure trigger = TRUE...\n";
        strongObj.strongUpdate(updateData, true);
    } catch (const std::exception& ex) {
        cout << "  - [CAUGHT EXCEPTION]: " << ex.what() << "\n";
        cout << "  - State After Failure (Rollback Verified):\n";
        strongObj.printState();
        cout << "    [ANALYSIS]: Complete Rollback! Object state remained 100% untouched.\n";
    }

    // Scenario B: Success leading to commit
    try {
        cout << "\n  - Scenario B: Executing `strongUpdate()` with failure trigger = FALSE...\n";
        strongObj.strongUpdate(updateData, false);
        cout << "  - State After Successful Commit:\n";
        strongObj.printState();
    } catch (const std::exception& ex) {
        cout << "  - [UNREACHABLE]: " << ex.what() << "\n";
    }

    // =====================================================================================
    // 4. NOTHROW GUARANTEE (`noexcept`)
    // =====================================================================================
    cout << "\n================ 4. NOTHROW GUARANTEE (`noexcept`) ================\n";

    NothrowResourceAgent agent(userInputValue, "Agent_Alpha");
    cout << "  - Initial Agent ID: " << agent.getId() << " (" << agent.getName() << ")\n";
    
    agent.safeReset(userInputValue + 999);
    cout << "  - Agent ID after `safeReset()` (noexcept execution): " << agent.getId() << "\n";
    cout << "  - Evaluated `noexcept(agent.safeReset(0))` at compile-time: " 
         << (noexcept(agent.safeReset(0)) ? "TRUE (Guaranteed non-throwing)" : "FALSE") << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXCEPTION SAFETY GUARANTEES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Safety Level          | Implementation Pattern / Strategy | Guarantee to Caller               |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Nothrow (`noexcept`)  | Move Ctors, Swaps, Destructors    | Zero exceptions thrown; 100% safe |\n"
         << "| Strong Guarantee      | Copy-and-Swap, Temp Buffer Rollback| Commit or Rollback (Original state)|\n"
         << "| Basic Guarantee       | RAII, Smart Pointers, Guard RAII  | Zero leaks; object remains valid  |\n"
         << "| No Guarantee          | Raw Pointers, Unguarded Mutexes   | Leaks, corruption, undefined state|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}