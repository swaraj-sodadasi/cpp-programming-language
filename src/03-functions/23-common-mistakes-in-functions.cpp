/*
 * =====================================================================================
 * CONCEPT        : Common Mistakes & Pitfalls in C++ Functions
 * DESCRIPTION    : Comprehensive implementation detailing top function anti-patterns:
 *                  1. Dangling Reference Return : Returning a reference (`T&`) to a local 
 *                                                stack variable (Undefined Behavior).
 *                  2. Accidental Pass-by-Value  : Passing large objects by value instead 
 *                                                of `const T&` (Unnecessary deep copies).
 *                  3. Forgotten Reference (`&`)  : Passing by value when caller mutation 
 *                                                is intended (Silent logic bugs).
 *                  4. Re-specifying Defaults     : Repeating default arguments in definitions 
 *                                                (Compilation error).
 *                  5. Overload Ambiguity         : Combining default arguments with 
 *                                                overloaded signatures causing ambiguity.
 *                  6. Raw Pointer Ownership      : Returning raw heap pointers causing memory 
 *                                                leaks (Fix: Use `std::unique_ptr`).
 *
 * TIME COMPLEXITY  : Best Case: O(1) scalar execution; demonstrates zero-copy vs copy costs.
 * SPACE COMPLEXITY : Best Case: O(1) for fixed stack allocations.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <utility>

using namespace std;

// Tracking object to demonstrate unnecessary copying during pass-by-value mistakes
class ResourceTracker {
private:
    string name_;

public:
    explicit ResourceTracker(string name) : name_(std::move(name)) {
        cout << "    [Constructed] Resource '" << name_ << "' initialized.\n";
    }

    ResourceTracker(const ResourceTracker& other) : name_(other.name_ + "_COPY") {
        cout << "    [DEEP COPY  ] Expensive copy created: '" << name_ << "'\n";
    }

    ~ResourceTracker() {
        cout << "    [Destructor ] Resource '" << name_ << "' destroyed.\n";
    }

    [[nodiscard]] const string& getName() const { return name_; }
};

// =====================================================================================
// MISTAKE 1: DANGLING REFERENCE RETURN
// =====================================================================================

/* 
 * CRITICAL MISTAKE (Commented out to prevent Undefined Behavior / Crash):
 *
 * const string& getDanglingReferenceBad() {
 *     string localStackStr = "Temporary Local String";
 *     return localStackStr; // BAD: Local variable destroyed when stack frame pops!
 * }
 */

// SAFE FIX: Return by value (compilers use RVO/NRVO or move semantics)
string getSafeValueGood() {
    string localStackStr = "Safe RVO/Move String";
    return localStackStr; // SAFE: Object moved or constructed directly in caller frame
}

// =====================================================================================
// MISTAKE 2: ACCIDENTAL PASS-BY-VALUE (UNNECESSARY COPYING)
// =====================================================================================

// MISTAKE: Passing heavy objects by value triggers full deep copies
void processTrackerBad(ResourceTracker tracker) {
    cout << "  - Inside processTrackerBad(): Processing " << tracker.getName() << endl;
}

// SAFE FIX: Pass by const reference (`const T&`) for zero-copy read-only access
void processTrackerGood(const ResourceTracker& tracker) {
    cout << "  - Inside processTrackerGood(): Processing " << tracker.getName() 
         << " at Address: " << &tracker << endl;
}

// =====================================================================================
// MISTAKE 3: SILENT LOGIC BUG - FORGETTING REFERENCE FOR MUTATION
// =====================================================================================

// MISTAKE: Expecting to modify caller variable when parameter is passed by value
void updateScoreBad(int score, int bonus) {
    score += bonus; // Modifies local copy only! Caller score remains unchanged.
}

// SAFE FIX: Pass by reference (`int&`) to modify caller argument directly
void updateScoreGood(int& score, int bonus) {
    score += bonus; // Directly mutates caller variable
}

// =====================================================================================
// MISTAKE 4: RE-SPECIFYING DEFAULT ARGUMENTS IN DEFINITIONS
// =====================================================================================

// Function Declaration with default argument
void configureSystem(const string& mode, int timeoutMs = 3000);

/*
 * MISTAKE (Will cause compilation error if uncommented):
 *
 * void configureSystem(const string& mode, int timeoutMs = 3000) { ... }
 * 
 * RULE: Default arguments belong ONLY in the function declaration/prototype.
 */

// SAFE FIX: Omit default argument in the definition
void configureSystem(const string& mode, int timeoutMs) {
    cout << "  - [System Config] Mode: " << mode << " | Timeout: " << timeoutMs << " ms\n";
}

// =====================================================================================
// MISTAKE 5: OVERLOAD AMBIGUITY WITH DEFAULT ARGUMENTS
// =====================================================================================

