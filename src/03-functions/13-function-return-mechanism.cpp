/*
 * =====================================================================================
 * CONCEPT        : Function Return Mechanism in C++ (Value, Reference, Pointer, RVO/NRVO)
 * DESCRIPTION    : Comprehensive implementation explaining how data and control flow
 *                  are transferred from a callee function back to the caller frame:
 *                  1. Control Flow Transfer  : Restoring instruction pointer and stack frame 
 *                                              deallocation upon executing 'return'.
 *                  2. Return by Value        : Copy/Move semantics and Guaranteed Copy Elision 
 *                                              (RVO / NRVO optimizations).
 *                  3. Return by Reference    : Returning lvalue references (T& / const T&), 
 *                                              safe usage, and dangling reference pitfalls.
 *                  4. Smart Pointer Return   : Returning heap-allocated objects via std::unique_ptr 
 *                                              for safe zero-copy ownership transfer.
 *                  5. Multiple Return Values : Packaging multiple values using std::tuple 
 *                                              and C++17 structured bindings.
 *
 * TIME COMPLEXITY  : O(1) for scalar, reference, move, and copy-elided returns.
 * SPACE COMPLEXITY : O(1) stack frame deallocation upon function exit.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <tuple>
#include <utility>
#include <iomanip>

using namespace std;

// =====================================================================================
// RESOURCE TRACKER CLASS
// Logs construction, copy, move, and destruction during function return execution.
// =====================================================================================
class ReturnTracker {
private:
    string label_;

public:
    explicit ReturnTracker(string label) : label_(std::move(label)) {
        cout << "    [Constructed] '" << label_ << "' created in callee frame.\n";
    }

    // Copy Constructor
    ReturnTracker(const ReturnTracker& other) : label_(other.label_ + "_copy") {
        cout << "    [Copy Const ] Deep copied '" << other.label_ << "' -> '" << label_ << "'\n";
    }

    // Move Constructor
    ReturnTracker(ReturnTracker&& other) noexcept : label_(std::move(other.label_) + "_moved") {
        cout << "    [Move Const ] Transferred ownership -> '" << label_ << "'\n";
    }

    // Destructor
    ~ReturnTracker() {
        cout << "    [Destructor ] Destroyed '" << label_ << "'\n";
    }

    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// 1. RETURN BY VALUE & RVO / NRVO
// Compilers construct the object directly in the caller's stack frame (Copy Elision).
// =====================================================================================
ReturnTracker demonstrateNRVO(const string& name) {
    ReturnTracker obj(name);
    return obj; // Named Return Value Optimization (NRVO) / Implicit Move
}

// =====================================================================================
// 2. RETURN BY REFERENCE (SAFE VS PITFALL)
// Returns an alias to an existing object whose lifetime outlives the function call.
// =====================================================================================
const string& getSafeReference(const string& callerOwnedStr) {
    // SAFE: Lifetime of callerOwnedStr outlives this function call frame
    return callerOwnedStr;
}

// =====================================================================================
// 3. RETURN BY SMART POINTER (OWNERSHIP TRANSFER)
// Move-only types (std::unique_ptr) cleanly transfer heap memory ownership.
// =====================================================================================
unique_ptr<ReturnTracker> createManagedTracker(const string& label) {
    return make_unique<ReturnTracker>(label); // Returns unique_ptr by value (zero-copy move)
}

// =====================================================================================
// 4. MULTIPLE RETURN VALUES (std::tuple & C++17 STRUCTURED BINDINGS)
// Bundles distinct data types into a single composite return type.
// =====================================================================================
tuple<int, double, string> calculateMetrics(int inputVal) {
    int doubled = inputVal * 2;
    double scaled = static_cast<double>(inputVal) * 1.5;
    string status = (inputVal >= 0) ? "SUCCESS" : "NEGATIVE_INPUT";
    return {doubled, scaled, status};
}

// =====================================================================================
// 5. VOID RETURN AND EARLY EXIT
// 'return;' immediately yields control back to the caller frame.
// =====================================================================================
void processEarlyReturn(int val) {
    cout << "  [Early Return Check] Evaluating input value: " << val << endl;
    if (val < 0) {
        cout << "  [Control Flow] Negative value detected. Executing 'return;' to exit frame early.\n";
        return; // Control immediately jumps back to caller frame
    }
    cout << "  [Control Flow] Positive value processed successfully.\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userVal = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a test integer value (e.g., 10 or -5): " << flush;
    if (!(cin >> userVal)) {
        cout << "Invalid input. Defaulting to 10." << endl;
        userVal = 10;
    }

    // 1. DEMONSTRATING RETURN BY VALUE & COPY ELISION (NRVO)
    cout << "\n================ 1. RETURN BY VALUE & RVO / NRVO ================" << endl;
    cout << "Calling demonstrateNRVO():\n";
    {
        ReturnTracker resultObj = demonstrateNRVO("NRVO_Test");
        cout << "In caller frame: 'resultObj' label = '" << resultObj.getLabel() << "'\n";
    } // resultObj destroyed here

    // 2. DEMONSTRATING RETURN BY REFERENCE & SAFETY
    cout << "\n================ 2. RETURN BY REFERENCE ================" << endl;
    string callerString = "CallerOwnedData";
    cout << "Before call : callerString = \"" << callerString << "\"\n";
    const string& refResult = getSafeReference(callerString);
    cout << "Ref returned: refResult = \"" << refResult << "\" at address: " << &refResult << "\n";
    cout << "\n  PITFALL WARNING: Never return a reference (T& or const T&) to a function-local variable!" << endl;
    cout << "                   The local variable's stack memory is popped on return, leaving a dangling reference." << endl;

    // 3. DEMONSTRATING RETURN BY SMART POINTER
    cout << "\n================ 3. SMART POINTER RETURN (OWNERSHIP TRANSFER) ================" << endl;
    {
        unique_ptr<ReturnTracker> ptrResult = createManagedTracker("HeapObject");
        cout << "In caller frame: Managed pointer holds '" << ptrResult->getLabel() << "'\n";
    } // Managed heap object automatically destroyed here

    // 4. DEMONSTRATING MULTIPLE RETURNS VIA C++17 STRUCTURED BINDINGS
    cout << "\n================ 4. MULTIPLE RETURNS (std::tuple & C++17) ================" << endl;
    auto [doubledVal, scaledVal, statusStr] = calculateMetrics(userVal);
    cout << "Unpacked Return Values:\n";
    cout << "  |- Doubled Value : " << doubledVal << "\n";
    cout << "  |- Scaled Value  : " << fixed << setprecision(2) << scaledVal << "\n";
    cout << "  |- Status String : " << statusStr << "\n";

    // 5. DEMONSTRATING CONTROL FLOW & EARLY RETURN
    cout << "\n================ 5. CONTROL FLOW & EARLY RETURN ================" << endl;
    processEarlyReturn(userVal);

    cout << "\n================ FUNCTION RETURN MECHANISM SUMMARY ================" << endl;
    cout << "1. Control Flow   : 'return' restores caller instruction pointer & unwinds stack frame." << endl;
    cout << "2. Copy Elision   : Modern C++ compilers optimize value returns (RVO/NRVO) to construct in-place." << endl;
    cout << "3. Reference Safety: Never return lvalue references to stack-allocated local variables." << endl;
    cout << "4. Ownership Move : Smart pointers (std::unique_ptr) cleanly transfer heap ownership on return." << endl;
    cout << "5. Structured Bind: std::tuple with C++17 structured bindings enables elegant multi-value returns." << endl;

    return 0;
}