void displayAlert(const string& msg) {
    cout << "  - [Alert Single] " << msg << endl;
}

// MISTAKE AMBIGUITY TRAP:
// If we uncomment the function below, calling `displayAlert("Test")` becomes ambiguous!
/*
 * void displayAlert(const string& msg, bool urgent = false) {
 *     cout << "  - [Alert Dual] " << msg << " (Urgent: " << urgent << ")\n";
 * }
 */

// =====================================================================================
// MISTAKE 6: RAW POINTER MEMORY LEAKS vs SMART POINTERS
// =====================================================================================

// MISTAKE: Returning raw heap pointer forces caller to manually call `delete`
ResourceTracker* createRawTrackerBad(const string& name) {
    return new ResourceTracker(name + "_Raw"); // Easy to forget delete -> Memory Leak
}

// SAFE FIX: Return `std::unique_ptr` for automatic ownership and resource cleanup
unique_ptr<ResourceTracker> createSmartTrackerGood(const string& name) {
    return make_unique<ResourceTracker>(name + "_Smart"); // Automatic RAII deallocation
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userScore = 100;

    // Dynamic input collection with stream flushing
    cout << "Enter an initial bonus value (e.g., 50): " << flush;
    int bonusValue = 0;
    if (!(cin >> bonusValue) || bonusValue <= 0) {
        cout << "Invalid input. Defaulting bonus to 50." << endl;
        bonusValue = 50;
    }

    // 1. MISTAKE 1: DANGLING REFERENCE vs SAFE VALUE RETURN
    cout << "\n================ 1. DANGLING REFERENCE vs SAFE RETURN ================" << endl;
    string safeReturnedStr = getSafeValueGood();
    cout << "  - Safely returned string from function: \"" << safeReturnedStr << "\"\n";

    // 2. MISTAKE 2: PASS-BY-VALUE vs PASS-BY-CONST-REF
    cout << "\n================ 2. PASS-BY-VALUE vs PASS-BY-CONST-REF ================" << endl;
    cout << "Creating original ResourceTracker in caller frame:\n";
    ResourceTracker callerResource("PrimaryData");

    cout << "\nA. Calling Bad Version (Pass-By-Value):\n";
    processTrackerBad(callerResource); // Triggers copy constructor

    cout << "\nB. Calling Good Version (Pass-By-Const-Ref):\n";
    processTrackerGood(callerResource); // Zero-copy execution

    // 3. MISTAKE 3: SILENT MUTATION LOGIC BUG
    cout << "\n================ 3. FORGETTING REFERENCE (&) MUTATION BUG ================" << endl;
    cout << "Initial score = " << userScore << endl;

    updateScoreBad(userScore, bonusValue);
    cout << "After updateScoreBad(" << bonusValue << ")  : score = " << userScore 
         << " (BUG: Value unchanged!)\n";

    updateScoreGood(userScore, bonusValue);
    cout << "After updateScoreGood(" << bonusValue << ") : score = " << userScore 
         << " (CORRECT: Mutated in caller!)\n";

    // 4. MISTAKE 4: DEFAULT ARGUMENT PLACEMENT
    cout << "\n================ 4. DEFAULT ARGUMENT PLACEMENT ================" << endl;
    configureSystem("ProductionMode"); // Uses default timeout of 3000ms
    configureSystem("DebugMode", 1000);   // Overrides default timeout

    // 5. MISTAKE 5: RAW POINTER LEAKS vs SMART POINTERS
    cout << "\n================ 5. RAW POINTER LEAKS vs SMART POINTERS ================" << endl;
    
    cout << "\nA. Bad Raw Pointer Allocation (Requires manual delete):\n";
    ResourceTracker* rawPtr = createRawTrackerBad("ManualMemory");
    cout << "  - Manually deleting raw pointer to avoid memory leak...\n";
    delete rawPtr; // Easy to forget!

    cout << "\nB. Good Smart Pointer Allocation (RAII Auto Cleanup):\n";
    {
        unique_ptr<ResourceTracker> smartPtr = createSmartTrackerGood("AutomaticMemory");
        cout << "  - Using smartPtr inside local scope...\n";
    } // Managed object automatically destroyed here upon exiting scope

    cout << "\n================ COMMON FUNCTION MISTAKES SUMMARY ================" << endl;
    cout << "1. Dangling Reference : NEVER return references to local stack objects (`T&`)." << endl;
    cout << "2. Pass-By-Value      : Prefer `const T&` for objects/vectors to eliminate deep copies." << endl;
    cout << "3. Missing Reference  : Always pass by `T&` when caller variable mutation is intended." << endl;
    cout << "4. Default Specs      : Declare default values in headers/prototypes, NOT in definitions." << endl;
    cout << "5. Raw Memory Leaks   : Return `std::unique_ptr` / `std::shared_ptr` instead of raw pointers." << endl;

    return 0;
}